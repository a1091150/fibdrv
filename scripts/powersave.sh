for i in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
do
    echo powersave > ${i}
done