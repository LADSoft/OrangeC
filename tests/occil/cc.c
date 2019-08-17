//
// -- Download all the include files and compiled binary
// -- https://drive.google.com/folderview?id=0B1nFzHmmj-jCaUJiVTVZUXd5WVk&usp
//
#include <ctype.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define strdup _strdup


int xstrlen(char *p);
int isTypeSpecifier(int ix);
int compoundStatement(int locBreak, int locContinue, int sizeRet);

enum
{
	oSRC = 1, oTOKEN = 2, oASM = 4, oTRACE = 8, oDEBUG = 16,
	oDLL = 32, oLINES = 64, oUOPT = 128, oNAME = 256
};

#define OPERATOR2 "== != <= >= |= += -= *= /= %= &= ^= >> << ++ -- && || -> "
enum
{
	TK_SYMBOL = 0, TK_NAME, TK_NUMBER, TK_STRING,
	TK_CHAR, TK_SHORT, TK_INT, TK_FLOAT, TK_DOUBLE
};

#define HASHSIZE_TOKEN  9973
enum { VAL = 0, ADDR };
#define globTable	0
#define AT_TYPE		0x01000000
#define AT_USER		0x02000000
#define AT_IMPT		0x04000000
#define AT_EXPT		0x08000000
#define AT_ADDR		0x00FFFFFF
enum { ST_FUNC = 1, ST_GVAR, ST_STRUCT, ST_ENUM };
enum { AD_CODE = 1, AD_DATA, AD_ZERO, AD_STACK, AD_IMPORT, AD_CONST };

#define NM_CDECL	0x0001
#define NM_WINAPI	0x0002
#define NM_VAR		0x0004
#define NM_EXPORT	0x0008
#define NM_FUNC		(NM_CDECL | NM_WINAPI | NM_EXPORT)	
#define NM_STRUCT	0x0010
#define NM_ATTR		0x0020
#define NM_ENUM		0x0040

#define MEMALIGN  	0x1000
#define RAWALIGN  	0x0200
#define MEMALIGN1	0x0fff		// MEMALIGN - 1
#define RAWALIGN1	0x01ff		// RAWALIGN - 1
#define IMAGEBASE 	0x00400000
#define DLLBASE   	0x10000000

typedef struct _CMDPARAM
{
	int  nSrc;
	char *srcfile[8];
	char outfile[MAX_PATH];
	char impfiles[522];
	char MCCDIR[MAX_PATH];
} CMDPARAM;

typedef struct _HDATA
{
	char *key;
	void *val;
	int seq;
} HDATA;
typedef struct _HASH
{
	HDATA *tbl;
	int   type;
	int   size, entries;
	struct _HASH *pNext;
} HASH;

typedef struct _SRCLINE
{
	int  filenumber, linenumber;
	char *srccode;
} SRCLINE;

typedef struct _TOKEN
{
	int    type;
	int    ival;
	double dval;
	char   *token;
	int    filenumber, linenumber;
} TOKEN;

typedef struct _MCC
{
	char *srcFile[128];
	int  lines[128];
	int  totalLines;
	int  nSrcFile;
	int  mainfile;
	SRCLINE *pSrcLine;
	int  sizeSrcLine;
	int  nSrcLine;
	HASH hash;
	int  typeApp;
	int  nPreFile;
} MCC;

typedef struct _PTRS_TYPE
{
	int ptrs, type;
} PTRS_TYPE;
typedef struct _Name
{
	int type;
	int idName;
	int dataType;
	int addrType;
	int address;
	int ptrs;
	int size[8];
	PTRS_TYPE *argpt;
	int argc;
	struct _Name *pBgn, *pEnd;
	struct _Name *pNext;
} Name;

typedef struct _Block
{
	int idFunc;
	int blockDepth;
	HASH hash;
} Block;

typedef struct _INSTRUCTION
{
	int  opcode;
	char *hexcode, *hexcode2, *mnemonic;
	int  regs_size;
} INSTRUCTION;

typedef struct _INSTRUCT
{
	int  inst, num, attr, offset, size, refs;
	double dval;
	char *sval;
	int  regs;
} INSTRUCT;

typedef struct _HID
{
	int  CHAR, SHORT, INT, FLOAT, DOUBLE, HVOID;
	int  STRUCT, TYPEDEF, IF, ELSE, WHILE, DO, FOR, RETURN, CONTINUE, BREAK, SWITCH, CASE,
		DEFAULT, SIZEOF, DOTS3, HWINAPI, HCONST, EXTERN, DECLSP, DLLIMPT, DLLEXPT, STATIC, ENUM;
} HID;

typedef struct _INDEX
{
	int  tix;
	int  ixCode, ixData, ixZero, ixString;
	int  ixLoc;
} INDEX;

typedef struct _VALUE
{
	int	 mode;
	int  ptrs, type;
	int  ival;
	double  rval;
	int	 fConst, fAddr;
} VALUE;

typedef struct _CODE
{
	TOKEN *token;
	int  nToken;
	HASH hash;
	INSTRUCT *pCode;
	int  sizeCode;
	int  currTable;
	Block block[100];
	int  baseSpace;
	int  offset;
} CODE;

typedef struct _SECTION
{
	int  CodeAddr, CodeSize, DataAddr, DataSize;
	int  ImptAddr, ImptSize, ExptAddr, ExptSize, RelocAddr, RelocSize;
} SECTION;

typedef struct _DLL
{
	char *dllname;
	int  idFunc[64];
	int  nFunc;
} DLL;

typedef struct _EXE
{
	int  base, entryPoint;
	int  lenImpt, lenExpt, sizeImage;
	DLL  dll[32];
	int  nDLL, useDLL;
	int  useFunc;
	int  locs[1000], nLocs;
	int  cnt[100], nPages;
} EXE;

void infixOperation(int op, VALUE *v1, VALUE *v2, int left_bgn, int left_end);
void expression(int mode, VALUE *pv);
void assignExpression(int mode, VALUE *pv);
void conditionalExpression(int mode, VALUE *pv);
void logicalOrExpression(int mode, VALUE *pv);
void logicalAndExpression(int mode, VALUE *pv);
void orExpression(int mode, VALUE *pv);
void xorExpression(int mode, VALUE *pv);
void andExpression(int mode, VALUE *pv);
void equalityExpression(int mode, VALUE *pv);
void relationalExpression(int mode, VALUE *pv);
void shiftExpression(int mode, VALUE *pv);
void addExpression(int mode, VALUE *pv);
void mulExpression(int mode, VALUE *pv);
void castExpression(int mode, VALUE *pv);
void unaryExpression(int mode, VALUE *pv);
void primaryExpression(int mode, VALUE *pv);

int  	 opt;
MCC  	 mcc;
CMDPARAM cmd;
HID  	 ID;
INDEX	 ix;
CODE 	 cd;
SECTION  mem, raw;
EXE	 exe;

void error(const char *loc, const char *format, ...)
{
	if ((opt&oDEBUG) && loc != NULL) fprintf(stderr, "%s: ", loc);
	if (0 <= ix.tix && ix.tix < cd.nToken)
	{
		int filenumber = cd.token[ix.tix].filenumber;
		char *srcfile = filenumber >= 0 ? mcc.srcFile[filenumber] : "startup";
		fprintf(stderr, "%s:%d: ", srcfile, cd.token[ix.tix].linenumber);
	}
	else if (mcc.nPreFile >= 0)
	{
		fprintf(stderr, "%s:%d: ", mcc.srcFile[mcc.nPreFile], mcc.lines[mcc.nPreFile]);
	}
	vfprintf(stderr, format, (char*)&format + sizeof(char*));
	exit(1);
}

int htoi(int c)
{
	return c >= 'A' ? (c - 'A' + 10) : (c - '0');
}
int esc_char(char *p)
{
	if (*p == 'x' || *p == 'X') return (htoi(p[1]) << 4) + htoi(p[2]);
	return *p == '0' ? '\0' : *p == 'r' ? '\r' : *p == 'n' ? '\n' : *p == 't' ? '\t' : *p;
}   // 0->\0, r->\r, ..., t->\t, \'->\', \"->\", \\->\\.
int isAlpha(int c)
{
	return (isalpha(c) || c == '_');
}
int isAlNum(int c)
{
	return isAlpha(c) || isdigit(c);
}
int isKanji(int c)
{
	return (c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC);
}


char *endOfQuote(char *p)
{
	int delim = *p++;	//   '"' or '\''
	while (*p != '\0' && *p != delim)
		p += (*p == '\\' || isKanji(*p & 0xff)) ? 2 : 1;
	if (*p != delim) error("endOfQuote", "missing terminating %c character", delim);
	return ++p;
}

void *xcheck(void *p)
{
	if (p == NULL) error("xcheck", "");
	return p;
}
void *xalloc(int size)
{
	return xcheck(calloc(1, size));
}
void *xrealloc(void *ptr, size_t size)
{
	return xcheck(realloc(ptr, size));
}
char *xstrdup(char *q)
{
	return xcheck(strdup(q));
}

void initHash(int type, int size, HASH *pHash)
{
	pHash->type = type;
	pHash->size = size;
	pHash->tbl = xalloc(size * sizeof(HDATA));
	pHash->entries = 0;
}

int put(char *key, void *val, HASH *pHash);
void reallocHash(int newSize, HASH *pHash)
{
	int  n, oldSize = pHash->size;
	HDATA *oldTbl = pHash->tbl;
	initHash(pHash->type, newSize, pHash);
	for (n = 0; n < oldSize; n++)
	{
		if (oldTbl[n].key != NULL) put(oldTbl[n].key, oldTbl[n].val, pHash);
	}
	free(oldTbl);
}

void freeHash(HASH *pHash)
{
	int n;
	for (n = 0; n < pHash->size; n++)
	{
		if (pHash->tbl[n].key != NULL) free(pHash->tbl[n].key);
		if ((pHash->type == 's' || pHash->type == 'n') && pHash->tbl[n].val != 0)
			free(pHash->tbl[n].val);
	}
	free(pHash->tbl);
}

int hash(char *key, int size)
{
	int n, h = 0;
	for (n = 0; key[n] != '\0'; n++)
		h = (h * 137 + (key[n] & 0xff)) % size;
	return h;
}

int put(char *key, void *val, HASH *pHash)
{
	int n, h = hash(key, pHash->size);
	for (n = 0; n < pHash->size; n++)
	{
		int ix = (h + n) % pHash->size;
		if (pHash->tbl[ix].key == NULL)
		{
			pHash->tbl[ix].key = xstrdup(key);
			pHash->tbl[ix].val = pHash->type == 's' ? xstrdup(val) : val;
			pHash->tbl[ix].seq = pHash->entries++;
			if (pHash->type != 'x' && pHash->entries > pHash->size / 2)
				reallocHash(pHash->size * 2, pHash);
			return ix;
		}
		else if (strcmp(pHash->tbl[ix].key, key) == 0)
		{
			return ix;
		}
	}
	error("lib.put", "Hash Table full!");
}

void *get(char *key, HASH *pHash)
{
	int n, h = hash(key, pHash->size);
	for (n = 0; n < pHash->size; n++)
	{
		int ix = (h + n) % pHash->size;
		if (pHash->tbl[ix].key == NULL) break;
		if (strcmp(pHash->tbl[ix].key, key) == 0)
			return pHash->tbl[ix].val;
	}
	return NULL;
}

void printHash(HASH *pHash)
{
	char *fmt = pHash->type == 's' ? "%4d %-8s %s\n" : "%4d %-8s %08X\n";
	int n;
	for (n = 0; n < pHash->size; n++)
	{
		if (pHash->tbl[n].key != NULL)
			printf(fmt, n, pHash->tbl[n].key, pHash->tbl[n].val);
	}
}


void prepro(char *srcfile);


int parseline(char *p, char *q)
{
	while (*p != '\0' && *p != '\n')
	{
		if (p[0] == '/' && p[1] == '/') break;	// "//"
		if (p[0] == '/' && p[1] == '*')
		{
			char *r = strstr(p + 2, "*/");
			if (r == NULL)
			{
				*q = '\0';
				return 1;
			}
			strcpy(p, r + 2);
		}
		else if (*p == '\'' || *p == '"')
		{
			char *pEnd = endOfQuote(p);
			while (p < pEnd) *q++ = *p++;
		}
		else
		{
			*q++ = *p++;
		}
	}
	*q = '\0';
	return 0;
}

void procInclude(char *src, char *p)
{
	char *q, *r, *s, path[260];

	if ((q = strchr(p, '<')) != NULL && (r = strchr(q, '>')) != NULL)
	{
		sprintf(path, "%s\\include\\%.*s", cmd.MCCDIR, r - q - 1, q + 1);
	}
	else if ((q = strchr(p, '"')) != NULL && (r = strchr(q + 1, '"')) != NULL)
	{
		if ((s = strrchr(src, '/')) == NULL) s = strrchr(src, '\\');
		if (s == NULL) sprintf(path, "%.*s", r - q - 1, q + 1);
		else sprintf(path, "%.*s%.*s", s - src + 1, src, r - q - 1, q + 1);
	}
	else
	{
		return;
	}
	if (get(path, &mcc.hash) == NULL)
	{
		put(path, p, &mcc.hash);
		prepro(path);
	}
}

void procDefine(char *str, HASH *pHash)
{
	char *p = strtok(str, " \t");
	char *q = strtok(NULL, "");
	while (*q <= ' ') q++;
	put(p, q, pHash);
}

void procPragma(char *p)  	// #pragma comment(lib, "comctl32.lib")
{  
	strtok(p, "\"");			// '"'
	char *q = strtok(NULL, ".");	// '.'
	sprintf(cmd.impfiles + strlen(cmd.impfiles), "%s.dll;", q);
}

void addLine(char *srccode, int nFile, int nLine)
{
	if (mcc.nSrcLine >= mcc.sizeSrcLine)
	{
		mcc.sizeSrcLine = mcc.sizeSrcLine * 3 / 2;
		mcc.pSrcLine = xrealloc(mcc.pSrcLine, mcc.sizeSrcLine*sizeof(SRCLINE));
	}
	SRCLINE *pSL = &mcc.pSrcLine[mcc.nSrcLine++];
	pSL->filenumber = nFile;
	pSL->linenumber = nLine;
	pSL->srccode = xstrdup(srccode);
	if (opt&oSRC) printf("%2d %3d: %s\n", nFile, nLine, srccode);
}

void prepro(char *srcfile)
{
	char buf[512], out[512], key[64], *p, *pBgn, *q;
	int  nLine, nFile = mcc.nSrcFile++;

	FILE *fpSrc = fopen(srcfile, "r");
	if (fpSrc == NULL) error("prepro", "file '%s' not found", srcfile);
	mcc.srcFile[nFile] = xstrdup(srcfile);
	int  fComment = 0;	// "*/"
	for (nLine = 1; fgets(buf, sizeof(buf), fpSrc) != NULL; nLine++)
	{
		mcc.nPreFile = nFile;
		mcc.lines[nFile] = nLine;
		if (fComment)
		{
			if ((p = strstr(buf, "*/")) == NULL) continue;
			strcpy(buf, p + 2);
			fComment = 0;
		}
		fComment = parseline(buf, out);
		for (p = out; *p != '\0' && *p <= ' '; p++);
		if (*p == '\0') continue;
		if (strncmp(out, "#include", 8) == 0)
		{
			procInclude(srcfile, out + 8);
		}
		else if (strncmp(out, "#define", 7) == 0)
		{
			procDefine(out + 7, &mcc.hash);
		}
		else if (strncmp(out, "#pragma", 7) == 0)
		{
			procPragma(out + 7);
		}
		else
		{
			for (p = out; *p != '\0';)
			{
				if (*p == '"' || *p == '\'')
				{
					p = endOfQuote(p);
				}
				else if (!isAlpha(*p))
				{
					if (*p++ == '(' && (strcmp(key, "main") == 0 || strcmp(key, "WinMain") == 0))
					{
						mcc.typeApp = *key == 'm' ? 3 : 2;
						mcc.mainfile = nFile;
					}
				}
				else
				{
					for (q = key, pBgn = p; isAlNum(*p);) *q++ = *p++;
					*q = '\0';
					char *val = get(key, &mcc.hash);
					if (val != NULL)
					{
						int vlen = strlen(val);
						memmove(pBgn + vlen, p, strlen(p) + 1);
						memmove(pBgn, val, vlen);
						p = pBgn;
					}
				}
			}
			addLine(out, nFile, nLine);
		}
	}
	if (opt&oDLL) mcc.typeApp = 0;
	if (opt&oLINES && strstr(srcfile, "include\\") == NULL)
	{
		mcc.totalLines += nLine;
		printf("%-24s\t%5d\n", srcfile, nLine - 1);
	}
}

void initPrepro()
{
	mcc.sizeSrcLine = 1000;
	mcc.pSrcLine = xalloc(mcc.sizeSrcLine * sizeof(SRCLINE));
	addLine("short _RoundNear = 0x137F;", -1, 1);
	addLine("short _RoundChop = 0x1F7F;", -1, 2);
}

void addStartup()
{
	int n = 3;
	addLine("void _main() {", -1, n++);
	if (mcc.typeApp == 3)  		// CUI
	{
		addLine("  char **argv, **env;", -1, n++);
		addLine("  int argc, new_mode = 0;", -1, n++);
		addLine("  __set_app_type(1);", -1, n++);
		addLine("  _controlfp(0x10000, 0x30000);", -1, n++);  // _PC_53, _MCW_PC
		addLine("  __getmainargs(&argc, &argv, &env, 0, &new_mode);", -1, n++);
		addLine("  exit(main(argc, argv, env));", -1, n++);
	}
	else if (mcc.typeApp == 2)  	// GUI
	{
		addLine("  STARTUPINFOA si;", -1, n++);
		addLine("  __set_app_type(2);", -1, n++);
		addLine("  _controlfp(0x10000, 0x30000);", -1, n++);
		addLine("  char *p = GetCommandLineA();", -1, n++);
		addLine("  int c = (*p++ == '\"') ? '\"' : ' ';", -1, n++);
		addLine("  while (*p != c && *p != 0) p++;", -1, n++);
		addLine("  if (*p != 0) p++;", -1, n++);
		addLine("  while (*p <= ' ' && *p != 0) p++;", -1, n++);
		addLine("  GetStartupInfoA(&si);", -1, n++);
		addLine("  exit(WinMain(GetModuleHandleA((void*)0), (void*)0,", -1, n++);
		addLine("       p, (si.dwFlags&1) ? si.wShowWindow : 10));", -1, n++);
	}
	else  	// DLL
	{
		addLine("  return 1;", -1, n++);
	}
	addLine("}", -1, n++);
}


