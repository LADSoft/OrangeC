;File .\ctype.c
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
[GLOBAL	___ctype_init]
___ctype_init:
; Line 100:	void __ctype_init(void)

; Line 101:	{

L_1:
; Line 102:	    _pctype = (unsigned short *)(((unsigned char *)__locale_data[LC_CTYPE]) + offsetof(CTYPE_DATA, ctypetab));

	MOV	EAX,DWORD [___locale_data+08H]
	ADD	EAX,BYTE 06H
	MOV	DWORD [__pctype],EAX
; Line 103:	}

L_2:
	RET
[GLOBAL	_tolower]
_tolower:
; Line 104:	int _RTL_FUNC (tolower)(int c)

; Line 105:	{

	PUSH	EBP
	MOV	EBP,ESP
L_7:
; Line 106:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_10
; Line 107:	      return c ;

	MOV	EAX,DWORD [EBP+08H]
	JMP	SHORT	L_8
L_10:
; Line 108:	    if (ct_data.flags & 1)

	MOV	EAX,DWORD [___locale_data+08H]
	MOV	AX,WORD [EAX+04H]
	CWDE
	AND	EAX,BYTE 01H
	JE	SHORT	L_15
; Line 109:	        c = ct_data.tolower[c];

	MOV	ECX,DWORD [___locale_data+08H]
	MOV	EAX,DWORD [EBP+08H]
	MOVZX	EAX,BYTE [ECX+EAX+0206H]
	MOV	DWORD [EBP+08H],EAX
	JMP	SHORT	L_18
; Line 110:	    else

L_15:
; Line 111:	        if (_pctype[c] & _IS_UPP)

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 01H
	JE	SHORT	L_21
; Line 112:	            c+=32;

	ADD	DWORD [EBP+08H],BYTE 020H
L_21:
L_18:
; Line 113:	    return c;

	MOV	EAX,DWORD [EBP+08H]
; Line 114:	}

L_8:
	POP	EBP
	RET
[GLOBAL	_toupper]
_toupper:
; Line 115:	int _RTL_FUNC (toupper)(int c)

; Line 116:	{

	PUSH	EBP
	MOV	EBP,ESP
L_31:
; Line 117:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_34
; Line 118:	      return c ;

	MOV	EAX,DWORD [EBP+08H]
	JMP	SHORT	L_32
L_34:
; Line 119:	    if (ct_data.flags & 1)

	MOV	EAX,DWORD [___locale_data+08H]
	MOV	AX,WORD [EAX+04H]
	CWDE
	AND	EAX,BYTE 01H
	JE	SHORT	L_39
; Line 120:	        c = ct_data.toupper[c];

	MOV	ECX,DWORD [___locale_data+08H]
	MOV	EAX,DWORD [EBP+08H]
	MOVZX	EAX,BYTE [ECX+EAX+0306H]
	MOV	DWORD [EBP+08H],EAX
	JMP	SHORT	L_42
; Line 121:	    else

L_39:
; Line 122:	        if (_pctype[c] & _IS_LOW)

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 02H
	JE	SHORT	L_45
; Line 123:	            c-=32;

	SUB	DWORD [EBP+08H],BYTE 020H
L_45:
L_42:
; Line 124:	    return c;

	MOV	EAX,DWORD [EBP+08H]
; Line 125:	}

L_32:
	POP	EBP
	RET
[GLOBAL	__tolower]
__tolower:
; Line 126:	int _RTL_FUNC (_tolower)(int c)

; Line 127:	{

	PUSH	EBP
	MOV	EBP,ESP
L_55:
; Line 128:	    return tolower(c);

	MOV	EAX,DWORD [EBP+08H]
	PUSH	EAX
	CALL	_tolower
	POP	ECX
; Line 129:	}

L_56:
	POP	EBP
	RET
[GLOBAL	__toupper]
__toupper:
; Line 130:	int _RTL_FUNC (_toupper)(int c)

