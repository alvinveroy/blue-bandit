#!/bin/bash
USAGE="flash <path-to-hex>"
HEX=$1
if [ -z $1 ] || [ ! -f $1 ] || [ ! "${1##*.}" = "hex" ] ; then
    echo $USAGE
    exit 1
fi

nrfjprog -f nrf52 --eraseall
nrfjprog -f nrf52 --program $HEX
nrfjprog -f nrf52 --reset