void printToken(int n)
{
	printf("%3d %2d:%03d %d \t%s\n", n + 1, cd.token[n].filenumber,
		cd.token[n].linenumber, cd.token[n].type, cd.token[n].token);
}

void lex()
{
	char *p, *pBgn, op2[4], token[256];
	int n, type, nToken = 0, sizeToken = 1000;

	cd.token = xalloc(sizeToken * sizeof(TOKEN));
	for (n = 0; n < mcc.nSrcLine; n++)
	{
		SRCLINE *pSrc = &mcc.pSrcLine[n];
		for (p = pSrc->srccode; *p != '\0';)
		{
			if (*p <= ' ')
			{
				p++;
				continue;
			}
			pBgn = p;
			if (*p == '\"' || *p == '\'')
			{
				type = TK_STRING;
				p = endOfQuote(pBgn);
			}
			else if (isdigit(*p))
			{
				type = TK_NUMBER;
				if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) strtol(p, &p, 16);
				else strtod(p, &p);
			}
			else if (isAlpha(*p))
			{
				type = TK_NAME;
				for (p++; *p != '\0' && isAlNum(*p);) p++;
			}
			else if (strncmp(p, "...", 3) == 0)
			{
				type = TK_NAME;
				p += 3;
			}
			else
			{
				type = TK_SYMBOL;
				sprintf(op2, "%c%c ", p[0], p[1]);
				p += strstr(OPERATOR2, op2) != NULL ? 2 : 1;
			}
			if (nToken >= sizeToken - 1)
			{
				sizeToken = sizeToken * 3 / 2;
				cd.token = xrealloc(cd.token, sizeToken * sizeof(TOKEN));
			}
			sprintf(token, "%.*s", p - pBgn, pBgn);
			if (nToken > 0 && cd.token[nToken - 1].type == TK_STRING && type == TK_STRING)
			{
				char *tk1 = cd.token[nToken - 1].token;
				int len = strlen(tk1);
				char *tk = xrealloc(tk1, len + (p - pBgn));
				strcpy(tk + len - 1, token + 1);
				free(tk1);
				cd.token[nToken - 1].token = tk;
			}
			else
			{
				TOKEN tkn = { type, 0, 0.0, xstrdup(token), pSrc->filenumber, pSrc->linenumber };
				memcpy(&cd.token[nToken++], &tkn, sizeof(TOKEN));
			}
		}
	}
	cd.nToken = nToken;
}

enum
{
	push = 1, push_eax, push_ecx, push_pbp, pop_eax, pop_ecx, pop_edx, inc_dbp, dec_dbp,
	add_eax_ecx, add_eax_edx, add_eax, add_ecx, add_pcx_eax, add_pcx_ax, add_pcx_al, add_esp,
	add_dax, add_dcx, add_ddx, add_bax, add_bcx, add_bdx, add_eax_pbp, add_ecx_pbp,
	add_pbp_eax, add_pbp_ecx, sub_pbp_ecx, sub_pbp_eax, and_pbp_eax, or_pbp_eax, xor_pbp_eax,
	sub_eax, sub_eax_pbp, sub_eax_ecx, sub_pcx_eax, sub_pcx_ax, sub_pcx_al, sub_esp, sub_dax,
	sub_dcx, sub_ddx, sub_bax, sub_bcx, sub_bdx, imul_eax_ecx, imul_eax_eax, imul_edx_edx,
	imul_eax_pbp, xdiv_dbp, cmp_eax_ecx, cmp_ecx_eax, cmp_eax, cmp_eax_pbp, cmp_ah, test_ah,
	test_eax_eax, and_eax_ecx, and_pcx_eax, and_pcx_ax, and_pcx_al, and_ah, or_eax_ecx,
	or_pcx_eax, or_pcx_ax, or_pcx_al, xor_eax_eax, xor_pcx_eax, xor_pcx_ax, xor_pcx_al,
	xor_eax_ecx, xor_ah, shl_eax, shl_edx, shl_eax_cl, shr_eax_cl, neg_eax, mov_ecx_eax,
	mov_edx_eax, mov_eax_edx, mov_eax, mov_ecx, mov_dax, mov_wax, mov_bax, mov_eax_pax,
	mov_eax_pcx, mov_pcx_eax, mov_pcx_ax, mov_pcx_al, mov_pax_ecx, mov_pax_cx, mov_pax_cl,
	mov_eax_pbp, mov_ecx_pbp, mov_pbp_ecx, mov_ecx_pax, mov_edx_pax, mov_edx_pbp, mov_pbp_eax,
	mov_pbp_al, mov_psp_eax, mov_eax_psp, movsx_eax_wax, movsx_eax_bax, movsx_ecx_wcx,
	movsx_ecx_bcx, lea_eax_pbp, lea_ecx_pbp, lea_edx_pbp, mov_eax_ad1, mov_eax_ad2,
	mov_eax_ad4, mov_eax_ad8, mov_eax_da1, mov_eax_da2, mov_eax_da4, mov_eax_da8, lea_eax_ad1,
	lea_eax_ad2, lea_eax_ad4,
	lea_eax_ad8, lea_eax_da1, lea_eax_da2, lea_eax_da4, lea_eax_da8, lea_ecx_da1, lea_ecx_da2,
	lea_ecx_da4, lea_ecx_da8, xchg_eax_ecx, cwde, jmp, jz, jnz, jl, jge, jle, jg, call,
	xent, xret, sete_eax, setne_eax, setl_eax, setge_eax, setle_eax, setg_eax, fchs, fxch_st1,
	fld_qax, fld_qcx, fld_qbp, fld_qp, fldcw, fstp_qsp, fstsw, fst_qax, fst_qcx, fst_qbp,
	fadd_qbp, fadd_qp, fsub_qbp, fsub_qp, fmul_qbp, fmul_qp, fdiv_qbp, fdiv_qp, fstp_st1,
	faddp_st1_st, fsubrp_st1_st, fmulp_st1_st, fdivrp_st1_st, fucompp, fistp_dsp, fild_dax,
	fild_dsp, xdiv_ecx, xmod_ecx, setint, setreal, setstr, fn_, exp_, loc_
};

#define Z ((void*)0)
enum { A = 0x0100, C = 0x0200, D = 0x0400, AC = 0x0300, AD = 0x0500, CD = 0x0600, ACD = 0x0700, X = 0x0700 };
INSTRUCTION instruction[] = {
	{ push, "68", Z, "push %d", 4 },
	{ push_pbp, "FFB5", "FF75", "push [ebp%+d]", 5 },
	{ push_eax, "50", Z, "push eax", A },
	{ push_ecx, "51", Z, "push ecx", C },
	{ pop_eax, "58", Z, "pop eax", A },
	{ pop_ecx, "59", Z, "pop ecx", C },
	{ pop_edx, "5A", Z, "pop edx", D },
	{ inc_dbp, "FF85", "FF45", "inc dp[ebp%+d]", 5 },
	{ dec_dbp, "FF8D", "FF4D", "dec dp[ebp%+d]", 5 },
	{ add_eax_ecx, "03C1", Z, "add eax,ecx", AC },
	{ add_eax_edx, "03C2", Z, "add eax,edx", AD },
	{ add_eax, "05", "83C0", "add eax,%d", 5 | A },
	{ add_ecx, "81C1", "83C1", "add ecx,%d", 5 | C },
	{ add_pcx_eax, "0101", Z, "add [ecx],eax", AC },
	{ add_pcx_ax, "660101", Z, "add [ecx],ax", AC },
	{ add_pcx_al, "0001", Z, "add [ecx],al", AC },
	{ add_esp, "81C4", "83C4", "add esp,%d", 5 },
	{ add_bax, "8000", Z, "add bp[eax]", A },
	{ add_bcx, "8001", Z, "add bp[ecx]", C },
	{ add_bdx, "8002", Z, "add bp[edx]", D },
	{ add_dax, "8100", "8300", "add dp[eax],%d", 5 | A },
	{ add_dcx, "8101", "8301", "add dp[ecx],%d", 5 | C },
	{ add_ddx, "8102", "8302", "add dp[edx],%d", 5 | D },
	{ add_eax_pbp, "0385", "0345", "add eax,[ebp%+d]", 5 | A },
	{ add_ecx_pbp, "038D", "034D", "add ecx,[ebp%+d]", 5 | C },
	{ add_pbp_eax, "0185", "0145", "add [ebp%+d],eax", 5 | A },
	{ add_pbp_ecx, "018D", "014D", "add [ebp%+d],ecx", 5 | C },
	{ sub_pbp_eax, "2985", "2945", "sub [ebp%+d],eax", 5 | A },
	{ or_pbp_eax, "0985", "0945", "or [ebp%+d],eax", 5 | A },
	{ xor_pbp_eax, "3185", "3145", "xor [ebp%+d],eax", 5 | A },
	{ sub_pbp_ecx, "298D", "294D", "sub [ebp%+d],ecx", 5 | C },
	{ sub_eax, "2D", "83E8", "sub eax,%d", 5 | A },
	{ sub_eax_pbp, "2B85", "2B45", "sub eax,[ebp%+d]", 5 | A },
	{ sub_eax_ecx, "2BC1", Z, "sub eax,ecx", AC },
	{ sub_pcx_eax, "2901", Z, "sub [ecx],eax", AC },
	{ sub_pcx_ax, "662901", Z, "sub [ecx],ax", AC },
	{ sub_pcx_al, "2801", Z, "sub [ecx],al", AC },
	{ sub_esp, "81EC", "83EC", "sub esp,%d", 5 },
	{ sub_dax, "8128", "8328", "sub dp[eax],%d", 5 | A },
	{ sub_dcx, "8129", "8329", "sub dp[ecx],%d", 5 | C },
	{ sub_ddx, "812A", "832A", "sub dp[edx],%d", 5 | D },
	{ sub_bax, Z, "8028", "sub bp[eax],%d", 1 | A },
	{ sub_bcx, Z, "8029", "sub bp[ecx],%d", 1 | C },
	{ sub_bdx, Z, "802A", "sub bp[edx],%d", 1 | D },
	{ imul_eax_ecx, "0FAFC1", Z, "imul eax,ecx", ACD }, // â€¢â€žÂâ€ â€¢tÂÃ¦Å½Z
	{ imul_eax_eax, "69C0", "6BC0", "imul eax,eax,%d", 5 | AD }, // R <- R*imm
	{ imul_edx_edx, "69D2", "6BD2", "imul edx,edx,%d", 5 | AD }, // R <- R*imm
	{ imul_eax_pbp, "0FAF85", "0FAF45", "imul eax,[ebp%+d]", 5 | A },
	{ xdiv_dbp, "99F7BD", "99F77D", "cdq&idiv dp[ebp%+d]", 5 | AD },
	{ cmp_eax_ecx, "39C8", Z, "cmp eax,ecx", AC },
	{ cmp_ecx_eax, "3BC8", Z, "cmp ecx,eax", AC },
	{ cmp_eax, "81F8", "83F8", "cmp eax,%d", 5 | A },
	{ cmp_eax_pbp, "3B85", "3B45", "cmp eax,[ebp%+d]", 5 | A },
	{ cmp_ah, Z, "80FC", "cmp ah,%d", 1 | A },
	{ test_ah, Z, "F6C4", "test ah,%d", 1 | A },
	{ test_eax_eax, "85C0", Z, "test eax,eax", A },
	{ and_pbp_eax, "2185", "2145", "and [ebp%+d],eax", 5 | A },
	{ and_eax_ecx, "23C1", Z, "and eax,ecx", AC },
	{ and_pcx_eax, "2101", Z, "and [ecx],eax", AC },
	{ and_pcx_ax, "662101", Z, "and [ecx],ax", AC },
	{ and_pcx_al, "2001", Z, "and [ecx],al", AC },
	{ and_ah, Z, "80E4", "and ah,%d", 1 | A },
	{ or_eax_ecx, "0BC1", Z, "or eax,ecx", AC },
	{ or_pcx_eax, "0901", Z, "or [ecx],eax", AC },
	{ or_pcx_ax, "660901", Z, "or [ecx],ax", AC },
	{ or_pcx_al, "0801", Z, "or [ecx],al", AC },
	{ xor_eax_eax, "31C0", Z, "xor eax,eax", A },
	{ xor_ah, Z, "80F4", "xor ah,%d", 1 | A },
	{ xor_pcx_ax, "663101", Z, "xor [ecx],ax", AC },
	{ xor_pcx_eax, "3101", Z, "xor [ecx],eax", AC },
	{ xor_eax_ecx, "33C1", Z, "xor eax,ecx", AC },
	{ xor_pcx_al, "3001", Z, "xor [ecx],al", AC },
	{ shl_eax, Z, "C1E0", "shl eax,%d", 1 | A },
	{ shl_edx, Z, "C1E2", "shl edx,%d", 1 | D },
	{ shl_eax_cl, "D3E0", Z, "shl eax,cl", AC },
	{ shr_eax_cl, "D3E8", Z, "shr eax,cl", AC },
	{ neg_eax, "F7D8", Z, "neg eax", A },
	{ mov_eax_edx, "8BC2", Z, "mov eax,edx", AD },
	{ mov_ecx_eax, "8BC8", Z, "mov ecx,eax", AC },
	{ mov_edx_eax, "8BD0", Z, "mov edx,eax", AD },
	{ mov_eax, "B8", Z, "mov eax,%d", 4 | A },
	{ mov_ecx, "B9", Z, "mov ecx,%d", 4 | C },
	{ mov_dax, "C700", Z, "mov dp[eax],%d", 4 | A },
	{ mov_wax, "66C700", Z, "mov wp[eax],%d", 2 | A },
	{ mov_bax, Z, "C600", "mov bp[eax],%d", 1 | A },
	{ mov_eax_pax, "8B00", Z, "mov eax,[eax]", A },
	{ mov_eax_pcx, "8B01", Z, "mov eax,[ecx]", AC },
	{ mov_pcx_eax, "8901", Z, "mov [ecx],eax", AC },
	{ mov_pcx_ax, "668901", Z, "mov [ecx],ax", AC },
	{ mov_pax_cx, "668908", Z, "mov [eax],cx", AC },
	{ mov_pcx_al, "8801", Z, "mov [ecx],al", AC },
	{ mov_pax_cl, "8808", Z, "mov [eax],cl", AC },
	{ mov_pax_ecx, "8908", Z, "mov [eax],ecx", AC },
	{ mov_ecx_pax, "8B08", Z, "mov ecx,[eax]", AC },
	{ mov_edx_pax, "8B10", Z, "mov edx,[eax]", AD },
	{ mov_eax_pbp, "8B85", "8B45", "mov eax,[ebp%+d]", 5 | A },
	{ mov_ecx_pbp, "8B8D", "8B4D", "mov ecx,[ebp%+d]", 5 | C },
	{ mov_edx_pbp, "8B95", "8B55", "mov edx,[ebp%+d]", 5 | D },
	{ mov_pbp_ecx, "898D", "894D", "mov [ebp%+d],ecx", 5 | C },
	{ mov_pbp_eax, "8985", "8945", "mov [ebp%+d],eax", 5 | A },
	{ mov_pbp_al, "8885", "8845", "mov [ebp%+d],al", 5 | A },
	{ mov_psp_eax, "898424", "894424", "mov [esp%+d],eax", 5 | A },
	{ mov_eax_psp, Z, "8B4424", "mov eax,[esp%+d]", 1 | X },
	{ movsx_eax_wax, "0FBF00", Z, "movsx eax,wp[eax]", A },
	{ movsx_eax_bax, "0FBE00", Z, "movsx eax,bp[eax]", A },
	{ movsx_ecx_wcx, "0FBF09", Z, "movsx ecx,wp[ecx]", C }, // â€¢â€žÂâ€ Å gâ€™Â£
	{ movsx_ecx_bcx, "0FBE09", Z, "movsx ecx,bp[ecx]", C }, // â€¢â€žÂâ€ Å gâ€™Â£
	{ lea_eax_pbp, "8D85", "8D45", "lea eax,[ebp%+d]", 5 | A },
	{ lea_ecx_pbp, "8D8D", "8D4D", "lea ecx,[ebp%+d]", 5 | C },
	{ lea_edx_pbp, "8D95", "8D55", "lea edx,[ebp%+d]", 5 | D },
	{ mov_eax_ad1, "8B0402", Z, "mov eax,[eax+edx]", AD },
	{ mov_eax_ad2, "8B0450", Z, "mov eax,[eax+edx*2]", AD },
	{ mov_eax_ad4, "8B0490", Z, "mov eax,[eax+edx*4]", AD },
	{ mov_eax_ad8, "8B04D0", Z, "mov eax,[eax+edx*8]", AD },
	{ mov_eax_da1, "8B0410", Z, "mov eax,[edx+eax]", AD },
	{ mov_eax_da2, "8B0442", Z, "mov eax,[edx+eax*2]", AD },
	{ mov_eax_da4, "8B0482", Z, "mov eax,[edx+eax*4]", AD },
	{ mov_eax_da8, "8B04C2", Z, "mov eax,[edx+eax*8]", AD },
	{ lea_eax_ad1, "8D0402", Z, "lea eax,[eax+edx]", AD },
	{ lea_eax_ad2, "8D0450", Z, "lea eax,[eax+edx*2]", AD },
	{ lea_eax_ad4, "8D0490", Z, "lea eax,[eax+edx*4]", AD },
	{ lea_eax_ad8, "8D04D0", Z, "lea eax,[eax+edx*8]", AD },
	{ lea_eax_da1, "8D0410", Z, "lea eax,[edx+eax]", AD },
	{ lea_eax_da2, "8D0442", Z, "lea eax,[edx+eax*2]", AD },
	{ lea_eax_da4, "8D0482", Z, "lea eax,[edx+eax*4]", AD },
	{ lea_eax_da8, "8D04C2", Z, "lea eax,[edx+eax*8]", AD },
	{ lea_ecx_da1, "8D0C10", Z, "lea ecx,[edx+eax]", ACD },
	{ lea_ecx_da2, "8D0C42", Z, "lea ecx,[edx+eax*2]", ACD },
	{ lea_ecx_da4, "8D0C82", Z, "lea ecx,[edx+eax*4]", ACD },
	{ lea_ecx_da8, "8D0CC2", Z, "lea ecx,[edx+eax*8]", ACD },
	{ xchg_eax_ecx, "91", Z, "xchg eax,ecx", AC },
	{ cwde, "98", Z, "cwde", A }, // eax <- ax
	{ jmp, "E9", "EB", "jmp ", 5 | X },
	{ jz, "0F84", "74", "jz ", 5 | X },
	{ jnz, "0F85", "75", "jnz ", 5 | X },
	{ jl, "0F8C", "7C", "jl ", 5 | X },
	{ jge, "0F8D", "7D", "jge ", 5 | X },
	{ jle, "0F8E", "7E", "jle ", 5 | X },
	{ jg, "0F8F", "7F", "jg ", 5 | X },
	{ call, "E8", Z, "call fn_%04d", 4 | X },
	{ xent, "5589E5", Z, "push ebp&mov ebp,esp" },
	{ xret, "C9C3", "C9C2", "leave&ret", X },
	{ sete_eax, "B8000000000F94C0", Z, "sete eax", X },  // mov eax,0/sete_al
	{ setne_eax, "B8000000000F95C0", Z, "setne eax", X },  // mov eax,0/setne_al
	{ setl_eax, "B8000000000F9CC0", Z, "setl eax", X },
	{ setge_eax, "B8000000000F9DC0", Z, "setge eax", X },
	{ setle_eax, "B8000000000F9EC0", Z, "setle eax", X },
	{ setg_eax, "B8000000000F9FC0", Z, "setg eax", X },
	{ fchs, "D9E0", Z, "fchs" },
	{ fxch_st1, "D9C9", Z, "fxch st(1)" },
	{ fld_qax, "DD00", Z, "fld qp[eax]", A },
	{ fld_qcx, "DD01", Z, "fld qp[ecx]", C },
	{ fst_qax, "DD10", Z, "fst qp[eax]", A },
	{ fst_qcx, "DD11", Z, "fst qp[ecx]", C },
	{ fld_qbp, "DD85", "DD45", "fld qp[ebp%+d]", 5 },
	{ fld_qp, "DD05", Z, "fld qp[off_%p]", 4 },
	{ fldcw, "D92D", Z, "fldcw [off_%p]", 4 },
	{ fstp_qsp, "DD5C2400", Z, "fstp qp[esp]" },
	{ fstsw, "DFE0", Z, "fstsw" },
	{ fstp_st1, "DDD9", Z, "fstp st1" },
	{ fst_qbp, "DD95", "DD55", "fst qp[ebp%+d]", 5 },
	{ fadd_qbp, "DC85", "DC45", "fadd qp[ebp%+d]", 5 },
	{ fsub_qbp, "DCA5", "DC65", "fsub qp[ebp%+d]", 5 },
	{ fmul_qbp, "DC8D", "DC4D", "fmul qp[ebp%+d]", 5 },
	{ fdiv_qbp, "DCB5", "DC75", "fdiv qp[ebp%+d]", 5 },
	{ fadd_qp, "DC05", Z, "fadd qp[%d]", 4 },
	{ fsub_qp, "DC25", Z, "fsub qp[%d]", 4 },
	{ fmul_qp, "DC0D", Z, "fmul qp[%d]", 4 },
	{ fdiv_qp, "DC35", Z, "fdiv qp[%d]", 4 },
	{ faddp_st1_st, "DEC1", Z, "faddp st1,st" },
	{ fmulp_st1_st, "DEC9", Z, "fmulp st1,st" },
	{ fsubrp_st1_st, "DEE9", Z, "fsubrp st1,st" },
	{ fdivrp_st1_st, "DEF9", Z, "fdivrp st1,st" },
	{ fistp_dsp, Z, "DB5C24", "fistp dp[esp%+d]", 1 },
	{ fild_dax, "DB00", Z, "fild dp[eax]", A },
	{ fild_dsp, "DB8424", "DB4424", "fild dp[esp%+d]", 5 },
	{ fucompp, "DAE9", Z, "fucompp" },
	{ xdiv_ecx, "99F7F9", Z, "cdq&idiv ecx", X },
	{ xmod_ecx, "99F7F989D0", Z, "cdq&idiv ecx&mov eax,edx", X },
	{ setint, "", Z, "setint %d @%d" },
	{ setreal, "", Z, "setreal %.3f @%d" },
	{ setstr, "", Z, "setstr '%s' @%d" },
	{ fn_, Z, Z, "fn_%04d:", 4 },
	{ exp_, Z, Z, "exp fn_%04d:", 4 },
	{ loc_, Z, Z, "loc_%03d:", 4 },
};