; Line 131:	{

	PUSH	EBP
	MOV	EBP,ESP
L_61:
; Line 132:	    return toupper(c);

	MOV	EAX,DWORD [EBP+08H]
	PUSH	EAX
	CALL	_toupper
	POP	ECX
; Line 133:	}

L_62:
	POP	EBP
	RET
[GLOBAL	__ltolower]
__ltolower:
; Line 134:	int _RTL_FUNC (_ltolower)(int c)

; Line 135:	{

	PUSH	EBP
	MOV	EBP,ESP
L_67:
; Line 136:	    return tolower(c);

	MOV	EAX,DWORD [EBP+08H]
	PUSH	EAX
	CALL	_tolower
	POP	ECX
; Line 137:	}

L_68:
	POP	EBP
	RET
[GLOBAL	__ltoupper]
__ltoupper:
; Line 138:	int _RTL_FUNC (_ltoupper)(int c)

; Line 139:	{

	PUSH	EBP
	MOV	EBP,ESP
L_73:
; Line 140:	    return toupper(c);

	MOV	EAX,DWORD [EBP+08H]
	PUSH	EAX
	CALL	_toupper
	POP	ECX
; Line 141:	}

L_74:
	POP	EBP
	RET
[GLOBAL	_isspace]
_isspace:
; Line 142:	int _RTL_FUNC (isspace)(int c)

; Line 143:	{

	PUSH	EBP
	MOV	EBP,ESP
L_79:
; Line 144:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_82
; Line 145:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_80
L_82:
; Line 146:	   return (_pctype[c] & _IS_SP) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 08H
; Line 147:	}

L_80:
	POP	EBP
	RET
[GLOBAL	_isblank]
_isblank:
; Line 148:	int _RTL_FUNC (isblank)(int c)

; Line 149:	{

	PUSH	EBP
	MOV	EBP,ESP
L_90:
; Line 150:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_93
; Line 151:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_91
L_93:
; Line 152:	   return (_pctype[c] & _IS_BLK) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 040H
; Line 153:	}

L_91:
	POP	EBP
	RET
[GLOBAL	_isalpha]
_isalpha:
; Line 154:	int _RTL_FUNC (isalpha)(int c)

; Line 155:	{

	PUSH	EBP
	MOV	EBP,ESP
L_101:
; Line 156:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_104
; Line 157:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_102
L_104:
; Line 158:	   return (_pctype[c] & (_IS_UPP | _IS_LOW)) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 03H
; Line 159:	}

L_102:
	POP	EBP
	RET
[GLOBAL	_ispunct]
_ispunct:
; Line 160:	int _RTL_FUNC (ispunct)(int c)

; Line 161:	{

	PUSH	EBP
	MOV	EBP,ESP
L_112:
; Line 162:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_115
; Line 163:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_113
L_115:
; Line 164:	   return (_pctype[c] & _IS_PUN) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 010H
; Line 165:	}

L_113:
	POP	EBP
	RET
[GLOBAL	_islower]
_islower:
; Line 166:	int _RTL_FUNC (islower)(int c)

; Line 167:	{

	PUSH	EBP
	MOV	EBP,ESP
L_123:
; Line 168:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_126
; Line 169:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_124
L_126:
; Line 170:	   return (_pctype[c] & _IS_LOW) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 02H
; Line 171:	}

L_124:
	POP	EBP
	RET
[GLOBAL	_isupper]
_isupper:
; Line 172:	int _RTL_FUNC (isupper)(int c)

; Line 173:	{

	PUSH	EBP
	MOV	EBP,ESP
L_134:
; Line 174:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_137
; Line 175:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_135
L_137:
; Line 176:	   return (_pctype[c] & _IS_UPP) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 01H
; Line 177:	}

L_135:
	POP	EBP
	RET
[GLOBAL	_isprint]
_isprint:
; Line 178:	int _RTL_FUNC (isprint)(int c)

