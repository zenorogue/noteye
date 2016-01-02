# installed version of caller.sh

echo
echo Looking for: \'$NOTEYEDIR/$2\', \'./$2\', \'$2\'

if [ -e $NOTEYEDIR/$2 ]
then
  $NOTEYEDIR/$2
elif [ -e ./$2 ]
then
  ./$2
elif [ -n "`which $2`" ]
then
  $2
else
echo
echo $3 not found on your system. 
echo
echo Make sure that it can be run as \'$2\' or \'./$2\'.
echo
echo Alternatively, you can start it manually from the prompt below.
echo
sh -l
fi