int cmpInst(const void *a, const void *b)
{
	return ((INSTRUCTION*)a)->opcode - ((INSTRUCTION*)b)->opcode;
}

void initInstruction()
{
	int n, size1 = sizeof(INSTRUCTION);
	qsort(instruction, sizeof(instruction) / size1, size1, cmpInst);
	for (n = 0; n < sizeof(instruction) / size1; n++)
	{
		INSTRUCTION *pI = &instruction[n];
		//printf("%s opcode=%d n+1=%d\n", pI->mnemonic, pI->opcode, n+1);
		if (pI->opcode != n + 1)
			error("INST", "mismatch! %s opcode=%d n+1=%d", pI->mnemonic, pI->opcode, n + 1);
	}
}

char *toStr(int tix)
{
	return cd.token[tix].token;
}
char *toString(int id)
{
	return cd.hash.tbl[id].key;
}

int cmpSeq(const void *a, const void *b)
{
	return ((HDATA*)a)->seq - ((HDATA*)b)->seq;
}
void printNameTable(int ixBlk)
{
	Block blk = cd.block[ixBlk];
	printf("------ %-16s#%d [%2d]-------\n", toString(blk.idFunc), blk.blockDepth, ixBlk);
	printf("%25s  %-25s %s\n", "dtype", "name", "ptrs atype   addr    size");
	qsort(blk.hash.tbl, blk.hash.size, sizeof(HDATA), cmpSeq);
	int n;
	for (n = 0; n < blk.hash.size; n++)
	{
		Name *e = blk.hash.tbl[n].val;
		if (e == NULL) continue;
		char *fmt = strlen(toString(e->dataType)) < 26 ? "%25s  %-25s" : "%.23s..  %.23s..";
		printf(fmt, toString(e->dataType), toString(e->idName));
		printf("   %d%5d%9d%8d\n", e->ptrs, e->addrType, e->address, e->size[0]);
	}
}

void createNameTable(int idFunc)
{
	if (++cd.currTable > 1) idFunc = cd.block[cd.currTable - 1].idFunc;
	cd.block[cd.currTable].idFunc = idFunc;
	cd.block[cd.currTable].blockDepth = cd.currTable;
	initHash('n', cd.currTable == 0 ? 1000 : 4, &cd.block[cd.currTable].hash);
}

void deleteNameTable()
{
	if (opt&oNAME) printNameTable(cd.currTable);
	freeHash(&cd.block[cd.currTable--].hash);
}

Name *newName(int type, int dataType, int name, int addrType, int address)
{
	Name buf = { type, name, dataType, addrType, address };
	Name *pNew = xalloc(sizeof(Name));
	memcpy(pNew, &buf, sizeof(Name));
	pNew->argc = -1;
	return pNew;
}

Name *putName(int ixBlk, Name *pName)
{
	char key[128];
	int type = pName->type;
	char *name = toString((type&NM_STRUCT) ? pName->dataType : pName->idName);
	sprintf(key, "%s%s", (type&NM_FUNC) ? "@" : (type&NM_STRUCT) ? "$" : "", name);
	put(key, pName, &cd.block[ixBlk].hash);
	return pName;
}

Name *appendName(int ixBlk, int type, int dataType, int name, int addrType, int address)
{
	return putName(ixBlk, newName(type, dataType, name, addrType, address));
}

Name *getNameFromTable(int ixBlk, int type, int name)
{
	char key[128];
	sprintf(key, "%s%s", (type&NM_FUNC) ? "@" : (type&NM_STRUCT) ? "$" : "", toString(name));
	return get(key, &cd.block[ixBlk].hash);
}

Name *getNameFromAllTable(int ixBlk, int type, int name)
{
	for (; ixBlk >= 0; ixBlk--)
	{
		Name *e = getNameFromTable(ixBlk, type, name);
		if (e != NULL) return e;
	}
	return NULL;
}

Name *getStruct(int type)
{
	return getNameFromTable(globTable, NM_STRUCT, type);
}

int getPtr(int type)
{
	Name *pStruct = getStruct(type);
	return pStruct != NULL ? pStruct->ptrs : 0;
}

Name *getAttr(int type, int name)
{
	Name *e = getStruct(type)->pBgn;
	while (e != NULL && e->idName != name) e = e->pNext;
	return e;
}

Name *getMember(int type, int n)
{
	Name *e = getStruct(type)->pBgn;
	while (n-- > 0 && e != NULL) e = e->pNext;
	return e;
}

void reallocCode(int size)
{
	if (ix.ixCode + size + 10 < cd.sizeCode) return;
	cd.sizeCode = cd.sizeCode * 3 / 2;
	cd.pCode = xrealloc(cd.pCode, cd.sizeCode*sizeof(INSTRUCT));
}

void outCode3(int code, int num, int attr)
{
	if (code <= 0) error("outCode3", "code = %d", code);
	reallocCode(1);
	INSTRUCT inst = { code, num, attr };
	memcpy(&cd.pCode[ix.ixCode], &inst, sizeof(inst));
	cd.pCode[ix.ixCode++].regs = instruction[code - 1].regs_size & 0xFF00;
}
void outCode2(int code, int num)
{
	outCode3(code, num, 0);
}
void outCode1(int code)
{
	outCode3(code, 0, 0);
}

void delCodes(int from, int to)
{
	memmove(&cd.pCode[from], &cd.pCode[to], (ix.ixCode - to)*sizeof(INSTRUCT));
	ix.ixCode -= (to - from);
}
void delCode(int n)
{
	delCodes(n, n + 1);
}

void jumpFalse(int loc, char *msg)
{
	int in = cd.pCode[ix.ixCode - 1].inst;
	if (in >= sete_eax && in <= setg_eax)  	// jNCC loc
	{
		cd.pCode[ix.ixCode - 1].inst = in == setg_eax ? jle : in == setge_eax ? jl
			: in == setl_eax ? jge : in == setle_eax ? jg : in == sete_eax ? jnz : jz;
		cd.pCode[ix.ixCode - 1].num = loc;
		//printf("jumpFalse %s\n", msg);
	}
	else  					// setCC/test eax,eax/jz loc
	{
		outCode1(test_eax_eax);
		outCode2(jz, loc);
	}
}

void jumpTrue(int loc, char *msg)
{
	int in = cd.pCode[ix.ixCode - 1].inst;
	if (in >= sete_eax && in <= setg_eax)  	// jCC loc
	{
		cd.pCode[ix.ixCode - 1].inst = in == setg_eax ? jg : in == setge_eax ? jge
			: in == setl_eax ? jl : in == setle_eax ? jle : in == sete_eax ? jz : jnz;
		cd.pCode[ix.ixCode - 1].num = loc;
		//printf("jumpTrue  %s\n", msg);
	}
	else  					// setCC/test eax,eax/jnz loc
	{
		outCode1(test_eax_eax);
		outCode2(jnz, loc);
	}
}

void outData(int inst, int offset)
{
	reallocCode(1);
	cd.pCode[ix.ixCode].inst = inst;
	cd.pCode[ix.ixCode].offset = offset;
}
void outInteger(int offset, int size, int ival)
{
	outData(setint, offset);
	cd.pCode[ix.ixCode].attr = size;
	cd.pCode[ix.ixCode++].num = ival;
}
void outReal(int offset, double dval)  		// setreal dval,offset
{
	outData(setreal, offset);
	cd.pCode[ix.ixCode++].dval = dval;
}
int outString(int offset, char *sval)
{
	outData(setstr, offset);
	cd.pCode[ix.ixCode++].sval = sval;
	return xstrlen(sval);
}

void outDataChar(int c)
{
	outInteger(ix.ixData, 1, c);
	ix.ixData++;
}
void outDataShort(int n)
{
	outInteger(ix.ixData, 2, n);
	ix.ixData += 2;
}
void outDataInt(int n)
{
	outInteger(ix.ixData, 4, n);
	ix.ixData += 4;
}
void outDataAddr(int p)
{
	outInteger(ix.ixData, 0, p);
	ix.ixData += 4;
}
void outDataDouble(double d)
{
	outReal(ix.ixData, d);
	ix.ixData += 8;
}

int xstrcpy(char *p, char *q)
{
	int len = 0;
	while (*q)
	{
		int fKanji = isKanji(*q & 0xff);
		len += fKanji ? 2 : 1;
		if (p != NULL && fKanji)
		{
			*p++ = *q;
			*p++ = q[1];
		}
		else if (p != NULL)
		{
			*p++ = *q == '\\' ? esc_char(q + 1) : *q;
		}
		q += *q == '\\' ? (q[1] == 'x' ? 4 : 2) : (fKanji ? 2 : 1);
	}
	return len;
}
int xstrlen(char *p)
{
	return xstrcpy(NULL, p);
}

void loadAddr(type, addr)
{
	if (type == AD_STACK) outCode2(lea_eax_pbp, addr);
	else outCode3(mov_eax, addr, type);
}

void loadValue(int type, int fPtr)
{
	int *pI = &cd.pCode[ix.ixCode - 1].inst;
	if (type == ID.INT || fPtr)
	{
		if (*pI >= lea_eax_ad1 && *pI <= lea_eax_ad8)
			*pI = mov_eax_ad1 + (*pI - lea_eax_ad1);
		else if (*pI == lea_eax_pbp) *pI = mov_eax_pbp;
		else outCode1(mov_eax_pax);
	}
	else if (type == ID.CHAR)
	{
		outCode1(movsx_eax_bax);
	}
	else if (type == ID.SHORT)
	{
		outCode1(movsx_eax_wax);
	}
	else if (type == ID.DOUBLE)
	{
		if (*pI == lea_eax_pbp) *pI = fld_qbp;
		else if (*pI == mov_eax) *pI = fld_qp;
		else outCode1(fld_qax);
	}
	else
	{
		error("code.loadValue", "undefined type '%s'", toString(type));
	}
}

void setFpuStack2(int type1, int type2)
{
	if (type1 == ID.INT)
	{
		outCode2(fild_dsp, 0);
		outCode2(add_esp, 4);
		outCode1(fxch_st1);
	}
	else if (type2 == ID.INT)
	{
		outCode2(mov_psp_eax, -4);   // outCode1(push_eax);
		outCode2(fild_dsp, -4);	     // outCode2(fild_dsp, 0); outCode2(add_esp, 4);
	}
}

int loc()
{
	return ++ix.ixLoc;
}

int id2(char* op)
{
	return (op[0] * 137 + op[1]) % cd.hash.size;
}
int isId(int n)
{
	return cd.token[ix.tix + n].type <= TK_NAME;    // TK_SYMBOL or TK_NAME
}
int is(int id)
{
	return isId(0) && cd.token[ix.tix].ival == id;
}
int isN(int id, int n)
{
	return isId(n) && cd.token[ix.tix + n].ival == id;
}
int is2(char *op)
{
	return isId(0) && cd.token[ix.tix].ival == id2(op);
}
int id(char* str)
{
	return put(str, NULL, &cd.hash);
}
int ispp(int id)
{
	if (is(id))
	{
		ix.tix++;
		return 1;
	}
	else return 0;
}
int is2pp(char *op)
{
	if (is2(op))
	{
		ix.tix++;
		return 1;
	}
	else return 0;
}
void skip(int id)
{
	if (!ispp(id)) error("code.skip", "'%s' expected", toString(id));
}

void setValue(int mode, int ptrs, int type, VALUE *pv)
{
	VALUE val = { mode, ptrs, type };
	memcpy(pv, &val, sizeof(VALUE));
}

int sizeOfDataType(int type)
{
	if (type == ID.CHAR || type == ID.HVOID) return 1;
	if (type == ID.SHORT)  return 2;
	if (type == ID.INT || type == ID.FLOAT) return 4;
	if (type == ID.DOUBLE) return 8;
	Name *pName = getStruct(type);
	return pName != NULL ? pName->size[0] : 0;
}

typedef struct _Variable
{
	int tag, type, id;
	int width;
	int pointers, arrays;
	int size[8], length;
} Variable;

Variable var;

void init();
void setToken(TOKEN *token, HASH *pHash);
void program();
void functionDefinition();
void variableDeclaration(Name *pStruct, int status);
void structDeclaration();
void enumDefinition();

