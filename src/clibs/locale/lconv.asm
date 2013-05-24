;File .\lconv.c
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
_tostring:
; Line 45:	static char *tostring(char *str, char **stringbufptr)

; Line 46:	{

	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
L_1:
; Line 47:	    char *rv = *stringbufptr;

	MOV	EAX,DWORD [EBP+0CH]
	MOV	EAX,DWORD [EAX+00H]
	MOV	ECX,DWORD [EBP+08H]
	MOV	CL,BYTE [ECX+00H]
	AND	CL,CL
	JE	SHORT	L_5
L_4:
; Line 49:	        *(*stringbufptr)++ = *str++;

	MOV	ESI,DWORD [EBP+08H]
	MOV	CL,BYTE [ESI+00H]
	MOV	EDX,DWORD [EBP+0CH]
	MOV	EBX,DWORD [EDX+00H]
	MOV	BYTE [EBX+00H],CL
	MOV	ECX,ESI
	INC	ECX
	MOV	DWORD [EBP+08H],ECX
	MOV	ECX,EBX
	INC	ECX
	MOV	DWORD [EDX+00H],ECX
L_6:
; Line 48:	    while (*str)

	MOV	ECX,DWORD [EBP+08H]
	MOV	CL,BYTE [ECX+00H]
	AND	CL,CL
	JNE	SHORT	L_4
L_5:
; Line 50:	    *(*stringbufptr)++ = *str++;

	MOV	ESI,DWORD [EBP+08H]
	MOV	CL,BYTE [ESI+00H]
	MOV	EDX,DWORD [EBP+0CH]
	MOV	EBX,DWORD [EDX+00H]
	MOV	BYTE [EBX+00H],CL
	MOV	ECX,ESI
	INC	ECX
	MOV	DWORD [EBP+08H],ECX
	MOV	ECX,EBX
	INC	ECX
	MOV	DWORD [EDX+00H],ECX
; Line 51:	    return rv;

; Line 52:	}

L_2:
	POP	ESI
	POP	EBX
	POP	EBP
	RET
[GLOBAL	__llocaleconv]
__llocaleconv:
; Line 53:	struct lconv *  _RTL_FUNC _llocaleconv( void )

