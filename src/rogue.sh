#!/bin/sh

if [ -e rogue ]
then

  if test -e rogue.save
  then
    rogue rogue.save
  else
    rogue
  fi

elif [ -n "`which rogue`" ]
then

  cd $NOTEYECONFIG
  if test -e rogue.save
  then
    rogue rogue.save
  else
    rogue
  fi

else

echo Rogue not found on your system.
echo
echo Make sure that it can be run as \'$2\' or \'./$2\'.
echo
echo For example, in Ubuntu you can install Rogue with:
echo
echo sudo apt-get install bsdgames-nonfree
echo
echo Otherwise, you can also run it manually from the prompt below.
echo
sh -l

fi