void parse()
{
	int n;
	cd.sizeCode = 10000;
	cd.pCode = xalloc(cd.sizeCode * sizeof(INSTRUCT));
	init();
	for (n = 0; n < cd.nToken; n++)
	{
		setToken(&cd.token[n], &cd.hash);
		if (opt&oTOKEN) printToken(n);
	}
	cd.currTable = -1;
	createNameTable(id("<global>"));
	program();
	for (n = 0; n < ix.ixCode - 2; n++)  	// 160:    jz loc_018	  jnz loc_017
	{
		int inst = cd.pCode[n].inst;	// 161:    jmp loc_017    jmp loc_018
		int num = cd.pCode[n].num;	// 162: loc_018:[48]   loc_018
		if ((inst == jge || inst == jl || inst == jz || inst == jnz)
			&& cd.pCode[n + 1].inst == jmp && cd.pCode[n + 2].num == num)
		{
			cd.pCode[n].num = cd.pCode[n + 1].num;
			cd.pCode[n + 1].num = num;
			cd.pCode[n].inst = inst == jge ? jl : inst == jl ? jge : inst == jz ? jnz : jz;
		}
	}
	for (n = 0; n < ix.ixCode - 1; n++)  	// jmp 0
	{
		if (cd.pCode[n].inst == jmp && cd.pCode[n + 1].inst == loc_
			&& cd.pCode[n].num == cd.pCode[n + 1].num) delCode(n);
	}
}

int isFunctionDefinition()
{
	int n = is(ID.DECLSP) ? 5 : 1;
	if (is(ID.STATIC)) return FALSE;
	if (n == 1 && !isTypeSpecifier(ix.tix)) return FALSE;
	while (ix.tix + n < cd.nToken && cd.token[ix.tix + n].ival == '*')
		n++;
	if (cd.token[ix.tix + n].ival == ID.HWINAPI) return TRUE;
	return cd.token[ix.tix + n + 1].ival == '(';
}

/// Program ::= (FunctionDefinition | VariableDeclaration ";")*
void program()
{
	for (ix.tix = 0; ix.tix + 2 < cd.nToken;)
	{
		if (opt&oTRACE) printToken(ix.tix);
		if (is(ID.TYPEDEF) && ix.tix + 5 < cd.nToken
			&& cd.token[ix.tix + 3].ival != '{')
		{
			ix.tix += 5; // typedef struct <tag-name> <type-name>;
		}
		else if (is(ID.TYPEDEF))
		{
			structDeclaration();
		}
		else if (is(ID.ENUM))
		{
			enumDefinition();			// enum
		}
		else if (isFunctionDefinition())
		{
			functionDefinition();
		}
		else
		{
			variableDeclaration(NULL, ST_GVAR);
			skip(';');
		}
	}
}

/// TypeSpecifier ::= "void" | "char" | "short" | "int" | "float" | "double"
void typeSpecifier()
{
	if (is(ID.DECLSP)) ix.tix += 4;  // __declspec(dllexport) | __declspec(dllimport)
	int fStruct = is(ID.STRUCT);
	if (fStruct || is(ID.TYPEDEF) || is(ID.HCONST)) ix.tix++;
	if (!fStruct && !isTypeSpecifier(ix.tix))
		error("typeSpecifier", "'%s' undeclared", toStr(ix.tix));
	var.type = cd.token[ix.tix++].ival;
}

int isTypeSpecifier(int ix)
{
	int hix = cd.token[ix].ival;
	return cd.token[ix].type == TK_NAME && ((int)cd.hash.tbl[hix].val)&AT_TYPE;
}

void setAttr(Name *pName)
{
	int n = var.size[0] == 0 ? 0 : var.arrays;
	pName->ptrs = var.pointers + var.arrays;
	pName->size[n] = var.width;
	while (--n >= 0) pName->size[n] = pName->size[n + 1] * var.size[n];
}	//  int d[5][3][6]   { 5*3*6*4, 3*6*4, 6*4, 4 }

void countMember(int depth, int *dim)
{
	VALUE v;
	int n;
	for (n = 0; !is('}'); n++)
	{
		if (ispp('{')) countMember(depth + 1, dim);
		else assignExpression(VAL, &v);
		if (!ispp(',')) break;
	}
	skip('}');
	if (++n > dim[depth]) dim[depth] = n;
}

void constExpression(VALUE *pv)
{
	int ixCodeSave = ix.ixCode;
	int ixDataSave = ix.ixData;
	conditionalExpression(VAL, pv);
	ix.ixCode = ixCodeSave;	// mov eax,imm
	ix.ixData = ixDataSave;
}

int constIntExpression()
{
	VALUE v;
	constExpression(&v);
	if (!v.fConst || v.type != ID.INT)
		error("parser", "constant expression expected");
	return v.ival;
}

/// VarDeclarator ::= ("*")* ["WINAPI"] Identifier ("[" [Constant] "]")*
int varDeclarator(int fParam)
{
	int n, fCount = FALSE, callConv = NM_CDECL;

	for (var.pointers = 0; ix.tix < cd.nToken && ispp('*');)
		var.pointers++;
	if (ispp(ID.HWINAPI)) callConv = NM_WINAPI;
	if (cd.token[ix.tix].type == TK_SYMBOL) var.id = -1;
	else var.id = cd.token[ix.tix++].ival;
	memset(&var.size, 0, sizeof(var.size));
	for (var.arrays = 0; var.arrays < 8 && ix.tix < cd.nToken && ispp('['); var.arrays++)
	{
		if (is(']'))
		{
			if (!fParam) fCount = TRUE;
		}
		else
		{
			var.size[var.arrays] = constIntExpression();
		}
		skip(']');
	}
	if (is('=') && fCount)
	{
		INDEX ixSave = ix;
		memset(var.size, 0, sizeof(var.size));
		if (cd.token[ix.tix + 1].type == TK_STRING)
		{
			var.size[0] = xstrlen(toString(cd.token[ix.tix + 1].ival)) + 1;
		}
		else
		{
			ix.tix += 2;	// '=' â€š '{'
			countMember(0, var.size);
		}
		ix = ixSave;
	}
	if (fParam)
	{
		var.pointers += var.arrays;
		var.arrays = 0;
	}
	var.width = var.pointers > 0 ? 4 : sizeOfDataType(var.type);
	var.length = 1;
	for (n = 0; n < var.arrays; n++)
		var.length *= var.size[n];
	return callConv;
}

void initializer(Name *name, int atype, int depth, int addr);
int  align(int pos, int width)
{
	if (pos == 0) return 0;
	if ((width & 7) == 0) return (pos + 7) & ~7;  // 8byte
	if ((width & 3) == 0) return (pos + 3) & ~3;  // 4byte.
	if ((width & 1) == 0) return (pos + 1) & ~1;  // 2byte
	return pos;
}

/// VariableDeclaration ::= TypeSpecifier VarDeclarator ["=" Initializer]
///				("," VarDeclarator ["=" Initializer])*
void variableDeclaration(Name *pStruct, int status)
{
	Name *pName;
	int fStatic = ispp(ID.STATIC);
	int fImp = is(ID.DECLSP);	// Import

	typeSpecifier();
	do
	{
		varDeclarator(FALSE);
		if (status == ST_FUNC)
		{
			if (!fStatic)
			{
				cd.baseSpace = -align(-cd.baseSpace, var.width);
				cd.baseSpace -= var.width * var.length;
				pName = newName(NM_VAR, var.type, var.id, AD_STACK, cd.baseSpace);
			}
			else  	// Ex. static int n;
			{
				ix.ixData = align(ix.ixData, var.width);
				pName = newName(NM_VAR, var.type, var.id, AD_DATA, ix.ixData);
			}
			putName(cd.currTable, pName);
		}
		else if (fImp && status == ST_GVAR)
		{
			pName = appendName(globTable, NM_VAR, var.type, var.id, AD_IMPORT, 0);
		}
		else if (status == ST_GVAR)
		{
			ix.ixData = align(ix.ixData, var.width);
			pName = appendName(globTable, NM_VAR, var.type, var.id, AD_DATA, ix.ixData);
		}
		else    // status==ST_STRUCT
		{
			cd.offset = align(cd.offset, var.width);
			pName = xalloc(sizeof(Name));
			Name buf = { NM_ATTR, var.id, var.type, 0, cd.offset };
			memcpy(pName, &buf, sizeof(Name));
			if (pStruct->pBgn == NULL) pStruct->pBgn = pName;
			else pStruct->pEnd->pNext = pName;
			pStruct->pEnd = pName;
			cd.offset += var.width * var.length;
		}
		setAttr(pName);
		if (ispp('='))
		{
			int ixString = ix.ixData + pName->size[0];
			if (status == ST_GVAR) ix.ixString = ixString;
			initializer(pName, pName->addrType, 0, pName->address);
			if (ix.ixString > ixString) ix.ixData = ix.ixString;
		}
		else if (!fImp && (status == ST_GVAR || fStatic))
		{
			ix.ixZero = align(ix.ixZero, var.width);
			pName->addrType = AD_ZERO;
			pName->address = ix.ixZero;
			ix.ixZero += pName->size[0];
		}
	} while (ispp(','));
}

void initMember(Name *pName, int atype, int depth, int addr)
{
	VALUE v1, v2;
	setValue(VAL, pName->ptrs, pName->dataType, &v1);
	if (atype == AD_STACK)
	{
		int left_bgn = ix.ixCode;
		loadAddr(AD_STACK, addr);
		int left_end = ix.ixCode;
		outCode1(push_eax);
		assignExpression(VAL, &v2);
		infixOperation('=', &v1, &v2, left_bgn, left_end);
	}
	else    // type == AD_DATA:
	{
		if (v1.ptrs - depth == 1 && v1.type == ID.CHAR)     // char *s = "abc";
		{
			if (cd.token[ix.tix].type == TK_STRING)
			{
				int ival = cd.token[ix.tix++].ival;
				outDataAddr(ix.ixString);
				outString(ix.ixString, toString(ival));
				ix.ixString += xstrlen(toString(ival)) + 1;
			}
			else
			{
				constExpression(&v2);
				outDataInt(v2.ival);
			}
		}
		else
		{
			constExpression(&v2);
			if (!v2.fConst) error("initMember", "constant expression expected");
			else if (v1.type == ID.DOUBLE)
				outDataDouble(v2.type == ID.DOUBLE ? v2.rval : (double)v2.ival);
			else if (v2.type != ID.INT) error("initMember", "type mismatch");
			else if (v1.type == ID.INT)   outDataInt(v2.ival);
			else if (v1.type == ID.SHORT) outDataShort(v2.ival);
			else if (v1.type == ID.CHAR)  outDataChar(v2.ival);
			else error("initMember", "v1.type=%d", v1.type);
		}
	}
}

void clear(int bgn, int end)
{
	outCode2(push, end - bgn);
	outCode2(push, 0);
	outCode2(lea_eax_pbp, bgn);
	outCode1(push_eax);
	outCode2(call, id("memset"));
	outCode2(add_esp, 12);
}

/// Initializer ::= AssignExpression | "{" Initializer ( "," Initializer )* "}"
void initializer(Name *pName, int atype, int depth, int addr)
{
	int  n, bgn;
	int  fArray = (pName->size[depth + 1] > 0);

	if (!ispp('{'))
	{
		if (fArray && pName->dataType == ID.CHAR && cd.token[ix.tix].type == TK_STRING)
		{
			int ival = cd.token[ix.tix++].ival;	 	// char s[] = "abc";
			if (atype == AD_DATA)
			{
				ix.ixData += outString(ix.ixData, toString(ival)) + 1;
			}
			else
			{
				int size = pName->size[depth] / pName->size[depth + 1];
				int length = xstrlen(toString(ival)) + 1;
				if (length > size) error("init'r", "initializer-string too long");
				char *buf = xalloc(length);
				xstrcpy(buf, toString(ival));
				for (n = 0; n < size; n++)
				{
					outCode2(mov_eax, n < length ? buf[n] & 0xff : 0);
					outCode2(mov_pbp_al, addr + n); 	// mov [ebp-disp],al
				}
				free(buf);
			}
			return;
		}
		initMember(pName, atype, depth, addr);
		return;
	}
	if (fArray)
	{
		int size1 = pName->size[depth + 1];
		int num = pName->size[depth] / size1;
		for (n = 0; !is('}');)
		{
			if (n >= num) error("init'r", "index too large");
			initializer(pName, atype, depth + 1, addr + size1*n++);
			if (!ispp(',')) break;
		}
		bgn = size1 * n;
	}
	else
	{
		bgn = pName->size[depth];
		for (n = 0; !is('}');)
		{
			Name *pChild = getMember(pName->dataType, n++);
			if (pChild == NULL) error("init'r", "too many field init");
			initializer(pChild, atype, 0, addr + pChild->address);
			bgn = pChild->address + pChild->size[0];
			if (!ispp(',')) break;
		}
	}
	if (atype == AD_STACK && bgn < pName->size[depth])
		clear(addr + bgn, addr + pName->size[depth]);
	skip('}');
}

/// StructDecl ::= "typedef" "struct" Identifier "{" (VarDecl)* ";" "}" Identifier ";"
void structDeclaration()
{
	ix.tix += 2;	// "typedef" "struct"
	int tagId = cd.token[ix.tix++].ival;
	Name *pName = newName(NM_STRUCT, 0, tagId, -1, 0);
	cd.offset = 0;
	int sizeAlign = 1;
	for (skip('{'); !ispp('}'); skip(';'))
	{
		variableDeclaration(pName, ST_STRUCT);
		if (var.type == ID.SHORT && sizeAlign < 2) sizeAlign = 2;
		if ((var.type == ID.INT || var.type == ID.FLOAT) && sizeAlign < 4) sizeAlign = 4;
		if (var.type == ID.DOUBLE && sizeAlign < 8) sizeAlign = 8;
	}
	if (sizeAlign > 1)
		cd.offset = align(cd.offset, sizeAlign);
	pName->size[0] = cd.offset;
	if (ispp('*')) pName->ptrs = 1;
	int nameId = cd.token[ix.tix++].ival;
	pName->dataType = nameId;
	putName(globTable, pName);
	cd.hash.tbl[nameId].val = (void*)((int)cd.hash.tbl[nameId].val | AT_TYPE);// Æ’fÂ[Æ’^Å’^.
	skip(';');
}

/// ParameterDeclaration ::= TypeSpecifier  VarDeclarator ["(" ")"] | "..."
void parameterDeclaration()
{
	typeSpecifier();
	varDeclarator(TRUE);
	if (ispp('(') && !ispp(')'))
		error("paramDecl", "')' expected");
	Name *pName = appendName(cd.currTable, NM_VAR, var.type, var.id, AD_STACK, cd.baseSpace);
	setAttr(pName);
	cd.baseSpace += var.width;
}

/// FunctionDefinition ::= TypeSpecifier  VarDeclarator
///     "(" [ VarDeclaration ("," VarDeclaration)* ] ")" (CompoundStatement | ";")
void functionDefinition()
{
	int callConv = NM_CDECL;	// Calling Convention
	int fRtnStmt = FALSE;
	int fProtoType = FALSE;
	int ixCodeSave = ix.ixCode;
	int ixSp, n, argc, ptrs;
	PTRS_TYPE argpt[256];

	int fExp = is(ID.DECLSP);	// Export
	typeSpecifier();
	if (varDeclarator(FALSE) == NM_WINAPI) callConv = NM_WINAPI;
	int idFunc = var.id;
	createNameTable(var.id);
	outCode2((fExp ? exp_ : fn_), var.id);
	Name *pName = getNameFromTable(globTable, NM_CDECL, var.id);
	if (pName == NULL)
	{
		pName = appendName(globTable, callConv, var.type, var.id, AD_CODE, 0);
		pName->ptrs = var.pointers + var.arrays + getPtr(var.type);
	}
	else if (pName->dataType != var.type || pName->type != callConv)
	{
		error("funcDef", "incompatible types for redefinition of '%s'", toString(var.id));
	}
	cd.baseSpace = 4 * 2;
	outCode1(xent);  // push ebp, mov ebp,esp
	ixSp = ix.ixCode;
	outCode2(sub_esp, 0);
	argc = 0;
	for (skip('('); !ispp(')'); ispp(','))
	{
		int fD3 = ispp(ID.DOTS3);
		if (!fD3)
		{
			parameterDeclaration();
			ptrs = var.pointers + var.arrays + getPtr(var.type);
		}
		argpt[argc].ptrs = fD3 ? 0 : ptrs;
		argpt[argc++].type = fD3 ? ID.DOTS3 : var.type;
	}
	pName->argc = argc;
	int sizeRet = callConv == NM_WINAPI ? (cd.baseSpace - 8) : 0;
	if (argc > 0)
	{
		pName->argpt = calloc(argc, sizeof(PTRS_TYPE));
		memcpy(pName->argpt, argpt, argc * sizeof(PTRS_TYPE));
	}
	cd.baseSpace = 0;
	if (ispp(';')) fProtoType = TRUE;
	else fRtnStmt = compoundStatement(0, 0, sizeRet);
	if (cd.baseSpace < 0)
		cd.baseSpace = -align(-cd.baseSpace, 4);
	if (cd.baseSpace != 0)
		cd.pCode[ixSp].num = -cd.baseSpace;
	else
		memmove(&cd.pCode[ixSp], &cd.pCode[ixSp + 1], (--ix.ixCode - ixSp)*sizeof(INSTRUCT));
	if (!fRtnStmt) outCode2(xret, sizeRet);
	deleteNameTable();
	if (fProtoType) ix.ixCode = ixCodeSave;
	else cd.hash.tbl[idFunc].val += (fExp ? AT_EXPT : AT_USER);
}

void setToken(TOKEN *token, HASH *pHash)
{
	char *tkstr = token->token;
	if (token->type == TK_SYMBOL || token->type == TK_NAME)
	{
		token->ival = put(tkstr, NULL, pHash);
	}
	else if (token->type == TK_NUMBER)
	{
		token->type = strchr(tkstr, '.') != NULL ? TK_DOUBLE : TK_INT;
		if (token->type == TK_DOUBLE) token->dval = strtod(tkstr, NULL);
		else token->ival = strtoul(tkstr, NULL, tolower(tkstr[1]) == 'x' ? 16 : 10);
	}
	else if (tkstr[0] == '\'')
	{
		token->type = TK_CHAR;
		token->ival = tkstr[1] != '\\' ? tkstr[1] : esc_char(tkstr + 2);
	}
	else
	{
		char *pTail = &tkstr[strlen(tkstr) - 1];
		if (*pTail == '\"') *pTail = '\0';
		token->ival = put(tkstr + 1, NULL, pHash);
	}
}

