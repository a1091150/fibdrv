#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>

#include "kdecnum.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_DESCRIPTION("Fibonacci engine driver");
MODULE_VERSION("0.1");

#define DEV_FIBONACCI_NAME "fibonacci"

/* MAX_LENGTH is set to 92 because
 * ssize_t can't fit the number > 92
 */
#define MAX_LENGTH 150

static dev_t fib_dev = 0;
static struct cdev *fib_cdev;
static struct class *fib_class;
static DEFINE_MUTEX(fib_mutex);

static int decnum_fib_fast_doubling(long long k, kdecnum_t *result)
{
    if (!result) {
        return -1;
    }
    
    long long int sp = (k / 42) + 2;

    kdecnum_t a = KDECNUM_INIT(1, sp);
    kdecnum_new(&a);

    if (!a.digits) {
        goto fa;
    }

    kdecnum_t b = KDECNUM_INIT(1, sp);
    kdecnum_new(&b);

    if (!b.digits) {
        goto fb;
    }

    kdecnum_t tmp = KDECNUM_INIT(1, sp);
    kdecnum_new(&tmp);

    if (!tmp.digits) {
        goto ft;
    }

    kdecnum_t tmp2 = KDECNUM_INIT(1, sp);
    kdecnum_new(&tmp2);

    if (!tmp2.digits) {
        goto ft2;
    }

    kdecnum_clean(&a);
    kdecnum_clean(&b);
    kdecnum_clean(&tmp);
    kdecnum_clean(&tmp2);

    a.size = 1;
    a.digits[0] = 0;
    b.size = 1;
    b.digits[0] = 1;

    unsigned long long mask = ULLONG_MAX ^ (ULLONG_MAX >> 1);
    mask >>= __builtin_clz(k);


    for (; mask; mask >>= 1) {
        if (mask & k) {
            // A = (a + b)^2 - 2ab = a^2 + b^2
            kdecnum_add(&a, &b, &tmp);
            kdecnum_mult(&tmp, &tmp, &tmp2);
            kdecnum_mult(&a, &b, &tmp);
            kdecnum_multi_by_two(&tmp);
            kdecnum_sub(&tmp2, &tmp, &tmp);

            // B = (a + b)^2 - a^2
            kdecnum_mult(&a, &a, &b);
            kdecnum_sub(&tmp2, &b, &b);

            kdecnum_assign(&a, &tmp);
        } else {
            // B = a^2 + b^2
            kdecnum_mult(&a, &a, &tmp);
            kdecnum_mult(&b, &b, &tmp2);
            kdecnum_add(&tmp, &tmp2, &tmp2);

            // A = a * (2 * b - a)
            kdecnum_multi_by_two(&b);
            kdecnum_sub(&b, &a, &b);
            kdecnum_mult(&b, &a, &tmp);
            kdecnum_assign(&b, &tmp2);

            kdecnum_assign(&a, &tmp);
        }
    }

    kdecnum_free(&tmp);
    kdecnum_free(&tmp2);
    kdecnum_free(&b);
    kdecnum_swap(result, &a);
    return 1;

ft2:
    kdecnum_free(&tmp);
ft:
    kdecnum_free(&b);
fb:
    kdecnum_free(&a);
fa:
    result->cap = 0;
    result->size = 0;
    result->digits = NULL;
    return -1;
}


/*
static long long fib_sequence(long long k)
{
    if (!k) {
        return 0;
    }

    long long y = 1, z = 1;

    for (int i = 2; i < k; i++) {
        long long tmp = y;
        y = z;
        z = y + tmp;
    }

    return z;
}
*/

static int fib_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&fib_mutex)) {
        printk(KERN_ALERT "fibdrv is in use");
        return -EBUSY;
    }
    return 0;
}

static int fib_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&fib_mutex);
    return 0;
}

/* calculate the fibonacci number at given offset */
static ssize_t fib_read(struct file *file,
                        char *buf,
                        size_t size,
                        loff_t *offset)
{
    kdecnum_t b1 = KDECNUM_INIT(0, 0);

    ktime_t kk = ktime_get();
    int res = decnum_fib_fast_doubling(*offset, &b1);
    kk = ktime_sub(ktime_get(), kk);
    ssize_t dd = ktime_to_ns(kk);

    size_t ss = min(size, b1.size * sizeof(int32_t));
    access_ok(buf, ss);
    res = copy_to_user(buf, b1.digits, ss);
    kdecnum_free(&b1);
    return dd;
}

/* write operation is skipped */
static ssize_t fib_write(struct file *file,
                         const char *buf,
                         size_t size,
                         loff_t *offset)
{
    return 1;
}

static loff_t fib_device_lseek(struct file *file, loff_t offset, int orig)
{
    loff_t new_pos = 0;
    switch (orig) {
    case 0: /* SEEK_SET: */
        new_pos = offset;
        break;
    case 1: /* SEEK_CUR: */
        new_pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END: */
        new_pos = MAX_LENGTH - offset;
        break;
    }

    if (new_pos > MAX_LENGTH)
        new_pos = MAX_LENGTH;  // max case
    if (new_pos < 0)
        new_pos = 0;        // min case
    file->f_pos = new_pos;  // This is what we'll use now
    return new_pos;
}

const struct file_operations fib_fops = {
    .owner = THIS_MODULE,
    .read = fib_read,
    .write = fib_write,
    .open = fib_open,
    .release = fib_release,
    .llseek = fib_device_lseek,
};

static int __init init_fib_dev(void)
{
    int rc = 0;

    mutex_init(&fib_mutex);

    // Let's register the device
    // This will dynamically allocate the major number
    rc = alloc_chrdev_region(&fib_dev, 0, 1, DEV_FIBONACCI_NAME);

    if (rc < 0) {
        printk(KERN_ALERT
               "Failed to register the fibonacci char device. rc = %i",
               rc);
        return rc;
    }

    fib_cdev = cdev_alloc();
    if (fib_cdev == NULL) {
        printk(KERN_ALERT "Failed to alloc cdev");
        rc = -1;
        goto failed_cdev;
    }
    fib_cdev->ops = &fib_fops;
    rc = cdev_add(fib_cdev, fib_dev, 1);

    if (rc < 0) {
        printk(KERN_ALERT "Failed to add cdev");
        rc = -2;
        goto failed_cdev;
    }

    fib_class = class_create(THIS_MODULE, DEV_FIBONACCI_NAME);

    if (!fib_class) {
        printk(KERN_ALERT "Failed to create device class");
        rc = -3;
        goto failed_class_create;
    }

    if (!device_create(fib_class, NULL, fib_dev, NULL, DEV_FIBONACCI_NAME)) {
        printk(KERN_ALERT "Failed to create device");
        rc = -4;
        goto failed_device_create;
    }
    return rc;
failed_device_create:
    class_destroy(fib_class);
failed_class_create:
    cdev_del(fib_cdev);
failed_cdev:
    unregister_chrdev_region(fib_dev, 1);
    return rc;
}

static void __exit exit_fib_dev(void)
{
    mutex_destroy(&fib_mutex);
    device_destroy(fib_class, fib_dev);
    class_destroy(fib_class);
    cdev_del(fib_cdev);
    unregister_chrdev_region(fib_dev, 1);
}

module_init(init_fib_dev);
module_exit(exit_fib_dev);