; Line 179:	{

	PUSH	EBP
	MOV	EBP,ESP
L_145:
; Line 180:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_148
; Line 181:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_146
L_148:
; Line 182:	   return (_pctype[c] & (_IS_PRINT)) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,01D7H
; Line 183:	}

L_146:
	POP	EBP
	RET
[GLOBAL	_isalnum]
_isalnum:
; Line 185:	int _RTL_FUNC (isalnum)(int c)

; Line 186:	{

	PUSH	EBP
	MOV	EBP,ESP
L_156:
; Line 187:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_159
; Line 188:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_157
L_159:
; Line 189:	   return (_pctype[c] & (_IS_UPP | _IS_LOW | _IS_DIG)) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 07H
; Line 190:	}

L_157:
	POP	EBP
	RET
[GLOBAL	_isgraph]
_isgraph:
; Line 191:	int _RTL_FUNC (isgraph)(int c)

; Line 192:	{

	PUSH	EBP
	MOV	EBP,ESP
L_167:
; Line 193:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_170
; Line 194:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_168
L_170:
; Line 195:	   return (_pctype[c] & _IS_GRAPH) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,0197H
; Line 196:	}

L_168:
	POP	EBP
	RET
[GLOBAL	_isdigit]
_isdigit:
; Line 197:	int _RTL_FUNC (isdigit)(int c)

; Line 198:	{

	PUSH	EBP
	MOV	EBP,ESP
L_178:
; Line 199:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_181
; Line 200:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_179
L_181:
; Line 201:	   return (_pctype[c] & _IS_DIG) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 04H
; Line 202:	}

L_179:
	POP	EBP
	RET
[GLOBAL	_isxdigit]
_isxdigit:
; Line 203:	int _RTL_FUNC (isxdigit)(int c)

; Line 204:	{

	PUSH	EBP
	MOV	EBP,ESP
L_189:
; Line 205:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_192
; Line 206:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_190
L_192:
; Line 207:	   return (_pctype[c] & _IS_HEX) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,080H
; Line 208:	}

L_190:
	POP	EBP
	RET
[GLOBAL	_toascii]
_toascii:
; Line 209:	int _RTL_FUNC (toascii)(int c)

; Line 210:	{

	PUSH	EBP
	MOV	EBP,ESP
L_200:
; Line 211:	   return (c & 0x7f) ;

	MOV	EAX,DWORD [EBP+08H]
	AND	EAX,BYTE 07FH
; Line 212:	}

L_201:
	POP	EBP
	RET
[GLOBAL	___toascii]
___toascii:
; Line 213:	int _RTL_FUNC (__toascii)(int c)

; Line 214:	{

	PUSH	EBP
	MOV	EBP,ESP
L_206:
; Line 215:	   return (c & 0x7f) ;

	MOV	EAX,DWORD [EBP+08H]
	AND	EAX,BYTE 07FH
; Line 216:	}

L_207:
	POP	EBP
	RET
[GLOBAL	_isascii]
_isascii:
; Line 217:	int _RTL_FUNC (isascii)(int c)

; Line 218:	{

	PUSH	EBP
	MOV	EBP,ESP
L_212:
; Line 219:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_215
; Line 220:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_213
L_215:
; Line 221:	    return ( c < 0x80);

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,080H
	SETL	AL
	AND	EAX,BYTE 01H
; Line 222:	}

L_213:
	POP	EBP
	RET
[GLOBAL	___isascii]
___isascii:
; Line 223:	int _RTL_FUNC (__isascii)(int c)

; Line 224:	{

	PUSH	EBP
	MOV	EBP,ESP
L_223:
; Line 225:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_226
; Line 226:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_224
L_226:
; Line 227:	    return ( c < 0x80);

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,080H
	SETL	AL
	AND	EAX,BYTE 01H
; Line 228:	}

L_224:
	POP	EBP
	RET
[GLOBAL	_iscntrl]
_iscntrl:
; Line 229:	int _RTL_FUNC (iscntrl)(int c)