void enumDefinition()
{
	int val = 0;
	skip(ID.ENUM);
	for (skip('{'); !ispp('}'); ispp(','))
	{
		int hid = cd.token[ix.tix++].ival;
		Name *pName = appendName(globTable, NM_ENUM, 0, hid, -1, val++);
		if (ispp('='))
		{
			val = cd.token[ix.tix++].ival;
			pName->address = val++;
		}
	}
	skip(';');
}

typedef struct _Keyword
{
	int *id;
	char *name;
} Keyword;
void init()
{
	int n;
	char *op2 = OPERATOR2, s[2] = { 0, '\0' }, s2[3];
	Keyword dataType[] =
	{
		{ &ID.HVOID, "void" }, { &ID.CHAR, "char" }, { &ID.SHORT, "short" },
		{ &ID.INT, "int" }, { &ID.FLOAT, "float" }, { &ID.DOUBLE, "double" },
		{ &ID.STATIC, "static" }, { &ID.STRUCT, "struct" }
	};
	Keyword keyword[] =
	{
		{ &ID.IF, "if" }, { &ID.ELSE, "else" }, { &ID.WHILE, "while" },
		{ &ID.DO, "do" }, { &ID.FOR, "for" },
		{ &ID.RETURN, "return" }, { &ID.CONTINUE, "continue" }, { &ID.BREAK, "break" },
		{ &ID.SWITCH, "switch" }, { &ID.CASE, "case" }, { &ID.DEFAULT, "default" },
		{ &ID.SIZEOF, "sizeof" }, { &ID.DOTS3, "..." },
		{ &ID.TYPEDEF, "typedef" }, { &ID.HWINAPI, "WINAPI" }, { &ID.ENUM, "enum" },
		{ &ID.HCONST, "const" }, { &ID.EXTERN, "extern" }, { &ID.DECLSP, "__declspec" },
		{ &ID.DLLIMPT, "dllimport" }, { &ID.DLLEXPT, "dllexport" }
	};
	for (*s = ' ' + 1; *s < 127; (*s)++)
	{
		if (isgraph(*s) && !isdigit(*s)) put(s, NULL, &cd.hash);
	}
	for (n = 0; op2[n] != '\0'; n += 3)
	{
		sprintf(s2, "%c%c", op2[n], op2[n + 1]);
		if (id2(&op2[n]) != put(s2, NULL, &cd.hash))
			error("parser.init", "ÂÃ•â€œÃ‹!! Æ’nÆ’bÆ’VÆ’â€¦Æ’TÆ’CÆ’Yâ€šÃ°Ë†Ã¡â€šÂ¤â€˜fÂâ€â€šÃ‰â€¢Ãâ€šÂ¦â€šÃ©");
	}
	for (n = 0; n < sizeof(dataType) / sizeof(Keyword); n++)
		*(dataType[n].id) = put(dataType[n].name, (void*)AT_TYPE, &cd.hash);
	for (n = 0; n < sizeof(keyword) / sizeof(Keyword); n++)
		*(keyword[n].id) = put(keyword[n].name, NULL, &cd.hash);
}

//------------------------------------------------------------------------//
//                                Statement                               //
//------------------------------------------------------------------------//
void statement(int locBreak, int locContinue, int sizeRet);

void expr(int mode)
{
	VALUE v;
	expression(mode, &v);
}
void expr2(int mode)
{
	skip('(');
	expr(mode);
	skip(')');
}

/// CompoundStatement :== "{" (VariableDeclaration ";" | Statement)* "}"
int compoundStatement(int locBreak, int locContinue, int sizeRet)
{
	createNameTable(-1);
	for (skip('{'); !ispp('}');)
	{
		if (cd.token[ix.tix + 1].ival != '(' && isTypeSpecifier(ix.tix))
		{
			variableDeclaration(NULL, ST_FUNC);
			skip(';');
		}
		else
		{
			statement(locBreak, locContinue, sizeRet);
		}
	}
	deleteNameTable();
	int n = ix.ixCode;
	while (--n > 0 && (cd.pCode[n].inst == loc_ || cd.pCode[n].inst == fn_));
	return cd.pCode[n].inst == xret;
}

/// IfStatement ::= "if" "(" Expression ")" Statement [ "else" Statement ]
void ifStatement(int locBreak, int locContinue, int sizeRet)
{
	int locElse = loc(), locEnd = -1;
	ix.tix++;
	expr2(VAL);
	jumpFalse(locElse, "if");
	int fReturn = is(ID.RETURN);
	statement(locBreak, locContinue, sizeRet);	    // then_statement
	if (is(ID.ELSE) && !fReturn) outCode2(jmp, locEnd = loc());
	outCode3(loc_, locElse, 'E');
	if (ispp(ID.ELSE))
	{
		statement(locBreak, locContinue, sizeRet);   // else_statement
		if (locEnd >= 0) outCode2(loc_, locEnd);
	}
}

/// ForStatement ::= "for" "(" [Expr1] ";" [Expr2] ";" [Expr3] ")" Statement
void forStatement(int isFOR, int sizeRet)
{
	int locExpr2 = loc(), locExpr3 = 0, locStmt = loc(), locNext = loc();
	ix.tix++;					// for
	skip('(');
	if (isFOR)
	{
		if (!is(';')) expr(VAL);
		skip(';');
	}
	outCode2(loc_, locExpr2);
	int bgn = ix.ixCode;
	if (!is(';')) expr(VAL);
	else outCode2(mov_eax, 1);
	if (isFOR) skip(';');
	jumpFalse(locNext, "for");
	outCode2(jmp, locStmt);
	if (cd.pCode[bgn].inst == mov_eax && cd.pCode[bgn + 1].inst == test_eax_eax)
	{
		ix.ixCode = bgn;
		outCode2(jmp, cd.pCode[bgn].num == 0 ? locNext : locStmt);
	}
	int bgnExpr = ix.ixCode;
	outCode2(loc_, locExpr3 = loc());
	if (isFOR && !is(')')) expr(VAL);
	skip(')');
	int bgnStmt = ix.ixCode;
	outCode3(loc_, locStmt, '1');
	statement(locNext, locExpr3, sizeRet);
	int endStmt = ix.ixCode;
	int lenStmt = endStmt - bgnStmt;
	int lenBoth = endStmt - bgnExpr;
	if (bgnStmt - bgnExpr > 0 && lenStmt > 0)
	{
		reallocCode(lenStmt);
		memmove(&cd.pCode[bgnExpr + lenStmt], &cd.pCode[bgnExpr], lenBoth*sizeof(INSTRUCT));
		memmove(&cd.pCode[bgnExpr], &cd.pCode[bgnStmt + lenStmt], lenStmt*sizeof(INSTRUCT));
	}
	outCode2(jmp, locExpr2);
	outCode3(loc_, locNext, '0');
}

/// WhileStatement ::= "while" "(" Expression ")" Statement
void whileStatement(int sizeRet)
{
	forStatement(FALSE, sizeRet);
}

/// DoStatement ::= "do" CompoundStatement "while" "(" Expression ")" ";"
void doStatement(int sizeRet)
{
	int locStmt = loc(), locBreak = loc();
	ix.tix++;
	outCode2(loc_, locStmt);
	compoundStatement(locBreak, locStmt, sizeRet);
	if (!ispp(ID.WHILE)) error("doStmt", "while â€šÂªâ€šÂ â€šÃ¨â€šÃœâ€šÂ¹â€šÃ±");
	expr2(VAL);
	skip(';');
	jumpTrue(locStmt, "do");	//outCode1(test_eax_eax); outCode2(jnz,locStmt);
	outCode2(loc_, locBreak);
}

/// ReturnStatement ::= "return" [Expression] ";"
void returnStatement(int sizeRet)
{
	ix.tix++;	// "return"
	if (!is(';'))
	{
		VALUE v;
		expression(VAL, &v);
	}
	skip(';');
	outCode2(xret, sizeRet);
}

/// ContinueStatement := "continue" ";"
void continueStatement(int locContinue)
{
	if (locContinue <= 0)
		error("", "continue statement not within a loop");
	ix.tix++;
	skip(';');
	outCode2(jmp, locContinue);
}

/// BreakStatement := "break" ";"
void breakStatement(int locBreak)
{
	if (locBreak <= 0)
		error("", "break statement not within loop or switch");
	ix.tix++;
	skip(';');
	outCode2(jmp, locBreak);
}

/// SwitchStatement ::= "switch" "(" Expression ")" "{" (LabeledStatement)+ "}"
/// LabeledStatement ::= "case" Expression ":" | Statement | "default" ":"
void switchStatement(int sizeRet)
{
	int locCase = loc(), locStmt = loc(), locBreak;
	int fDefault = 0;
	ix.tix++;	// switch
	expr2(VAL);	// eax: sw_expr
	skip('{');
	locBreak = loc();
	while (!ispp('}'))
	{
		if (ispp(ID.CASE))
		{
			outCode2(loc_, locCase);
			outCode2(cmp_eax, constIntExpression());
			skip(':');
			outCode2(jnz, locCase = loc());
			outCode2(loc_, locStmt);
			if (!is(ID.CASE))
			{
				do
				{
					statement(locBreak, -1, sizeRet);	// break
				} while (!is(ID.CASE) && !is(ID.DEFAULT) && !is('}'));
			}
			outCode2(jmp, locStmt = loc());
		}
		else if ((fDefault = ispp(ID.DEFAULT)))
		{
			outCode2(loc_, locCase);
			skip(':');
			outCode2(loc_, locStmt);
			do
			{
				statement(locBreak, -1, sizeRet);
			} while (!is('}'));
		}
		else  	// error
		{
			error("switchStmt", "'case' or 'default' expected");
		}
	}
	if (!fDefault)
	{
		outCode2(loc_, locCase);
		outCode2(loc_, locStmt);
	}
	outCode2(loc_, locBreak);
}

/// Statement ::= CompoundStmt | IfStmt | ForStmt | WhileStmt | DoStmt | ReturnStmt
///		  | BreakStmt | ContinueStmt | SwitchStmt | Expression ";" | ";"
void statement(int locBreak, int locContinue, int sizeRet)
{
	if (is(';')) ix.tix++;	// null statement
	else if (is('{')) compoundStatement(locBreak, locContinue, sizeRet);
	else if (is(ID.IF)) ifStatement(locBreak, locContinue, sizeRet);
	else if (is(ID.FOR)) forStatement(TRUE, sizeRet);
	else if (is(ID.WHILE)) whileStatement(sizeRet);
	else if (is(ID.DO)) doStatement(sizeRet);
	else if (is(ID.RETURN)) returnStatement(sizeRet);
	else if (is(ID.BREAK)) breakStatement(locBreak);
	else if (is(ID.CONTINUE)) continueStatement(locContinue);
	else if (is(ID.SWITCH)) switchStatement(sizeRet);
	else
	{
		expr(VAL);
		skip(';');
	}
}


void infixOperation(int op, VALUE *v1, VALUE *v2, int lbgn, int lend)
{
	if (v1->fConst && v2->fConst)
	{
		if (v1->type == ID.INT && v2->type == ID.INT)
		{
			if (op == '+') v1->ival += v2->ival;
			else if (op == '-') v1->ival -= v2->ival;
			else if (op == '*') v1->ival *= v2->ival;
			else if (op == '/') v1->ival /= v2->ival;
			else if (op == '%') v1->ival %= v2->ival;
			else if (op == '|') v1->ival |= v2->ival;
			else if (op == '&') v1->ival &= v2->ival;
			else if (op == '^') v1->ival ^= v2->ival;
			else v1->fConst = FALSE;
		}
		else if (v1->type == ID.DOUBLE || v2->type == ID.DOUBLE)
		{
			double d1 = v1->type == ID.DOUBLE ? v1->rval : (double)v1->ival;
			double d2 = v2->type == ID.DOUBLE ? v2->rval : (double)v2->ival;
			v1->type = ID.DOUBLE;
			if (op == '+') v1->rval = d1 + d2;
			else if (op == '-') v1->rval = d1 - d2;
			else if (op == '*') v1->rval = d1 * d2;
			else if (op == '/') v1->rval = d1 / d2;
			else v1->fConst = FALSE;
		}
		else
		{
			v1->fConst = FALSE;
		}
	}
	else if (v2->fConst && lend > 0 && v1->type == ID.INT && v2->type == ID.INT)
	{

		if (v1->ptrs == 0 && (op == '+' || op == '-'))
		{
			ix.ixCode = lend;
			outCode2(op == '+' ? add_eax : sub_eax, v2->ival);
			return;
		}
		else if (op == '<' || op == '>' || op == id2("<=") || op == id2(">=") || op == id2("==") || op == id2("!="))
		{
			ix.ixCode = lend;
			outCode2(cmp_eax, v2->ival);
			outCode1(op == '<' ? setl_eax : op == '>' ? setg_eax : op == id2("<=") ? setle_eax :
				op == id2(">=") ? setge_eax : op == id2("==") ? sete_eax : setne_eax);
			return;
		}
		else if (op == '*')
		{
			ix.ixCode = lend;
			outCode2(imul_eax_eax, v2->ival);
			return;
		}
		else
		{
			v1->fConst = FALSE;
		}
	}
	else
	{
		v1->fConst = FALSE;
	}
	if ((v1->ptrs == 0 && v1->type == ID.DOUBLE) || (v2->ptrs == 0 && v2->type == ID.DOUBLE))
	{
		setFpuStack2(v1->type, v2->type);
		if (op == '=')
		{
			int fLV = (lend == lbgn + 1 && cd.pCode[lbgn].inst == lea_eax_pbp);
			if (fLV)
			{
				outCode3(fst_qbp, cd.pCode[lbgn].num, cd.pCode[lbgn].attr);
				delCodes(lbgn, lbgn + 2);	// lea_eax_pbp, push_eax â€šÃ°ÂÃ­ÂÅ“
			}
			else
			{
				outCode1(pop_ecx);
				outCode1(fst_qcx);	// [ecx] <- st0
			}
			outCode1(fstp_st1);
			v1->type = ID.DOUBLE;
		}
		else if (op == id2("==") || op == id2("!="))
		{
			outCode1(fucompp);
			outCode1(fstsw);
			outCode2(and_ah, 0x45);
			outCode2(op == id2("==") ? cmp_ah : xor_ah, 0x40);
			outCode1(op == id2("==") ? sete_eax : setne_eax);
			v1->type = ID.INT;
		}
		else if (op == '<' || op == '>' || op == id2("<=") || op == id2(">="))
		{
			if (op == '>' || op == id2(">=")) outCode1(fxch_st1);
			outCode1(fucompp);
			outCode1(fstsw);
			outCode2(test_ah, (op == '<' || op == '>') ? 0x45 : 0x05);
			outCode1(sete_eax);		// eax = ZF.
			v1->type = ID.INT;
		}
		else if (op == '+' || op == '-' || op == '*' || op == '/')
		{
			int *pI = &cd.pCode[ix.ixCode - 1].inst;
			if (*pI == fld_qbp)  	// fld qp[ebp-8]/fadd qp[ebp-16]
			{
				*pI = op == '+' ? fadd_qbp : op == '-' ? fsub_qbp : op == '*' ? fmul_qbp : fdiv_qbp;
			}
			else if (*pI == fld_qp)
			{
				*pI = op == '+' ? fadd_qp : op == '-' ? fsub_qp : op == '*' ? fmul_qp : fdiv_qp;
			}
			else  	// fld qp[ebp-8]/fld qp[ebp-16]/faddp st1,st
			{
				outCode1(op == '+' ? faddp_st1_st : op == '-' ? fsubrp_st1_st :
					op == '*' ? fmulp_st1_st : fdivrp_st1_st);
			}
			v1->type = ID.DOUBLE;
		}
		else
		{
			error("infixOp", "unsuported infixOp '%s'", toString(op));
		}
	}
	else
	{
		int n = v1->ptrs > 0 ? 4 : sizeOfDataType(v1->type);
		int k = ix.ixCode - 1;
		int fLV4 = (n == 4 && lend == lbgn + 1 && cd.pCode[lbgn].inst == lea_eax_pbp);
		if (fLV4 && (op == '=' || op == id2("+=") || op == id2("-=")))
		{
			int num = cd.pCode[lbgn].num;	// x = exp, x += exp, x -= exp
			int attr = cd.pCode[lbgn].attr;
			int inst = op == '=' ? mov_pbp_eax : op == id2("+=") ? add_pbp_eax : sub_pbp_eax;
			delCodes(lbgn, lbgn + 2);	// lea_eax_pbpâ€šÃ†push_eaxâ€šÃ°ÂÃ­ÂÅ“.
			outCode3(inst, num, attr);
			return;
		}
		int fECX = 1;
		if (lend > 0 && cd.pCode[lend].inst == push_eax)
		{
			int i;
			for (i = lend + 1; i < ix.ixCode && !(cd.pCode[i].regs & C); i++);
			if (i == ix.ixCode) fECX = 0;
		}
		if (fECX)
		{
			outCode1(pop_ecx);
		}
		else
		{
			int *pI = &cd.pCode[lend - 1].inst;
			if (*pI == mov_eax)
			{
				*pI = mov_ecx;
				delCode(lend);	// push_eax â€šval
			}
			else if (*pI == mov_eax_pbp)
			{
				*pI = mov_ecx_pbp;
				delCode(lend);	// push_eax â€šval
			}
			else if (*pI == mov_eax_pax)
			{
				*pI = mov_ecx_pax;
				delCode(lend);	// push_eax â€šval
			}
			else if (*pI >= lea_eax_da1 && *pI <= lea_eax_da8)
			{
				*pI = lea_ecx_da1 + (*pI - lea_eax_da1);
				delCode(lend);	// push_eax â€šval
			}
			else
			{
				cd.pCode[lend].inst = mov_ecx_eax;	// <- push_eax
			}
		}
		if (op == '=') outCode1(n == 4 ? mov_pcx_eax : n == 2 ? mov_pcx_ax : mov_pcx_al);
		else if (op == id2("+=")) outCode1(n == 4 ? add_pcx_eax : n == 2 ? add_pcx_ax : add_pcx_al);
		else if (op == id2("-=")) outCode1(n == 4 ? sub_pcx_eax : n == 2 ? sub_pcx_ax : sub_pcx_al);
		else if (op == id2("&=")) outCode1(n == 4 ? and_pcx_eax : n == 2 ? and_pcx_ax : and_pcx_al);
		else if (op == id2("|=")) outCode1(n == 4 ? or_pcx_eax : n == 2 ? or_pcx_ax : or_pcx_al);
		else if (op == id2("^=")) outCode1(n == 4 ? xor_pcx_eax : n == 2 ? xor_pcx_ax : xor_pcx_al);
		else if (op == id2("==") || op == id2("!="))
		{
			outCode1(cmp_eax_ecx);
			outCode1(op == id2("==") ? sete_eax : setne_eax);
		}
		else if (op == '<' || op == '>' || op == id2("<=") || op == id2(">="))
		{
			outCode1(cmp_ecx_eax);
			outCode1(op == '<' ? setl_eax : op == '>' ? setg_eax : op == id2("<=") ? setle_eax : setge_eax);
		}
		else if (op == '+' || op == '-')
		{
			n = v1->ptrs > 1 ? 4 : sizeOfDataType(v1->type);
			if (v1->ptrs > 0 && v2->ptrs == 0 && n > 1) outCode2(imul_eax_eax, n);
			if (op == '+')
			{
				outCode1(add_eax_ecx);	// eax: operand_1 + operand_2
			}
			else
			{
				outCode1(xchg_eax_ecx);	// ecx: operand_2
				outCode1(sub_eax_ecx);	// eax: operand_1 - operand_2
				if (v1->ptrs > 0 && v2->ptrs > 0)
				{
					outCode2(mov_ecx, n);
					outCode1(xdiv_ecx);	// eax: (operand_1 - operand_2)/size
				}
			}
			return;
		}
		else if (op == '*')
		{
			int k = ix.ixCode - 1;
			if (cd.pCode[k - 1].inst == mov_ecx_pbp && cd.pCode[k].inst == mov_eax_pbp)
			{
				cd.pCode[k - 1].inst = mov_eax_pbp;
				cd.pCode[k].inst = imul_eax_pbp;
			}
			else
			{
				outCode1(imul_eax_ecx);
			}
		}
		else if (op == '/' || op == '%')
		{
			outCode1(xchg_eax_ecx);
			outCode1(op == '/' ? xdiv_ecx : xmod_ecx);
		}
		else if (op == '|' || op == '&' || op == '^')
		{
			outCode1(op == '|' ? or_eax_ecx : op == '&' ? and_eax_ecx : xor_eax_ecx);
		}
		else if (op == id2("<<") || op == id2(">>"))
		{
			outCode1(xchg_eax_ecx);
			outCode1(op == id2("<<") ? shl_eax_cl : shr_eax_cl);
		}
		else
		{
			error("infixOp", "unsuported infixOp '%s'", toString(op));
		}
		v1->type = ID.INT;
	}
}

