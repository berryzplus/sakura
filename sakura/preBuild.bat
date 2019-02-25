@echo off

SETLOCAL

set EXEDIR=%1
set SRCDIR=..\sakura_core

call funccode.bat     %EXEDIR% %SRCDIR%
call gen-makefile.bat %EXEDIR% %SRCDIR%
call githash.bat %SRCDIR%
