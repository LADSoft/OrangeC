;File .\slconv.c
;Compiler version 5.0.1.1
	SECTION code align=2 CLASS=CODE USE32
	SECTION data align=8 CLASS=DATA USE32
	SECTION bss  align=8 CLASS=BSS USE32
	SECTION const  align=8 CLASS=CONST USE32
	SECTION string  align=2 CLASS=STRING USE32
	SECTION tls  align=2 CLASS=TLS USE32
	SECTION cstartup align=2 CLASS=INITDATA USE32
	SECTION crundown align=2 CLASS=EXITDATA USE32
SECTION code
[GLOBAL	_localeconv]
_localeconv:
; Line 43:	struct lconv *  _RTL_FUNC localeconv( void )

; Line 44:	{

	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ECX
L_1:
; Line 45:	    struct lconv *lc = &__getRtlData()->lconvbuf;

	CALL	___getRtlData
	ADD	EAX,DWORD 012CH
	MOV	DWORD [EBP-04H],EAX
; Line 46:	    char *stringbuf = __getRtlData()->lconvstringbuf;

	CALL	___getRtlData
; Line 47:	    char *stringbufptr = stringbuf;

; Line 48:	

; Line 49:	    lc->decimal_point = ".";

	MOV	ECX,DWORD [EBP-04H]
	MOV	EAX,DWORD L_4
	MOV	DWORD [ECX+00H],EAX
; Line 50:	    lc->thousands_sep = "";

	MOV	EAX,DWORD [EBP-04H]
	MOV	ECX,DWORD L_5
	MOV	DWORD [EAX+04H],ECX
; Line 51:	    lc->grouping = "";

	MOV	ECX,DWORD L_6
	MOV	DWORD [EAX+08H],ECX
; Line 52:	

; Line 53:	    lc->int_curr_symbol = "";

	MOV	ECX,DWORD L_7
	MOV	DWORD [EAX+0CH],ECX
; Line 54:	    lc->currency_symbol = "";

	MOV	ECX,DWORD L_8
	MOV	DWORD [EAX+010H],ECX
; Line 55:	    lc->mon_decimal_point = "";

	MOV	ECX,DWORD L_9
	MOV	DWORD [EAX+014H],ECX
; Line 56:	    lc->mon_thousands_sep = "";

	MOV	ECX,DWORD L_10
	MOV	DWORD [EAX+018H],ECX
; Line 57:	    lc->mon_grouping = "";

	MOV	ECX,DWORD L_11
	MOV	DWORD [EAX+01CH],ECX
; Line 58:	    lc->positive_sign = "";

	MOV	ECX,DWORD L_12
	MOV	DWORD [EAX+020H],ECX
; Line 59:	    lc->negative_sign = "";

	MOV	ECX,DWORD L_13
	MOV	DWORD [EAX+024H],ECX
; Line 60:	    lc->int_frac_digits = CHAR_MAX;

	MOV	BYTE [EAX+028H],07FH
; Line 61:	    lc->frac_digits = CHAR_MAX;

	MOV	BYTE [EAX+029H],07FH
; Line 62:	    lc->p_cs_precedes = CHAR_MAX;

	MOV	BYTE [EAX+02AH],07FH
; Line 63:	    lc->p_sep_by_space = CHAR_MAX;

	MOV	BYTE [EAX+02BH],07FH
; Line 64:	    lc->n_cs_precedes = CHAR_MAX;

	MOV	BYTE [EAX+02CH],07FH
; Line 65:	    lc->n_sep_by_space = CHAR_MAX;

	MOV	BYTE [EAX+02DH],07FH
; Line 66:	    lc->p_sign_posn = CHAR_MAX;

	MOV	BYTE [EAX+02EH],07FH
; Line 67:	    lc->n_sign_posn = CHAR_MAX;

	MOV	BYTE [EAX+02FH],07FH
; Line 68:	    lc->int_p_cs_precedes = CHAR_MAX;

	MOV	BYTE [EAX+030H],07FH
; Line 69:	    lc->int_p_sep_by_space = CHAR_MAX;

	MOV	BYTE [EAX+032H],07FH
; Line 70:	    lc->int_n_cs_precedes = CHAR_MAX;

	MOV	BYTE [EAX+031H],07FH
; Line 71:	    lc->int_n_sep_by_space = CHAR_MAX;

	MOV	BYTE [EAX+033H],07FH
; Line 72:	    lc->int_p_sign_posn = CHAR_MAX;

	MOV	BYTE [EAX+034H],07FH
; Line 73:	    lc->int_n_sign_posn = CHAR_MAX;

	MOV	BYTE [EAX+035H],07FH
; Line 74:	    return &lc;

	LEA	EAX,[EBP-04H]
; Line 75:	}

L_2:
	MOV	ESP,EBP
	POP	EBP
	RET
[GLOBAL	_setlocale]
_setlocale:
; Line 76:	char *          _RTL_FUNC setlocale( int __category, const char *__locale )

; Line 77:	{	

	PUSH	EBP
	MOV	EBP,ESP
L_17:
; Line 78:	    return 0;

	XOR	EAX,EAX
; Line 79:	}

L_18:
	POP	EBP
	RET
SECTION string
L_13:
	DB	00H
L_12:
	DB	00H
L_11:
	DB	00H
L_10:
	DB	00H
L_9:
	DB	00H
L_8:
	DB	00H
L_7:
	DB	00H
L_6:
	DB	00H
L_5:
	DB	00H
L_4:
	DB	"."
	DB	00H
SECTION const

SECTION code
[EXTERN	___getRtlData]