; Line 54:	{

	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ECX
	PUSH	ECX
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
L_14:
; Line 55:	    MONETARY_DATA *monetary_data =((MONETARY_DATA *)(__locale_data[LC_MONETARY])) ;

	MOV	EDI,DWORD [___locale_data+0CH]
; Line 56:	    NUMERIC_DATA *numeric_data = ((NUMERIC_DATA *)(__locale_data[LC_NUMERIC])) ;

	MOV	EBX,DWORD [___locale_data+010H]
; Line 57:	    struct lconv *lc = &__getRtlData()->lconvbuf;

	CALL	___getRtlData
	ADD	EAX,DWORD 012CH
	MOV	DWORD [EBP-04H],EAX
; Line 58:	    char *stringbuf = __getRtlData()->lconvstringbuf;

	CALL	___getRtlData
	ADD	EAX,DWORD 0162H
; Line 59:	    char *stringbufptr = stringbuf;

	MOV	DWORD [EBP-08H],EAX
; Line 60:	

; Line 61:	    lc->decimal_point = tostring(numeric_data->decimal_point,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 04H
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	ECX,EAX
	MOV	EAX,DWORD [EBP-04H]
	MOV	DWORD [EAX+00H],ECX
; Line 62:	    lc->thousands_sep = tostring(numeric_data->thousands_sep,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 06H
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	ESI,DWORD [EBP-04H]
	MOV	DWORD [ESI+04H],EAX
; Line 63:	    lc->grouping = tostring(numeric_data->grouping,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 010H
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
; Line 64:	

; Line 65:	    lc->int_curr_symbol = tostring(monetary_data->int_curr_symbol,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EBX,EDI
	MOV	EAX,EBX
	ADD	EAX,BYTE 04H
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+0CH],EAX
; Line 66:	    lc->currency_symbol = tostring(monetary_data->currency_symbol,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 09H
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+010H],EAX
; Line 67:	    lc->mon_decimal_point = tostring(monetary_data->mon_decimal_point,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 0EH
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+014H],EAX
; Line 68:	    lc->mon_thousands_sep = tostring(monetary_data->mon_thousands_sep,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 010H
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+018H],EAX
; Line 69:	    lc->mon_grouping = tostring(monetary_data->mon_grouping,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 015H
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+01CH],EAX
; Line 70:	    lc->positive_sign = tostring(monetary_data->positive_sign,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 01AH
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+020H],EAX
; Line 71:	    lc->negative_sign = tostring(monetary_data->negative_sign,&stringbufptr);

	LEA	EAX,[EBP-08H]
	PUSH	EAX
	MOV	EAX,EBX
	ADD	EAX,BYTE 01FH
	PUSH	EAX
	CALL	_tostring
	POP	ECX
	POP	ECX
	MOV	DWORD [ESI+024H],EAX
; Line 72:	    lc->int_frac_digits = monetary_data->int_frac_digits;

	MOV	AL,BYTE [EDI+024H]
	MOV	BYTE [ESI+028H],AL
; Line 73:	    lc->frac_digits = monetary_data->frac_digits;

	MOV	AL,BYTE [EDI+025H]
	MOV	BYTE [ESI+029H],AL
; Line 74:	    lc->p_cs_precedes = monetary_data->p_cs_precedes;

	MOV	AL,BYTE [EDI+026H]
	MOV	BYTE [ESI+02AH],AL
; Line 75:	    lc->p_sep_by_space = monetary_data->p_sep_by_space;

	MOV	AL,BYTE [EDI+027H]
	MOV	BYTE [ESI+02BH],AL
; Line 76:	    lc->n_cs_precedes = monetary_data->n_cs_precedes;

	MOV	AL,BYTE [EDI+028H]
	MOV	BYTE [ESI+02CH],AL
; Line 77:	    lc->n_sep_by_space = monetary_data->n_sep_by_space;

	MOV	AL,BYTE [EDI+029H]
	MOV	BYTE [ESI+02DH],AL
; Line 78:	    lc->p_sign_posn = monetary_data->p_sign_posn;

	MOV	AL,BYTE [EDI+02AH]
	MOV	BYTE [ESI+02EH],AL
; Line 79:	    lc->n_sign_posn = monetary_data->n_sign_posn;

	MOV	AL,BYTE [EDI+02BH]
	MOV	BYTE [ESI+02FH],AL
; Line 80:	    lc->int_p_cs_precedes = monetary_data->int_p_cs_precedes;

	MOV	AL,BYTE [EDI+02CH]
	MOV	BYTE [ESI+030H],AL
; Line 81:	    lc->int_p_sep_by_space = monetary_data->int_p_sep_by_space;

	MOV	AL,BYTE [EDI+02DH]
	MOV	BYTE [ESI+032H],AL
; Line 82:	    lc->int_n_cs_precedes = monetary_data->int_n_cs_precedes;

	MOV	AL,BYTE [EDI+02EH]
	MOV	BYTE [ESI+031H],AL
; Line 83:	    lc->int_n_sep_by_space = monetary_data->int_n_sep_by_space;

	MOV	AL,BYTE [EDI+02FH]
	MOV	BYTE [ESI+033H],AL
; Line 84:	    lc->int_p_sign_posn = monetary_data->int_p_sign_posn;

	MOV	AL,BYTE [EDI+030H]
	MOV	BYTE [ESI+034H],AL
; Line 85:	    lc->int_n_sign_posn = monetary_data->int_n_sign_posn;    

	MOV	AL,BYTE [EDI+031H]
	MOV	BYTE [ESI+035H],AL
; Line 86:	    return &lc;

	LEA	EAX,[EBP-04H]
; Line 87:	}

L_15:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
SECTION bss

_stringbufptr	RESB	04H

SECTION code
SECTION const

SECTION code
[EXTERN	___locale_data]
[EXTERN	___getRtlData]
