@echo off
::Set up your compilers here
set tcc32=c:\Distr\tcc-busybox-32/tcc/tcc.exe
set tcc64=c:\Distr\tcc-busybox-64/tcc/tcc.exe

echo Building 32-bit
%tcc32% -shared acryptprimitives.c -o acryptprimitives32.dll
%tcc32% -run patchname.c acryptprimitives32.dll > nul

echo.
%tcc32% rand.c -run

echo.
echo Building 64-bit
%tcc64% -shared acryptprimitives.c -o acryptprimitives64.dll
%tcc64% -run patchname.c acryptprimitives64.dll > nul

echo.
%tcc64% rand.c -run

move /y *.dll .. > nul
del *.def > nul

echo.
echo Done.