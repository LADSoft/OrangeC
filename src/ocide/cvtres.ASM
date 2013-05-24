	SECTION _TEXT align=1 CLASS=CODE USE32
	SECTION _DATA align=16 CLASS=DATA USE32
	SECTION _BSS align=16 CLASS=BSS USE32
	SECTION _CONST align=16 CLASS=CONST USE32
	SECTION _STRING align=4 CLASS=STRING USE32
	SECTION _INIT_ align=1 CLASS=INITDATA USE32
	SECTION _EXIT_ align=1 CLASS=EXITDATA USE32
	SECTION cppinit  align=1 CLASS=CPPINIT USE32
	SECTION cppexit  align=1 CLASS=CPPEXIT USE32
	GROUP DGROUP _DATA _BSS _CONST _STRING

SECTION _TEXT
_get_16:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	MOV	ESI,DWORD [EBP+0CH]
L_2:
;
; Line 33:	static int get_16(int be, unsigned char *s)
;
;
; Line 34:	{
;
;
; Line 35:	if (be)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JE	SHORT	L_3
;
; Line 37:	return (*s << 8) + *(s + 1);
;
	MOVZX	EAX,BYTE [ESI+00H]
	SAL	EAX,08H
	MOVZX	ECX,BYTE [ESI+01H]
	ADD	EAX,ECX
	JMP	SHORT	L_1
L_3:
;
; Line 41:	return *(short*)s;
;
	MOVSX	EAX,WORD [ESI+00H]
	JMP	SHORT	L_1
L_4:
;
; Line 43:	}
;
L_1:
	POP	ESI
	POP	EBP
	RET
_get_32:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	MOV	ESI,DWORD [EBP+0CH]
L_6:
;
; Line 47:	static long get_32(int be, unsigned char *s)
;
;
; Line 48:	{
;
;
; Line 49:	if (be)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JE	SHORT	L_7
;
; Line 51:	return (*s << 24) + (*(s + 1) << 16) + (*(s + 2) << 8) + *(s + 3);
;
	MOVZX	EAX,BYTE [ESI+01H]
	SAL	EAX,010H
	MOVZX	ECX,BYTE [ESI+00H]
	SAL	ECX,018H
	ADD	EAX,ECX
	MOVZX	ECX,BYTE [ESI+02H]
	SAL	ECX,08H
	ADD	EAX,ECX
	MOVZX	ECX,BYTE [ESI+03H]
	ADD	EAX,ECX
	JMP	SHORT	L_5
L_7:
;
; Line 55:	return *(long*)s;
;
	MOV	EAX,DWORD [ESI+00H]
	JMP	SHORT	L_5
L_8:
;
; Line 57:	}
;
L_5:
	POP	ESI
	POP	EBP
	RET
_checksize:
	PUSH	EBP
	MOV	EBP,ESP
L_11:
;
; Line 61:	static void checksize(int left, int size)
;
;
; Line 62:	{
;
;
; Line 63:	if (left < size)
;
	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,DWORD [EBP+0CH]
	JGE	SHORT	L_12
;
; Line 64:	fatal("malformed resource in RES file, exiting");
;
	PUSH	DWORD L_9
	CALL	_fatal
	POP	ECX
L_12:
;
; Line 65:	}
;
L_10:
	POP	EBP
	RET
[GLOBAL	_convert_to_internal]
_convert_to_internal:
	PUSH	EBP
	MOV	EBP,ESP
