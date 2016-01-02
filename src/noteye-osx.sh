#!/bin/bash
export CFGDIR=$HOME/.config/noteye/
mkdir -p $CFGDIR
cd "${0%/*}"
cd ../Resources
../MacOS/NotEye -f $CFGDIR/hydra.sav -b $CFGDIR/hydra.sav -t $CFGDIR/hydralog.txt -g $CFGDIR/hydrascore.sav -N -U $CFGDIR
