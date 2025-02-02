CONFIG_MODULE_SIG = n
TARGET_MODULE := fibdrv_new

obj-m := $(TARGET_MODULE).o
$(TARGET_MODULE)-objs := fibdrv.o kdecnum.o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

GIT_HOOKS := .git/hooks/applied

performance:
#	cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
	sudo sh scripts/performance.sh

powersave:
#	cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
	sudo sh scripts/powersave.sh

noturbo:
	sudo sh -c "echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo"

noaslr:
	sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"

irqaffinity:
	sudo sh scripts/changeIRQAffinity.sh

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
#	$(RM) eclient_picture.png
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

eplot: eclient
	sudo taskset -c 19 ./eclient > ./eclient_time
	gnuplot scripts/eclient_plot.gp

eall: all eclient noturbo noaslr performance irqaffinity
	$(MAKE) unload
	$(MAKE) load
	sudo taskset -c 19 ./eclient > ./eclient_time
	gnuplot scripts/eclient_plot.gp


etrace: eclient
	sudo trace-cmd record --max-graph-depth 1  -e all taskset -c 19 ./eclient > ./eclient_time
	gnuplot scripts/eclient_plot.gp