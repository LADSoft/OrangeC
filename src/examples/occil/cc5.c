#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define  SIZETOKEN	1000	
#define  OPERATOR2 	"== != <= >= |= += -= *= /= >> << ++ -- && || -> "

int  fToken, fTrace, fCode;
char *Token[SIZETOKEN];
int  nToken;
int  tix = -1;

int is_alpha(int c)
{
  return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

int is_digit(int c)
{
  return ((c>='0') && (c<='9'));
}

void error(const char *format, char *arg)
{
	if (0 <= tix && tix < nToken)
		printf("tix=%d: ", tix);
	printf(format, arg);
	exit(1);
}

int isAlpha(int c)
{
	return (is_alpha(c) || c == '_');
}

int isAlNum(int c)
{
	return isAlpha(c) || is_digit(c);
}

int isKanji(int c)
{
	return (c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC);
}

char *strdup (const char *s) {
    char *d = (char*)malloc (strlen (s) + 1);   
    if (d == NULL) return NULL;         
    strcpy (d,s);                       
    return d;                          
}

void lex(const char *srcfile)
{
	char linebuf[256], buf[128], op2[4], *p, *pBgn;
	FILE *fpSrc = fopen(srcfile, "r");
	if (fpSrc == NULL) 
		error("file '%s' can't be opened", srcfile);
	while (fgets(linebuf, sizeof(linebuf), fpSrc) != NULL)
	{
		for (p = linebuf; *p != '\0'; )
		{
			if (*p <= ' ')
			{
				p++;
				continue;
			}
			if (p[0] == '/' && p[1] == '/') break;
			pBgn = p;
			if (*p == '"')
			{
				for (p++; *p != '\0' && *p != '"'; p++)
				{
					if (*p == '\\' && strchr("rn\"t", p[1]) != NULL)  // '\r','\n','\"','\t'
					{
						*p = p[1] == 'r' ? '\r' : p[1] == 'n' ? '\n' : p[1] == '"' ? '\"' : '\t';
						strcpy(p + 1, p + 2);
					}
					else if (*p == '\\' || isKanji(*p & 0xff))
					{
						p++;
					}
				}
				if (*p++ != '"') 
					error("Expected '\"' <%s>", pBgn);
			}
			else if (is_digit(*p))
			{
				strtoul(p, &p, 10); //for (p++; *p != '\0' && isdigit(*p); p++) ;
			}
			else if (strncmp(pBgn, "char*", 5) == 0)  	// char*
			{
				p += 5;
			}
			else if (isAlpha(*p))
			{
				for (p++; *p != '\0' && isAlNum(*p); ) 
					p++;
			}
			else
			{
				sprintf(op2, "%c%c ", p[0], p[1]);
				p += strstr(OPERATOR2, op2) != NULL ? 2 : 1;
			}
			sprintf(buf, "%.*s", p - pBgn, pBgn);
			if (nToken == SIZETOKEN)
				error("Token is to big", "");
			Token[nToken++] = strdup(buf);
			if (fToken) 
				printf(strchr(";{}", *buf) != NULL ? "%s\n" : "%s ", buf);
		}
	}
	fclose(fpSrc);
}

#define  GSIZE	1000	
#define  LSIZE	1000	
enum { NM_VAR = 0x01, NM_FUNC = 0x02 };
enum { AD_DATA = 0, AD_STACK, AD_CODE };

typedef struct _Name
{
	int  type;
	char *dataType, *name;
	int  addrType, address;
	int  size;
} Name;

Name *GlobalName[GSIZE], *LocalName[LSIZE];
int  nGlobal, nLocal;

Name *appendName(int nB, int type, char *dType, char *name, int aType, int addr)
{
	Name nm = { type, dType, name, aType, addr };
	Name *pNew = calloc(1, sizeof(Name));
	if (pNew == NULL) 
		error("Fail to allocate memory in appendName", "");
	memcpy(pNew, &nm, sizeof(Name));
	if (nB == 0 && nGlobal < GSIZE - 1) 
		GlobalName[nGlobal++] = pNew;
	else if (nB == 1 && nLocal < LSIZE - 1) 
		LocalName[nLocal++] = pNew;
	else 
		error("Fail in appendName", "");
	return pNew;
}

Name *getNameFromTable(int nB, int type, char *name)
{
	int  n;
	int  nEntry = nB == 0 ? nGlobal : nLocal;
	for (n = 0; n < nEntry; n++)
	{
		Name *e = nB == 0 ? GlobalName[n] : LocalName[n];
		if (strcmp(e->name, name) == 0 && (e->type&type) != 0) return e;
	}
	return NULL;
}

Name *getNameFromAllTable(int type, char *name)
{
	Name *pName = getNameFromTable(1, type, name);
	return pName != NULL ? pName : getNameFromTable(0, type, name);
}

typedef struct _INSTRUCT
{
	int inst, offset, imm, type;
} INSTRUCT;
typedef struct _INSTS
{
	char *hcode, *mnemo;
	int fImm;
} INSTS;
typedef struct _Function
{
	int offset;
	char *name;
} Function;

enum
{
	push_eax = 0, push_ecx, push_ebp, pop_eax, pop_ecx,
	pop_edx,
	lea_eax_pbp,
	lea_eax_da4,
	mov_ebp_esp, mov_ecx_eax, mov_eax_pax, mov_pcx_eax, mov_eax_pbp, mov_eax, add_esp,
	add_eax_ecx, sub_esp, sub_eax_ecx, mul_eax_ecx, div_eax_ecx, mod_eax_ecx, cmp_ecx_eax,
	test_eax_eax, jmp, jz, jnz, jl, jge, jle, jg, call, call_dll, xret, func, loc_
};

INSTS insts[] =
{
	{"50", "push eax", 0},       {"51", "push ecx", 0},     {"55", "push ebp", 0},
	{"58", "pop eax", 0},        {"59", "pop ecx", 0},
	{"5A", "pop edx", 0},
	{"8D85","lea eax,[ebp%+d]",1},
	{"8D0482", "lea eax,[edx+eax*4]", 0},
	{"89E5", "mov ebp,esp", 0},  {"8BC8", "mov ecx,eax", 0},{"8B00", "mov eax,[eax]", 0},
	{"8901", "mov [ecx],eax", 0},{"8B85","mov eax,[ebp%+d]",1},{"B8", "mov eax,%d", 1},
	{"81C4", "add esp,%d", 1},   {"03C1", "add eax,ecx", 0},{"81EC", "sub esp,%d", 1},
	{"2BC1", "sub eax,ecx", 0},  {"0FAFC1", "mul eax,ecx", 0},{"99F7F9", "div eax,ecx", 0},
	{"99F7F989D0","mod eax,ecx",0},{"3BC8", "cmp ecx,eax", 0},{"85C0", "test eax,eax", 0},
	{"E9", "jmp loc_%03d", 1},   {"0F84", "jz loc_%03d", 1}, {"0F85" ,"jnz loc_%03d", 1},
	{"0F8C", "jl loc_%03d", 1},  {"0F8D", "jge loc_%03d", 1},{"0F8E", "jle loc_%03d", 1},
	{"0F8F", "jg loc_%03d", 1},  {"E8", "call fn_%02d", 1},  {"FF15", "call %s", 1},
	{"C9C3", "leave&ret", 0},    {NULL, "func_%02d %s:", 1}, {NULL, "loc_%03d:", 1}
};

enum { VAL = 0, ADDR };
enum { ST_FUNC = 0, ST_GVAR };
enum { IVAL = 0, LOC, FUNC, DLL, DISP, CODE, DATA };

int  baseSpace;
INSTRUCT Inst[1000];
int  nInst = 0;
Function Func[100];
int nFunc;
BYTE CodeBuf[0x1000], DataBuf[0x1000];
int nCode, nData;
int  numLabel = 1;

int loc()
{
	return numLabel++;
}
int is(char *s)
{
	return strcmp(Token[tix], s) == 0;
}
int ispp(char *s)
{
	if (is(s))
	{
		tix++;
		return 1;
	}
	return 0;
}
int isTypeSpecifier()
{
	return is("void") || is("int") || is("char*");
}
int isFunctionDefinition()
{
	return *Token[tix + 2] == '(';
}
void skip(char *s)
{
	if (!ispp(s)) error("'%s' expected", s);
}

void outCode1(int inst)
{
	Inst[nInst++].inst = inst;
}
void outCode3(int inst, int imm, int type)
{
	Inst[nInst].inst = inst;
	Inst[nInst].type = type;
	Inst[nInst++].imm = imm;
}

void outDataStr(char *str)
{
	int len = strlen(str);
	memcpy(&DataBuf[nData], str + 1, len - 2);
	nData += len - 2;
	DataBuf[nData++] = '\0';
}
void outDataInt(int num)
{
	*((int*)&DataBuf[nData]) = num;
	nData += 4;
}

void assign();
void expression(int mode);
void equalityExpression(int mode);
void relationalExpression(int mode);
void addExpression(int mode);
void primaryExpression(int mode);
void mulExpression(int mode);
void statement(int locBreak, int locContinue);
void compoundStatement(int locBreak, int locContinue);

// TypeSpecifier ::= "void" | "char*" | "int"
char *typeSpecifier()
{
	if (!isTypeSpecifier()) error("'%s' not typespecfier", Token[tix]);
	return Token[tix++];
}

// VarDeclarator ::= Identifier ["[" Constant "]"]
char *varDeclarator(int *pSize)
{
	char *name = Token[tix++];
	if (ispp("["))
	{
		*pSize = atoi(Token[tix++]);
		skip("]");
	}
	else
	{
		*pSize = 0;
	}
	return name;
}

// VariableDeclaration ::= TypeSpecifier VarDeclarator ["=" Initializer]
//				("," VarDeclarator ["=" Initializer])*
void variableDeclaration(int status)
{
	char *varType = typeSpecifier();
	do
	{
		Name *pName;
		int size;
		char *varName = varDeclarator(&size);
		if (status == ST_FUNC)
		{
			baseSpace -= 4 * (size > 0 ? size : 1);
			pName = appendName(1, NM_VAR, varType, varName, AD_STACK, baseSpace);
			if (is("="))
			{
				tix--;
				assign();
			}
		}
		else
		{
			pName = appendName(0, NM_VAR, varType, varName, AD_DATA, nData);
			if (ispp("="))
			{
				char *tk = Token[tix++];
				if (is_digit(*tk)) outDataInt(atoi(tk));
				else error("unsupported: <%s>", tk);
			}
			else
			{
				nData += 4 * (size > 0 ? size : 1);
			}
		}
		pName->size = size;
	} while (ispp(","));
}

// FunctionDefinition ::= TypeSpecifier  VarDeclarator
//     "(" [ VarDeclaration ("," VarDeclaration)* ] ")" CompoundStatement
void functionDefinition()
{
	int dummy;
	int nSub;
	char *varType;
	char *varName;

	nLocal = 0;
	varType = typeSpecifier();
	varName = varDeclarator(&dummy);
	outCode3(func, nFunc, IVAL);
	appendName(0, NM_FUNC, varType, varName, AD_CODE, nFunc);
	Func[nFunc++].name = varName;
	baseSpace = 2 * 4;
	outCode1(push_ebp);
	outCode1(mov_ebp_esp);
	nSub = nInst;
	outCode3(sub_esp, 0, DISP);
	for (skip("("); !ispp(")"); ispp(","))
	{
		varType = typeSpecifier();
		varName = varDeclarator(&dummy);
		appendName(1, NM_VAR, varType, varName, AD_STACK, baseSpace);
		baseSpace += 4;
	}
	baseSpace = 0;
	compoundStatement(0, 0);
	Inst[nSub].imm = -baseSpace;
	if (Inst[nInst - 1].inst != xret) outCode1(xret);
}

// Program ::= (FunctionDefinition | VariableDeclaration ";")*
void program()
{
	for (tix = 0; tix < nToken; )
	{
		if (fTrace) printf("%3d %s\n", tix, Token[tix]);
		if (isFunctionDefinition()) functionDefinition();
		else
		{
			variableDeclaration(ST_GVAR);
			skip(";");
		}
	}
}

void parser()
{
	appendName(0, NM_FUNC, "int", "printf", AD_CODE, -1);
	appendName(0, NM_FUNC, "void", "exit", AD_CODE, -1);
	program();
}


// CompoundStatement :== "{" (VariableDeclaration ";" | Statement)* "}"
void compoundStatement(int locBreak, int locContinue)
{
	for (skip("{"); !ispp("}"); )
	{
		if (isTypeSpecifier())
		{
			variableDeclaration(ST_FUNC);
			skip(";");
		}
		else
		{
			statement(locBreak, locContinue);
		}
	}
}

// IfStatement ::= "if" "(" Expression ")" Statement [ "else" Statement ]
void ifStatement(int locBreak, int locContinue)
{
	int locElse, locEnd;
	tix++;		// "if"
	skip("(");
	expression(VAL);
	skip(")");
	outCode1(test_eax_eax);
	outCode3(jz, locElse = loc(), LOC);
	statement(locBreak, locContinue);	    	// then_statement
	if (is("else")) outCode3(jmp, locEnd = loc(), LOC);
	outCode3(loc_, locElse, LOC);
	if (ispp("else"))
	{
		statement(locBreak, locContinue); 	// else_statement
		outCode3(loc_, locEnd, LOC);
	}
}

// WhileStatement ::= "while" "(" Expression ")" Statement
void whileStatement()
{
	int locExpr, locNext;
	tix++;		// "while" 
	skip("(");
	outCode3(loc_, locExpr = loc(), LOC);
	expression(VAL);
	skip(")");
	outCode1(test_eax_eax);    // expression
	outCode3(jz, locNext = loc(), LOC);  // 0
	statement(locNext, locExpr);	// locBreak, locContinue
	outCode3(jmp, locExpr, LOC);
	outCode3(loc_, locNext, LOC);
}

// ReturnStatement ::= "return" [Expression] ";"
void returnStatement()
{
	tix++;	// "return"
	if (!is(";")) expression(VAL);
	skip(";");
	outCode1(xret);
}

// BreakStatement := "break" ";"
void breakStatement(int locBreak)
{
	tix++;	// break 
	skip(";");
	outCode3(jmp, locBreak, LOC);
}

// ContinueStatement := "continue" ";"
void continueStatement(int locContinue)
{
	tix++;	// continue 
	skip(";");
	outCode3(jmp, locContinue, LOC);
}

// Statement ::= CompoundStmt | IfStmt | WhileStmt |
//               ReturnStmt | BreakStmt | ContinueStmt | Expression ";" | ";"
void statement(int locBreak, int locContinue)
{
	if (is(";")) tix++;	// null statement
	else if (is("{")) compoundStatement(locBreak, locContinue);
	else if (is("if")) ifStatement(locBreak, locContinue);
	else if (is("while")) whileStatement();
	else if (is("return")) returnStatement();
	else if (is("break")) breakStatement(locBreak);
	else if (is("continue")) continueStatement(locContinue);
	else if (*Token[tix + 1] == '[') assign();
	else
	{
		expression(VAL);
		skip(";");
	}
}

// Assign ::= Identifier [ "[" Expression "]" ] "=" Expression
void assign()
{
	primaryExpression(ADDR);	// eax
	skip("=");
	outCode1(push_eax);
	expression(VAL);		// eax
	outCode1(pop_ecx);
	outCode1(mov_pcx_eax);	// mov [ecx],eax
}

void expression(int mode)
{
	if (*Token[tix + 1] == '=' && Token[tix + 1][1] == '\0') assign();
	else equalityExpression(mode);
}

// EqualityExpression ::= RelationalExpr [ ("==" | "!=") RelationalExpr ]
void equalityExpression(int mode)
{
	int fEQ;
	relationalExpression(mode);
	if ((fEQ = ispp("==")) || ispp("!="))
	{
		outCode1(push_eax);
		relationalExpression(mode);
		outCode1(pop_ecx);
		outCode1(cmp_ecx_eax);
		outCode3(mov_eax, 1, IVAL);
		outCode3(fEQ ? jz : jnz, 5, IVAL);
		outCode3(mov_eax, 0, IVAL);
	}
}

// RelationalExpression ::= AddExpr [("<" | ">" | "<=" | ">=") AddExpr]
void relationalExpression(int mode)
{
	int fLT = 0, fGT = 0, fLE = 0;
	addExpression(mode);
	if ((fLT = ispp("<")) || (fGT = ispp(">")) || (fLE = ispp("<=")) || ispp(">="))
	{
		outCode1(push_eax);
		addExpression(mode);
		outCode1(pop_ecx);
		outCode1(cmp_ecx_eax);
		outCode3(mov_eax, 1, IVAL);
		outCode3(fLT ? jl : fGT ? jg : fLE ? jle : jge, 5, IVAL);
		outCode3(mov_eax, 0, IVAL);
	}
}

// AddExpression ::= MulExpression ( ("+" | "-") MulExpression )*
void addExpression(int mode)
{
	int fAdd;
	mulExpression(mode);		// operand_1
	while ((fAdd = ispp("+")) || ispp("-"))
	{
		outCode1(push_eax);
		mulExpression(mode);		// eax: operand_2
		outCode1(mov_ecx_eax);
		outCode1(pop_eax);
		outCode1(fAdd ? add_eax_ecx : sub_eax_ecx);
	}
}

// MulExpression ::= PrimaryExpression ( ("*" | "/" | "%") PrimaryExpression )*
void mulExpression(int mode)
{
	int fMul = 0, fDiv = 0;
	primaryExpression(mode);	// operand_1
	while ((fMul = ispp("*")) || (fDiv = ispp("/")) || ispp("%"))
	{
		outCode1(push_eax);
		primaryExpression(mode);	// eax: operand_2
		outCode1(mov_ecx_eax);
		outCode1(pop_eax);
		outCode1(fMul ? mul_eax_ecx : fDiv ? div_eax_ecx : mod_eax_ecx);
	}
}

void functionCall()
{
	int n, nP, p[20], nSize, dst, mv;
	char *name = Token[tix++];
	Name *pName;
	skip("(");
	pName = getNameFromTable(0, NM_FUNC, name);
	if (pName == NULL) error("'%s' undeclared", name);
	for (nP = 0; !ispp(")"); ispp(","))
	{
		p[nP++] = nInst;
		expression(VAL);
		outCode1(push_eax);
	}
	if (nP > 1)
	{
		mv = nInst - p[1];
		memmove(&Inst[p[0] + mv], &Inst[p[0]], (nInst - p[0]) * sizeof(INSTRUCT));
		for (n = nP, dst = p[0]; --n > 0; dst += nSize)
		{
			nSize = (n + 1 < nP ? p[n + 1] : nInst) - p[n];
			memmove(&Inst[dst], &Inst[mv + p[n]], nSize * sizeof(INSTRUCT));
		}
	}
	if (pName->address >= 0) outCode3(call, pName->address, FUNC);
	else outCode3(call_dll, (int)pName->name, DLL);
	outCode3(add_esp, nP * 4, IVAL);
}

// PrimExpression ::= Identifier ["[" Expression "]"] | "(" Expr ")" | Constant | FunctionCall
void primaryExpression(int mode)
{
	if (is_digit(*Token[tix]))  			// Constant
	{
		outCode3(mov_eax, atoi(Token[tix++]), IVAL);
	}
	else if (*Token[tix] == '"')  		// Constant 
	{
		outCode3(mov_eax, nData, DATA);
		outDataStr(Token[tix++]);
	}
	else if (ispp("("))  			// "(" Expr ")"
	{
		expression(VAL);
		skip(")");
	}
	else if (*Token[tix + 1] == '(')  		// FuctionCall
	{
		functionCall();
	}
	else if (isAlpha(*Token[tix]))  		// Identifier ["[" Expression "]"]
	{
		char *name = Token[tix++];
		Name *pName = getNameFromAllTable(NM_VAR, name);
		if (pName == NULL)
			error("primExpr: '%s' undeclared", name);

		if (pName->addrType == AD_STACK)
		{
			outCode3(lea_eax_pbp, pName->address, DISP);
		}
		else
		{
			outCode3(mov_eax, pName->address, DATA);
		}

		if (ispp("["))
		{
			outCode1(push_eax);
			expression(VAL);		// eax
			skip("]");
			outCode1(pop_edx);
			outCode1(lea_eax_da4);	// lea eax,[edx+eax*4]
		}
		if (mode == VAL) outCode1(mov_eax_pax);

		/********************************************************************************
			if (pName->addrType == AD_STACK) {	// ローカル変数
					outCode3(mode==ADDR ? lea_eax_pbp : mov_eax_pbp, pName->address, DISP);
			} else {				// グローバル変数
				outCode3(mov_eax, pName->address, DATA);
				if (mode == VAL) outCode1(mov_eax_pax);
			}
				 tix++;
		*********************************************************************************/
	}
	else
	{
		error("primExpr: <%s>", Token[tix]);
	}
}

#define ImageBase 0x00400000
#define MEMALIGN  0x1000 
#define RAWALIGN  0x1000 
#define USE_DLL   1
#define USE_FUNC  2

typedef struct _EXE_HEADER
{
	IMAGE_DOS_HEADER      DosHeader;	// 64
	BYTE 		  DosStub[64];
	BYTE                  Signature[4];	// "PE\0\0"
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
	IMAGE_SECTION_HEADER  SectionHeaders[2];
} EXE_HEADER;

typedef struct _IMPORT_SECTION
{
	DWORD ImptDesc[USE_DLL + 1][5];
	DWORD LookupTable[USE_DLL + USE_FUNC];
	DWORD ImptAddrTable[USE_DLL + USE_FUNC];
	char  szNames[8][16];
} IMPORT_SECTION;

#define POSDATA sizeof(IMPORT_SECTION)			// DataSection
#define IMPT ((USE_DLL+1)*20 + (USE_DLL+USE_FUNC)*4)	// ImptAddrTable

int sizeCodeSection = MEMALIGN, sizeDataSection = MEMALIGN;
int posCodeSection = MEMALIGN, posDataSection = MEMALIGN * 2;

BYTE bufEXE[RAWALIGN * 3];
void exe(BYTE *Code, int nCode, BYTE *Data, int nData, int entryPoint, char *outfile)
{
	FILE *fpDst;
	int posLookupTable = posDataSection + (USE_DLL + 1) * 20;
	int sizeLookupTable = (USE_DLL + USE_FUNC) * 4;
	int posImptAddrTable = posLookupTable + sizeLookupTable;
	int sizeImptAddrTable = sizeLookupTable;
	int posName = posLookupTable + sizeLookupTable * 2;

	IMPORT_SECTION imptSection =
	{
		{
			{posLookupTable, 0, 0, posName, posImptAddrTable},
			{0,0,0,0,0}
		},  // msvcrt.dll
		{
			posName + 16 - 2, posName + 32 - 2, 0
		},
		{
			posName + 16 - 2, posName + 32 - 2, 0
		},
		{
				"msvcrt.dll", "printf", "exit", "", "", "", "", ""
		}
	};
	int lenDataSection = sizeof(imptSection) + nData;
	EXE_HEADER exeHeader =
	{
		{
			// IMAGE_DOS_HEADER
			0x5A4D, 0x90, 3, 0, 4,                // "MZ", ...
			0, 0xFFFF, 0, 0xB8,	                  // min-, maxalloc, ss, sp
			0,0,0, 0x40, 0, {0,0,0,0}, 0,0, {0,0,0,0,0, 0,0,0,0,0},
			0x0080,                               // PE
		},
		"\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21"
		"This program cannot be run in DOS mode.\r\r\n$",	// DosStub
		"PE",
		{
			// IMAGE_FILE_HEADER;
			0x014c, 2, 0, 0, 0,			  // Intel 386
			sizeof(IMAGE_OPTIONAL_HEADER),0x030F  // 0x00E0, Characteristics
		},
		{   	// IMAGE_OPTIONAL_HEADER
			0x010B, 0x06, 0x00,			  // Magic, Linker Version
			0, 0, 0, posCodeSection + entryPoint,   // AddressOfEntryPoint
			posCodeSection, posDataSection,	  // Base of Code, Data.
			ImageBase,	MEMALIGN, RAWALIGN,	  // ImageBase, Section-, File-Alignment
			4, 0,  0, 0,  4, 0,	 0,		  // OS, Image, Subsystem Version
			MEMALIGN * 3, MEMALIGN, 0,		  // Image size, Header size, ...
			3, 0,				  // 2/3: gui/console, DllCharacteristics
			0x100000, 0x1000, 0x100000, 0x1000,	  // stack & heap reserve, commit
			0, 0x10,
			{   {0,0}, {posDataSection, sizeDataSection},                     //  1
				{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
				{posImptAddrTable, sizeImptAddrTable},{0,0},{0,0},{0,0}
			}	    // 12
		},
		{ 	// IMAGE_SECTION_HEADER
			{
				".text\0\0", {nCode}, posCodeSection, RAWALIGN,
				RAWALIGN, 0, 0, 0, 0, 0x60000020  // Code | Executable | Readable
			},
			{    // データセクションヘッダ
				".data\0\0", {lenDataSection}, posDataSection, RAWALIGN,
				RAWALIGN * 2, 0,0,0,0, 0xC0000040   // InitializedData|Readable|Writeable
			}
		}
	};
	memcpy(bufEXE, &exeHeader, sizeof(exeHeader));
	memcpy(bufEXE + RAWALIGN, Code, nCode);
	memcpy(bufEXE + RAWALIGN * 2, &imptSection, sizeof(imptSection));
	//memcpy(bufEXE+RAWALIGN*2+sizeof(imptSection), Data, nData);
	memcpy(bufEXE + RAWALIGN * 2 + 0xc0, Data, nData); // sizeof(imptSection)==0xc0
	fpDst = fopen(outfile, "wb");
	if (fpDst == NULL)
	{
		printf("Impossible to create the file %s", outfile);
		exit(1);
	}
	fwrite(bufEXE, sizeof(bufEXE), 1, fpDst);
	fclose(fpDst);
}

int htoi(int c)
{
	return c >= 'A' ? (c - 'A' + 10) : (c - '0');
}
int gen_mcode()
{
	int n, entryPoint = 0, offset = 0;
	int loc2addr[1000];

	for (n = 0; n < nInst; n++)
	{
		int inst = Inst[n].inst;
		int imm = Inst[n].imm;
		Inst[n].offset = offset;
		if (inst == loc_) loc2addr[imm] = offset;
		if (inst == func)
		{
			Func[imm].offset = offset;
			if (strcmp(Func[imm].name, "main") == 0)
				entryPoint = offset;
		}
		if (inst != func && inst != loc_)
			offset += strlen(insts[inst].hcode) / 2 + insts[inst].fImm * 4;
		if (fCode)
		{
			printf("%03d:%c", n + 1, (inst == func || inst == loc_) ? ' ' : '\t');
			printf(insts[inst].mnemo, imm, inst == func ? Func[imm].name : 0);
			if (insts[inst].mnemo[0] == 'j' && Inst[n].type == IVAL)
				printf("[relative addr]");
			else if (Inst[n].type == DATA)
				printf(" [gvar addr]");
			printf("\n");
		}
	}
	for (n = 0; n < nInst; n++)
	{
		int inst = Inst[n].inst;
		int imm = Inst[n].imm;
		int type = Inst[n].type;
		char *q = insts[inst].hcode;
		if (q == NULL) continue;
		for (; *q != '\0'; q += 2)
			CodeBuf[nCode++] = (htoi(*q) << 4) + htoi(q[1]);
		if (insts[inst].fImm)
		{
			int *pI = (int*)&CodeBuf[nCode];
			if (type == DATA)
			{
				*pI = ImageBase + posDataSection + POSDATA + imm;
			}
			else if (type == DLL)
			{
				int pos = strcmp((char*)imm, "printf") == 0 ? 0 : 4;
				*pI = ImageBase + posDataSection + IMPT + pos;
			}
			else if (type == FUNC)
			{
				*pI = Func[imm].offset - Inst[n + 1].offset;
			}
			else if (type == LOC)
			{
				*pI = loc2addr[imm] - Inst[n + 1].offset;
			}
			else
			{
				*pI = imm;
			}
			nCode += 4;
		}
	}
	return entryPoint;
}

int main(int argc, char *argv[])
{
	int n, nSrc = 1;
	int entryPoint;
	for (n = 1; n < argc; n++)
	{
		if (strcmp(argv[n], "-token") == 0)
			fToken = 1;
		else if (strcmp(argv[n], "-trace") == 0)
			fTrace = 1;
		else if (strcmp(argv[n], "-code") == 0)
			fCode = 1;
		else
			nSrc = n;
	}
	lex(argv[nSrc]);
	parser();
	entryPoint = gen_mcode();
	exe(CodeBuf, nCode, DataBuf, nData, entryPoint, "a.exe");
	return 0;
}
