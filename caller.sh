#!/bin/sh

# disable UTF-8
export LANG=`echo $LANG | sed s/UTF-8/ISO-8859-1/`

dir=$1
name=$2
displayname=$3
shift
shift
shift

echo
echo Looking for: $dir/$name

if [ -e $dir/$name ] 
then
  cd $dir
  ./$name "$@"
elif [ -n "`which $name`" ]
then
  $2 "$@"
else
echo
echo $displayname not found. 
echo
echo Please install it in your system, or download it and move the
echo contents to the subdirectory \'$dir\' in the NotEye directory.
echo Make sure that the executable is named \'$name\'.
echo
echo Alternatively, you can start it manually from the prompt below.
echo
sh -l
fi
