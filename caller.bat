@echo off
if exist batch\%2.bat goto :batchfound
if not exist %1 goto :gamemissing
if exist %1\%2.bat goto :gamefound
if not exist %1\%2.exe goto :gamemissing

:gamefound
cd %1
%2
exit

:batchfound
cd batch
%2
exit

:gamemissing
echo %3 not found.
echo ---
echo Please download it and move the contents to the directory %1
echo in the NotEye directory.
echo ---
echo Make sure that the executable is named %1\%2.exe or %1\%2.bat.
echo ---
echo Alternatively, you can start it manually from the prompt below,
echo or use the file batch\%2.bat.
echo ---
echo Note that you can also download the "bundle" version of NotEye,
echo which includes several playable games out of the box.
echo ---
cmd
