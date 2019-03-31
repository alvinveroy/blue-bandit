#!/bin/bash

USAGE="zenv.sh <zephyr-root dir>"
if [ -z $1 ] || [ ! -d $1 ]; then
    echo $USAGE
    exit 1
fi


#See getting started: https://docs.zephyrproject.org/latest/getting_started/index.html
export ZEPHYR_BASE=$1
export ZEPHYR_TOOLCHAIN_VARIANT=gnuarmemb
export GNUARMEMB_TOOLCHAIN_PATH=~/Downloads/gcc-arm-none-eabi-7-2018-q2-update-linux/gcc-arm-none-eabi-7-2018-q2-update


# See board list: https://docs.zephyrproject.org/latest/boards/index.html#boards
export BOARD=nrf52840_pca10056
