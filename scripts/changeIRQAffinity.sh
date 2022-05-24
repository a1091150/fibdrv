for file in `find /proc/irq -name "smp_affinity"`
do
    cat ${file}
    # echo $var
    sudo sh -c "echo 7ffff > ${file}"
done
# sudo bash -c "echo 7f > /proc/irq/default_smp_affinity"