void incdec(int type, int fPtr, int fI, int reg)  	// ++, --
{
	if (type == ID.INT || fPtr) outCode2(reg == 'a' ? (fI ? add_dax : sub_dax) : (fI ? add_ddx : sub_ddx), 1);
	else if (type == ID.CHAR) outCode2(reg == 'a' ? (fI ? add_bax : sub_bax) : (fI ? add_bdx : sub_bdx), 1);
	else error("expr.incdec", "wrong use of ++ or --", toString(type));
}

/// Expression ::= AssignExpression ("," AssignExpression)*
void expression(int mode, VALUE *pv)
{
	assignExpression(mode, pv);
	while (ispp(',')) assignExpression(mode, pv);
}

/// AssignExpr ::= CondExpr | CastExpr ("=" | "|=" | "+=" | "-=" | "&=" | "^=") AssignExpr
void assignExpression(int mode, VALUE *pv)
{
	VALUE vr;
	INDEX ixSave = ix;
	castExpression(ADDR, pv);
	char *t = cd.token[ix.tix].token;
	if (strchr(";,])", *t) != NULL && pv->mode == VAL) return;
	ix = ixSave;
	int fAssign = (*t == '=' && t[1] == '\0') || (strchr("+-/*%|&^", *t) != NULL && t[1] == '=');
	if (!fAssign)
	{
		conditionalExpression(mode, pv);
		return;
	}
	castExpression(ADDR, pv);
	if (pv->mode != ADDR) error("assignExpr", "lvalue expected");
	int op = cd.token[ix.tix].ival;
	t = cd.token[ix.tix++].token;
	int ixOp2 = ix.ixCode;
	outCode1(push_eax);
	if ((pv->ptrs > 0 || pv->type != ID.DOUBLE)
		&& op != id2("*=") && op != id2("/=") && op != id2("%="))
	{
		assignExpression(VAL, &vr);
		infixOperation(op, pv, &vr, ixSave.ixCode, ixOp2);
		int k = ix.ixCode - 1;
		if (op == '=' && cd.pCode[k - 1].inst == pop_ecx && cd.pCode[k].inst == mov_pcx_eax)
		{
			if (k - ixOp2 == 3 && cd.pCode[ixOp2 + 1].inst == mov_eax)
			{
				cd.pCode[ixOp2 + 1].inst = mov_dax;
				delCode(k);
				delCode(k - 1);	// pop_ecxâ€šval
				delCode(ixOp2); // push_eaxâ€šval
			}
		}
		return;
	}
	if (t[1] != '\0')
	{
		int size = pv->ptrs > 0 ? 4 : sizeOfDataType(pv->type);
		if (size == 8)
		{
			outCode1(fld_qax);
		}
		else
		{
			outCode1(size == 4 ? mov_eax_pax : size == 2 ? movsx_eax_wax : movsx_eax_bax);
			outCode1(push_eax);
		}
	}
	assignExpression(VAL, &vr);
	if (t[0] != '=')
	{
		infixOperation(t[0], pv, &vr, -1, -1);
		infixOperation('=', pv, &vr, -1, -1);
	}
	else
	{
		infixOperation('=', pv, &vr, ixSave.ixCode, ixOp2);
	}
}

/// ConditionalExpression := LogicOrExpr ["?" AssignExpr ":" AssignExpr]
void conditionalExpression(int mode, VALUE *pv)
{
	int locFalse, locEnd;
	INDEX ixBgn = ix;
	logicalOrExpression(mode, pv);
	if (ispp('?'))
	{
		outCode1(test_eax_eax);
		outCode2(jz, locFalse = loc());
		INSTRUCT *pC = &cd.pCode[ix.ixCode - 3];
		if (pC[0].inst == sete_eax && pC[1].inst == test_eax_eax && pC[2].inst == jz)
		{
			pC[2].inst = jnz;
			delCodes(ix.ixCode - 3, ix.ixCode - 1);	// sete eax/test eax,eax â€šÃ°ÂÃ­ÂÅ“.
		}
		assignExpression(VAL, pv);
		outCode2(jmp, locEnd = loc());
		skip(':');
		outCode2(loc_, locFalse);
		assignExpression(VAL, pv);
		outCode2(loc_, locEnd);
		pv->fConst = FALSE;
	}
	if (pv->fConst)
	{
		int tixCurr = ix.tix;
		ix = ixBgn;
		ix.tix = tixCurr;
		if (pv->ptrs > 0)
		{
			outCode3(mov_eax, pv->ival, AD_CONST);
		}
		else if (pv->type == ID.INT)
		{
			outCode3(mov_eax, pv->ival, cd.pCode[ix.ixCode].attr);
		}
		else
		{
			loadAddr(AD_DATA, ix.ixData);
			outDataDouble(pv->rval);
			outCode1(fld_qax);
		}
	}
}

/// LogicalOrExpression ::= LogicalAndExpr ( "||" LogicalAndExpr )*
void logicalOrExpression(int mode, VALUE *pv)
{
	logicalAndExpression(mode, pv);
	if (!is2("||")) return;
	int locTRUE = loc();
	int locNext = loc();
	while (is2pp("||"))
	{
		jumpTrue(locTRUE, "||");	// Â^(eax!=0)
		logicalAndExpression(mode, pv);
		pv->type = ID.INT;
	}
	jumpTrue(locTRUE, "||");
	outCode2(mov_eax, 0);
	outCode2(jmp, locNext);
	outCode2(loc_, locTRUE);
	outCode2(mov_eax, 1);
	outCode2(loc_, locNext);
}

/// LogicalAndExpression ::= OrExpression ( "&&" OrExpression )*
void logicalAndExpression(int mode, VALUE *pv)
{
	orExpression(mode, pv);
	if (!is2("&&")) return;
	int locFalse = loc();
	int locNext = loc();
	while (is2pp("&&"))
	{
		jumpFalse(locFalse, "&&");
		orExpression(mode, pv);
		pv->type = ID.INT;
	}
	jumpFalse(locFalse, "&&");
	outCode2(mov_eax, 1);
	outCode2(jmp, locNext);
	outCode3(loc_, locFalse, 'F');
	outCode2(mov_eax, 0);
	outCode3(loc_, locNext, 'N');
}

/// OrExpression ::= ExclusiveOrExpression ( "|" ExclusiveOrExpression )*
void orExpression(int mode, VALUE *pv)
{
	xorExpression(mode, pv);
	while (ispp('|'))
	{
		VALUE v2;
		outCode1(push_eax);
		xorExpression(mode, &v2);
		infixOperation('|', pv, &v2, -1, -1);
	}
}

/// ExclusiveOrExpression ::= AndExpression ( "^" AndExpression )*
void xorExpression(int mode, VALUE *pv)
{
	andExpression(mode, pv);
	while (ispp('^'))
	{
		VALUE v2;
		outCode1(push_eax);
		andExpression(mode, &v2);
		infixOperation('^', pv, &v2, -1, -1);
	}
}

/// AndExpression ::= EqualityExpression ( "&" EqualityExpression )*
void andExpression(int mode, VALUE *pv)
{
	equalityExpression(mode, pv);
	while (ispp('&'))
	{
		VALUE v2;
		int ixOp2 = ix.ixCode;
		outCode1(push_eax);
		equalityExpression(mode, &v2);
		infixOperation('&', pv, &v2, -1, ixOp2);
	}
}

/// EqualityExpression ::= RelationalExpr [ ("==" | "!=") RelationalExpr ]
void equalityExpression(int mode, VALUE *pv)
{
	int fEQ;
	relationalExpression(mode, pv);
	if ((fEQ = is2pp("==")) || is2pp("!="))
	{
		VALUE v2;
		int ixOp2 = ix.ixCode;
		if (pv->type != ID.DOUBLE) outCode1(push_eax);
		relationalExpression(mode, &v2);
		if (ix.ixCode - ixOp2 == 2 && cd.pCode[ix.ixCode - 1].inst == mov_eax_pbp)
		{
			cd.pCode[ix.ixCode - 1].inst = cmp_eax_pbp;
			outCode1(fEQ ? sete_eax : setne_eax);
			delCode(ixOp2);
		}
		else
		{
			infixOperation(fEQ ? id2("==") : id2("!="), pv, &v2, -1, ixOp2);
		}
	}
}

/// RelationalExpression ::= UnaryExpr [("<" | ">" | "<=" | ">=") UnaryExpr]
void relationalExpression(int mode, VALUE *pv)
{
	int fLT = 0, fGT = 0, fLE = 0;
	shiftExpression(mode, pv);
	if ((fLT = ispp('<')) || (fGT = ispp('>')) || (fLE = is2pp("<=")) || is2pp(">="))
	{
		VALUE v2;
		int ixOp2 = ix.ixCode;
		if (pv->type != ID.DOUBLE) outCode1(push_eax);
		shiftExpression(mode, &v2);
		if (ix.ixCode - ixOp2 == 2 && cd.pCode[ix.ixCode - 1].inst == mov_eax_pbp)
		{
			cd.pCode[ix.ixCode - 1].inst = cmp_eax_pbp;
			outCode1(fLT ? setl_eax : fGT ? setg_eax : fLE ? setle_eax : setge_eax);
			delCode(ixOp2);
		}
		else
		{
			infixOperation(fLT ? '<' : fGT ? '>' : fLE ? id2("<=") : id2(">="), pv, &v2, -1, ixOp2);
		}
	}
}

/// ShiftExpression ::= AddExpression [ ("<<" | ">>") AddExpression ]
void shiftExpression(int mode, VALUE *pv)
{
	int fShl;
	addExpression(mode, pv);
	if ((fShl = is2pp("<<")) || is2pp(">>"))
	{
		VALUE v2;
		int ixOp2 = ix.ixCode;
		outCode1(push_eax);
		addExpression(mode, &v2);
		infixOperation(fShl ? id2("<<") : id2(">>"), pv, &v2, -1, -1);
	}
}

/// AddExpression ::= MulExpression ( ("+" | "-") MulExpression )*
void addExpression(int mode, VALUE *pv)
{
	int fAdd;
	mulExpression(mode, pv);		// operand_1
	while ((fAdd = ispp('+')) || ispp('-'))
	{
		VALUE v2;
		int ixOp2 = ix.ixCode;
		if (pv->type != ID.DOUBLE) outCode1(push_eax);
		mulExpression(mode, &v2);	// operand_2
		infixOperation(fAdd ? '+' : '-', pv, &v2, -1, ixOp2);
	}
}

/// MulExpression ::= CastExpression ( ("*" | "/" | "%") CastExpression )*
void mulExpression(int mode, VALUE *pv)
{
	int fMul = 0, fDiv = 0;
	castExpression(mode, pv);	// operand_1
	while ((fMul = ispp('*')) || (fDiv = ispp('/')) || ispp('%'))
	{
		VALUE v2;
		int ixOp2 = ix.ixCode;
		if (pv->type != ID.DOUBLE) outCode1(push_eax);
		castExpression(mode, &v2);	// eax/st: operand_2
		infixOperation(fMul ? '*' : fDiv ? '/' : '%', pv, &v2, -1, ixOp2);
	}
}

/// CastExpression := UnaryExpression | "(" TypeName ")" UnaryExpression
void castExpression(int mode, VALUE *pv)  	//
{
	if (!is('(') || !(isTypeSpecifier(ix.tix + 1) && !isN('(', 2)))
	{
		unaryExpression(mode, pv);
		return; //  (_stat(
	}
	int idCastType = cd.token[++ix.tix].ival;
	int ptr = getPtr(idCastType);
	for (ix.tix++; ispp('*');) ptr++;
	skip(')');
	unaryExpression(mode, pv);
	if (ptr > 0)
	{
		;
	}
	else if (idCastType == ID.INT && pv->type == ID.DOUBLE)  	// (int)v
	{
		outCode3(fldcw, 2, AD_DATA);	// _RoundNear
		outCode2(fistp_dsp, -4);
		outCode2(mov_eax_psp, -4);
		outCode3(fldcw, 0, AD_DATA);	// _RoundChop
		if (pv->fConst) pv->ival = (int)pv->rval;
	}
	else if (idCastType == ID.SHORT && pv->type == ID.INT)
	{
		outCode1(cwde);
		if (pv->fConst) pv->ival = (short)pv->ival;
	}
	else if (idCastType == ID.DOUBLE && pv->type != ID.DOUBLE)  	// (double)n
	{
		outCode2(mov_psp_eax, -4);	//outCode1(push_eax);
		outCode2(fild_dsp, -4);		//outCode2(fild_dsp,0); outCode2(add_esp, 4);
		if (pv->fConst) pv->rval = (double)pv->ival;
	}
	else if (idCastType != ID.INT)
	{
		error("castExpr", "unsupported cast(casttype=%s type=%s)\n",
			toString(idCastType), toString(pv->type));
	}
	pv->ptrs = ptr;
	pv->type = idCastType;
}

/// UnaryExpression ::= ["+" | "-" | "!" | "++" | "--" | "&" | "*" ] PrimExpr
void unaryExpression(int mode, VALUE *pv)
{
	int fNEG = 0, fNOT = 0, fINC = 0;
	if (ispp('+') || (fNEG = ispp('-')) || (fNOT = ispp('!')))
	{
		int bgn = ix.ixCode;
		primaryExpression(VAL, pv);
		if (fNEG)
		{
			if (pv->fConst && pv->type == ID.INT)
			{
				cd.pCode[ix.ixCode - 1].num = (pv->ival = -pv->ival);
			}
			else if (pv->fConst && pv->type == ID.DOUBLE)
			{
				cd.pCode[bgn].dval = (pv->rval = -pv->rval);
			}
			else
			{
				outCode1(pv->type == ID.DOUBLE ? fchs : neg_eax);
			}
		}
		else if (fNOT)
		{
			if (pv->fConst) pv->ival = !pv->ival;
			outCode1(test_eax_eax);
			outCode1(sete_eax);	// mov eax,0 & sete_al
		}
	}
	else if ((fINC = is2pp("++")) || is2pp("--"))
	{
		primaryExpression(ADDR, pv);
		incdec(pv->type, pv->ptrs > 0, fINC, 'a');
		loadValue(pv->type, pv->ptrs > 0);
	}
	else if (ispp('&'))
	{
		primaryExpression(ADDR, pv);
		pv->fAddr = TRUE;
		pv->ptrs++;
	}
	else if (ispp('*'))
	{
		primaryExpression(VAL, pv);
		pv->mode = ADDR;
		pv->ptrs--;
		if (mode == VAL) loadValue(pv->type, pv->ptrs > 0);
	}
	else
	{
		primaryExpression(mode, pv);
	}
}

