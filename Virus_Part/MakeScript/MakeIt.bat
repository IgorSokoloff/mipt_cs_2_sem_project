::Compiler .BAT file.
::Do not touch anything. PLS. This stuff works great.

::THIS SCRIPT REQUIRES MASM32 SDK. You can download it here: <www.masm32.com> .

::Author: Maslov Mikhail, MIPT DPQE 453 group

@echo off

C:\masm32\bin\ml /nologo /c /coff main.asm
C:\masm32\bin\link /nologo /subsystem:windows /out:bin\main.exe /SECTION:.text,rwe main.obj
del *.obj

pause
