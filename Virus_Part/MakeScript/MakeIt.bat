::-----------------------------------------------------------------------------------------
::Authors: Maslov Mikhail, Sokolov Igor, Bolotin Petr, Kaloshin Pavel.
::-----------------------------------------------------------------------------------------

::Compiler .BAT file.
::Do not touch anything. PLS. This stuff works great.

::THIS SCRIPT REQUIRES MASM32 SDK. You can download it here: <www.masm32.com> .

::This script is used just to compile assembler code with MASM32 compiler.
::All functions are just for compiling.

@echo off

C:\masm32\bin\ml /nologo /c /coff main.asm
C:\masm32\bin\link /nologo /subsystem:windows /out:bin\main.exe /SECTION:.text,rwe main.obj
del *.obj

pause
