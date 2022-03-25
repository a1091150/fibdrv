#include "bn_kernel.h"

#ifndef DIV_ROUNDUP
#define DIV_ROUNDUP(x, len) (((x) + (len) -1) / (len))
#endif

static inline void bnfree(struct bn *src)
{
    free(src->number);
    free(src);
}

struct bn *bn_new(size_t size)
{
    struct bn *src;
    src = malloc(sizeof(struct bn));
    if (!src) {
        return NULL;
    }
    src->number = malloc(sizeof(unsigned int) * size);
    if (!src->number) {
        free(src);
        return NULL;
    }

    memset(src->number, 0, sizeof(unsigned int) * size);
    src->size = size;

    return src;
}

static size_t bn_clz(const struct bn *src)
{
    size_t cnt = 0;
    for (size_t i = src->size - 1; i != 0; i--) {
        if (src->number[i]) {
            cnt += __builtin_clz(src->number[i]);
            return cnt;
        } else {
            cnt += 32;
        }
    }

    if (src->number[0]) {
        cnt += __builtin_clz(src->number[0]);
        return cnt;
    } else {
        cnt += 32;
    }

    return cnt;
}

static size_t bn_msb(const struct bn *src)
{
    return src->size * 32 - bn_clz(src);
}

void bn_free(struct bn *src)
{
    if (!src) {
        return;
    }

    bnfree(src);
}

void bn_resize(struct bn *src, size_t size)
{
    if (!size && !src) {
        return;
    }

    if (size == src->size) {
        return;
    }

    src->number = realloc(src->number, sizeof(struct bn) * size);
    if (!src->number) {
        return;
    }
    if (size > src->size) {
        memset(src->number + src->size, 0, sizeof(struct bn) * size);
    }

    src->size = size;
}

void bn_multbytwo(struct bn *src)
{
    size_t size = src->size;
    const unsigned int mask = (UINT_MAX ^ (UINT_MAX >> 1));
    if (src->number[(size - 1)] & mask) {
        bn_resize(src, src->size + 1);
    }

    for (size_t i = src->size - 1; i > 0; i--) {
        src->number[i] = (src->number[i] << 1) | (!!(src->number[i] & mask));
    }

    src->number[0] <<= 1;
}

static void bn_mult_add(struct bn *src,
                        unsigned int offset,
                        unsigned long long int x)
{
    unsigned long long int carry = 0;
    for (unsigned int i = offset; i < src->size; i++) {
        carry += src->number[i] + (UINT_MAX & x);
        src->number[i] = carry;
        carry >>= 32;
        x >>= 32;
        if (!x && !carry) {
            return;
        }
    }
}

struct bn *bn_square(struct bn *src)
{
    if (!src) {
        return NULL;
    }
    // bn_resize(src, src->size << 1);
    struct bn *dest = bn_new(src->size << 1);

    const size_t size = src->size;
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            unsigned long long int carry = 0;
            carry = src->number[i] * src->number[j];
            bn_mult_add(dest, i + j, carry);
        }
    }

    return dest;
}

struct bn *bn_mult(struct bn *a, struct bn *b)
{
    if (!a && !b) {
        return NULL;
    }

    size_t d = bn_msb(a) + bn_msb(b);
    d = DIV_ROUNDUP(d, 32) + !d;
    struct bn *dest = bn_new(d);

    for (size_t i = 0; i < a->size; i++) {
        for (size_t j = 0; j < b->size; j++) {
            unsigned long long int carry = 0;
            carry = a->number[i] * b->number[j];
            bn_mult_add(dest, i + j, carry);
        }
    }
    return dest;
}


struct bn *bn_add(struct bn *a, struct bn *b)
{
    size_t ma = bn_msb(a);
    size_t mb = bn_msb(b);
    size_t d = (ma > mb ? ma : mb) + 1;
    d = DIV_ROUNDUP(d, 32) + !d;
    struct bn *dest = bn_new(d);

    unsigned long long int carry = 0;
    for (size_t i = 0; i < dest->size; i++) {
        unsigned int tmp1 = (i < a->size) ? a->number[i] : 0;
        unsigned int tmp2 = (i < b->size) ? b->number[i] : 0;
        carry += tmp1 + tmp2;
        dest->number[i] = carry;
        carry >>= (sizeof(unsigned long long int) << 2);
    }

    if (!dest->number[dest->size - 1] && (dest->size > 1)) {
        bn_resize(dest, dest->size - 1);
    }

    return dest;
}

struct bn *bn_sub(struct bn *a, struct bn *b)
{
    size_t d = a->size > b->size ? a->size : b->size;
    struct bn *dest = bn_new(d);

    long long int carry = 0;
    for (size_t i = 0; i < dest->size; i++) {
        unsigned int tmp1 = i < a->size ? a->number[i] : 0;
        unsigned int tmp2 = i < b->size ? b->number[i] : 0;

        carry = tmp1 - tmp2 - carry;
        if (carry < 0) {
            dest->number[i] = carry + (1LL << 32);
            carry = 1;
        } else {
            dest->number[i] = carry;
            carry = 0;
        }
    }

    d = bn_clz(dest) / 32;
    if (d == dest->size) {
        --d;
    }

    bn_resize(dest, dest->size - d);
    return dest;
}

/*
 * output bn to decimal string
 * Note: the returned string should be freed with the kfree()
 */
char *bn_to_string(const struct bn *src)
{
    // log10(x) = log2(x) / log2(10) ~= log2(x) / 3.322
    size_t len = (8 * sizeof(int) * src->size) / 3 + 2 + 1;
    char *s = malloc(len);
    char *p = s;

    memset(s, '0', len - 1);
    s[len - 1] = '\0';

    /* src.number[0] contains least significant bits */
    for (int i = src->size - 1; i >= 0; i--) {
        /* walk through every bit of bn */
        for (unsigned int d = UINT_MAX ^ (UINT_MAX >> 1); d; d >>= 1) {
            /* binary -> decimal string */
            int carry = !!(d & src->number[i]);
            for (int j = len - 2; j >= 0; j--) {
                s[j] += s[j] - '0' + carry;
                carry = (s[j] > '9');
                if (carry)
                    s[j] -= 10;
            }
        }
    }
    // skip leading zero
    while (p[0] == '0' && p[1] != '\0') {
        p++;
    }
    return s;
}