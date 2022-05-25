for file in `find /proc/irq -name "smp_affinity_list"`
do
    # cat ${file}
    # echo $var
    sudo sh -c "echo 0-18 > ${file}"
done
# sudo bash -c "echo 7f > /proc/irq/default_smp_affinity"