int checkarg(int pt1, int type1, int pt2, int type2, char *name, int nArg)
{
	char *f = "%s arg#%d assignment makes %s from %s";
	char *g = "%s arg#%d assignment from incompatible pointer type";
	if (type2 == ID.DOTS3) return FALSE;
	if (pt1 > 0 && pt2 == 0) error("chk", f, name, nArg, "integer", "pointer");
	if (pt1 == 0 && pt2 > 0) error("chk", f, name, nArg, "pointer", "integer");
	if (pt1 != pt2 && type1 != ID.HVOID && type2 != ID.HVOID) error("chk", g, name, nArg);
	return TRUE;
}

/// FunctionCall ::= Identifier "(" [AssignExpression ("," AssignExpression)*] ")"
void functionCall(VALUE *pv)
{
	int n, nSize, nP, p[20], depth = 0;

	int fid = cd.token[ix.tix++].ival;
	Name *pName = getNameFromTable(globTable, NM_FUNC, fid);
	if (pName == NULL)
	{
		char *func = toString(fid);
		if (isupper(*func)) error("funcCall", "'%s' undeclared", func);
		pName = appendName(globTable, NM_CDECL, ID.INT, fid, AD_CODE, 0);
	}
	skip('(');
	int fCheck = (pName->argc >= 0 && fid != id("main"));
	for (nP = 0; !ispp(')'); nP++)
	{
		p[nP] = ix.ixCode;
		assignExpression(VAL, pv);
		if (fCheck && nP < pName->argc)
			fCheck = checkarg(pv->ptrs, pv->type, pName->argpt[nP].ptrs,
			pName->argpt[nP].type, toString(pName->idName), nP);
		if (pv->ptrs > 0 || pv->fAddr || pv->type != ID.DOUBLE)
		{
			outCode1(push_eax);
			depth += 4;
		}
		else
		{
			outCode2(sub_esp, 8);
			outCode1(fstp_qsp);	    //outCode1(fst_qsp);
			depth += 8;
		}
		ispp(',');
	}
	int diff = pName->argc - nP;
	if (diff > 0 && pName->argpt[nP].type == ID.DOTS3) fCheck = FALSE;
	if (fCheck && diff != 0)
		error("funcCall", "too %s arguments", diff < 0 ? "many" : "few");
	if (nP > 1)
	{
		int mv = ix.ixCode - p[1];
		reallocCode(mv + 1);
		memmove(&cd.pCode[*p + mv], &cd.pCode[*p], (ix.ixCode - *p)*sizeof(INSTRUCT));
		int dst = p[0];
		for (n = nP; --n > 0; dst += nSize)
		{
			nSize = (n + 1 < nP ? p[n + 1] : ix.ixCode) - p[n];
			memmove(&cd.pCode[dst], &cd.pCode[mv + p[n]], nSize*sizeof(INSTRUCT));
		}
	}
	outCode2(call, pName->idName);
	if ((pName->type&NM_CDECL) && depth > 0)
		outCode2(add_esp, depth);
	setValue(VAL, pName->ptrs, pName->dataType, pv);
}

void sizeOf(VALUE *pv)
{
	int cnt = 0, pointers = 0;
	ix.tix++;
	while (ispp('(')) cnt++;
	int id = cd.token[ix.tix++].ival;
	while (ispp('*')) pointers++;
	int size = pointers > 0 ? 4 : sizeOfDataType(id);
	if (size == 0)
	{
		Name *pName = getNameFromAllTable(cd.currTable, NM_VAR, id);
		if (pName != NULL) size = pName->size[0];
	}
	outCode2(mov_eax, size);
	while (cnt-- > 0) skip(')');
	setValue(VAL, 0, ID.INT, pv);
	pv->ival = size;
	pv->fConst = TRUE;
}

/// Constant ::= Number | String | Character
void constant(VALUE *pv)
{
	switch (cd.token[ix.tix].type)
	{
	case TK_STRING:
		loadAddr(AD_DATA, ix.ixData);
		ix.ixData += outString(ix.ixData, toString(cd.token[ix.tix++].ival)) + 1;
		setValue(VAL, 1, ID.CHAR, pv);
		break;
	case TK_DOUBLE:
		setValue(VAL, 0, ID.DOUBLE, pv);
		pv->rval = cd.token[ix.tix++].dval;
		pv->fConst = TRUE;
		outDataDouble(pv->rval);	// setreal 0.000 @100
		outCode3(fld_qp, ix.ixData - 8, AD_DATA);
		break;
	case TK_INT:
	case TK_CHAR:
		setValue(VAL, 0, ID.INT, pv);
		pv->ival = cd.token[ix.tix++].ival;
		pv->fConst = TRUE;
		outCode2(mov_eax, pv->ival);
		break;
	default:
		error("expr.constant", "type=%d\n", cd.token[ix.tix].type);
	}
}

/// PrimExpression ::= Id | "(" Expression ")" | Constant | FunctionCall
int pot[] = { 0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4 };
void primExpr(int mode, VALUE *pv)
{
	Name *pName = NULL;
	int ptrs, depth, type, size;
	int fInc = 0, fArray = 0, fDot = 0, fArrow = 0, fDone = 0;
	int hid = cd.token[ix.tix].ival;
	if (ispp('('))   	// "(" Expression ")"
	{
		expression(mode, pv); 	// eax: Expression
		skip(')'); 		//  Id | "(" "(" Id "*" ")" Id ")"
		if (!is2("->")) return;
		ptrs = pv->ptrs;
		type = pv->type;
		fDone = 1;	// outCode("mov eax,[eax]");
	}
	else  	// NM_VAR|NM_CDECL|NM_WINAPI|NM_ENUM
	{
		pName = getNameFromAllTable(cd.currTable, NM_VAR | NM_ENUM, hid);
		if (pName == NULL)
			pName = getNameFromTable(globTable, NM_FUNC, hid);
		if (pName == NULL)
			error("primExpr", "'%s' undeclared", toString(hid));
		ix.tix++;
		if (pName->type == NM_ENUM)
		{
			outCode2(mov_eax, pName->address);
			setValue(VAL, 0, ID.INT, pv);
			pv->ival = pName->address;
			pv->fConst = TRUE;
			return;
		}
		type = pName->dataType;
		ptrs = pName->ptrs + getPtr(type);
		if (pName->addrType == AD_CODE)
		{
			loadAddr(AD_CODE, hid);
			setValue(VAL, ptrs + 1, type, pv);
			return;
		}
		loadAddr(pName->addrType, pName->addrType == AD_IMPORT ? hid : pName->address);
	}
	depth = 0;
	while ((fArray = ispp('[')) || (fDot = ispp('.')) || (fArrow = is2pp("->")))
	{
		if (fArray)
		{
			ptrs--;
			size = pName->size[++depth];
			if (size == 0) size = ptrs > 0 ? 4 : sizeOfDataType(pName->dataType);
			if (pName->size[depth] == 0)
			{
				int *pI = &cd.pCode[ix.ixCode - 1].inst;
				if (*pI == lea_eax_pbp) *pI = mov_eax_pbp;
				else outCode1(mov_eax_pax);
			}
			int ixAddr = ix.ixCode - 1;
			outCode1(push_eax);
			int bgn = ix.ixCode;
			expression(VAL, pv);
			skip(']');
			if (pv->fConst)
			{
				ix.ixCode = bgn - 1;
				outCode2(add_eax, pv->ival*size);
				continue;
			}
			int fLocVar = ix.ixCode == bgn + 1 && cd.pCode[ix.ixCode - 1].inst == mov_eax_pbp;
			if (fLocVar)
			{
				delCode(bgn - 1);
				cd.pCode[ix.ixCode - 1].inst = mov_edx_pbp;
				if (size == 1 || (size <= 8 && pot[size] > 0))
				{
					outCode1(lea_eax_ad1 + pot[size]);
				}
				else
				{
					if (size > 1) outCode2(imul_edx_edx, size);
					outCode1(add_eax_edx);
				}
			}
			else
			{
				if (cd.pCode[ixAddr].inst == mov_eax_pbp)
				{
					outCode3(mov_edx_pbp, cd.pCode[ixAddr].num, cd.pCode[ixAddr].attr);
					delCodes(ixAddr, ixAddr + 2);
				}
				else
				{
					outCode1(pop_edx);
				}
				if (size == 1 || (size <= 8 && pot[size] > 0))
				{
					outCode1(lea_eax_da1 + pot[size]);
				}
				else
				{
					if (size > 1) outCode2(imul_eax_eax, size);
					outCode1(add_eax_edx);
				}
			}
		}
		else      // ("." | "->") Identifier
		{
			int id = cd.token[ix.tix++].ival;
			pName = getAttr(type, id);
			if (pName == NULL)
				error("prim", "'%s' not struct member", toString(id));
			if (!fDone && fArrow) outCode1(mov_eax_pax);
			fDone = 0;
			fArrow = 0;
			ptrs = pName->ptrs;
			type = pName->dataType;
			depth = 0;
			outCode2(add_eax, pName->address);
		}
	}
	if (((fInc = is2pp("++")) || is2pp("--")) && mode == VAL)
	{
		outCode1(mov_edx_eax);
		loadValue(type, ptrs > 0);
		incdec(type, ptrs > 0, fInc, 'd');
		int k = ix.ixCode - 1;
		int inst = cd.pCode[k].inst;
		if (cd.pCode[k - 3].inst == lea_eax_pbp && cd.pCode[k - 2].inst == mov_edx_eax
			&& cd.pCode[k - 1].inst == mov_eax_pax && cd.pCode[k].num == 1
			&& (inst == add_ddx || inst == sub_ddx))
		{
			cd.pCode[k - 3].inst = mov_eax_pbp;
			cd.pCode[k - 2] = cd.pCode[k - 3];
			cd.pCode[k - 2].inst = inst == add_ddx ? inc_dbp : dec_dbp;
			ix.ixCode -= 2;
		}
	}
	else if (pName->size[depth + 1] == 0 && mode == VAL)
	{
		loadValue(type, ptrs > 0);
	}
	setValue(mode, ptrs, type, pv);
}

void primaryExpression(int mode, VALUE *pv)
{
	if (cd.token[ix.tix].type >= TK_STRING)
	{
		constant(pv);
	}
	else if (is(ID.SIZEOF))
	{
		sizeOf(pv);
	}
	else if (!is('(') && isN('(', 1))
	{
		functionCall(pv);
		if (pv->type == ID.DOUBLE)
			outCode1(fstp_st1);
	}
	else
	{
		primExpr(mode, pv);
	}
}

void initDLL(char *dllnames)
{
	int i, n;
	char *dllname = strtok(dllnames, ";");
	for (n = 0; dllname != NULL; dllname = strtok(NULL, ";"))
	{
		ULONG hM = (ULONG)LoadLibrary(dllname);
		if (hM == 0) error("exe.initDLL", "DLL '%s' Æ’ÂÂ[Æ’hÆ’GÆ’â€°Â[", dllname);
		IMAGE_DOS_HEADER *pDOSHeader = (IMAGE_DOS_HEADER *)hM;
		IMAGE_NT_HEADERS *pPEHeader = (IMAGE_NT_HEADERS *)(hM + pDOSHeader->e_lfanew);
		IMAGE_DATA_DIRECTORY *pD = pPEHeader->OptionalHeader.DataDirectory;
		IMAGE_EXPORT_DIRECTORY *pE = (IMAGE_EXPORT_DIRECTORY*)(hM + pD[0].VirtualAddress);
		char **ppszFunctionName = (char**)((int)pE->AddressOfNames + hM);
		for (i = 0; i < pE->NumberOfNames; i++)
		{
			char *name = (char*)(ppszFunctionName[i] + hM);
			int ix = put(strncmp(name, "__p_", 4) == 0 ? (name + 4) : name, NULL, &cd.hash);
			cd.hash.tbl[ix].val = (void*)(AT_IMPT | n);
		}
		FreeLibrary((HMODULE)hM);
		exe.dll[n++].dllname = dllname;
	}
	exe.nDLL = n;
}

int importDLL()
{
	int k, n, size = 0;
	for (n = 0; n < exe.nDLL; n++)
	{
		if (exe.dll[n].nFunc == 0) continue;
		size += strlen(exe.dll[n].dllname) + 1;
		exe.useDLL++;
		for (k = 0; k < exe.dll[n].nFunc; k++)
			size += strlen(toString(exe.dll[n].idFunc[k])) + 3;
		exe.useFunc += exe.dll[n].nFunc;
	}
	size += exe.useDLL * 20 + 20;
	size += (exe.useFunc + exe.useDLL) * 4 * 2;
	size = ((size - 1) / 16 + 1) * 16;
	return size;
}

BYTE *getImport()
{
	int i, n = 0, k;
	DWORD *pImpt = calloc(exe.lenImpt, 1);
	if (pImpt == NULL) error("exe.getImport", "Error to import function");
	int nLookup = (exe.useDLL + 1) * 5;
	int nImptAddr = nLookup + (exe.useDLL + exe.useFunc);
	int nName = (nImptAddr + (exe.useDLL + exe.useFunc)) * 4;
	for (i = 0; i < exe.nDLL; i++)
	{
		if (exe.dll[i].nFunc == 0) continue;
		pImpt[n * 5] = mem.ImptAddr + nLookup * 4;
		pImpt[n * 5 + 4] = mem.ImptAddr + nImptAddr * 4;
		for (k = 0; k < exe.dll[i].nFunc; k++)
		{
			int ix = exe.dll[i].idFunc[k];
			char *fname = toString(ix);
			cd.hash.tbl[ix].val = (void*)(AT_IMPT | (pImpt[n * 5 + 4] + k * 4));
			pImpt[nLookup++] = mem.ImptAddr + nName;
			pImpt[nImptAddr++] = mem.ImptAddr + nName;
			strcpy((char*)pImpt + nName + 2, fname);
			nName += strlen(fname) + 3;
		}
		nLookup++;
		nImptAddr++;
		pImpt[n * 5 + 3] = mem.ImptAddr + nName;
		strcpy((char*)pImpt + nName, exe.dll[i].dllname);
		nName += strlen(exe.dll[i].dllname) + 1;
		n++;
	}
	return (BYTE*)pImpt;
}

typedef struct _Export
{
	int  addr;
	char *name;
} Export;

Export export[100];
int nExport;

int cmpName(const void *a, const void *b)
{
	return strcmp(((Export *)a)->name, ((Export *)b)->name);
}

int cmpAddr(const void *a, const void *b)
{
	return *((int *)a) - *((int *)b);
}

int initExport()
{
	int n;
	int size = strlen(cmd.outfile) + 1;
	for (n = 0; n < cd.hash.size; n++)
	{
		char *name = cd.hash.tbl[n].key;
		int val = (int)cd.hash.tbl[n].val;
		if (name != NULL && (val&AT_EXPT) != 0)
		{
			export[nExport].addr = mem.CodeAddr + (val&AT_ADDR);
			export[nExport++].name = name;
			size += 10 + strlen(name) + 1;
		}
	}
	qsort(export, nExport, sizeof(Export), cmpName);
	return sizeof(IMAGE_EXPORT_DIRECTORY) + size;
}

BYTE *getExport()
{
	int n;
	int sizeIED = sizeof(IMAGE_EXPORT_DIRECTORY);
	int baseMem = mem.ExptAddr;
	int baseRVA = baseMem + sizeIED;
	char *buf = xalloc(exe.lenExpt);
	IMAGE_EXPORT_DIRECTORY *pIed = (IMAGE_EXPORT_DIRECTORY *)buf;
	IMAGE_EXPORT_DIRECTORY ied =
	{
		0, 0, 0, 0, baseRVA + 10 * nExport,
		1, nExport, nExport, baseRVA, baseRVA + 4 * nExport, baseRVA + 8 * nExport
	};
	memcpy(buf, &ied, sizeof(ied));
	DWORD *pAddr = (DWORD*)(buf + sizeIED);
	DWORD *pName = (DWORD*)(buf + sizeIED + 4 * nExport);
	WORD  *pOrd = (WORD*)(buf + sizeIED + 8 * nExport);
	char  *pStr = buf + sizeIED + 10 * nExport;
	for (n = 0; n < nExport; n++)
		pAddr[n] = export[n].addr;
	strcpy(pStr, cmd.outfile);
	pStr += strlen(cmd.outfile) + 1;
	for (n = 0; n < nExport; n++)
	{
		pName[n] = baseMem + (pStr - buf);
		strcpy(pStr, export[n].name);
		pStr += strlen(export[n].name) + 1;
		pOrd[n] = n;
	}
	return (BYTE*)buf;
}

void initReloc()
{
	int n;
	qsort(exe.locs, exe.nLocs, sizeof(int), cmpAddr);
	exe.nPages = exe.nLocs > 0 ? (exe.locs[exe.nLocs - 1] - 1) / 4096 : 0;
	for (n = 0; n < exe.nLocs; n++)
		exe.cnt[exe.locs[n] / 4096 - 1]++;
	mem.RelocAddr = mem.DataAddr + (mem.DataSize + MEMALIGN1) & ~MEMALIGN1;
	mem.RelocSize = exe.nPages * 8 + exe.nLocs * 2;
	raw.RelocAddr = raw.DataAddr + raw.DataSize;
	raw.RelocSize = (mem.RelocSize + RAWALIGN1) & ~RAWALIGN1;
}

BYTE *getReloc()
{
	int n, k, ix = 0;
	BYTE *buf = xalloc(raw.RelocSize);
	WORD *p = (WORD*)buf;
	for (n = 0; n < exe.nPages; n++)
	{
		int *q = (int*)p;
		*q++ = (n + 1) * 4096;		// addr
		*q++ = 8 + exe.cnt[n] * 2;	// size
		p = (WORD*)q;
		for (k = 0; k < exe.cnt[n]; k++)
			*p++ = 0x3000 + (exe.locs[ix++] & 0x0FFF);
	}
	return buf;
}

