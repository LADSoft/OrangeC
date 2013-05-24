;File q.c
;Compiler version 4.2.4
    SECTION code align=2 CLASS=CODE USE32
    SECTION data align=8 CLASS=DATA USE32
    SECTION bss  align=8 CLASS=BSS USE32
    SECTION const  align=8 CLASS=CONST USE32
    SECTION string  align=2 CLASS=STRING USE32
    SECTION tls  align=2 CLASS=TLS USE32
    SECTION cstartup align=2 CLASS=INITDATA USE32
    SECTION crundown align=2 CLASS=EXITDATA USE32
SECTION code
[GLOBAL	_main]
_main:
; Line 2:	main(struct aa *v)

    PUSH	EBP
    MOV	EBP,ESP
L_1:
; Line 3:	{

; Line 4:	    int cc;

; Line 5:	    cc = *bb;

    MFENCE
    MOV	EAX,DWORD [_bb]
    MOV	EAX,DWORD [EAX+00H]
; Line 6:	    *aa = cc;

    MOV	ECX,DWORD [_aa]
    MOV	DWORD [ECX+00H],EAX
; Line 7:	    

; Line 8:	    *aa = *bb;

    MFENCE
    MOV	EAX,DWORD [_bb]
    MOV	ECX,DWORD [EAX+00H]
    MOV	EAX,DWORD [_aa]
    MOV	DWORD [EAX+00H],ECX
; Line 9:	}

    XOR	EAX,EAX
L_2:
    POP	EBP
    RET
SECTION bss
[GLOBAL	_aa]

_aa	RESB	04H

[GLOBAL	_bb]

_bb	RESB	04H

SECTION code
SECTION const

