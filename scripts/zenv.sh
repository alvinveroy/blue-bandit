#!/bin/bash
WD=`pwd`
if [  "${WD##*/}" != "blue-bandit" ]; then
    echo "ERROR: Must run in blue-bandit root dir"
    return 1
fi

#See getting started: https://docs.zephyrproject.org/latest/getting_started/index.html
export ZEPHYR_BASE=`readlink -f ./zephyr`
export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=~/Downloads/gcc-arm-none-eabi-7-2018-q2-update-linux/gcc-arm-none-eabi-7-2018-q2-update

echo "You should update $0 to have zephyr point to, and use, YOUR embedded toolchain"

# See board list: https://docs.zephyrproject.org/latest/boards/index.html#boards
export BOARD=nrf52840_pca10056