L_15:
;
; Line 940:	RES_RES *convert_to_internal(RES_ID *type, unsigned char *data, int length, int
;
;
; Line 942:	{
;
;
; Line 943:	return 0;
;
	MOV	EAX,00H
L_14:
	POP	EBP
	RET
_put_16:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
	MOV	EBX,DWORD [EBP+0CH]
	MOV	ESI,DWORD [EBP+010H]
L_17:
;
; Line 990:	static void put_16(int be, int v, unsigned char *s)
;
;
; Line 991:	{
;
;
; Line 992:	if (be)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JE	SHORT	L_18
;
; Line 994:	*s++ = v >> 8;
;
	MOV	EAX,EBX
	SAR	EAX,08H
	MOV	ECX,ESI
	INC	ESI
	MOV	BYTE [ECX+00H],AL
;
; Line 995:	*s++ = v;
;
	MOV	EAX,EBX
	MOV	ECX,ESI
	INC	ESI
	MOV	BYTE [ECX+00H],AL
	JMP	SHORT	L_19
L_18:
;
; Line 999:	*(short*)s = v;
;
	MOV	EAX,EBX
	MOV	WORD [ESI+00H],AX
L_19:
;
; Line 1001:	}
;
L_16:
	POP	ESI
	POP	EBX
	POP	EBP
	RET
_put_32:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
	MOV	EBX,DWORD [EBP+0CH]
	MOV	ESI,DWORD [EBP+010H]
L_21:
;
; Line 1005:	static long put_32(int be, int v, unsigned char *s)
;
;
; Line 1006:	{
;
;
; Line 1007:	if (be)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JE	SHORT	L_22
;
; Line 1009:	*s++ = v >> 24;
;
	MOV	EAX,EBX
	SAR	EAX,018H
	MOV	ECX,ESI
	INC	ESI
	MOV	BYTE [ECX+00H],AL
;
; Line 1010:	*s++ = v >> 16;
;
	MOV	EAX,EBX
	SAR	EAX,010H
	MOV	ECX,ESI
	INC	ESI
	MOV	BYTE [ECX+00H],AL
;
; Line 1011:	*s++ = v >> 8;
;
	MOV	EAX,EBX
	SAR	EAX,08H
	MOV	ECX,ESI
	INC	ESI
	MOV	BYTE [ECX+00H],AL
;
; Line 1012:	*s++ = v;
;
	MOV	EAX,EBX
	MOV	ECX,ESI
	INC	ESI
	MOV	BYTE [ECX+00H],AL
	JMP	SHORT	L_23
L_22:
;
; Line 1016:	*(long*)s = v;
;
	MOV	DWORD [ESI+00H],EBX
L_23:
;
; Line 1018:	}
;
L_20:
	POP	ESI
	POP	EBX
	POP	EBP
	RET
_align:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EDI,DWORD [EBP+08H]
L_25:
;
; Line 1022:	static void align(BINDATA * * * ppp, long *length)
;
;
; Line 1023:	{
;
;
; Line 1027:	if ((*length &3) == 0)
;
	MOV	EAX,DWORD [EBP+0CH]
	MOV	EAX,DWORD [EAX+00H]
	AND	EAX,BYTE 03H
	JE	SHORT	L_24
;
; Line 1028:	return ;
;
L_26:
;
; Line 1030:	add = 4-(*length &3);
;
	MOV	EAX,DWORD [EBP+0CH]
	MOV	EAX,DWORD [EAX+00H]
	AND	EAX,BYTE 03H
	NEG	EAX
	ADD	EAX,BYTE 04H
	MOV	EBX,EAX
;
; Line 1032:	d = (BINDATA*)AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1033:	d->length = add;
;
	MOV	DWORD [ESI+04H],EBX
;
; Line 1034:	d->data = AllocateMemory(add);
;
	PUSH	EBX
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1035:	memset(d->data, 0, add);
;
	PUSH	EBX
	PUSH	BYTE 00H
	PUSH	DWORD [ESI+08H]
	CALL	_memset
	ADD	ESP,BYTE 0CH
;
; Line 1037:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1038:	**ppp = d;
;
	MOV	EAX,DWORD [EDI+00H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1039:	*ppp = &(**ppp)->link;
;
	MOV	EAX,DWORD [EDI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EDI+00H],EAX
;
; Line 1041:	*length += add;
;
	MOV	EAX,DWORD [EBP+0CH]
	ADD	DWORD [EAX+00H],EBX
;
; Line 1042:	}
;
L_24:
	POP	EDI
	POP	ESI
	POP	EBX
	POP	EBP
	RET
_from_resid:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EDI,DWORD [EBP+014H]
L_29:
;
; Line 1046:	static BINDATA *from_resid(RES_ID id, int be)
;
;
; Line 1047:	{
;
;
; Line 1048:	BINDATA *d = AllocateMemory(sizeof(*d));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1049:	if (!id.hasname)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JNE	SHORT	L_30
;
; Line 1051:	if (!id.v.id)
;
	MOV	EAX,DWORD [EBP+0CH]
	TEST	EAX,EAX
	JNE	SHORT	L_32
;
; Line 1054:	d->length = 2;
;
	MOV	DWORD [ESI+04H],02H
;
; Line 1055:	d->data = AllocateMemory(4);
;
	PUSH	BYTE 04H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1056:	put_16(be, 0, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	BYTE 00H
	PUSH	EDI
	CALL	_put_16
	ADD	ESP,BYTE 0CH
	JMP	SHORT	L_33
L_32:
;
; Line 1060:	d->length = 4;
;
	MOV	DWORD [ESI+04H],04H
;
; Line 1061:	d->data = AllocateMemory(4);
;
	PUSH	BYTE 04H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1062:	put_16(be, 0xffff, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD 0FFFFH
	PUSH	EDI
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1063:	put_16(be, id.v.id, d->data + 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	DWORD [EBP+0CH]
	PUSH	EDI
	CALL	_put_16
	ADD	ESP,BYTE 0CH
L_33:
	JMP	SHORT	L_31
L_30:
;
; Line 1070:	d->length = id.v.n.len * 2+2;
;
	MOV	EAX,DWORD [EBP+0CH]
	SHL	EAX,01H
	ADD	EAX,BYTE 02H
	MOV	DWORD [ESI+04H],EAX
;
; Line 1071:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
	MOV	EBX,00H
	JMP	SHORT	L_36
L_34:
;
; Line 1073:	put_16(be, (unsigned char)id.v.n.name[i], d->data + i * 2);
;
	MOV	EAX,DWORD [ESI+08H]
	MOV	ECX,EBX
	SHL	ECX,01H
	ADD	EAX,ECX
	PUSH	EAX
	LEA	EAX,[EBX*2+00H]
	MOV	ECX,DWORD [EBP+010H]
	MOVZX	EAX,BYTE [EAX+ECX+00H]
	PUSH	EAX
	PUSH	EDI
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1072:	for (i = 0; i < id.v.n.len + 1; i++)
;
L_35:
	INC	EBX
L_36:
	MOV	EAX,EBX
	MOV	ECX,DWORD [EBP+0CH]
	INC	ECX
	CMP	EAX,ECX
	JL	SHORT	L_34
L_37:
L_31:
;
; Line 1076:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1078:	return d;
;
	MOV	EAX,ESI
L_28:
	POP	EDI
	POP	ESI
	POP	EBX
	POP	EBP
	RET
_from_string:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 0CH
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EBX,DWORD [EBP+0CH]
L_39:
;
; Line 1083:	static BINDATA *from_string(CHARACTER *str, int be)
;
;
; Line 1084:	{
;
;
; Line 1089:	len = 1;
;
	MOV	DWORD [EBP-08H],01H
;
; Line 1091:	if (str)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JE	SHORT	L_40
	MOV	EDI,DWORD [EBP+08H]
	JMP	SHORT	L_44
L_42:
;
; Line 1093:	++len;
;
	INC	DWORD [EBP-08H]
;
; Line 1092:	for (s = str;  *s != 0; s++)
;
L_43:
	ADD	EDI,BYTE 02H
L_44:
	MOVZX	EAX,WORD [EDI+00H]
	TEST	EAX,EAX
	JNE	SHORT	L_42
L_45:
L_40:
;
; Line 1095:	d = AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1096:	d->length = len * 2;
;
	MOV	EAX,DWORD [EBP-08H]
	SHL	EAX,01H
	MOV	DWORD [ESI+04H],EAX
;
; Line 1097:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1099:	if (str)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JE	SHORT	L_46
	MOV	EDI,DWORD [EBP+08H]
	MOV	DWORD [EBP-04H],00H
	JMP	SHORT	L_50
L_48:
;
; Line 1101:	put_16(be,  (unsigned char)*s, d->data + i * 2);
;
	MOV	EAX,DWORD [ESI+08H]
	MOV	ECX,DWORD [EBP-04H]
	SHL	ECX,01H
	ADD	EAX,ECX
	PUSH	EAX
	MOVZX	EAX,BYTE [EDI+00H]
	PUSH	EAX
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1100:	for (s = str, i = 0; i < len; s++, i++)
;
L_49:
	ADD	EDI,BYTE 02H
	INC	DWORD [EBP-04H]
L_50:
	MOV	EAX,DWORD [EBP-04H]
	CMP	EAX,DWORD [EBP-08H]
	JL	SHORT	L_48
L_51:
	JMP	SHORT	L_47
L_46:
;
; Line 1103:	put_16(be, 0, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
L_47:
;
; Line 1105:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1107:	return d;
;
	MOV	EAX,ESI
L_38:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_string_len:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 08H
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EDI,DWORD [EBP+0CH]
L_53:
;
; Line 1112:	static BINDATA *from_string_len(CHARACTER *str, int be, int len)
;
;
; Line 1113:	{
;
;
; Line 1118:	d = AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1119:	d->length = len * 2;
;
	MOV	EAX,DWORD [EBP+010H]
	SHL	EAX,01H
	MOV	DWORD [ESI+04H],EAX
;
; Line 1120:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1122:	if (str)
;
	MOV	EAX,DWORD [EBP+08H]
	TEST	EAX,EAX
	JE	SHORT	L_54
	MOV	EAX,DWORD [EBP+08H]
	MOV	DWORD [EBP-04H],EAX
	MOV	EBX,00H
	JMP	SHORT	L_58
L_56:
;
; Line 1124:	put_16(be,  (unsigned char)*s, d->data + i * 2);
;
	MOV	EAX,DWORD [ESI+08H]
	MOV	ECX,EBX
	SHL	ECX,01H
	ADD	EAX,ECX
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	MOVZX	EAX,BYTE [EAX+00H]
	PUSH	EAX
	PUSH	EDI
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1123:	for (s = str, i = 0; i < len; s++, i++)
;
L_57:
	ADD	DWORD [EBP-04H],BYTE 02H
	INC	EBX
L_58:
	MOV	EAX,EBX
	CMP	EAX,DWORD [EBP+010H]
	JL	SHORT	L_56
L_59:
	JMP	SHORT	L_55
L_54:
;
; Line 1126:	put_16(be, 0, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	BYTE 00H
	PUSH	EDI
	CALL	_put_16
	ADD	ESP,BYTE 0CH
L_55:
;
; Line 1128:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1130:	return d;
;
	MOV	EAX,ESI
L_52:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_ascii_to_string:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
L_61:
;
; Line 1135:	static BINDATA *ascii_to_string(char *s, int be)
;
;
; Line 1136:	{
;
;
; Line 1138:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1140:	len = strlen(s);
;
	PUSH	DWORD [EBP+08H]
	CALL	_strlen
	POP	ECX
	MOV	EDI,EAX
;
; Line 1142:	d->length = len * 2+2;
;
	MOV	EAX,EDI
	SHL	EAX,01H
	ADD	EAX,BYTE 02H
	MOV	DWORD [ESI+04H],EAX
;
; Line 1143:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
	MOV	EBX,00H
	JMP	SHORT	L_64
L_62:
;
; Line 1146:	put_16(be, (unsigned char)s[i], d->data + i * 2);
;
	MOV	EAX,EBX
	SHL	EAX,01H
	ADD	EAX,DWORD [ESI+08H]
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	MOVZX	EAX,BYTE [EBX+EAX+00H]
	PUSH	EAX
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1145:	for (i = 0; i < len + 1; i++)
;
L_63:
	INC	EBX
L_64:
	MOV	EAX,EBX
	MOV	ECX,EDI
	INC	ECX
	CMP	EAX,ECX
	JB	SHORT	L_62
L_65:
;
; Line 1148:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1150:	return d;
;
	MOV	EAX,ESI
L_60:
	POP	EDI
	POP	ESI
	POP	EBX
	POP	EBP
	RET
[GLOBAL	_extended_dialog]
_extended_dialog:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
	PUSH	EDI
	MOV	EDI,DWORD [EBP+08H]
L_67:
;
; Line 1155:	int extended_dialog(struct dialog *dialog)
;
;
; Line 1156:	{
;
;
; Line 1159:	if (dialog->ex != NULL)
;
	MOV	EAX,DWORD [EDI+03CH]
	TEST	EAX,EAX
	JE	SHORT	L_68
;
; Line 1160:	return 1;
;
	MOV	EAX,01H
	JMP	SHORT	L_66
L_68:
	MOV	ESI,DWORD [EDI+040H]
	JMP	SHORT	L_72
L_70:
;
; Line 1163:	if (c->data != NULL || c->help != 0)
;
	MOV	EAX,DWORD [ESI+038H]
	TEST	EAX,EAX
	JNE	SHORT	L_76
	MOV	EAX,DWORD [ESI+03CH]
	TEST	EAX,EAX
	JE	SHORT	L_74
L_76:
;
; Line 1164:	return 1;
;
	MOV	EAX,01H
	JMP	SHORT	L_66
L_74:
;
; Line 1162:	for (c = dialog->controls; c != NULL; c = c->link)
;
L_71:
	MOV	ESI,DWORD [ESI+00H]
L_72:
	MOV	EAX,ESI
	TEST	EAX,EAX
	JNE	SHORT	L_70
L_73:
;
; Line 1166:	return 0;
;
	MOV	EAX,00H
L_66:
	POP	EDI
	POP	ESI
	POP	EBP
	RET
_extended_menuitems:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
L_78:
;
; Line 1169:	static int extended_menuitems(struct menuitem *menuitems)
;
;
; Line 1170:	{
;
	MOV	ESI,DWORD [EBP+08H]
	JMP	SHORT	L_81
L_79:
;
; Line 1175:	if (mi->help != 0 || mi->state != 0 || mi->type != 0)
;
	MOV	EAX,DWORD [ESI+01CH]
	TEST	EAX,EAX
	JNE	SHORT	L_85
	MOV	EAX,DWORD [ESI+0CH]
	TEST	EAX,EAX
	JNE	SHORT	L_85
	MOV	EAX,DWORD [ESI+04H]
	TEST	EAX,EAX
	JE	SHORT	L_83
L_85:
;
; Line 1176:	return 1;
;
	MOV	EAX,01H
	JMP	SHORT	L_77
L_83:
;
; Line 1177:	if (mi->popup != NULL && mi->id != 0)
;
	MOV	EAX,DWORD [ESI+018H]
	TEST	EAX,EAX
	JE	SHORT	L_86
	MOV	EAX,DWORD [ESI+010H]
	TEST	EAX,EAX
	JE	SHORT	L_86
;
; Line 1178:	return 1;
;
	MOV	EAX,01H
	JMP	SHORT	L_77
L_86:
;
; Line 1182:	if (mi->popup != NULL)
;
	MOV	EAX,DWORD [ESI+018H]
	TEST	EAX,EAX
	JE	SHORT	L_88
;
; Line 1184:	if (extended_menuitems(mi->popup))
;
	PUSH	DWORD [ESI+018H]
	CALL	_extended_menuitems
	POP	ECX
	TEST	EAX,EAX
	JE	SHORT	L_90
;
; Line 1185:	return 1;
;
	MOV	EAX,01H
	JMP	SHORT	L_77
L_90:
L_88:
;
; Line 1173:	for (mi = menuitems; mi != NULL; mi = mi->link)
;
L_80:
	MOV	ESI,DWORD [ESI+00H]
L_81:
	MOV	EAX,ESI
	TEST	EAX,EAX
	JNE	SHORT	L_79
L_82:
;
; Line 1189:	return 0;
;
	MOV	EAX,00H
L_77:
	POP	ESI
	POP	EBP
	RET
[GLOBAL	_extended_menu]
_extended_menu:
	PUSH	EBP
	MOV	EBP,ESP
L_93:
;
; Line 1194:	int extended_menu(struct menu *menu)
;
;
; Line 1195:	{
;
;
; Line 1196:	return extended_menuitems(menu->items);
;
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+00H]
	CALL	_extended_menuitems
	POP	ECX
L_92:
	POP	EBP
	RET
_from_rcdata:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 0CH
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
L_96:
;
; Line 1200:	static BINDATA *from_rcdata(struct rcdata_item *items, int be)
;
;
; Line 1201:	{
;
;
; Line 1202:	BINDATA *first = NULL,  **pp = &first;
;
	MOV	DWORD [EBP-08H],00H
	LEA	EAX,[EBP-08H]
	MOV	DWORD [EBP-04H],EAX
	MOV	EDI,DWORD [EBP+08H]
	JMP	NEAR	L_99
L_97:
;
; Line 1207:	BINDATA *d = AllocateMemory(sizeof(*d));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1208:	switch (ri->type)
;
	MOV	EAX,DWORD [EDI+04H]
	TEST	EAX,EAX
	JL	SHORT	L_102
	CMP	EAX,BYTE 05H
	JGE	SHORT	L_102
	JMP	DWORD [EAX*4+L_103]
L_103:
	DD	L_104
	DD	L_105
	DD	L_106
	DD	L_107
	DD	L_108
;
; Line 1210:	default:
;
L_102:
;
; Line 1211:	fatal("invalid RCDATA sect");
;
	PUSH	DWORD L_94
	CALL	_fatal
	POP	ECX
;
; Line 1213:	case RCDATA_WORD:
;
L_104:
;
; Line 1214:	d->length = 2;
;
	MOV	DWORD [ESI+04H],02H
;
; Line 1215:	d->data = AllocateMemory(2);
;
	PUSH	BYTE 02H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1216:	put_16(be, ri->u.word, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EDI+08H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1217:	break;
;
	JMP	NEAR	L_101
L_105:
;
; Line 1220:	d->length = 4;
;
	MOV	DWORD [ESI+04H],04H
;
; Line 1221:	d->data = AllocateMemory(4);
;
	PUSH	BYTE 04H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1222:	put_32(be, ri->u.dword, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EDI+08H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1223:	break;
;
	JMP	SHORT	L_101
L_106:
;
; Line 1226:	d->length = ri->u.string.length;
;
	MOV	EAX,DWORD [EDI+08H]
	MOV	DWORD [ESI+04H],EAX
;
; Line 1227:	d->data = (unsigned char*)ri->u.string.s;
;
	MOV	EAX,DWORD [EDI+0CH]
	MOV	DWORD [ESI+08H],EAX
;
; Line 1228:	break;
;
	JMP	SHORT	L_101
L_107:
;
; Line 1234:	d->length = ri->u.wstring.length *2;
;
	MOV	EAX,DWORD [EDI+08H]
	SHL	EAX,01H
	MOV	DWORD [ESI+04H],EAX
;
; Line 1235:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
	MOV	EBX,00H
	JMP	SHORT	L_111
L_109:
;
; Line 1237:	put_16(be, ri->u.wstring.w[i], d->data + i * 2);
;
	MOV	EAX,EBX
	SHL	EAX,01H
	ADD	EAX,DWORD [ESI+08H]
	PUSH	EAX
	MOV	EAX,DWORD [EDI+0CH]
	MOVZX	EAX,WORD [EAX+EBX*2+00H]
	PUSH	EAX
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1236:	for (i = 0; i < ri->u.wstring.length; i++)
;
L_110:
	INC	EBX
L_111:
	MOV	EAX,EBX
	CMP	EAX,DWORD [EDI+08H]
	JB	SHORT	L_109
L_112:
;
; Line 1238:	break;
;
	JMP	SHORT	L_101
L_108:
;
; Line 1242:	d->length = ri->u.buffer.length;
;
	MOV	EAX,DWORD [EDI+08H]
	MOV	DWORD [ESI+04H],EAX
;
; Line 1243:	d->data = (unsigned char*)ri->u.buffer.data;
;
	MOV	EAX,DWORD [EDI+0CH]
	MOV	DWORD [ESI+08H],EAX
;
; Line 1244:	break;
;
	JMP	SHORT	L_101
L_101:
;
; Line 1247:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1248:	*pp = d;
;
	MOV	EAX,DWORD [EBP-04H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1249:	pp = &d->link;
;
	MOV	DWORD [EBP-04H],ESI
;
; Line 1205:	for (ri = items; ri != NULL; ri = ri->link)
;
L_98:
	MOV	EDI,DWORD [EDI+00H]
L_99:
	MOV	EAX,EDI
	TEST	EAX,EAX
	JNE	NEAR	L_97
L_100:
;
; Line 1252:	return first;
;
	MOV	EAX,DWORD [EBP-08H]
L_95:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_accelerator:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 0CH
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EBX,DWORD [EBP+0CH]
L_114:
;
; Line 1257:	static BINDATA *from_accelerator(struct accelerator *acc, int be)
;
;
; Line 1258:	{
;
;
; Line 1259:	BINDATA *first = NULL,  **pp = &first;
;
	MOV	DWORD [EBP-08H],00H
	LEA	EAX,[EBP-08H]
	MOV	DWORD [EBP-04H],EAX
	MOV	EDI,DWORD [EBP+08H]
	JMP	NEAR	L_117
L_115:
;
; Line 1263:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1264:	d->length = 8;
;
	MOV	DWORD [ESI+04H],08H
;
; Line 1265:	d->data = AllocateMemory(8);
;
	PUSH	BYTE 08H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1267:	put_16(be, a->flags | (a->link != NULL ? 0 : ACC_LAST), d->data);
;
	PUSH	DWORD [ESI+08H]
	MOV	EAX,DWORD [EDI+00H]
	TEST	EAX,EAX
	JE	SHORT	L_119
	MOV	EAX,00H
	JMP	SHORT	L_120
L_119:
	MOV	EAX,080H
L_120:
	OR	EAX,DWORD [EDI+04H]
	PUSH	EAX
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1268:	put_16(be, a->key, d->data + 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	DWORD [EDI+08H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1269:	put_16(be, a->id, d->data + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD [EDI+0CH]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1270:	put_16(be, 0, d->data + 6);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 06H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1272:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1273:	*pp = d;
;
	MOV	EAX,DWORD [EBP-04H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1274:	pp = &d->link;
;
	MOV	DWORD [EBP-04H],ESI
;
; Line 1261:	for (a = acc; a != NULL; a = a->link)
;
L_116:
	MOV	EDI,DWORD [EDI+00H]
L_117:
	MOV	EAX,EDI
	TEST	EAX,EAX
	JNE	NEAR	L_115
L_118:
;
; Line 1277:	return first;
;
	MOV	EAX,DWORD [EBP-08H]
L_113:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_cursor:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EDI,DWORD [EBP+08H]
	MOV	EBX,DWORD [EBP+0CH]
L_122:
;
; Line 1282:	static BINDATA *from_cursor(struct cursor *c, int be)
;
;
; Line 1283:	{
;
;
; Line 1284:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1285:	d->length = 4;
;
	MOV	DWORD [ESI+04H],04H
;
; Line 1286:	d->data = AllocateMemory(4);
;
	PUSH	BYTE 04H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1288:	put_16(be, c->xhotspot, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EDI+00H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1289:	put_16(be, c->yhotspot, d->data + 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	DWORD [EDI+04H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1291:	d->link = AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+00H],EAX
;
; Line 1292:	d->link->length = c->length;
;
	MOV	EAX,DWORD [EDI+08H]
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+04H],EAX
;
; Line 1293:	d->link->data = (unsigned char*)c->data;
;
	MOV	EAX,DWORD [EDI+0CH]
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1294:	d->link->link = NULL;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	DWORD [EAX+00H],00H
;
; Line 1296:	return d;
;
	MOV	EAX,ESI
L_121:
	POP	EDI
	POP	ESI
	POP	EBX
	POP	EBP
	RET
_from_dialog:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 024H
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EBX,DWORD [EBP+0CH]
L_124:
;
; Line 1299:	static BINDATA *from_dialog(struct dialog *dialog, int be)
;
;
; Line 1300:	{
;
;
; Line 1302:	BINDATA *first = AllocateMemory(sizeof(BINDATA)),  **pp = &first->link;
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-018H],EAX
	MOV	EAX,DWORD [EBP-018H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1307:	dialogex = extended_dialog(dialog);
;
	PUSH	DWORD [EBP+08H]
	CALL	_extended_dialog
	POP	ECX
	MOV	DWORD [EBP-01CH],EAX
;
; Line 1309:	first->length = dialogex ? 26 : 18;
;
	MOV	EAX,DWORD [EBP-01CH]
	TEST	EAX,EAX
	JE	SHORT	L_125
	MOV	EAX,01AH
	JMP	SHORT	L_126
L_125:
	MOV	EAX,012H
L_126:
	MOV	ECX,DWORD [EBP-018H]
	MOV	DWORD [ECX+04H],EAX
;
; Line 1310:	first->data = AllocateMemory(first->length);
;
	MOV	EAX,DWORD [EBP-018H]
	PUSH	DWORD [EAX+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-018H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1311:	first->link = NULL;
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	DWORD [EAX+00H],00H
;
; Line 1313:	length = first->length;
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+04H]
	MOV	DWORD [EBP-010H],EAX
;
; Line 1315:	if (!dialogex)
;
	MOV	EAX,DWORD [EBP-01CH]
	TEST	EAX,EAX
	JNE	SHORT	L_127
;
; Line 1317:	put_32(be, dialog->style, first->data);
;
	MOV	EAX,DWORD [EBP-018H]
	PUSH	DWORD [EAX+08H]
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+00H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1318:	put_32(be, dialog->exstyle, first->data + 4);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+04H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1319:	off = 8;
;
	MOV	DWORD [EBP-0CH],08H
	JMP	NEAR	L_128
L_127:
;
; Line 1323:	put_16(be, 1, first->data);
;
	MOV	EAX,DWORD [EBP-018H]
	PUSH	DWORD [EAX+08H]
	PUSH	BYTE 01H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1324:	put_16(be, 0xffff, first->data+2);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	DWORD 0FFFFH
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1325:	if (dialog->ex == NULL)
;
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+03CH]
	TEST	EAX,EAX
	JNE	SHORT	L_129
;
; Line 1326:	put_32(be, 0, first->data + 4);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
	JMP	SHORT	L_130
L_129:
;
; Line 1328:	put_32(be, dialog->ex->help, first->data + 4);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+03CH]
	PUSH	DWORD [EAX+00H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
L_130:
;
; Line 1329:	put_32(be, dialog->exstyle, first->data + 8);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+04H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1330:	put_32(be, dialog->style, first->data + 12);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 0CH
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+00H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1331:	off = 16;
;
	MOV	DWORD [EBP-0CH],010H
L_128:
;
; Line 1333:	put_16(be, dialog->x, first->data + off + 2);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,DWORD [EBP-0CH]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+08H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1334:	put_16(be, dialog->y, first->data + off + 4);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,DWORD [EBP-0CH]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+0CH]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1335:	put_16(be, dialog->width, first->data + off + 6);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,DWORD [EBP-0CH]
	ADD	EAX,BYTE 06H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+010H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1336:	put_16(be, dialog->height, first->data + off + 8);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,DWORD [EBP-0CH]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+014H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1338:	*pp = from_resid(dialog->menu, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+018H+08H]
	PUSH	DWORD [EAX+018H+04H]
	PUSH	DWORD [EAX+018H+00H]
	CALL	_from_resid
	ADD	ESP,BYTE 010H
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1339:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1340:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1342:	*pp = from_resid(dialog->class , be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+024H+08H]
	PUSH	DWORD [EAX+024H+04H]
	PUSH	DWORD [EAX+024H+00H]
	CALL	_from_resid
	ADD	ESP,BYTE 010H
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1343:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1344:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1346:	*pp = from_string(dialog->caption, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+030H]
	CALL	_from_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1347:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1348:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1350:	if ((dialog->style &DS_SETFONT) != 0)
;
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+00H]
	AND	EAX,BYTE 040H
	JE	NEAR	L_131
;
; Line 1352:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-024H],EAX
;
; Line 1354:	d->length = dialogex ? 6 : 2;
;
	MOV	EAX,DWORD [EBP-01CH]
	TEST	EAX,EAX
	JE	SHORT	L_133
	MOV	EAX,06H
	JMP	SHORT	L_134
L_133:
	MOV	EAX,02H
L_134:
	MOV	ECX,DWORD [EBP-024H]
	MOV	DWORD [ECX+04H],EAX
;
; Line 1355:	d->data = AllocateMemory(d->length);
;
	MOV	EAX,DWORD [EBP-024H]
	PUSH	DWORD [EAX+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-024H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1357:	length += d->length;
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1359:	put_16(be, dialog->pointsize, d->data);
;
	MOV	EAX,DWORD [EBP-024H]
	PUSH	DWORD [EAX+08H]
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+034H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1361:	if (dialogex)
;
	MOV	EAX,DWORD [EBP-01CH]
	TEST	EAX,EAX
	JE	SHORT	L_135
;
; Line 1362:	if (dialog->ex == NULL)
;
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+03CH]
	TEST	EAX,EAX
	JNE	SHORT	L_137
;
; Line 1364:	put_16(be, 0, d->data + 2);
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1365:	put_16(be, 0, d->data + 4);
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
	JMP	SHORT	L_138
L_137:
;
; Line 1369:	put_16(be, dialog->ex->weight, d->data + 2);
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+03CH]
	PUSH	DWORD [EAX+04H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1370:	put_16(be, dialog->ex->italic, d->data + 4);
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+03CH]
	PUSH	DWORD [EAX+08H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
L_138:
L_135:
;
; Line 1373:	*pp = d;
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1374:	pp = &d->link;
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1376:	*pp = from_string(dialog->font, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP+08H]
	PUSH	DWORD [EAX+038H]
	CALL	_from_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1377:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1378:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-014H],EAX
L_131:
;
; Line 1381:	c = 0;
;
	MOV	DWORD [EBP-08H],00H
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+040H]
	MOV	DWORD [EBP-04H],EAX
	JMP	NEAR	L_141
L_139:
;
; Line 1384:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1387:	++c;
;
	INC	DWORD [EBP-08H]
;
; Line 1389:	align(&pp, &length);
;
	LEA	EAX,[EBP-010H]
	PUSH	EAX
	LEA	EAX,[EBP-014H]
	PUSH	EAX
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1391:	d->length = dialogex ? 24 : 18;
;
	MOV	EAX,DWORD [EBP-01CH]
	TEST	EAX,EAX
	JE	SHORT	L_143
	MOV	EAX,018H
	JMP	SHORT	L_144
L_143:
	MOV	EAX,012H
L_144:
	MOV	DWORD [ESI+04H],EAX
;
; Line 1392:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1394:	length += d->length;
;
	MOV	EAX,DWORD [ESI+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1396:	if (!dialogex)
;
	MOV	EAX,DWORD [EBP-01CH]
	TEST	EAX,EAX
	JNE	SHORT	L_145
;
; Line 1398:	put_32(be, dc->style, d->data);
;
	PUSH	DWORD [ESI+08H]
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+08H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1399:	put_32(be, dc->exstyle, d->data + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+0CH]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1400:	dcoff = 8;
;
	MOV	DWORD [EBP-024H],08H
	JMP	SHORT	L_146
L_145:
;
; Line 1404:	put_32(be, dc->help, d->data);
;
	PUSH	DWORD [ESI+08H]
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+03CH]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1405:	put_32(be, dc->exstyle, d->data + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+0CH]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1406:	put_32(be, dc->style, d->data + 8);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+08H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1407:	dcoff = 12;
;
	MOV	DWORD [EBP-024H],0CH
L_146:
;
; Line 1410:	put_16(be, dc->x, d->data + dcoff);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,DWORD [EBP-024H]
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+010H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1411:	put_16(be, dc->y, d->data + dcoff + 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,DWORD [EBP-024H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+014H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1412:	put_16(be, dc->width, d->data + dcoff + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,DWORD [EBP-024H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+018H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1413:	put_16(be, dc->height, d->data + dcoff + 6);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,DWORD [EBP-024H]
	ADD	EAX,BYTE 06H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+01CH]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1414:	if (dialogex)
;
	MOV	EAX,DWORD [EBP-01CH]
	TEST	EAX,EAX
	JE	SHORT	L_147
;
; Line 1415:	put_32(be, dc->id, d->data + dcoff + 8);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,DWORD [EBP-024H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+04H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
	JMP	SHORT	L_148
L_147:
;
; Line 1417:	put_16(be, dc->id, d->data + dcoff + 8);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,DWORD [EBP-024H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+04H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
L_148:
;
; Line 1419:	*pp = d;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1420:	pp = &d->link;
;
	MOV	DWORD [EBP-014H],ESI
;
; Line 1422:	*pp = from_resid(dc->class , be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+020H+08H]
	PUSH	DWORD [EAX+020H+04H]
	PUSH	DWORD [EAX+020H+00H]
	CALL	_from_resid
	ADD	ESP,BYTE 010H
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1423:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1424:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1426:	*pp = from_resid(dc->text, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+02CH+08H]
	PUSH	DWORD [EAX+02CH+04H]
	PUSH	DWORD [EAX+02CH+00H]
	CALL	_from_resid
	ADD	ESP,BYTE 010H
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1427:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-010H],EAX
;
; Line 1428:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1430:	d = AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1431:	d->length = 2;
;
	MOV	DWORD [ESI+04H],02H
;
; Line 1432:	d->data = AllocateMemory(2);
;
	PUSH	BYTE 02H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1434:	length += 2;
;
	ADD	DWORD [EBP-010H],BYTE 02H
;
; Line 1436:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1437:	*pp = d;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1438:	pp = &d->link;
;
	MOV	DWORD [EBP-014H],ESI
;
; Line 1440:	if (dc->data == NULL)
;
	MOV	EAX,DWORD [EBP-04H]
	MOV	EAX,DWORD [EAX+038H]
	TEST	EAX,EAX
	JNE	SHORT	L_149
;
; Line 1441:	put_16(be, 0, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
	JMP	SHORT	L_150
L_149:
;
; Line 1446:	align(&pp, &length);
;
	LEA	EAX,[EBP-010H]
	PUSH	EAX
	LEA	EAX,[EBP-014H]
	PUSH	EAX
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1448:	*pp = from_rcdata(dc->data, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+038H]
	CALL	_from_rcdata
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [EBP-014H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1449:	i = 0;
;
	MOV	EDI,00H
	JMP	SHORT	L_151
L_153:
;
; Line 1452:	i += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	EDI,EAX
;
; Line 1453:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-014H],EAX
L_151:
;
; Line 1450:	while (*pp != NULL)
;
	MOV	EAX,DWORD [EBP-014H]
	MOV	EAX,DWORD [EAX+00H]
	TEST	EAX,EAX
	JNE	SHORT	L_153
L_152:
;
; Line 1456:	put_16(be, i, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	EDI
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1458:	length += i;
;
	ADD	DWORD [EBP-010H],EDI
L_150:
;
; Line 1382:	for (dc = dialog->controls; dc != NULL; dc = dc->link)
;
L_140:
	MOV	EAX,DWORD [EBP-04H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-04H],EAX
L_141:
	MOV	EAX,DWORD [EBP-04H]
	TEST	EAX,EAX
	JNE	NEAR	L_139
L_142:
;
; Line 1462:	put_16(be, c, first->data + off);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,DWORD [EBP-0CH]
	PUSH	EAX
	PUSH	DWORD [EBP-08H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1464:	return first;
;
	MOV	EAX,DWORD [EBP-018H]
L_123:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_fontdir:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 010H
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EBX,DWORD [EBP+0CH]
L_155:
;
; Line 1469:	static BINDATA *from_fontdir(struct fontdir *fontdirs, int be)
;
;
; Line 1470:	{
;
;
; Line 1471:	BINDATA *first = AllocateMemory(sizeof(BINDATA)),  **pp = &first->link;
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-0CH],EAX
	MOV	EAX,DWORD [EBP-0CH]
	MOV	DWORD [EBP-08H],EAX
;
; Line 1475:	first->length = 2;
;
	MOV	EAX,DWORD [EBP-0CH]
	MOV	DWORD [EAX+04H],02H
;
; Line 1476:	first->data = AllocateMemory(2);
;
	PUSH	BYTE 02H
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-0CH]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1477:	first->link = NULL;
;
	MOV	EAX,DWORD [EBP-0CH]
	MOV	DWORD [EAX+00H],00H
;
; Line 1479:	c = 0;
;
	MOV	DWORD [EBP-04H],00H
	MOV	EDI,DWORD [EBP+08H]
	JMP	SHORT	L_158
L_156:
;
; Line 1484:	++c;
;
	INC	DWORD [EBP-04H]
;
; Line 1486:	d = AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1487:	d->length = 2;
;
	MOV	DWORD [ESI+04H],02H
;
; Line 1488:	d->data = AllocateMemory(2);
;
	PUSH	BYTE 02H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1490:	put_16(be, fd->index, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EDI+04H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1492:	*pp = d;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1493:	pp = &d->link;
;
	MOV	DWORD [EBP-08H],ESI
;
; Line 1495:	d = AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1496:	d->length = fd->length;
;
	MOV	EAX,DWORD [EDI+08H]
	MOV	DWORD [ESI+04H],EAX
;
; Line 1497:	d->data = (unsigned char*)fd->data;
;
	MOV	EAX,DWORD [EDI+0CH]
	MOV	DWORD [ESI+08H],EAX
;
; Line 1499:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1500:	*pp = d;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1501:	pp = &d->link;
;
	MOV	DWORD [EBP-08H],ESI
;
; Line 1480:	for (fd = fontdirs; fd != NULL; fd = fd->link)
;
L_157:
	MOV	EDI,DWORD [EDI+00H]
L_158:
	MOV	EAX,EDI
	TEST	EAX,EAX
	JNE	SHORT	L_156
L_159:
;
; Line 1504:	put_16(be, c, first->data);
;
	MOV	EAX,DWORD [EBP-0CH]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD [EBP-04H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1506:	return first;
;
	MOV	EAX,DWORD [EBP-0CH]
L_154:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_generic:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	ESI
L_161:
;
; Line 1511:	static BINDATA *from_generic(unsigned long length, unsigned char *data)
;
;
; Line 1512:	{
;
;
; Line 1513:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1514:	d->length = length;
;
	MOV	EAX,DWORD [EBP+08H]
	MOV	DWORD [ESI+04H],EAX
;
; Line 1515:	d->data = (unsigned char*)data;
;
	MOV	EAX,DWORD [EBP+0CH]
	MOV	DWORD [ESI+08H],EAX
;
; Line 1517:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1519:	return d;
;
	MOV	EAX,ESI
L_160:
	POP	ESI
	POP	EBP
	RET
_from_group_cursor:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 0CH
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
L_163:
;
; Line 1524:	static BINDATA *from_group_cursor(struct group_cursor *gcs, int be)
;
;
; Line 1525:	{
;
;
; Line 1526:	BINDATA *first = AllocateMemory(sizeof(BINDATA)),  **pp = &first->link;
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	EBX,EAX
	MOV	DWORD [EBP-08H],EBX
;
; Line 1530:	first->length = 6;
;
	MOV	DWORD [EBX+04H],06H
;
; Line 1531:	first->data = AllocateMemory(6);
;
	PUSH	BYTE 06H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBX+08H],EAX
;
; Line 1532:	first->link = NULL;
;
	MOV	DWORD [EBX+00H],00H
;
; Line 1533:	pp = &first->link;
;
	MOV	DWORD [EBP-08H],EBX
;
; Line 1535:	put_16(be, 0, first->data);
;
	PUSH	DWORD [EBX+08H]
	PUSH	BYTE 00H
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1536:	put_16(be, 2, first->data + 2);
;
	MOV	EAX,DWORD [EBX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 02H
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1538:	c = 0;
;
	MOV	DWORD [EBP-04H],00H
	MOV	EDI,DWORD [EBP+08H]
	JMP	NEAR	L_166
L_164:
;
; Line 1541:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1543:	++c;
;
	INC	DWORD [EBP-04H]
;
; Line 1545:	d->length = 14;
;
	MOV	DWORD [ESI+04H],0EH
;
; Line 1546:	d->data = AllocateMemory(14);
;
	PUSH	BYTE 0EH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1548:	put_16(be, gc->width, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EDI+04H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1549:	put_16(be, gc->height, d->data + 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	DWORD [EDI+08H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1550:	put_16(be, gc->planes, d->data + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD [EDI+0CH]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1551:	put_16(be, gc->bits, d->data + 6);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 06H
	PUSH	EAX
	PUSH	DWORD [EDI+010H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1552:	put_32(be, gc->bytes, d->data + 8);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	PUSH	DWORD [EDI+014H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1553:	put_16(be, gc->index, d->data + 12);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 0CH
	PUSH	EAX
	PUSH	DWORD [EDI+018H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1555:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1556:	*pp = d;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1557:	pp = &d->link;
;
	MOV	DWORD [EBP-08H],ESI
;
; Line 1539:	for (gc = gcs; gc != NULL; gc = gc->link)
;
L_165:
	MOV	EDI,DWORD [EDI+00H]
L_166:
	MOV	EAX,EDI
	TEST	EAX,EAX
	JNE	NEAR	L_164
L_167:
;
; Line 1560:	put_16(be, c, first->data + 4);
;
	MOV	EAX,DWORD [EBX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD [EBP-04H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1562:	return first;
;
	MOV	EAX,EBX
L_162:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_group_icon:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 0CH
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
L_169:
;
; Line 1567:	static BINDATA *from_group_icon(struct group_icon *group_icons, int be)
;
;
; Line 1568:	{
;
;
; Line 1569:	BINDATA *first = AllocateMemory(sizeof(BINDATA)),  **pp = &first->link;
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	EBX,EAX
	MOV	DWORD [EBP-08H],EBX
;
; Line 1573:	first->length = 6;
;
	MOV	DWORD [EBX+04H],06H
;
; Line 1574:	first->data = AllocateMemory(6);
;
	PUSH	BYTE 06H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBX+08H],EAX
;
; Line 1575:	first->link = NULL;
;
	MOV	DWORD [EBX+00H],00H
;
; Line 1577:	put_16(be, 0, first->data);
;
	PUSH	DWORD [EBX+08H]
	PUSH	BYTE 00H
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1578:	put_16(be, 1, first->data + 2);
;
	MOV	EAX,DWORD [EBX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 01H
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1580:	c = 0;
;
	MOV	DWORD [EBP-04H],00H
	MOV	EDI,DWORD [EBP+08H]
	JMP	NEAR	L_172
L_170:
;
; Line 1583:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1585:	++c;
;
	INC	DWORD [EBP-04H]
;
; Line 1587:	d->length = 14;
;
	MOV	DWORD [ESI+04H],0EH
;
; Line 1588:	d->data = AllocateMemory(14);
;
	PUSH	BYTE 0EH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1590:	d->data[0] = gi->width;
;
	MOV	AL,BYTE [EDI+04H]
	MOV	ECX,DWORD [ESI+08H]
	MOV	BYTE [ECX+00H],AL
;
; Line 1591:	d->data[1] = gi->height;
;
	MOV	AL,BYTE [EDI+05H]
	MOV	ECX,DWORD [ESI+08H]
	MOV	BYTE [ECX+01H],AL
;
; Line 1592:	d->data[2] = gi->colors;
;
	MOV	AL,BYTE [EDI+06H]
	MOV	ECX,DWORD [ESI+08H]
	MOV	BYTE [ECX+02H],AL
;
; Line 1593:	d->data[3] = 0;
;
	MOV	EAX,DWORD [ESI+08H]
	MOV	BYTE [EAX+03H],00H
;
; Line 1594:	put_16(be, gi->planes, d->data + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD [EDI+08H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1595:	put_16(be, gi->bits, d->data + 6);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 06H
	PUSH	EAX
	PUSH	DWORD [EDI+0CH]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1596:	put_32(be, gi->bytes, d->data + 8);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	PUSH	DWORD [EDI+010H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1597:	put_16(be, gi->index, d->data + 12);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 0CH
	PUSH	EAX
	PUSH	DWORD [EDI+014H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1599:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1600:	*pp = d;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1601:	pp = &d->link;
;
	MOV	DWORD [EBP-08H],ESI
;
; Line 1581:	for (gi = group_icons; gi != NULL; gi = gi->link)
;
L_171:
	MOV	EDI,DWORD [EDI+00H]
L_172:
	MOV	EAX,EDI
	TEST	EAX,EAX
	JNE	NEAR	L_170
L_173:
;
; Line 1604:	put_16(be, c, first->data + 4);
;
	MOV	EAX,DWORD [EBX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD [EBP-04H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1606:	return first;
;
	MOV	EAX,EBX
L_168:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_menuitems:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 010H
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EBX,DWORD [EBP+0CH]
L_175:
;
; Line 1611:	static BINDATA *from_menuitems(struct menuitem *items, int be)
;
;
; Line 1612:	{
;
;
; Line 1613:	BINDATA *first = NULL,  **pp = &first;
;
	MOV	DWORD [EBP-04H],00H
	LEA	EAX,[EBP-04H]
	MOV	EDI,EAX
	MOV	ESI,DWORD [EBP+08H]
	JMP	NEAR	L_178
L_176:
;
; Line 1618:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-010H],EAX
;
; Line 1621:	d->length = mi->popup == NULL ? 4 : 2;
;
	MOV	EAX,DWORD [ESI+018H]
	TEST	EAX,EAX
	JNE	SHORT	L_180
	MOV	EAX,04H
	JMP	SHORT	L_181
L_180:
	MOV	EAX,02H
L_181:
	MOV	ECX,DWORD [EBP-010H]
	MOV	DWORD [ECX+04H],EAX
;
; Line 1622:	d->data = AllocateMemory(d->length);
;
	MOV	EAX,DWORD [EBP-010H]
	PUSH	DWORD [EAX+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-010H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1624:	flags = mi->flags;
;
	MOV	EAX,DWORD [ESI+08H]
	MOV	DWORD [EBP-0CH],EAX
;
; Line 1625:	if (mi->link == NULL)
;
	MOV	EAX,DWORD [ESI+00H]
	TEST	EAX,EAX
	JNE	SHORT	L_182
;
; Line 1626:	flags |= MI_ENDMENU;
;
	OR	DWORD [EBP-0CH],080H
L_182:
;
; Line 1627:	if (mi->popup != NULL)
;
	MOV	EAX,DWORD [ESI+018H]
	TEST	EAX,EAX
	JE	SHORT	L_184
;
; Line 1628:	flags |= MI_POPUP;
;
	OR	DWORD [EBP-0CH],BYTE 010H
L_184:
;
; Line 1630:	put_16(be, flags, d->data);
;
	MOV	EAX,DWORD [EBP-010H]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD [EBP-0CH]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1632:	if (mi->popup == NULL)
;
	MOV	EAX,DWORD [ESI+018H]
	TEST	EAX,EAX
	JNE	SHORT	L_186
;
; Line 1633:	put_16(be, mi->id, d->data + 2);
;
	MOV	EAX,DWORD [EBP-010H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	DWORD [ESI+010H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
L_186:
;
; Line 1635:	*pp = d;
;
	MOV	EAX,DWORD [EBP-010H]
	MOV	DWORD [EDI+00H],EAX
;
; Line 1636:	pp = &d->link;
;
	MOV	EDI,DWORD [EBP-010H]
;
; Line 1638:	*pp = from_string(mi->text, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+014H]
	CALL	_from_string
	ADD	ESP,BYTE 08H
	MOV	DWORD [EDI+00H],EAX
;
; Line 1639:	pp = &(*pp)->link;
;
	MOV	EDI,DWORD [EDI+00H]
;
; Line 1641:	if (mi->popup != NULL)
;
	MOV	EAX,DWORD [ESI+018H]
	TEST	EAX,EAX
	JE	SHORT	L_188
;
; Line 1643:	*pp = from_menuitems(mi->popup, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+018H]
	CALL	_from_menuitems
	ADD	ESP,BYTE 08H
	MOV	DWORD [EDI+00H],EAX
	JMP	SHORT	L_190
L_192:
;
; Line 1645:	pp = &(*pp)->link;
;
	MOV	EDI,DWORD [EDI+00H]
L_190:
;
; Line 1644:	while (*pp != NULL)
;
	MOV	EAX,DWORD [EDI+00H]
	TEST	EAX,EAX
	JNE	SHORT	L_192
L_191:
L_188:
;
; Line 1616:	for (mi = items; mi != NULL; mi = mi->link)
;
L_177:
	MOV	ESI,DWORD [ESI+00H]
L_178:
	MOV	EAX,ESI
	TEST	EAX,EAX
	JNE	NEAR	L_176
L_179:
;
; Line 1649:	*pp = NULL;
;
	MOV	DWORD [EDI+00H],00H
;
; Line 1650:	return first;
;
	MOV	EAX,DWORD [EBP-04H]
L_174:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_menuexitems:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 010H
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
L_194:
;
; Line 1655:	static BINDATA *from_menuexitems(struct menuitem *items, int be)
;
;
; Line 1656:	{
;
;
; Line 1657:	BINDATA *first = NULL,  **pp = &first;
;
	MOV	DWORD [EBP-0CH],00H
	LEA	EAX,[EBP-0CH]
	MOV	DWORD [EBP-08H],EAX
;
; Line 1658:	unsigned long length = 0;
;
	MOV	DWORD [EBP-04H],00H
	MOV	EDI,DWORD [EBP+08H]
	JMP	NEAR	L_197
L_195:
;
; Line 1663:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1666:	align(&pp, &length);
;
	LEA	EAX,[EBP-04H]
	PUSH	EAX
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1668:	d->length = 14;
;
	MOV	DWORD [ESI+04H],0EH
;
; Line 1669:	d->data = AllocateMemory(14);
;
	PUSH	BYTE 0EH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1671:	length += 14;
;
	ADD	DWORD [EBP-04H],BYTE 0EH
;
; Line 1673:	put_32(be, mi->type, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EDI+04H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1674:	put_32(be, mi->state, d->data + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD [EDI+0CH]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1675:	put_32(be, mi->id, d->data + 8);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	PUSH	DWORD [EDI+010H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1677:	flags = 0;
;
	MOV	EBX,00H
;
; Line 1678:	if (mi->link == NULL)
;
	MOV	EAX,DWORD [EDI+00H]
	TEST	EAX,EAX
	JNE	SHORT	L_199
;
; Line 1679:	flags |= 0x80;
;
	OR	EBX,080H
L_199:
;
; Line 1680:	if (mi->popup != NULL)
;
	MOV	EAX,DWORD [EDI+018H]
	TEST	EAX,EAX
	JE	SHORT	L_201
;
; Line 1681:	flags |= 1;
;
	OR	EBX,BYTE 01H
L_201:
;
; Line 1682:	put_16(be, flags, d->data + 12);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 0CH
	PUSH	EAX
	PUSH	EBX
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1684:	*pp = d;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1685:	pp = &d->link;
;
	MOV	DWORD [EBP-08H],ESI
;
; Line 1687:	*pp = from_string(mi->text, be);
;
	PUSH	DWORD [EBP+0CH]
	PUSH	DWORD [EDI+014H]
	CALL	_from_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [EBP-08H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1688:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-04H],EAX
;
; Line 1689:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-08H],EAX
;
; Line 1691:	if (mi->popup != NULL)
;
	MOV	EAX,DWORD [EDI+018H]
	TEST	EAX,EAX
	JE	SHORT	L_203
;
; Line 1693:	align(&pp, &length);
;
	LEA	EAX,[EBP-04H]
	PUSH	EAX
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1695:	d = AllocateMemory(sizeof *d);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1696:	d->length = 4;
;
	MOV	DWORD [ESI+04H],04H
;
; Line 1697:	d->data = AllocateMemory(4);
;
	PUSH	BYTE 04H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1699:	put_32(be, mi->help, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EDI+01CH]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1701:	*pp = d;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1702:	pp = &d->link;
;
	MOV	DWORD [EBP-08H],ESI
;
; Line 1704:	*pp = from_menuexitems(mi->popup, be);
;
	PUSH	DWORD [EBP+0CH]
	PUSH	DWORD [EDI+018H]
	CALL	_from_menuexitems
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [EBP-08H]
	MOV	DWORD [ECX+00H],EAX
	JMP	SHORT	L_205
L_207:
;
; Line 1707:	length += (*pp)->length;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-04H],EAX
;
; Line 1708:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-08H],EAX
L_205:
;
; Line 1705:	while (*pp != NULL)
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	EAX,DWORD [EAX+00H]
	TEST	EAX,EAX
	JNE	SHORT	L_207
L_206:
L_203:
;
; Line 1661:	for (mi = items; mi != NULL; mi = mi->link)
;
L_196:
	MOV	EDI,DWORD [EDI+00H]
L_197:
	MOV	EAX,EDI
	TEST	EAX,EAX
	JNE	NEAR	L_195
L_198:
;
; Line 1713:	*pp = NULL;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EAX+00H],00H
;
; Line 1714:	return first;
;
	MOV	EAX,DWORD [EBP-0CH]
L_193:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_menu:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 08H
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EBX,DWORD [EBP+0CH]
	MOV	EDI,DWORD [EBP+08H]
L_209:
;
; Line 1719:	static BINDATA *from_menu(struct menu *menu, int be)
;
;
; Line 1720:	{
;
;
; Line 1722:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1724:	menuex = extended_menu(menu);
;
	PUSH	EDI
	CALL	_extended_menu
	POP	ECX
	MOV	DWORD [EBP-04H],EAX
;
; Line 1726:	d->length = menuex ? 8 : 4;
;
	MOV	EAX,DWORD [EBP-04H]
	TEST	EAX,EAX
	JE	SHORT	L_210
	MOV	EAX,08H
	JMP	SHORT	L_211
L_210:
	MOV	EAX,04H
L_211:
	MOV	DWORD [ESI+04H],EAX
;
; Line 1727:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1729:	if (!menuex)
;
	MOV	EAX,DWORD [EBP-04H]
	TEST	EAX,EAX
	JNE	SHORT	L_212
;
; Line 1731:	put_16(be, 0, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1732:	put_16(be, 0, d->data + 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1734:	d->link = from_menuitems(menu->items, be);
;
	PUSH	EBX
	PUSH	DWORD [EDI+00H]
	CALL	_from_menuitems
	ADD	ESP,BYTE 08H
	MOV	DWORD [ESI+00H],EAX
	JMP	SHORT	L_213
L_212:
;
; Line 1738:	put_16(be, 1, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	BYTE 01H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1739:	put_16(be, 4, d->data + 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 04H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1740:	put_32(be, menu->help, d->data + 4);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD [EDI+04H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1742:	d->link = from_menuexitems(menu->items, be);
;
	PUSH	EBX
	PUSH	DWORD [EDI+00H]
	CALL	_from_menuexitems
	ADD	ESP,BYTE 08H
	MOV	DWORD [ESI+00H],EAX
L_213:
;
; Line 1745:	return d;
;
	MOV	EAX,ESI
L_208:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_stringtable:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 014H
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
L_215:
;
; Line 1751:	static BINDATA *from_stringtable(struct stringtable *st, int be)
;
;
; Line 1752:	{
;
;
; Line 1753:	BINDATA *first = NULL,  **pp = &first;
;
	MOV	DWORD [EBP-08H],00H
	LEA	EAX,[EBP-08H]
	MOV	DWORD [EBP-04H],EAX
	MOV	EDI,00H
	JMP	NEAR	L_218
L_216:
;
; Line 1759:	BINDATA *d = AllocateMemory(sizeof(*d));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	ESI,EAX
;
; Line 1762:	slen = st->strings[i].length;
;
	LEA	EAX,[EDI*8+00H]
	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+ECX+00H]
	MOV	DWORD [EBP-014H],EAX
;
; Line 1763:	s = st->strings[i].string;
;
	LEA	EAX,[EDI*8+00H]
	MOV	ECX,DWORD [EBP+08H]
	LEA	EAX,[EAX+ECX+00H]
	MOV	EAX,DWORD [EAX+04H]
	MOV	DWORD [EBP-010H],EAX
;
; Line 1765:	d->length = 2+slen * 2;
;
	MOV	EAX,DWORD [EBP-014H]
	SHL	EAX,01H
	ADD	EAX,BYTE 02H
	MOV	DWORD [ESI+04H],EAX
;
; Line 1766:	d->data = AllocateMemory(d->length);
;
	PUSH	DWORD [ESI+04H]
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [ESI+08H],EAX
;
; Line 1768:	put_16(be, slen, d->data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [EBP-014H]
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
	MOV	EBX,00H
	JMP	SHORT	L_222
L_220:
;
; Line 1771:	put_16(be, s[j], d->data + 2+j * 2);
;
	MOV	EAX,DWORD [ESI+08H]
	ADD	EAX,BYTE 02H
	MOV	ECX,EBX
	SHL	ECX,01H
	ADD	EAX,ECX
	PUSH	EAX
	LEA	EAX,[EBX*2+00H]
	MOV	ECX,DWORD [EBP-010H]
	MOVZX	EAX,WORD [EAX+ECX+00H]
	PUSH	EAX
	PUSH	DWORD [EBP+0CH]
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1770:	for (j = 0; j < slen; j++)
;
L_221:
	INC	EBX
L_222:
	MOV	EAX,EBX
	CMP	EAX,DWORD [EBP-014H]
	JL	SHORT	L_220
L_223:
;
; Line 1773:	d->link = NULL;
;
	MOV	DWORD [ESI+00H],00H
;
; Line 1774:	*pp = d;
;
	MOV	EAX,DWORD [EBP-04H]
	MOV	DWORD [EAX+00H],ESI
;
; Line 1775:	pp = &d->link;
;
	MOV	DWORD [EBP-04H],ESI
;
; Line 1756:	for (i = 0; i < 16; i++)
;
L_217:
	INC	EDI
L_218:
	MOV	EAX,EDI
	CMP	EAX,BYTE 010H
	JL	NEAR	L_216
L_219:
;
; Line 1778:	return first;
;
	MOV	EAX,DWORD [EBP-08H]
L_214:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
_from_versioninfo:
	PUSH	EBP
	MOV	EBP,ESP
	SUB	ESP,BYTE 03CH
	PUSH	EBX
	PUSH	ESI
	PUSH	EDI
	MOV	EBX,DWORD [EBP+0CH]
	LEA	ESI,[EBP-0CH]
L_229:
;
; Line 1783:	static BINDATA *from_versioninfo(struct versioninfo *versioninfo, int be)
;
;
; Line 1784:	{
;
;
; Line 1785:	BINDATA *first = AllocateMemory(sizeof(BINDATA)),  **pp = &first->link;
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-010H],EAX
	MOV	EAX,DWORD [EBP-010H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1789:	first->length = 6;
;
	MOV	EAX,DWORD [EBP-010H]
	MOV	DWORD [EAX+04H],06H
;
; Line 1790:	first->data = AllocateMemory(6);
;
	PUSH	BYTE 06H
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-010H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1792:	length = 6;
;
	MOV	DWORD [EBP-08H],06H
;
; Line 1794:	if (versioninfo->fixed == NULL)
;
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+00H]
	TEST	EAX,EAX
	JNE	SHORT	L_230
;
; Line 1795:	put_16(be, 0, first->data + 2);
;
	MOV	EAX,DWORD [EBP-010H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
	JMP	SHORT	L_231
L_230:
;
; Line 1797:	put_16(be, 52, first->data + 2);
;
	MOV	EAX,DWORD [EBP-010H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 034H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
L_231:
;
; Line 1799:	put_16(be, 0, first->data + 4);
;
	MOV	EAX,DWORD [EBP-010H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1801:	*pp = ascii_to_string("VS_VERSION_INFO", be);
;
	PUSH	EBX
	PUSH	DWORD L_224
	CALL	_ascii_to_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1802:	length += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-08H],EAX
;
; Line 1803:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1805:	align(&pp, &length);
;
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	PUSH	ESI
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1807:	if (versioninfo->fixed)
;
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+00H]
	TEST	EAX,EAX
	JE	NEAR	L_232
;
; Line 1810:	BINDATA *d = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-018H],EAX
;
; Line 1812:	d->length = 52;
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	DWORD [EAX+04H],034H
;
; Line 1813:	d->data = AllocateMemory(52);
;
	PUSH	BYTE 034H
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-018H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1815:	length += 52;
;
	ADD	DWORD [EBP-08H],BYTE 034H
;
; Line 1817:	fi = versioninfo->fixed;
;
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-01CH],EAX
;
; Line 1819:	put_32(be, 0xfeef04bd, d->data);
;
	MOV	EAX,DWORD [EBP-018H]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD 0FEEF04BDH
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1820:	put_32(be, 0x10000, d->data + 4);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	DWORD 010000H
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1821:	put_32(be, fi->file_version_ms, d->data + 8);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 08H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+00H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1822:	put_32(be, fi->file_version_ls, d->data + 12);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 0CH
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+04H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1823:	put_32(be, fi->product_version_ms, d->data + 16);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 010H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+08H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1824:	put_32(be, fi->product_version_ls, d->data + 20);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 014H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+0CH]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1825:	put_32(be, fi->file_flags_mask, d->data + 24);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 018H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+010H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1826:	put_32(be, fi->file_flags, d->data + 28);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 01CH
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+014H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1827:	put_32(be, fi->file_os, d->data + 32);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 020H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+018H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1828:	put_32(be, fi->file_type, d->data + 36);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 024H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+01CH]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1829:	put_32(be, fi->file_subtype, d->data + 40);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 028H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+020H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1830:	put_32(be, fi->file_date_ms, d->data + 44);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02CH
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+024H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1831:	put_32(be, fi->file_date_ls, d->data + 48);
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 030H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+028H]
	PUSH	EBX
	CALL	_put_32
	ADD	ESP,BYTE 0CH
;
; Line 1833:	d->link = NULL;
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	DWORD [EAX+00H],00H
;
; Line 1834:	*pp = d;
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1835:	pp = &d->link;
;
	MOV	EAX,DWORD [EBP-018H]
	MOV	DWORD [ESI+00H],EAX
L_232:
	MOV	EAX,DWORD [EBP+08H]
	MOV	EAX,DWORD [EAX+04H]
	MOV	DWORD [EBP-04H],EAX
	JMP	NEAR	L_236
L_234:
;
; Line 1840:	BINDATA *vid = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-01CH],EAX
;
; Line 1843:	align(&pp, &length);
;
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	PUSH	ESI
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1845:	vid->length = 6;
;
	MOV	EAX,DWORD [EBP-01CH]
	MOV	DWORD [EAX+04H],06H
;
; Line 1846:	vid->data = AllocateMemory(6);
;
	PUSH	BYTE 06H
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-01CH]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1848:	length += 6;
;
	ADD	DWORD [EBP-08H],BYTE 06H
;
; Line 1849:	vilen = 6;
;
	MOV	DWORD [EBP-018H],06H
;
; Line 1851:	put_16(be, 0, vid->data + 2);
;
	MOV	EAX,DWORD [EBP-01CH]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1852:	put_16(be, 1, vid->data + 4);
;
	MOV	EAX,DWORD [EBP-01CH]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	BYTE 01H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1854:	*pp = vid;
;
	MOV	EAX,DWORD [EBP-01CH]
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1855:	pp = &vid->link;
;
	MOV	EAX,DWORD [EBP-01CH]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1857:	switch (vi->type)
;
	MOV	EAX,DWORD [EBP-04H]
	MOV	EAX,DWORD [EAX+04H]
	CMP	EAX,BYTE 00H
	JE	SHORT	L_240
	CMP	EAX,BYTE 01H
	JE	NEAR	L_241
	JMP	SHORT	L_239
L_239:
;
; Line 1860:	fatal("invalid var version type");
;
	PUSH	DWORD L_225
	CALL	_fatal
	POP	ECX
;
; Line 1862:	case VERINFO_STRING:
;
L_240:
;
; Line 1865:	BINDATA *vsd = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-024H],EAX
;
; Line 1868:	*pp = ascii_to_string("StringFileInfo", be);
;
	PUSH	EBX
	PUSH	DWORD L_226
	CALL	_ascii_to_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1869:	length += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-08H],EAX
;
; Line 1870:	vilen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1871:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1873:	hold = length;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EBP-02CH],EAX
;
; Line 1874:	align(&pp, &length);
;
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	PUSH	ESI
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1875:	vilen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-02CH]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1877:	vsd->length = 6;
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	DWORD [EAX+04H],06H
;
; Line 1878:	vsd->data = AllocateMemory(6);
;
	PUSH	BYTE 06H
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-024H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1880:	length += 6;
;
	ADD	DWORD [EBP-08H],BYTE 06H
;
; Line 1881:	vilen += 6;
;
	ADD	DWORD [EBP-018H],BYTE 06H
;
; Line 1882:	vslen = 6;
;
	MOV	DWORD [EBP-028H],06H
;
; Line 1884:	put_16(be, 0, vsd->data + 2);
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1885:	put_16(be, 1, vsd->data + 4);
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	BYTE 01H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1887:	*pp = vsd;
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1888:	pp = &vsd->link;
;
	MOV	EAX,DWORD [EBP-024H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1890:	*pp = from_string(vi->u.string.language, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP-04H]
	PUSH	DWORD [EAX+08H]
	CALL	_from_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1891:	length += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-08H],EAX
;
; Line 1892:	vilen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1893:	vslen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-028H],EAX
;
; Line 1894:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [ESI+00H],EAX
	MOV	EAX,DWORD [EBP-04H]
	MOV	EAX,DWORD [EAX+0CH]
	MOV	DWORD [EBP-020H],EAX
	JMP	NEAR	L_244
L_242:
;
; Line 1901:	hold = length;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EBP-02CH],EAX
;
; Line 1902:	align(&pp, &length);
;
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	PUSH	ESI
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1903:	vilen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-02CH]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1904:	vslen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-02CH]
	ADD	DWORD [EBP-028H],EAX
;
; Line 1906:	vssd = AllocateMemory(sizeof *vssd);
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-034H],EAX
;
; Line 1907:	vssd->length = 6;
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	DWORD [EAX+04H],06H
;
; Line 1908:	vssd->data = AllocateMemory(6);
;
	PUSH	BYTE 06H
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-034H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1910:	length += 6;
;
	ADD	DWORD [EBP-08H],BYTE 06H
;
; Line 1911:	vilen += 6;
;
	ADD	DWORD [EBP-018H],BYTE 06H
;
; Line 1912:	vslen += 6;
;
	ADD	DWORD [EBP-028H],BYTE 06H
;
; Line 1913:	vsslen = 6;
;
	MOV	DWORD [EBP-030H],06H
;
; Line 1915:	put_16(be, vs->length, vssd->data + 2);
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-020H]
	PUSH	DWORD [EAX+0CH]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1916:	put_16(be, 1, vssd->data + 4);
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	BYTE 01H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1918:	*pp = vssd;
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1919:	pp = &vssd->link;
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1921:	*pp = from_string(vs->key, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP-020H]
	PUSH	DWORD [EAX+04H]
	CALL	_from_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1922:	length += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-08H],EAX
;
; Line 1923:	vilen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1924:	vslen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-028H],EAX
;
; Line 1925:	vsslen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-030H],EAX
;
; Line 1926:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1928:	hold = length;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EBP-02CH],EAX
;
; Line 1929:	align(&pp, &length);
;
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	PUSH	ESI
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1930:	vilen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-02CH]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1931:	vslen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-02CH]
	ADD	DWORD [EBP-028H],EAX
;
; Line 1932:	vsslen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-02CH]
	ADD	DWORD [EBP-030H],EAX
;
; Line 1934:	*pp = from_string_len(vs->value, be, vs->length);
;
	MOV	EAX,DWORD [EBP-020H]
	PUSH	DWORD [EAX+0CH]
	PUSH	EBX
	MOV	EAX,DWORD [EBP-020H]
	PUSH	DWORD [EAX+08H]
	CALL	_from_string_len
	ADD	ESP,BYTE 0CH
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1935:	length += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-08H],EAX
;
; Line 1936:	vilen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1937:	vslen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-028H],EAX
;
; Line 1938:	vsslen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-030H],EAX
;
; Line 1939:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1941:	put_16(be, vsslen, vssd->data);
;
	MOV	EAX,DWORD [EBP-034H]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD [EBP-030H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1896:	for (vs = vi->u.string.strings; vs != NULL; vs = vs->link)
;
L_243:
	MOV	EAX,DWORD [EBP-020H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-020H],EAX
L_244:
	MOV	EAX,DWORD [EBP-020H]
	TEST	EAX,EAX
	JNE	NEAR	L_242
L_245:
;
; Line 1944:	put_16(be, vslen, vsd->data);
;
	MOV	EAX,DWORD [EBP-024H]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD [EBP-028H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1946:	break;
;
	JMP	NEAR	L_238
L_241:
;
; Line 1954:	*pp = ascii_to_string("VarFileInfo", be);
;
	PUSH	EBX
	PUSH	DWORD L_227
	CALL	_ascii_to_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1955:	length += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-08H],EAX
;
; Line 1956:	vilen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1957:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1959:	hold = length;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EBP-024H],EAX
;
; Line 1960:	align(&pp, &length);
;
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	PUSH	ESI
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1961:	vilen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-024H]
	ADD	DWORD [EBP-018H],EAX
	MOV	EAX,DWORD [EBP-04H]
	MOV	EAX,DWORD [EAX+08H]
	MOV	DWORD [EBP-020H],EAX
	JMP	NEAR	L_248
L_246:
;
; Line 1967:	BINDATA *vvd = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EBP-034H],EAX
;
; Line 1970:	vvd->length = vvlen = 6;
;
	MOV	DWORD [EBP-03CH],06H
	MOV	EAX,DWORD [EBP-034H]
	MOV	DWORD [EAX+04H],06H
;
; Line 1971:	vvd->data = AllocateMemory(6);
;
	PUSH	BYTE 06H
	CALL	_AllocateMemory
	POP	ECX
	MOV	ECX,DWORD [EBP-034H]
	MOV	DWORD [ECX+08H],EAX
;
; Line 1973:	length += 6;
;
	ADD	DWORD [EBP-08H],BYTE 06H
;
; Line 1974:	vilen += 6;
;
	ADD	DWORD [EBP-018H],BYTE 06H
;
; Line 1976:	put_16(be, 0, vvd->data + 4);
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 04H
	PUSH	EAX
	PUSH	BYTE 00H
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1978:	*pp = vvd;
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1979:	pp = &vvd->link;
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1981:	*pp = from_string(vv->key, be);
;
	PUSH	EBX
	MOV	EAX,DWORD [EBP-020H]
	PUSH	DWORD [EAX+00H]
	CALL	_from_string
	ADD	ESP,BYTE 08H
	MOV	ECX,DWORD [ESI+00H]
	MOV	DWORD [ECX+00H],EAX
;
; Line 1982:	length += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-08H],EAX
;
; Line 1983:	vilen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1984:	vvlen += (*pp)->length;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	EAX,DWORD [EAX+04H]
	ADD	DWORD [EBP-03CH],EAX
;
; Line 1985:	pp = &(*pp)->link;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [ESI+00H],EAX
;
; Line 1987:	hold = length;
;
	MOV	EAX,DWORD [EBP-08H]
	MOV	DWORD [EBP-024H],EAX
;
; Line 1988:	align(&pp, &length);
;
	LEA	EAX,[EBP-08H]
	PUSH	EAX
	PUSH	ESI
	CALL	_align
	ADD	ESP,BYTE 08H
;
; Line 1989:	vilen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-024H]
	ADD	DWORD [EBP-018H],EAX
;
; Line 1990:	vvlen += length - hold;
;
	MOV	EAX,DWORD [EBP-08H]
	SUB	EAX,DWORD [EBP-024H]
	ADD	DWORD [EBP-03CH],EAX
;
; Line 1992:	vvvlen = 0;
;
	MOV	DWORD [EBP-038H],00H
	MOV	EAX,DWORD [EBP-020H]
	MOV	EAX,DWORD [EAX+08H]
	MOV	DWORD [EBP-030H],EAX
	JMP	SHORT	L_252
L_250:
;
; Line 1996:	BINDATA *vvsd = AllocateMemory(sizeof(BINDATA));
;
	PUSH	BYTE 0CH
	CALL	_AllocateMemory
	POP	ECX
	MOV	EDI,EAX
;
; Line 1998:	vvsd->length = 4;
;
	MOV	DWORD [EDI+04H],04H
;
; Line 1999:	vvsd->data = AllocateMemory(4);
;
	PUSH	BYTE 04H
	CALL	_AllocateMemory
	POP	ECX
	MOV	DWORD [EDI+08H],EAX
;
; Line 2001:	length += 4;
;
	ADD	DWORD [EBP-08H],BYTE 04H
;
; Line 2002:	vilen += 4;
;
	ADD	DWORD [EBP-018H],BYTE 04H
;
; Line 2003:	vvlen += 4;
;
	ADD	DWORD [EBP-03CH],BYTE 04H
;
; Line 2004:	vvvlen += 4;
;
	ADD	DWORD [EBP-038H],BYTE 04H
;
; Line 2006:	put_16(be, vlc->language, vvsd->data);
;
	PUSH	DWORD [EDI+08H]
	MOV	EAX,DWORD [EBP-030H]
	PUSH	DWORD [EAX+04H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 2007:	put_16(be, vlc->charset, vvsd->data + 2);
;
	MOV	EAX,DWORD [EDI+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	MOV	EAX,DWORD [EBP-030H]
	PUSH	DWORD [EAX+08H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 2009:	vvsd->link = NULL;
;
	MOV	DWORD [EDI+00H],00H
;
; Line 2010:	*pp = vvsd;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	DWORD [EAX+00H],EDI
;
; Line 2011:	pp = &vvsd->link;
;
	MOV	DWORD [ESI+00H],EDI
;
; Line 1994:	for (vlc = vv->intident; vlc != NULL; vlc = vlc->link)
;
L_251:
	MOV	EAX,DWORD [EBP-030H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-030H],EAX
L_252:
	MOV	EAX,DWORD [EBP-030H]
	TEST	EAX,EAX
	JNE	SHORT	L_250
L_253:
;
; Line 2014:	put_16(be, vvlen, vvd->data);
;
	MOV	EAX,DWORD [EBP-034H]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD [EBP-03CH]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 2015:	put_16(be, vvvlen, vvd->data + 2);
;
	MOV	EAX,DWORD [EBP-034H]
	MOV	EAX,DWORD [EAX+08H]
	ADD	EAX,BYTE 02H
	PUSH	EAX
	PUSH	DWORD [EBP-038H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1964:	for (vv = vi->u.var.var; vv != NULL; vv = vv->link)
;
L_247:
	MOV	EAX,DWORD [EBP-020H]
	MOV	EAX,DWORD [EAX+04H]
	MOV	DWORD [EBP-020H],EAX
L_248:
	MOV	EAX,DWORD [EBP-020H]
	TEST	EAX,EAX
	JNE	NEAR	L_246
L_249:
;
; Line 2019:	break;
;
	JMP	SHORT	L_238
L_238:
;
; Line 2022:	put_16(be, vilen, vid->data);
;
	MOV	EAX,DWORD [EBP-01CH]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD [EBP-018H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 1838:	for (vi = versioninfo->var; vi != NULL; vi = vi->link)
;
L_235:
	MOV	EAX,DWORD [EBP-04H]
	MOV	EAX,DWORD [EAX+00H]
	MOV	DWORD [EBP-04H],EAX
L_236:
	MOV	EAX,DWORD [EBP-04H]
	TEST	EAX,EAX
	JNE	NEAR	L_234
L_237:
;
; Line 2025:	put_16(be, length, first->data);
;
	MOV	EAX,DWORD [EBP-010H]
	PUSH	DWORD [EAX+08H]
	PUSH	DWORD [EBP-08H]
	PUSH	EBX
	CALL	_put_16
	ADD	ESP,BYTE 0CH
;
; Line 2026:	*pp = NULL;
;
	MOV	EAX,DWORD [ESI+00H]
	MOV	DWORD [EAX+00H],00H
;
; Line 2027:	return first;
;
	MOV	EAX,DWORD [EBP-010H]
L_228:
	POP	EDI
	POP	ESI
	POP	EBX
	MOV	ESP,EBP
	POP	EBP
	RET
[GLOBAL	_convert_from_internal]
_convert_from_internal:
	PUSH	EBP
	MOV	EBP,ESP
	PUSH	EBX
	PUSH	ESI
	MOV	ESI,DWORD [EBP+08H]
	MOV	EBX,DWORD [EBP+0CH]
L_255:
;
; Line 2032:	BINDATA *convert_from_internal(RES_RES *res, int be)
;
;
; Line 2033:	{
;
;
; Line 2034:	switch (res->type)
;
	MOV	EAX,DWORD [ESI+00H]
	DEC	EAX
	JL	SHORT	L_257
	CMP	EAX,BYTE 010H
	JGE	SHORT	L_257
	JMP	DWORD [EAX*4+L_258]
L_258:
	DD	L_264
	DD	L_259
	DD	L_265
	DD	L_266
	DD	L_267
	DD	L_263
	DD	L_260
	DD	L_268
	DD	L_261
	DD	L_269
	DD	L_270
	DD	L_262
	DD	L_271
	DD	L_272
	DD	L_273
	DD	L_274
;
; Line 2036:	default:
;
L_257:
;
; Line 2037:	abort();
;
	CALL	_abort
;
; Line 2038:	case RST_BITMAP:
;
L_259:
;
; Line 2039:	case RST_FONT:
;
L_260:
;
; Line 2040:	case RST_ICON:
;
L_261:
;
; Line 2041:	case RST_MESSAGETABLE:
;
L_262:
;
; Line 2042:	case RST_DLGINCLUDE:
;
L_263:
;
; Line 2043:	return from_generic(res->u.data.length, res->u.data.data);
;
	PUSH	DWORD [ESI+08H]
	PUSH	DWORD [ESI+04H]
	CALL	_from_generic
	ADD	ESP,BYTE 08H
	JMP	NEAR	L_254
L_264:
;
; Line 2045:	return from_accelerator(res->u.acc, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_accelerator
	ADD	ESP,BYTE 08H
	JMP	NEAR	L_254
L_265:
;
; Line 2047:	return from_cursor(res->u.cursor, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_cursor
	ADD	ESP,BYTE 08H
	JMP	NEAR	L_254
L_266:
;
; Line 2049:	return from_group_cursor(res->u.group_cursor, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_group_cursor
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_267:
;
; Line 2051:	return from_dialog(res->u.dialog, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_dialog
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_268:
;
; Line 2053:	return from_fontdir(res->u.fontdir, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_fontdir
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_269:
;
; Line 2055:	return from_group_icon(res->u.group_icon, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_group_icon
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_270:
;
; Line 2057:	return from_menu(res->u.menu, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_menu
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_271:
;
; Line 2059:	return from_rcdata(res->u.rcdata, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_rcdata
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_272:
;
; Line 2061:	return from_stringtable(res->u.stringtable, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_stringtable
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_273:
;
; Line 2063:	return from_rcdata(res->u.rcdata, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_rcdata
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_274:
;
; Line 2065:	return from_versioninfo(res->u.versioninfo, be);
;
	PUSH	EBX
	PUSH	DWORD [ESI+04H]
	CALL	_from_versioninfo
	ADD	ESP,BYTE 08H
	JMP	SHORT	L_254
L_256:
;
; Line 2067:	}
;
L_254:
	POP	ESI
	POP	EBX
	POP	EBP
	RET
SECTION _STRING
L_227:
	DB	056H,061H,072H,046H,069H,06CH,065H,049H,06EH,066H,06FH,00H
L_226:
	DB	053H,074H,072H,069H,06EH,067H,046H,069H,06CH,065H,049H,06EH
	DB	066H,06FH,00H
L_225:
	DB	069H,06EH,076H,061H,06CH,069H,064H,020H,076H,061H,072H,020H
	DB	076H,065H,072H,073H,069H,06FH,06EH,020H,074H,079H,070H,065H
	DB	00H
L_224:
	DB	056H,053H,05FH,056H,045H,052H,053H,049H,04FH,04EH,05FH,049H
	DB	04EH,046H,04FH,00H
L_94:
	DB	069H,06EH,076H,061H,06CH,069H,064H,020H,052H,043H,044H,041H
	DB	054H,041H,020H,073H,065H,063H,074H,00H
L_9:
	DB	06DH,061H,06CH,066H,06FH,072H,06DH,065H,064H,020H,072H,065H
	DB	073H,06FH,075H,072H,063H,065H,020H,069H,06EH,020H,052H,045H
	DB	053H,020H,066H,069H,06CH,065H,02CH,020H,065H,078H,069H,074H
	DB	069H,06EH,067H,00H
SECTION _CONST

SECTION _TEXT
[EXTERN	_AllocateMemory]
[EXTERN	_abort]
[EXTERN	_strlen]
[EXTERN	_memset]
[EXTERN	_fatal]
