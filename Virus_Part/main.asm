;-----------------------------------------------------------------------------------------
;Authors: Maslov Mikhail, Sokolov Igor, Bolotin Petr, Kaloshin Pavel.
;-----------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------
;The structure of program is incapsulated in separate file, to simplify debugging.
;-----------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------
;Some options for compiler.
.386 ;Processor directive. Shows what architecture we use.
.model flat, stdcall ;Initialising memory models (memorymodel - flat, langtype - stdcall).
option casemap:none ;Turning on symbol case sensitivity.
;-----------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------
;Some really strange macros. It works, but for me it's like magic.
pushz macro szText:VARARG
    local nexti
    call  nexti
    db    szText, 0
nexti:
endm
;-----------------------------------------------------------------------------------------
;Some pre-code includes.
includelib C:\masm32\lib\kernel32.lib ;Including library for ExitProcess function.
;-----------------------------------------------------------------------------------------
ExitProcess PROTO :DWORD ;We need this string that declares the prototype of ExitProcess 
						 ;function (arguments that function will use).						
;-----------------------------------------------------------------------------------------
.data ;Beginning of data section.
    db 0 ;Define byte -> 0 (declaring nothing, OMG).
;-----------------------------------------------------------------------------------------
.code ;Beginning of a code section.
    invoke ExitProcess, 0 ;Exiting to Windows.
;-----------------------------------------------------------------------------------------
start: ;Declaring start link.

include inc\start_code.inc ;Including start_code.inc.
include inc\virus_code.inc ;Including virus_code.inc - main virus code.
include inc\data.inc ;Including data.inc

end start ;End of code section by start link.
;-----------------------------------------------------------------------------------------