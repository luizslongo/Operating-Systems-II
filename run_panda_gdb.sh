#!/bin/bash
PANDA="/home/kira/teaching"
GDB='arm-xilinx-eabi-gdb'
#if [ $# -ne 1 ]; then
#	echo "Usage: ${0} [1|2]"
#exit
#fi

echo "Running qemu."
qemu -no-reboot -nographic -s -S -machine xilinx-zynq-a9 -cpu cortex-a9 -kernel $PANDA/img/panda_app -fda $PANDA/img/panda_app.img &
QEMUPID=$!
$GDB -ex "target remote :1234"
echo Killing $QEMUPID
kill -9 $QEMUPID

#reset
#gnome-terminal -x bash -c 'arm-none-eabi-gdb -ex "target remote :1234";bash'
