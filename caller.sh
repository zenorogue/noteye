#!/bin/sh

# disable UTF-8
export LANG=`echo $LANG | sed s/UTF-8/ISO-8859-1/`

echo
echo Looking for: $1/$2

if [ -e $1/$2 ] 
then
  cd $1
  ./$2
elif [ -n "`which $2`" ]
then
  $2
else
echo
echo $3 not found. 
echo
echo Please install it in your system, or download it and move the
echo contents to the subdirectory \'$1\' in the NotEye directory.
echo Make sure that the executable is named \'$2\'.
echo
echo Alternatively, you can start it manually from the prompt below.
echo
sh -l
fi