; Line 230:	{

	PUSH	EBP
	MOV	EBP,ESP
L_234:
; Line 231:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_237
; Line 232:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_235
L_237:
; Line 233:	  return (_pctype[c] & _IS_CTL) ;

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,BYTE 020H
; Line 234:	}

L_235:
	POP	EBP
	RET
[GLOBAL	__isctype]
__isctype:
; Line 235:	int _RTL_FUNC (_isctype)(int c, int flag)

; Line 236:	{

	PUSH	EBP
	MOV	EBP,ESP
L_245:
; Line 237:	   if ((unsigned)c >= 256)

	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,0100H
	JC	SHORT	L_248
; Line 238:	      return 0 ;

	XOR	EAX,EAX
	JMP	SHORT	L_246
L_248:
; Line 239:	    return(_pctype[c] & flag);

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	ECX,WORD [EAX+ECX*2+00H]
	MOV	EAX,DWORD [EBP+0CH]
	AND	EAX,ECX
; Line 240:	}

L_246:
	POP	EBP
	RET
[GLOBAL	___iscsymf]
___iscsymf:
; Line 241:	int _RTL_FUNC (__iscsymf)(int c)

; Line 242:	{

	PUSH	EBP
	MOV	EBP,ESP
L_256:
; Line 243:	    return isalpha(c) || c == '_';

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,0103H
	JNE	SHORT	L_261
	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,BYTE 05FH
	JE	SHORT	L_261
	XOR	EAX,EAX
	JMP	SHORT	L_262
L_261:
	MOV	EAX,01H
L_262:
; Line 244:	}

L_257:
	POP	EBP
	RET
[GLOBAL	___iscsym]
___iscsym:
; Line 245:	int _RTL_FUNC (__iscsym)(int c)

; Line 246:	{

	PUSH	EBP
	MOV	EBP,ESP
L_264:
; Line 247:	    return isalnum(c) || c == '_';

	MOV	ECX,DWORD [EBP+08H]
	MOV	EAX,DWORD [__pctype]
	MOVZX	EAX,WORD [EAX+ECX*2+00H]
	AND	EAX,0107H
	JNE	SHORT	L_269
	MOV	EAX,DWORD [EBP+08H]
	CMP	EAX,BYTE 05FH
	JE	SHORT	L_269
	XOR	EAX,EAX
	JMP	SHORT	L_270
L_269:
	MOV	EAX,01H
L_270:
; Line 248:	}

L_265:
	POP	EBP
	RET
SECTION bss
[GLOBAL	__pctype]

__pctype	RESB	04H

SECTION data

_C_qualified_name	DB	043H
	DB	00H
	RESB	01H

[GLOBAL	_C_ctype_data]

_C_ctype_data	DD	_C_qualified_name+0
	DW	00H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	028H
	DW	028H
	DW	028H
	DW	028H
	DW	028H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	020H
	DW	048H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	084H
	DW	084H
	DW	084H
	DW	084H
	DW	084H
	DW	084H
	DW	084H
	DW	084H
	DW	084H
	DW	084H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	081H
	DW	081H
	DW	081H
	DW	081H
	DW	081H
	DW	081H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	01H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	082H
	DW	082H
	DW	082H
	DW	082H
	DW	082H
	DW	082H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	02H
	DW	010H
	DW	010H
	DW	010H
	DW	010H
	DW	020H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DW	00H

[GLOBAL	_C_numeric_data]

_C_numeric_data	DD	_C_qualified_name+0
	DB	02EH
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H
	DB	00H

	RESB	03H
[GLOBAL	___locale_data]

___locale_data	DD	00H
	DD	00H
	DD	_C_ctype_data+0
	DD	00H

	DD	_C_numeric_data+0
	DD	00H
	DD	00H
	DD	00H
SECTION code
SECTION cstartup
	DB	0,231
	DD	___ctype_init
SECTION const

