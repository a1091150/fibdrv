CONFIG_MODULE_SIG = n
TARGET_MODULE := fibdrv_new

obj-m := $(TARGET_MODULE).o
$(TARGET_MODULE)-objs := fibdrv.o kdecnum.o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) client
	$(MAKE) -C $(KDIR) M=$(PWD) modules

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

clean: unload
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) client out
	$(RM) uclient out
	$(RM) uclient_picture.png
	$(RM) uclient_time
	$(RM) dclient out
	$(RM) eclient_time
	$(RM) eclient_picture.png
	$(RM) eclient out
load:
	sudo insmod $(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE) || true >/dev/null

client: client.c
	$(CC) -o $@ $^

PRINTF = env printf
PASS_COLOR = \e[32;01m
NO_COLOR = \e[0m
pass = $(PRINTF) "$(PASS_COLOR)$1 Passed [-]$(NO_COLOR)\n"

check: all
	$(MAKE) unload
	$(MAKE) load
	sudo ./client > out
	$(MAKE) unload
	@diff -u out scripts/expected.txt && $(call pass)
	@scripts/verify.py


uclient: uclient.c fibseq.c
	$(CC) -o $@ $^

ucheck: uclient
	./uclient > ./uclient_time
	gnuplot scripts/uclieng_plot.gp


dclient: dclient.c decnum.c fibseq.c
	$(CC) -o $@ $^

eclient: eclient.c
	$(CC) -o $@ $^

eplot: all eclient
	$(MAKE) unload
	$(MAKE) load
	sudo taskset -c 1 ./eclient > ./eclient_time
	gnuplot scripts/eclient_plot.gp

AA := $(shell cat /proc/sys/kernel/randomize_va_space)
BB := $(shell cat /sys/devices/system/cpu/intel_pstate/no_turbo)
eall: all eclient
	@echo "原本 ASLR 數值: $(AA)"
	@echo "原本 Intel turbo mode 數值: $(BB)"
	sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"
	sudo sh -c "echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo"
	$(MAKE) unload
	$(MAKE) load
	sudo taskset -c 1 ./eclient > ./eclient_time
	gnuplot scripts/eclient_plot.gp
	sudo sh -c "echo $(AA) > /proc/sys/kernel/randomize_va_space"
	sudo sh -c "echo $(BB) > /sys/devices/system/cpu/intel_pstate/no_turbo"