typedef struct _EXE_HEADER
{
	IMAGE_DOS_HEADER      DosHeader;
	BYTE 		  DosStub[64];
	DWORD                 Signature;	// "PE\0\0"
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
	IMAGE_SECTION_HEADER  SectionHeaders[2];
} EXE_HEADER;

void writeExe(BYTE *bufImport, BYTE *bufExport, BYTE *CodeBuffer,
	int lenCode, BYTE *DataBuffer, int lenData)
{
	BYTE *bufReloc = NULL;
	int sizeImptAddrTable = (exe.useDLL + exe.useFunc) * 4;
	int posImptAddrTable = mem.DataAddr + (exe.useDLL + 1) * 20 + sizeImptAddrTable;
	int numSections = mcc.typeApp == 0 ? 3 : 2;	// dll: 3  exe: 2
	int typeApp = mcc.typeApp == 0 ? 2 : mcc.typeApp;
	int exptAddr = mcc.typeApp == 0 ? mem.ExptAddr : 0;
	EXE_HEADER exeHeader =
	{
		{
			// IMAGE_DOS_HEADER
			0x5A4D, 0x90, 3, 0, 4,                // "MZ", ...
			0, 0xFFFF, 0, 0xB8,	                  // min-, maxalloc, ss, sp
			0, 0, 0, 0x40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0x0080,
		},
		"\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21"
		"This program cannot be run in DOS mode.\r\r\n$",	// DosStub
		0x00004550,				  // "PE\0\0"
		{
			// IMAGE_FILE_HEADER;
			0x014c, numSections, 0, 0, 0,	  // Intel 386
			sizeof(IMAGE_OPTIONAL_HEADER), 0x030F  // 0x00E0, Characteristics
		}, {   // IMAGE_OPTIONAL_HEADER
			0x010B, 0x06, 0x00,			  // Magic, Linker Version
			0, 0, 0, exe.entryPoint, 		  // AddressOfEntryPoint
			mem.CodeAddr, mem.DataAddr,	  	  // Base of Code, Data.
			exe.base, MEMALIGN, RAWALIGN,	  // ImageBase, Section-, File-Alignment
			4, 0, 0, 0, 4, 0, 0,		  // OS, Image, Subsystem Version
			exe.sizeImage, RAWALIGN, 0,		  // Image size, Header size, ...
			typeApp, 0,			  	  // 2/3: gui/console, DllCharacteristics
			0x100000, 0x1000, 0x100000, 0x1000,	  // stack & heap reserve, commit
			0, 0x10,
			{ { exptAddr, exe.lenExpt }, { mem.ImptAddr, (exe.useDLL + 1) * 20 },    // 0,1
			{ 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
			{ posImptAddrTable, sizeImptAddrTable }, { 0, 0 }, { 0, 0 }, { 0, 0 }
			}	    // 12
		}, { {
				".text", { mem.CodeSize }, mem.CodeAddr, raw.CodeSize,
				raw.CodeAddr, 0, 0, 0, 0, 0x60000020  // Code | Executable | Readable
			}, {
				".data", { mem.DataSize }, mem.DataAddr, raw.DataSize,
				raw.DataAddr, 0, 0, 0, 0, 0xC0000040   // InitializedData|Readable|Writeable
			}
		}
	};
	IMAGE_SECTION_HEADER reloc =  	// 0x1C8-0x1EF
	{
		".reloc", { mem.RelocSize }, mem.RelocAddr, raw.RelocSize, raw.RelocAddr,
		0, 0, 0, 0, 0x42000040	// Initialized Data | Discardable | Readable
	};

	BYTE *bufEXE = xalloc(raw.DataAddr + raw.DataSize);
	memcpy(bufEXE, &exeHeader, sizeof(exeHeader));
	if (mcc.typeApp == 0)
	{
		memcpy(bufEXE + sizeof(exeHeader), &reloc, sizeof(reloc));
		bufReloc = getReloc();
	}
	memcpy(bufEXE + RAWALIGN, CodeBuffer, lenCode);
	memcpy(bufEXE + raw.ImptAddr, bufImport, exe.lenImpt);
	if (bufExport != NULL)
		memcpy(bufEXE + raw.ImptAddr + exe.lenImpt, bufExport, exe.lenExpt);
	memcpy(bufEXE + raw.ImptAddr + exe.lenImpt + exe.lenExpt, DataBuffer, lenData);
	FILE *fpDst = fopen(cmd.outfile, "wb");
	if (fpDst == NULL) error("writeExe", "'%s' open error", cmd.outfile);
	fwrite(bufEXE, raw.DataAddr + raw.DataSize, 1, fpDst);
	if (bufReloc != NULL) fwrite(bufReloc, raw.RelocSize, 1, fpDst);
	fclose(fpDst);
}

void setDLL(int ixDLL, int id)
{
	int n = 0;
	while (n < exe.dll[ixDLL].nFunc && exe.dll[ixDLL].idFunc[n] != id)
		n++;
	if (n >= exe.dll[ixDLL].nFunc)
	{
		exe.dll[ixDLL].nFunc++;
		exe.dll[ixDLL].idFunc[n] = id;
	}
}

void instAt(int n, int *pinst, int *pnum, int *pattr, int *psize)
{
	*pinst = cd.pCode[n].inst;
	*pnum = cd.pCode[n].num;
	*pattr = cd.pCode[n].attr;
	*psize = cd.pCode[n].size;
}

int setOffset(int *pLoc, int *pOffCode, int cnt)
{
	int n, inst, num, attr, size, nChg = 0;
	int offCode = 0;
	for (n = 0; n < ix.ixCode; n++)
	{
		instAt(n, &inst, &num, &attr, &size);
		INSTRUCTION *pI = &instruction[inst - 1];
		char *fmt = pI->mnemonic;
		if ((opt&oASM) && cnt == 0)
		{
			printf((strchr(fmt, ':') != NULL ? "%4d: " : "%4d:     "), n);
			if (inst == setreal)
				printf(fmt, cd.pCode[n].dval, cd.pCode[n].offset);
			else if (inst == setstr)
				printf(fmt, cd.pCode[n].sval, cd.pCode[n].offset);
			else
				printf(fmt, num, cd.pCode[n].offset);
			if (fmt[0] == 'j') printf(attr == AD_CONST ? "%d" : "loc_%03d", num);
			else if (inst == xret && num > 0) printf(" %d", num);
			else if (attr != 0)	printf("[%d]", attr);
			if (pI->opcode == fn_ || pI->opcode == call)
				printf("[%s]", cd.hash.tbl[num].key);
			printf("\n");
		}
		if (setint <= inst && inst <= setstr) continue;
		if (cnt == 0) cd.pCode[n].size = 0;		// default
		cd.pCode[n].offset = offCode;
		if (pI->opcode == loc_ && num <= ix.ixLoc)
		{
			pLoc[num] = offCode;
		}
		else if (pI->opcode == fn_ || pI->opcode == exp_)
		{
			if (strcmp(cd.hash.tbl[num].key, "_main") == 0)
				exe.entryPoint = offCode;
			int at = ((int)cd.hash.tbl[num].val) & 0xFF000000;
			cd.hash.tbl[num].val = (void*)(at + offCode);
		}
		else if (pI->mnemonic[0] == 'j')
		{
			int fByte = abs(pLoc[num] - offCode - 2) < 128;
			int fS = (opt&oUOPT) ? (pLoc[num] > 0 && fByte) : (cnt == 0 || fByte);
			fS = (fS || attr == AD_CONST);
			offCode += strlen(fS ? pI->hexcode2 : pI->hexcode) / 2 + (fS ? 1 : 4);
			int sizeOld = cd.pCode[n].size;
			cd.pCode[n].size = fS ? 1 : 4;
			if (cd.pCode[n].size != sizeOld) nChg++;
		}
		else if (pI->opcode == call)
		{
			int fImpt = ((int)cd.hash.tbl[num].val) & AT_IMPT;
			offCode += fImpt ? 6 : 5;
			if (fImpt && cnt == 0) setDLL((int)cd.hash.tbl[num].val & AT_ADDR, num);
		}
		else if (pI->opcode == xret)    // C20400     ret     4
		{
			offCode += num == 0 ? 2 : 4;
			cd.pCode[n].size = num == 0 ? 0 : 2;
		}
		else if (abs(num) < 128 && (pI->regs_size & 0x01) == 1)
		{
			offCode += strlen(pI->hexcode2) / 2 + 1;
			cd.pCode[n].size = 1;
		}
		else if (abs(num) < (1 << 15) && (pI->regs_size & 0x02) == 2)
		{
			offCode += strlen(pI->hexcode2) / 2 + 2;
			cd.pCode[n].size = 2;
		}
		else if (pI->hexcode != NULL && pI->hexcode[0] != '\0')
		{
			offCode += strlen(pI->hexcode) / 2 + (pI->regs_size & 0x04);
			cd.pCode[n].size = pI->regs_size & 0x04;
		}
		else
		{
			error("linker", "mnemonic=%s, num=%d", pI->mnemonic, num);
		}
		if (cnt == 0 && attr == AD_IMPORT)
			setDLL((int)cd.hash.tbl[num].val & AT_ADDR, num);
	}
	*pOffCode = offCode;
	return nChg;
}

void link()
{
	int  n, inst, num, attr, size, offset, offCode, cnt = 0, nChg;
	char *p, *hexcode, *caText;

	int *pLoc = xalloc((ix.ixLoc + 1) * sizeof(int));
	initDLL(cmd.impfiles);
	do
	{
		nChg = setOffset(pLoc, &offCode, cnt++);
	} while (!(opt&oUOPT) && nChg > 0);

	mem.CodeAddr = MEMALIGN;
	mem.CodeSize = offCode;
	raw.CodeAddr = RAWALIGN;
	raw.CodeSize = (offCode + RAWALIGN1) & ~RAWALIGN1;

	exe.lenImpt = importDLL();
	exe.lenExpt = (opt&oDLL) ? initExport() : 0;
	int posData = exe.lenImpt + exe.lenExpt;


	mem.DataAddr = mem.CodeAddr + (offCode + MEMALIGN1) & ~MEMALIGN1;
	mem.DataSize = exe.lenImpt + exe.lenExpt + ix.ixData + ix.ixZero;

	raw.DataAddr = raw.CodeAddr + raw.CodeSize;
	raw.DataSize = (exe.lenImpt + exe.lenExpt + ix.ixData + RAWALIGN1) & ~RAWALIGN1;
	mem.ImptAddr = mem.DataAddr;
	raw.ImptAddr = raw.DataAddr;
	mem.ExptAddr = mem.ImptAddr + exe.lenImpt;
	raw.ExptAddr = raw.ImptAddr + exe.lenImpt;

	exe.base = (opt&oDLL) ? DLLBASE : IMAGEBASE;
	exe.entryPoint += mem.CodeAddr;
	BYTE *bufImport = getImport();
	BYTE *bufExport = (opt&oDLL) ? getExport() : NULL;
	p = caText = xalloc(raw.CodeSize);
	int dataAddr = exe.base + mem.DataAddr + posData;
	for (n = 0; n < ix.ixCode; n++)
	{
		BYTE  *q;
		instAt(n, &inst, &num, &attr, &size);
		offset = cd.pCode[n].offset;
		INSTRUCTION *pI = &instruction[inst - 1];
		hexcode = size == 1 ? pI->hexcode2 : pI->hexcode;
		if (hexcode == NULL) continue;
		int fReloc = TRUE;
		if (pI->opcode == call)
		{
			int fImpt = (int)cd.hash.tbl[num].val & AT_IMPT;  // FF10:call dwdptr[eax]
			int fUser = (int)cd.hash.tbl[num].val & AT_USER;
			int addr = (int)cd.hash.tbl[num].val & AT_ADDR;
			if (!fImpt && !fUser) error("linker", "'%s' undeclared", toString(num));
			num = fImpt ? (exe.base + addr) : (addr - cd.pCode[n + 1].offset);
			hexcode = fImpt ? "FF15" : "E8";
			if (!fImpt)	fReloc = FALSE;
			size = 4;
		}
		else if (pI->mnemonic[0] == 'j')
		{
			int sizeInst = size == 1 ? 2 : inst == jmp ? 5 : 6;
			if (attr != AD_CONST) num = pLoc[num] - (cd.pCode[n].offset + sizeInst);
			fReloc = FALSE;
		}
		else if (attr == AD_CODE)
		{
			int foffset = (int)cd.hash.tbl[num].val & AT_ADDR;
			num = exe.base + mem.CodeAddr + foffset;
		}
		else if (attr == AD_DATA)
		{
			num += dataAddr;
		}
		else if (attr == AD_ZERO)
		{
			num += dataAddr + ix.ixData;
		}
		else if (attr == AD_IMPORT)
		{
			num = exe.base + ((int)cd.hash.tbl[num].val&AT_ADDR);
		}
		else
		{
			fReloc = FALSE;
		}
		for (q = hexcode; *q != '\0'; q += 2)
			*p++ = (htoi(*q) << 4) + htoi(q[1]);
		if (size == 1) *p = num;
		else if (size == 2) *((short*)p) = num;
		else if (size == 4) *((int*)p) = num;
		if (fReloc) exe.locs[exe.nLocs++] = num - exe.base;
		p += size;
	}
	char  *caData = calloc(ix.ixData + 1, sizeof(char));
	for (n = 0; n < ix.ixCode; n++)
	{
		instAt(n, &inst, &num, &attr, &size);
		offset = cd.pCode[n].offset;
		if (inst < setint || setstr < inst) continue;
		if (inst == setint && attr == 1) caData[offset] = num;
		else if (inst == setint && attr == 2) *((short*)&caData[offset]) = num;
		else if (inst == setint && attr == 4) *((int*)&caData[offset]) = num;
		else if (inst == setint) *((int*)&caData[offset]) = num + dataAddr;
		else if (inst == setreal) *((double*)&caData[offset]) = cd.pCode[n].dval;
		else xstrcpy(&caData[offset], cd.pCode[n].sval);	// setstr
	}
	if (opt&oDLL)
	{
		initReloc();
		exe.sizeImage = mem.RelocAddr + (mem.RelocSize + MEMALIGN1) & ~MEMALIGN1;
	}
	else
	{
		exe.sizeImage = mem.DataAddr + (mem.DataSize + MEMALIGN1) & ~MEMALIGN1;
	}
	writeExe(bufImport, bufExport, caText, offCode, caData, ix.ixData);
}


void printUsage()
{
	printf("%s", "Usage: cc [options] file...\n"
		"Options:\n"
		"  -shared    Generate a DLL file\n"
		"  -o <file>  ÂSet output file name\n"
		"  -l<name>   DLL <name>.dll â€š link with a dll\n"
		"  -E         Expression tree\n"
		"  -token     Tokens list\n"
		"  -asm       Generate ASM code\n"
		"  --lines    ÂLine count\n");
	exit(0);
}

void main(int argc, char *argv[])
{
	char *impfiles = "msvcrt.dll;kernel32.dll;user32.dll;gdi32.dll;";
	char *opts[] = { "-E", "-shared", "-token", "-asm", "--lines", "-trace", "-debug", "-name", "--opt" };
	int  flags[] = { oSRC, oDLL, oTOKEN, oASM, oLINES, oTRACE, oDEBUG, oNAME, oUOPT, 0 };
	int  k, n, len;

	mcc.nPreFile = -1;
	strcpy(cmd.impfiles, impfiles);
	GetModuleFileName(NULL, cmd.MCCDIR, MAX_PATH);
	if ((len = strlen(cmd.MCCDIR)) <= 12)
		error("main", "ModulePath=%s", cmd.MCCDIR);
	cmd.MCCDIR[len - 12] = '\0';	//  <CC>\bin\mcc.exe
	for (n = 1; n < argc; n++)
	{
		for (k = 0; flags[k] > 0 && strcmp(argv[n], opts[k]) != 0; k++);
		if (flags[k] > 0) opt |= flags[k];
		else if (strcmp(argv[n], "-o") == 0) strcpy(cmd.outfile, argv[++n]);
		else if (strncmp(argv[n], "-l", 2) == 0)
			sprintf(cmd.impfiles + strlen(cmd.impfiles), "%s.dll;", argv[n] + 2);
		else if (argv[n][0] != '-') cmd.srcfile[cmd.nSrc++] = argv[n];
		else error("main", "invalid option -- '%s'", argv[n]);
	}
	if (argc == 1) printUsage();
	if (cmd.nSrc == 0) error("main", "No source files to build");

	opt |= oUOPT;

	ix.tix = -1;
	char path[MAX_PATH], *p, *q;
	initHash('s', 1000, &mcc.hash);
	initPrepro();
	for (n = 0; n < cmd.nSrc; n++)
	{
		struct _finddata_t fdata;
		int hFile = _findfirst((q = cmd.srcfile[n]), &fdata);
		if (hFile == -1) error("mcc.main", "file '%s' not found", q);
		if ((p = strrchr(q, '/')) == NULL) p = strrchr(q, '\\');
		do
		{
			sprintf(path, "%.*s%s", p == NULL ? 0 : p - q + 1, q, fdata.name);
			prepro(path);
		} while (_findnext(hFile, &fdata) == 0);
	}
	mcc.nPreFile = -1;
	addStartup();
	if (opt&oSRC) exit(0);
	if (opt&oLINES)
	{
		printf("%-24s\t%5d\n", " Ââ€¡    Å’v  ", mcc.totalLines);
		exit(0);
	}
	if (cmd.outfile[0] == '\0')
	{
		q = mcc.srcFile[mcc.mainfile];
		if ((p = strrchr(q, '/')) == NULL) p = strrchr(q, '\\');
		strcpy(cmd.outfile, p != NULL ? p + 1 : q);
		p = strrchr(cmd.outfile, '.');
		strcpy(p, (opt&oDLL) ? ".dll" : ".exe");
	}
	freeHash(&mcc.hash);

	lex();

	initInstruction();
	initHash('x', HASHSIZE_TOKEN, &cd.hash);
	parse();
	if (opt&oNAME) printNameTable(globTable);

	link();
}
