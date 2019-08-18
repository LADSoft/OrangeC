/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
/* 
 * error handler
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "diag.h"

extern int prm_cmangle;
extern int prm_errfile;
extern FILE *errFile;
extern int global_flag;
extern FILE *listFile;
extern int errlineno;
extern char *errfile;
extern int prm_asmfile;
extern FILE * outputFile;
extern int prm_maxerr;
extern int prm_diag;
extern int prm_listfile;
extern int lastch;
extern enum e_sym lastst;
extern char lastid[];
extern int inclfile[10]; /* shared with preproc */
extern int incldepth; /* shared with preproc */
extern char *infile, *srcfile;
extern SYM *currentfunc;
extern int prm_warning, prm_cplusplus, prm_extwarning, prm_quieterrors, prm_c99;
extern int prm_ansi;

int diagcount = 0;
int referrorlvl = 3;
ERRORS *errlist = 0;
static ERRORS *errtail = 0;
static ERRORS *curerr = 0;
static int errline;

/* table of which warnings are enabled */
char nowarn[ERR_MAX];
/* table of warning keywords for the -w command line option
 */
char warnarray[ERR_MAX][4] = 
{
    "all", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "cln", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
        "ret", "sun", "sud", "sas", "npo", "urc", "fun", "cno", "ieq", "", 
        "nco", "lad", "", "", "zer", "dpc", "nsf", "lun", "pro", "cnv", "", 
        "irg", "san", "ssu", "", "", "", "", "", "tua", "", "tui", "", "", "", 
        "", "", "", "", "", "", "", "", "", "", "", "", "", "suz", "fsu", "lli",
        "", "", "", "", "", "spc", "", "", "", "", "", "", "", "", "", "", "", 
        "", "", "", "", "", "IPR", "", "", "", "", "", "", "", "", "", "", "", 
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "",
};
int total_errors = 0;
void initerr(void)
{
    errlist = errtail = curerr = 0;
    total_errors = 0;
    diagcount = 0;
    errline = 0;
}

/*
 * handling for warnings on the command line
 */
void warning_setup(char select, char *string)
{
    int bool = FALSE;
    while (*string)
    {
        int i;
        if (string[0] == '-')
        {
            bool = TRUE;
            string++;
        }
        else
            if (string[0] == '+')
                string++;
		if (string[0])
		{
	        for (i = 0; i < ERR_MAX; i++)
	        if (!strncmp(warnarray[i], string, 3))
	        {
	            if (i == 0)
	            {
	                int j;
	                for (j = 0; j < ERR_MAX; j++)
	                    nowarn[j] = (unsigned char)bool;
					prm_extwarning = TRUE;
	            }
	            else
	                nowarn[i] = (unsigned char)bool;
	            string += 3;
	            break;
	        }
	        if (i == ERR_MAX)
	        {
	            fatal("Invalid warning");
	        }
		}
    }
}

//-------------------------------------------------------------------------

#ifdef DIAGNOSTICS
    void diag(char *s)
    /*
     * internal diags come here
     */
    {
        diagcount++;
        if (prm_diag)
        {
            char buf[256];
            if (currentfunc)
                sprintf(buf, "%s(%s)", s, currentfunc->name);
            else
                strcpy(buf, s);
            printf("DIAG - %s\n", buf);
            if (prm_errfile && errFile)
                fprintf(errFile, "/*DIAG - %s*/\n", buf);
            if (prm_listfile && listFile)
                fprintf(listFile, "/*DIAG - %s*/\n", buf);
            if (prm_asmfile)
                oprintf(outputFile, ";DIAG - %s\n", buf);
        }
    }
#endif 
char *getpunct (int punct)
{
	switch(punct)
	{
		case begin: return "{";
		case openpa: return "(";
		case closepa: return ")";
		case semicolon: return ";";
		case end: return "}";
		case closebr: return "]";
		case comma: return ",";
		case colon: return ":";
		case assign: return "=";
		case lt: return "<";
		case gt: return ">";
		default: return "unknown";
	}
}
int printerr(char *buf0, ERRORS *err)
/*
 * subroutine gets the error code and returns whether it is an error or
 * warning
 */
{
    char buf[256];
    int errlvl = 0;
    switch (err->errornumber)
    {
        case ERR_NOCASE:
            sprintf(buf, "Expected 'case' or 'default'");
            break;
        case ERR_PUNCT:
            sprintf(buf, "Expected '%s'", getpunct((int)err->data));
            break;
        case ERR_INSERT:
            sprintf(buf, "Inserted '%s'", getpunct((int)err->data));
            break;
        case ERR_NEEDCHAR:
            sprintf(buf, "Expected '%c'", (char)err->data);
            break;
        case ERR_ILLCHAR:
            sprintf(buf, "Illegal character '%c'", (char)err->data);
            break;
        case ERR_NEEDCONST:
            sprintf(buf, "Constant value expected");
            break;
        case ERR_UNDEFINED:
            sprintf(buf, "Undefined symbol %s", (char*)err->data);
            break;
        case ERR_DUPSYM:
            sprintf(buf, "Duplicate symbol '%s'", (char*)err->data);
            break;
        case ERR_IDENTEXPECT:
            sprintf(buf, "Expected '%s'", (char*)err->data);
            break;
        case ERR_IDEXPECT:
            sprintf(buf, "Identifier expected");
            break;
        case ERR_EXTRAIDENT:
            sprintf(buf,"Too many identifiers");
            break;
        case ERR_INITSIZE:
            sprintf(buf, "Too many initializers");
            break;
        case ERR_NOINIT:
            sprintf(buf, "Cannot initialize '%s' here", (char*)err->data);
            break;
        case ERR_PREPROCID:
            sprintf(buf, "Invalid preprocessor directive '%s'", (char*)err
                ->data);
            break;
        case ERR_INCLFILE:
            sprintf(buf, "File name expected in #include directive");
            break;
        case ERR_CANTOPEN:
            sprintf(buf, "Cannot open file \"%s\" for read access", (char*)err
                ->data);
            break;
        case ERR_ILLCLASS:
            sprintf(buf, "Illegal storage class specifier '%s'", (char*)err
                ->data);
            break;
        case ERR_ILLCLASS2:
            sprintf(buf, "Illegal storage class specifier on '%s'", (char*)err
                ->data);
            break;
        case ERR_DUPCASE:
            sprintf(buf, "Duplicate case %d", (int)err->data);
            break;
        case ERR_RETMISMATCH:
            sprintf(buf, "Type mismatch in return");
            break;
        case ERR_ARGMISMATCH:
            sprintf(buf, "Type mismatch in arg '%s'", (char*)err->data);
            break;
        case ERR_ARGLENSHORT:
            sprintf(buf, 
                "parameter list too short in redeclaration of function '%s'", 
                (char*)err->data);
            break;
        case ERR_ARGLENLONG:
            sprintf(buf, 
                "parameter list too long in redeclaration of function '%s'", 
                (char*)err->data);
            break;
        case ERR_DECLMISMATCH:
            sprintf(buf, "Type mismatch in redeclaration of '%s'", (char*)err
                ->data);
            break;
        case ERR_CALLMISMATCH:
            sprintf(buf, "Type mismatch in arg '%s'", (char*)err->data);
            break;
        case ERR_CALLLENSHORT:
            sprintf(buf, "parameter list too short '%s'", (char*)err->data);
            break;
        case ERR_CALLLENLONG:
            sprintf(buf, "parameter list too long '%s'", (char*)err->data);
            break;
        case ERR_LABEL:
            sprintf(buf, "'%s' is not a label", (char*)err->data);
            break;
        case ERR_NOPOINTER:
            sprintf(buf, "Pointer type expected");
            break;
        case ERR_LVALUE:
            sprintf(buf, "Lvalue expected");
            break;
        case ERR_NOFUNC:
            sprintf(buf, "'%s' is not a function", (char*)err->data);
            break;
        case ERR_MISMATCH:
            sprintf(buf, "Type mismatch");
            break;
        case ERR_ELSE:
            sprintf(buf, "Misplaced else");
            break;
        case ERR_EXPREXPECT:
            sprintf(buf, "Expression expected");
            break;
        case ERR_DEREF:
            sprintf(buf, "Illegal pointer");
            break;
        case ERR_UNEXPECT:
            if (lastst == id)
                sprintf(buf, "Unexpected '%s'", lastid);
            else
                sprintf(buf, "Unexpected '%c'", lastch);
            break;
        case ERR_ILLTYPE:
            sprintf(buf, "Illegal typedef of '%s'", (char*)err->data);
            break;
        case ERR_ARRAYMISMATCH:
            sprintf(buf, "Non-scalar or negative array index");
            break;
        case ERR_PREPROCMATCH:
            sprintf(buf, "Unbalanced preprocessor directives");
            break;
        case ERR_MACROSUBS:
            sprintf(buf, "Macro substitution error");
            break;
        case ERR_DECLEXPECT:
            sprintf(buf, "Declaration expected");
            break;
        case ERR_INVFLOAT:
            sprintf(buf, "Invalid floating point");
            break;
        case ERR_INVTRAP:
            sprintf(buf, "Invalid trap id");
            break;
        case ERR_BFILLEGAL:
            sprintf(buf, "Cannot use bit field as a non-member");
            break;
        case ERR_BFTOOBIG:
            sprintf(buf, "Bit field too big");
            break;
        case ERR_BFTYPE:
            sprintf(buf, "Bit field only allowed on scalar types");
            break;
        case ERR_ERROR:
            sprintf(buf, "Error Directive: %s", (char*)err->data);
            break;
        case ERR_WARNING:
            sprintf(buf, "Warning Directive: %s", (char*)err->data);
            errlvl = 1;
            break;
        case ERR_INTERP:
            sprintf(buf, "%s", (char*)err->data);
            break;
        case ERR_BFADDR:
            sprintf(buf, "Cannot take address of bit field");
            break;
        case ERR_MODCONS:
            sprintf(buf, "Cannot modify a const object");
            break;
        case ERR_SUSPICIOUSCONST:
            sprintf(buf, "Suspicious use of const pointer");
            errlvl = 1;
            break;
        case ERR_SZTYPE:
            sprintf(buf, "Type expected in sizeof");
            break;
        case ERR_FUNCRETVAL2:
            if (!prm_c99 && !prm_cplusplus)
                errlvl = 1;
            sprintf(buf, "Function should return a value");
            break;
        case ERR_FUNCRETVAL:
            errlvl = 1 ;
            sprintf(buf, "Function should return a value");
            break;
        case ERR_STATICSYMUNUSED:
            sprintf(buf, "Static variable '%s' is never used", (char*)err->data)
                ;
            errlvl = 3;
            break;
        case ERR_SYMUNUSED:
            sprintf(buf, "Variable '%s' is never used", (char*)err->data);
            errlvl = 3;
            break;
        case ERR_PARAMUNUSED:
            sprintf(buf, "Parameter '%s' is never used", (char*)err->data);
            errlvl = 3;
            break;
        case ERR_FUNCUNUSED:
            sprintf(buf, "Static function '%s' is never used", (char*)err->data)
                ;
            errlvl = 3;
            break;
        case ERR_SYMUNDEF:
            sprintf(buf, "Possible use of '%s' before assignment", (char*)err
                ->data);
            errlvl = 3;
            break;
        case ERR_SYMASSIGNED:
            sprintf(buf, 
                "Variable '%s' is possibly assigned a value which is never used", (char*)err->data);
            errlvl = 3;
            break;
        case ERR_NONPORT:
            sprintf(buf, "Nonportable pointer conversion");
            if (!(int)err->data)
                errlvl = 1;
            break;
        case ERR_UNREACHABLE:
            sprintf(buf, "Unreachable code");
            errlvl = 1;
            break;
        case ERR_CODENONE:
            sprintf(buf, "Code has no effect");
            errlvl = 1;
            break;
        case ERR_BADEQUATE:
            sprintf(buf, "Possible incorrect assignment");
            errlvl = 1;
            break;
        case ERR_NOANDREG:
            sprintf(buf, "Cannot take address of register variable '%s'", 
                (char*)err->data);
            break;
        case ERR_NOCONTINUE:
            sprintf(buf, "Break or Continue outside loop");
            break;
        case ERR_DUPLABEL:
            sprintf(buf, "Duplicate label '%s'", (char*)err->data);
            break;
        case ERR_NOFUNCARRAY:
            sprintf(buf, "Function cannot return array");
            break;
        case ERR_NOVOIDRET:
            sprintf(buf, "Return type is void");
            errlvl = 1;
            break;
        case ERR_ZEROSTORAGE:
            sprintf(buf, "Size of '%s' is unknown or zero", (char*)err->data);
            break;
        case ERR_ZEROPTR:
            sprintf(buf, "Illegal use of void pointer");
            break;
        case ERR_SHORTPOINTER:
            sprintf(buf, "Dangerous pointer conversion");
            errlvl = 1;
            break;
        case ERR_NOSTATICFUNC:
            sprintf(buf, "Nonexistant static func '%s'", (char*)err->data);
            break;
        case ERR_UNUSEDLABEL:
            errlvl = 3;
            sprintf(buf, "Unused label '%s'", (char*)err->data);
            break;
        case ERR_NOPROTO:
            sprintf(buf, "Call to function '%s' with no prototype", (char*)err
                ->data);
            errlvl = 1;
            //                                    errlvl = prm_cplusplus==0;
            break;
        case ERR_LOSTCONV:
            sprintf(buf, "Conversion may truncate significant digits");
            errlvl = 3;
            break;
        case ERR_UNDEFLABEL:
            sprintf(buf, "Undefined label '%s'", (char*)err->data);
            break;
        case ERR_ILLREGISTER:
            sprintf(buf, "Illegal register var '%s'", (char*)err->data);
            errlvl = 1;
            break;
        case ERR_SUPERAND:
            sprintf(buf, "Possible superfluous &");
            errlvl = 3;
            break;
        case ERR_NODECLARE:
            sprintf(buf, "Declaration not allowed here");
            break;
        case ERR_NOMAIN:
            sprintf(buf, "Illegal call to main() from within program");
            break;
        case ERR_NOREF:
            sprintf(buf, "Illegal use of reference operator");
            break;
        case ERR_CANTREF:
            sprintf(buf, 
                "Cannot define a reference to a pointer or a reference");
            break;
        case ERR_TEMPUSED:
            sprintf(buf, "Temporary used for parameter '%s'", (char*)err->data);
            errlvl = 3;
            break;
        case ERR_REFMUSTINIT:
            sprintf(buf, "Reference member '%s' must be initialized", (char*)
                err->data);
            break;
        case ERR_TEMPINIT:
            sprintf(buf, "Temporary used to initialize '%s'", (char*)err->data);
            errlvl = referrorlvl;
            break;
        case ERR_REFLVALUE:
            sprintf(buf, "Reference initialization needs lvalue");
            break;
        case ERR_REFNOCONS:
            sprintf(buf, 
                "Reference member '%s' in a class with no constructors", (char*)
                err->data);
            break;
        case ERR_CONSTNOCONS:
            sprintf(buf, "Constant member '%s' in a class with no constructors",
                (char*)err->data);
            break;
        case ERR_MISSINGDEFAULT:
            sprintf(buf, "Default missing after parameter '%s'", (char*)err
                ->data);
            break;
        case ERR_AMBIGFUNC:
            sprintf(buf, "Ambiguity between '%s'", (char*)err->data);
            break;
        case ERR_NOLOCALDEFAULT:
            sprintf(buf, 
                "Local variables may not be used as parameter defaults");
            break;
        case ERR_CPPMISMATCH:
            sprintf(buf, "Cannot convert %s", (char*)err->data);
            break;
        case ERR_NOOVERMAIN:
            sprintf(buf, "Cannot overload 'main'", (char*)err->data);
            break;
        case ERR_SWITCHINT:
            sprintf(buf, "Switch controlling expression must be of integral type");
            break;
        case ERR_NOFUNCMATCH:
            sprintf(buf, "Could not find a match for '%s'", (char*)err->data);
            break;

        case ERR_PREDEFSTRUCT:
            sprintf(buf, "'%s' must be a predefined class or struct", (char*)
                err->data);
            break;
        case ERR_LOCALCLASS:
            sprintf(buf, "Local class functions not supported", (char*)err
                ->data);
            break;
        case ERR_PUREDECL:
            sprintf(buf, "Illegal pure declaration syntax of '%s'", (char*)err
                ->data);
            break;
        case ERR_BADESTRUCT:
            sprintf(buf, "Destructor for class '%s' expected", (char*)err->data)
                ;
            break;
        case ERR_TYPECONSTRUCT:
            sprintf(buf, "Constructor/destructor must be untyped");
            break;
        case ERR_NOTYPEQUAL:
            sprintf(buf, "Variable '%s' cannot have a type qualifier", (char*)
                err->data);
            break;
        case ERR_NOTACLASS:
            sprintf(buf, "Variable '%s' is not a class instance", (char*)err
                ->data);
            break;
        case ERR_SIZE:
            if (err->data)
                sprintf(buf, "Size of '%s' is unknown or zero", (char*)err
                    ->data);
            else
                sprintf(buf, "Size is unknown or zero");
            break;
        case ERR_NEVERSTRUCT:
            sprintf(buf, "Structure '%s' is undefined", (char*)err->data);
            errlvl = 3;
            break;

        case ERR_LONGLONG:
            sprintf(buf, 
                "long long int type not supported, defaulting to long int");
            errlvl = 1;
            break;
        case ERR_UPDOWN:
            sprintf(buf, 
                "Startup/rundown function '%s' is unknown or not a function", 
                (char*)err->data);
            break;
        case ERR_INTBITFIELDS:
            sprintf(buf, "Bit fields must be signed or unsigned int");
            break;
        case ERR_COMMENTMATCH:
            sprintf(buf, "File ended with comment in progress");
            break;
        case ERR_NESTEDCOMMENT:
            sprintf(buf, "Nested comments");
            errlvl = 3;
            break;
        case ERR_PASCAL_NO_ELLIPSE:
            sprintf(buf, "Ellipse (...) not allowed in pascal declaration");
            break;
        case ERR_PASCAL_NO_INT:
            sprintf(buf, "_interrupt keyword not allowed in pascal declaration")
                ;
            break;
        case ERR_SUSPICIOUS:
            sprintf(buf, "Suspicious pointer conversion");
            errlvl = 1;
            break;
        case ERR_NOFUNCSTRUCT:
            sprintf(buf, "Function declaration not allowed here");
            break;
        case ERR_STRINGTOOBIG:
            sprintf(buf, "String constant too long");
            break;
        case ERR_FPCON:
            sprintf(buf, "Invalid floating point constant");
            break;
        case ERR_CONSTTOOLARGE:
            sprintf(buf, "Numeric constant is too large");
            break;
        case ERR_MULTIPLEINIT:
            sprintf(buf, "Multiple initialization for '%s'", (char*)err->data);
            break;
        case ERR_INVALIDSTRING:
            sprintf(buf, "Invalid string operation");
            break;
        case ERR_AMODEEXPECT:
            sprintf(buf, "Assembler: Address mode expected");
            break;
        case ERR_ASCALE:
            sprintf(buf, "Assembler: Valid scale factor expected");
            break;
        case ERR_AINVINDXMODE:
            sprintf(buf, "Assembler: Invalid indexing");
            break;
        case ERR_AILLADDRESS:
            sprintf(buf, "Assembler: Invalid address mode");
            break;
        case ERR_ATOOMANYSPECS:
            sprintf(buf, "Assembler: Too many specifiers");
            break;
        case ERR_ATOOMANYSEGS:
            sprintf(buf, "Assembler: Too many segments");
            break;
        case ERR_AINVOP:
            sprintf(buf, "Assembler: Invalid opcode");
            break;
        case ERR_AINVSIZE:
            sprintf(buf, "Assembler: Size mismatch");
            break;
        case ERR_AUSELEA:
            sprintf(buf, 
                "Assembler: Must use LEA to take the address of a local variable");
            break;
        case ERR_ALABEXPECT:
            sprintf(buf, "Assembler: Label expected");
            break;
        case ERR_ANEEDFP:
            sprintf(buf, "Assembler: Floating point register expected");
            break;
        case ERR_AINVSELPROC:
            sprintf(buf, "Assembler: Invalid for selected processor");
            break;
        case ERR_ATOOMANYREGS:
            sprintf(buf, "Assembler: Too many registers");
            break;
        case ERR_AAREGEXPECT:
            sprintf(buf, "Assembler: Address reg expected");
            break;

        case ERR_PREPIG:
            sprintf(buf, "Preprocessor directive ignored");
            errlvl = 1;
            break;
        case ERR_ANEEDAD:
            sprintf(buf, "Assembler: Need adress or data register");
            break;
        case ERR_AREGMISMATCH:
            sprintf(buf, "Assembler: Register type mismatch");
            break;
        case ERR_CHAR4CHAR:
            sprintf(buf, "Character constant must be 1 to 4 characters");
            break;
        case ERR_LOSTCONVCASE:
            sprintf(buf, "CASE value out of range");
            break;
        case ERR_TYPEQUALEXP:
            sprintf(buf, "Type qualifier expected");
            errlvl = 1;
            break;
        case ERR_TYPENAMEEXP:
            sprintf(buf, "Type name expected");
            errlvl = 1;
            break;
        case ERR_CANTHAVETYPE:
            sprintf(buf, 
                "Constructor/Destructor '%s' cannot have a return type qualifier", (char*)err->data);
            break;
        case ERR_VIRTFUNC:
            sprintf(buf, "Virtual keyword must be used on a class function");
            break;
        case ERR_CONSFUNC:
            sprintf(buf, "Constructor/Destructor must be a function");
            break;
        case ERR_NODESTRUCTARG:
            sprintf(buf, "Destructor cannot have parameters");
            break;
        case ERR_NODESTRUCTQUAL:
            sprintf(buf, "Constructor/Destructor must be pure");
            break;
        case ERR_BASESTRUCTCLASS:
            sprintf(buf, "Base class must be a struct or class type");
            break;
        case ERR_LOCALCLASSNOFUNC:
            sprintf(buf, "Local class may not have functions");
            break;
        case ERR_CONSDECLNOMATCH:
            sprintf(buf, "Constructor declaration does not match");
            break;
        case ERR_NOBASECONS:
            sprintf(buf, "No base constructor found for class '%s'", (char*)err
                ->data);
            break;
        case ERR_CONSNOREFINIT:
            sprintf(buf, "Reference member '%s' not initialized in constructor",
                (char*)err->data);
            break;
        case ERR_CONSNOCONSTINIT:
            sprintf(buf, "Constant member '%s' not initialized in constructor",
                (char*)err->data);
            break;
        case ERR_STATICNOTHIS:
            sprintf(buf, "'this' cannot be used without an object");
            break;
        case ERR_STATICNOACCESS:
            sprintf(buf, "Member '%s' cannot be used without an object", (char*)
                err->data);
            break;
        case ERR_NOCLASSNOTHIS:
            sprintf(buf, "'this' may only be used in member functions");
            break;
        case ERR_MAINNOPTR:
            sprintf(buf, "Cannot take the address of 'main'");
            break;
        case ERR_OLDFUNCSTYLE:
            sprintf(buf, "Old K&R function style is obsolete");
            break;
        case ERR_CLASSINSTDOTSTAR:
            sprintf(buf, "Class instance must be on left of .* or ->*");
            break;
        case ERR_MEMBERPTRMISMATCH:
            sprintf(buf, "Member pointer types do not match in assignment");
            break;
        case ERR_NOASSIGNCOMPLEXMEMBERPTR:
            sprintf(buf, 
                "Cannot use arithmetic assignments with member pointers");
            break;
        case ERR_NOLOCALNAMESPACE:
            sprintf(buf, "Cannot use namespaces within a function");
            break;
        case ERR_NAMESPACEEXP:
            sprintf(buf, "Namespace name expected");
            break;
        case ERR_NOADDRMEMBERPTR:
            sprintf(buf, "Cannot take the address when .* or ->* are used");
            break;
        case ERR_ILLSTRUCT:
            sprintf(buf, "Illegal structure operation");
            break;
        case ERR_NOTACCESSIBLE:
            sprintf(buf, "'%s' is not accessible", (char*)err->data);
            break;
        case ERR_UNIONNOVIRT:
            sprintf(buf, "Unions cannot have virtual functions");
            break;
        case ERR_UNIONNOBASE:
            sprintf(buf, "Unions cannot serve as a base class");
            break;
        case ERR_NOCLASSMEMBERFUNC:
            sprintf(buf, "'%s' is not a class member function", (char*)err
                ->data);
            break;
        case ERR_NOINTRINSFOUND:
            sprintf(buf, 
                "Unknown intrinsic function '%s', defaulting to external", 
                (char*)err->data);
            errlvl = 1;
            break;
        case ERR_NONSTATICMUSTCLASS:
            sprintf(buf, "Must use . or -> to call '%s'", (char*)err->data);
            break;
        case ERR_NOVOID:
            sprintf(buf, "Void value not allowed here");
            break;
        case ERR_POINTTOSTRUCT:
            sprintf(buf, 
                "Pointer to structure required on left side of -> or ->*");
            break;
        case ERR_ADDROFSTRUCT:
            sprintf(buf, "Structure required on left side of . or .*");
            break;
        case ERR_MEMBERNAMEEXP:
            sprintf(buf, "Member name expected");
            break;
        case ERR_MUSTCALLMEMBER:
            sprintf(buf, "Member function must be called or its address taken");
            break;
        case ERR_OPERATOREXPECTED:
            sprintf(buf, "Overloadable operator expected");
            break;
        case ERR_OPERATORMUSTCLASS:
            sprintf(buf, "'%s'must be class member or have a class parameter", 
                (char*)err->data);
            break;
        case ERR_CONSTNOMEMBER:
            sprintf(buf, "Only member functions may be 'const' or 'volatile'");
            break;
        case ERR_USERERR:
            sprintf(buf, "User: %s", (char*)err->data);
            break;
        case ERR_USERWARN:
            sprintf(buf, "User: %s", (char*)err->data);
            errlvl = 1;
            break;
        case ERR_DECL0ARG:
            sprintf(buf, "'%s' must be defined with no parameters", (char*)err
                ->data);
            break;
        case ERR_DECL1ARG:
            sprintf(buf, "'%s' must be defined with one parameter", (char*)err
                ->data);
            break;
        case ERR_DECL2ARG:
            sprintf(buf, "'%s' must be defined with two parameters", (char*)err
                ->data);
            break;
        case ERR_DECL0OR1ARG:
            sprintf(buf, "'%s' must be defined with zero or one parameters", 
                (char*)err->data);
            break;
        case ERR_DECL1OR2ARG:
            sprintf(buf, "'%s' must be defined with one or two parameters", 
                (char*)err->data);
            break;
        case ERR_SIZE_T_NEW:
            sprintf(buf, 
                "First parameter to 'operator new' must be type (size_t)");
            break;
        case ERR_VOIDSTAR_NEW:
            sprintf(buf, "return type of 'operator new' must be type (void *)");
            break;
        case ERR_VOIDSTAR_DELETE:
            sprintf(buf, "Parameter to 'operator delete' must be type (void *)")
                ;
            break;
        case ERR_VOID_DELETE:
            sprintf(buf, "'operator delete' cannot return a value");
            break;
        case ERR_OPMUSTBEMEMBER:
            sprintf(buf, "'%s' must be a member function", (char*)err->data);
            break;
        case ERR_POINTTOCLASS:
            sprintf(buf, "'%s' must return a pointer to a class", (char*)err
                ->data);
            break;
        case ERR_OVPOSTFIXINTLAST:
            sprintf(buf, 
                "overloaded postfix operator '%s' must have 'int' as type of last parameter", (char*)err->data);
            break;
        case ERR_NOREFARRAY:
            sprintf(buf, "Array of references not allowed");
            break;
        case ERR_AMBIGCONTEXT:
            sprintf(buf, "'%s' is ambiguous in this context", (char*)err->data);
            break;
        case ERR_WRONGMACROARGS:
            sprintf(buf, 
                "Wrong number of macro arguments in call to macro '%s'", (char*)
                err->data);
            break;
        case ERR_OBJECTNEEDED:
            sprintf(buf, "Object required to use member");
            break;
        case ERR_NOSTATIC:
            sprintf(buf, "Storage class 'static' is not allowed here");
            break;
        case ERR_ILLINIT:
            sprintf(buf, "Illegal initialization");
            break;
        case ERR_NOTUNAMBIGUOUSBASE:
            sprintf(buf, "'%s' is not an unambiguous base class of '%s'", (
                (char **)err->data)[0], ((char **)err->data)[1]);
            break;
        case ERR_NOINITSTATICMEMBER:
            sprintf(buf, 
                "'%s' is a static data member and cannot be initialized here", 
                (char*)err->data);
            break;
        case ERR_MEMBERREINIT:
            sprintf(buf, "Member '%s' is initialized more than once", (char*)
                err->data);
            break;
        case ERR_NEWARRAYINIT:
            sprintf(buf, "Array created with 'new' cannot have initializer");
            break;
        case ERR_VOIDFUNCNOVALUE:
            sprintf(buf, "Void functions cannot return a value");
            if (!prm_cplusplus && !prm_c99)
                errlvl++ ;
            break;
        case ERR_FRIENDMUSTCLASS:
            sprintf(buf, "Storage class 'friend' not allowed here");
            break;
        case ERR_FRIENDNEEDFUNC:
            sprintf(buf, 
                "Storage class 'friend' needs a function without a body");
            break;
        case ERR_FRIENDCLASSNEEDSTRUC:
            sprintf(buf, "Storage class 'friend class' needs a structure");
            break;
        case ERR_ADDRMEMORYLOC:
            sprintf(buf, "Must take the address of a memory location");
            break;
        case ERR_INVCONST:
            sprintf(buf, "Invalid constant value");
            break;
        case ERR_INVALIDTEMPLATE:
            sprintf(buf, "Invalid template declaration");
            break;
        case ERR_TEMPLATECLASSFUNC:
            sprintf(buf, "Template must declare a class or function");
            break;
        case ERR_TEMPLATEEOF:
            sprintf(buf, "End of file encountered in template");
            break;
        case ERR_DECLARATIONCOMMA:
            sprintf(buf, "Declaration missing ';'");
            break;
        case ERR_TEMPLATENOINSTANTIATION:
            sprintf(buf, "Cannot instantiate template here");
            break;
        case ERR_INCOMPATTYPECONV:
            sprintf(buf, "Incompatible type conversion");
            break;
        case ERR_TEMPLATEBODYDECLARED:
            sprintf(buf, "Body has already been defined for template '%s'", 
                (char*)err->data);
            break;
        case ERR_TEMPLATEEXTRAARG:
            sprintf(buf, "Extra argument in invocation of template '%s'", 
                (char*)err->data);
            break;
        case ERR_TEMPLATEFEWARG:
            sprintf(buf, "Too few argumens in invocation of template '%s'", 
                (char*)err->data);
            break;
        case ERR_ILLEGALNAMESPACEUSE:
            sprintf(buf, "Illegal use of namespace '%s'", (char*)err->data);
            break;
        case ERR_NOTNSORTYPQUAL:
            sprintf(buf, "'%s' is not a namespace or type qualifier", (char*)
                err->data);
            break;
        case ERR_INVALIDARGTOTYPEOF:
            sprintf(buf, "Invalid argument to typeof operator");
            break;
        case ERR_TEMPLATEFUNCMATCHCLASS:
            sprintf(buf, "Invalid template qualified name");
            break;
        case ERR_EXPMEMBERPTRFUNC:
            sprintf(buf, "Expected member pointer function declarator");
            break;
        case ERR_NOTPUBLICBASECLASS:
            sprintf(buf, "'%s' is not a public base class of '%s'", ((char **)
                err->data)[0], ((char **)err->data)[1]);
            break;
        case ERR_ALSOBASECLASS:
            sprintf(buf, "'%s' is also a base class of '%s'", ((char **)err
                ->data)[0], ((char **)err->data)[1]);
            break;
        case ERR_MEMBERSAMENAME:
            sprintf(buf, "Member '%s' has the same name as its base class", (
                (char*)err->data));
            break;
        case ERR_CONSTMUSTINIT:
            sprintf(buf, "Constant value needs initialization");
            break;
        case ERR_TYPENOVIRTFUNC:
            sprintf(buf, 
                "Type '%s' is not a defined class with virtual functions", 
                (char*)err->data);
            break;
        case ERR_NOXCEPT:
            sprintf(buf, "Exception/RTTI handling is disabled");
            break;
        case ERR_STRUCTEXP:
            sprintf(buf, "Structured type expected");
            break;
        case ERR_CATCHEXP:
            sprintf(buf, "'catch' expected");
            break;
        case ERR_ELLIPSEHANDLERLAST:
            sprintf(buf, "The '...' handler must be last");
            break;
        case ERR_DUPHANDLER:
            sprintf(buf, "Duplicate handler for '%s'", (char*)err->data);
            break;
        case ERR_CANTRESOLVE:
            sprintf(buf, "Cannot resolve function '%s'", (char*)err->data);
            break;
        case ERR_TYPEINFOUNDEF:
            sprintf(buf, "'typeinfo' is undefined");
            break;
        case ERR_DECLNOTALLOWED:
            sprintf(buf, "Variable declaration not allowed here");
            break;
        case ERR_OPERATORNOTDEFINED:
            sprintf(buf, "Could not find a match for operator");
            break;

        case ERR_REDECLDEFAULT:
            sprintf(buf, "Default parameter %s redeclared", (char*)err->data);
            break;
        case ERR_OBJMEMPTRREQ:
            sprintf(buf, "Object required to use member pointer %s", (char*)err
                ->data);
            break;
        case ERR_REDEFINE:
            sprintf(buf, "Redefinition of macro '%s' changes value", (char*)err
                ->data);
            if (!prm_ansi)
                errlvl++;
            break;
        case ERR_AMBIGTEMPLATE:
            sprintf(buf, "Cannot instantiate template '%s'", (char*)err->data);
            break;
        case ERR_SIMPLETEMPLATEARGS:
            sprintf(buf, 
                "Non-typed template arguments must be an unstructured type");
            break;
        case ERR_SPECIALIZATION:
            sprintf(buf, 
                "Template '%s' does not specialize an existing template", 
                (char*)err->data);
            break;
        case ERR_EXPLICITNOTALLOWED:
            sprintf(buf, "Storage class 'explicit' not allowed here");
            break;
        case ERR_NOEXPLICITHERE:
            sprintf(buf, "Explicit constructor cannot be used implicitly");
            break;
        case ERR_RETURNLOCAL:
            sprintf(buf, "Returning local object");
            errlvl += !prm_cplusplus;
            break;
        case ERR_REDEFTEMPLATEDEFAULTS:
            sprintf(buf, 
                "Redefinition of default values for template arguments");
            break;
        case ERR_TEMPLATEDEFAULTLIMITED:
            sprintf(buf, 
                "Default values may only be specified on primary class templates");
            break;
        case ERR_NEEDSPECIAL:
            sprintf(buf, "Cannot use template %s here without specializing", 
                (char*)err->data);
            break;
        case ERR_NEEDSTRING:
            sprintf(buf, "String constant required");
            break;
        case ERR_PARAMMISSINGNAME:
            sprintf(buf, "Parameter missing name");
            break;
        case ERR_VLABLOCKSCOPE:
            sprintf(buf, 
                "Variable Length Arrays must have block or parameter scope");
            break;
        case ERR_FLEXARRAYLAST:
            sprintf(buf, "Flexible Array must be last member of structure");
            break;
        case ERR_RESTRICTPTR:
            sprintf(buf, "'restrict' qualifier needs pointer");
            break;
        case ERR_MAINNOINLINE:
            sprintf(buf, "Cannot use inline specifier on 'main'");
            break;
        case ERR_INLINENOSTATIC:
            sprintf(buf, 
                "Cannot use non-const static members in inline functions");
            break;
        case ERR_VMSTATIC:
            sprintf(buf, 
                "Global pointers to variable length arrays must be 'static'");
            break;
        case ERR_VLAMUSTSIZE:
            sprintf(buf, 
                "Variable length array specifier must have explicit size except in prototypes");
            break;
        case ERR_DEFINEENUM:
            sprintf(buf, "Enumeration tag must be previously defined");
            break;
        case ERR_INLINEFUNC:
            sprintf(buf, "'inline' qualifier must be used with a function");
            break;
        case ERR_INLINENOBODY:
            sprintf(buf, "inline function %s used without a body",(char *)err->data);
            break;
        case ERR_ARRELEMSIZE:
            sprintf(buf,"Array element must be a complete type");
            break ;
        case ERR_SELECTSCALAR:
            sprintf(buf,"Expression must be scalar");
            break;
        case ERR_VLAGOTO:
            sprintf(buf,"'goto' cannot bypass variable length array initialization");
            break;
        case ERR_FUNCBADRET:
            sprintf(buf,"Functions cannot return functions or arrays");
            break;
        case ERR_ILLUSEPTR:
            sprintf(buf,"Illegal use of pointer");
            break;
        case ERR_ILLPTRADD:
            sprintf(buf,"Illegal pointer addition");
            break;
        case ERR_INVPTRSUB:
            sprintf(buf,"Invalid pointer subtraction");
            break;
        case ERR_HASDEF:
            sprintf(buf,"Redefinition of switch default");
            break;
        case ERR_CASENOSWITCH:
            sprintf(buf,"case or default outside a switch statement");
            break;
		case ERR_DUPVIRTUAL:
			sprintf(buf,"virtual keyword specified more than once");
			break;
		case ERR_ALLOCA:
			sprintf(buf,"alloca may only be used within a function body");
			break;
		case ERR_NONESTEDFUNC:
			sprintf(buf,"already defining a function body");
			break;
		case ERR_C99_KEYWORD:
			sprintf (buf, "Use of %s keyword requires C99", (char *)err->data);
			break;
		case ERR_TYPE_C99:
			sprintf (buf, "Use of %s type requires C99", (char *)err->data);
			break;
		case ERR_VLA_c99:
			sprintf(buf, "Use of Variable Length Arrays requires C99");
			break;
		case ERR_ARRAY_QUALIFIERS_C99:
			sprintf(buf, "Use of array index qualifiers requires C99");
			break;
		case ERR_C99_STYLE_INITIALIZATION_USED:
			sprintf(buf, "Use of this type of declarator requires C99");
			break;
		case ERR_C99_NON_CONSTANT_INITIALIZATION:
			sprintf(buf, "Use of non-constant expression requires C99");
			break;
		case ERR_C99_FLEXIBLE_ARRAY:
			sprintf(buf, "Use of flexible array requires C99");
			break;
		case ERR_C99_VARIABLE_MACRO_ARGS:
			sprintf(buf, "Use of variable macro arguments requires C99");
			break;
        default:
            sprintf(buf, "Error #%d", err->errornumber);
            break;
    }
    sprintf(buf0, "(%d) %s", err->errornumber, buf);
    return errlvl;
}

//-------------------------------------------------------------------------

void lferror(void)
/*
 * sticck an error in the list file
 */
{
    char buf[256];
    while (curerr)
    {
        int errlvl = printerr(buf, curerr);
        if (!(errlvl &1))
        {
            if (prm_listfile)
                fprintf(listFile, "**** ERROR: %s\n", buf);
        }
        else if (prm_warning && (prm_extwarning || !(errlvl &2)))
        {
            if (prm_listfile)
                fprintf(listFile, "** WARNING: %s\n", buf);
        }
        curerr = curerr->link;
    }

}

//-------------------------------------------------------------------------

void basicskim(int *skimlist)
/*
 * simple skim for a token with no nesting
 */
{
    int i;
    while (TRUE)
    {
        if (lastst == eof)
            break;
        for (i = 0; skimlist[i]; i++)
        {
            if (lastst == skimlist[i])
                return ;
        }
        getsym();
    }
}

/*
 * the following routines do token skimming and keep track of parenthesis
 * and brace nesting levels as well
 */
BALANCE *newbalance(BALANCE *bal)
{
    BALANCE *rv = xalloc(sizeof(BALANCE));
    rv->back = bal;
    rv->count = 0;
    if (lastst == openpa)
        rv->type = BAL_PAREN;
    else if (lastst == openbr)
        rv->type = BAL_BRACKET;
    else
        rv->type = BAL_BEGIN;
    return (rv);
}

//-------------------------------------------------------------------------

void setbalance(BALANCE **bal)
{
    switch (lastst)
    {
        case end:
            while (*bal && (*bal)->type != BAL_BEGIN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case closepa:
            while (*bal && (*bal)->type != BAL_PAREN)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case closebr:
            while (*bal && (*bal)->type != BAL_BRACKET)
            {
                (*bal) = (*bal)->back;
            }
            if (*bal && !(--(*bal)->count))
                (*bal) = (*bal)->back;
            break;
        case begin:
            if (! *bal || (*bal)->type != BAL_BEGIN)
                *bal = newbalance(*bal);
            (*bal)->count++;
            break;
        case openpa:
            if (! *bal || (*bal)->type != BAL_PAREN)
                *bal = newbalance(*bal);
            (*bal)->count++;
            break;

        case openbr:
            if (! *bal || (*bal)->type != BAL_BRACKET)
                *bal = newbalance(*bal);
            (*bal)->count++;
            break;
    }
}

//-------------------------------------------------------------------------

void expskim(int *skimlist, int tpl)
{
    BALANCE *bal = 0;
    int i;
    while (TRUE)
    {
        setbalance(&bal);
        if (lastst == eof)
            break;
        if (!bal)
        {
            for (i = 0; skimlist[i]; i++)
                if (lastst == skimlist[i])
                    return ;
        }
        getsym();
    }

}

//-------------------------------------------------------------------------

void tplskm(int *skmlist)
{
    expskim(skmlist, TRUE);
}

//-------------------------------------------------------------------------

void basicerror(int n, void *data)
/*
 * standard routine for putting out an error
 */
{
    char buf[256];
    ERRORS *nexterr;
    int errlvl, errored = 0;
	char *name = errfile;
	if (strchr(srcfile, '\\') != 0 || strchr(srcfile, ':') != 0)
	{
		name = fullqualify(errfile);
	}
    ;
    global_flag++;
    nexterr = xalloc(sizeof(ERRORS));
    global_flag--;
    nexterr->errornumber = n;
    nexterr->link = 0;
    nexterr->data = data;
    if (errlist == 0)
        errlist = errtail = nexterr;
    else
    {
        errtail->link = nexterr;
        errtail = nexterr;
    }
    errlvl = printerr(buf, nexterr);
    if (curerr == 0)
        curerr = nexterr;
    if (!(errlvl &1))
    {
        errline = errlineno;
        if (!prm_quieterrors)
            fprintf(stdout, "Error   %s(%d):  %s", name, errlineno, buf);
        if (prm_errfile)
            fprintf(errFile, "Error   %s(%d):  %s", name, errlineno, buf);
        errored++;
        total_errors++;
    }
    else if (prm_warning && !nowarn[n] && (errline != errlineno) && 
        (prm_extwarning || !(errlvl &2)))
    {
        errored++;
        if (!prm_quieterrors)
            fprintf(stdout, "Warning %s(%d):  %s", name, errlineno, buf);
        if (prm_errfile)
            fprintf(errFile, "Warning %s(%d):  %s", name, errlineno, buf);
    }
    if (errored)
    {
        if (currentfunc)
        {
            char *p = buf;
            if (currentfunc->value.classdata.parentns)
            {
                p = currentfunc->value.classdata.parentns->sp->name;
                p += prm_cmangle &&  *p == '_';
                sprintf(buf, "%s::", p);
                p = buf + strlen(buf);
            }
            unmangle(p, currentfunc->name);
            if (!prm_quieterrors)
                fprintf(stdout, " in function '%s'", buf);
            if (prm_errfile)
                fprintf(errFile, " in function '%s'", buf);
        }
        if (!prm_quieterrors)
            fputc('\n', stdout);
        if (prm_errfile)
            fputc('\n', errFile);
    }
    if (total_errors > prm_maxerr)
    {
        fatal("Too many errors");
    }
}

//-------------------------------------------------------------------------

void Error(char *string)
/*
 * some of the library functions required a generic error function
 *
 * we are remapping it to the C/C++ error routines
 */
{
    basicerror(ERR_INTERP, (void*)string);
}

//-------------------------------------------------------------------------

void generror(int n, int data, int *skimlist)
/*
 * most errors come here
 */
{

    basicerror(n, (void*)data);
    if (skimlist)
        expskim(skimlist, FALSE);
}

//-------------------------------------------------------------------------

void gensymerror(int n, char *data)
/*
 * errors come here if the error has a symbol name
 */
{
    char buf[256];
    if (data)
        unmangle(buf, data);
    else
        buf[0] = 0;
    global_flag++;
    basicerror(n, (void*)litlate(buf));
    global_flag--;
}

/*
 * the next two functions are for reporting full C++ functions with
 * the argument list types
 */
void genfuncerror(int n, char *func, char *data)
{
    char buf[256], buf1[256], buf2[256];
    unmangle(buf1, func);
    if (data)
    {
        unmangle(buf2, data);
        buf[0] = '\'';
        buf[1] = 0;
        strcat(buf, buf2);
        strcat(buf, "' ");
    }
    else
        buf[0] = 0;
    strcat(buf, "in call to function ");
    strcat(buf, "'");
    strcat(buf, buf1);
    strcat(buf, "'");
    global_flag++;
    basicerror(n, (void*)litlate(buf));
    global_flag--;
}

//-------------------------------------------------------------------------

void genfunc2error(int n, char *func, char *func2)
{
    char buf[256], buf1[256], buf2[256];
    unmangle(buf1, func);
    unmangle(buf2, func2);
    buf[0] = '\'';
    buf[1] = 0;
    strcpy(buf, buf2);
    strcat(buf, "'");
    strcat(buf, " and ");
    strcat(buf, "'");
    strcat(buf, buf1);
    strcat(buf, "'");
    global_flag++;
    basicerror(n, (void*)litlate(buf));
    global_flag--;
}

/*
 * C++ errors for class names and type checking
 */
void genclasserror(int n, char *name)
{
    char buf[256], buf1[256];
    unmangle(buf1, name);
    buf[0] = '\'';
    buf[1] = 0;
    strcat(buf, buf1);
    strcat(buf, "'");
    global_flag++;
    basicerror(n, (void*)litlate(buf));
    global_flag--;
}

//-------------------------------------------------------------------------

void genclass2error(int n, char *struc, char *elem)
{
    char buf[256], buf1[256], buf2[256];
    unmangle(buf1, elem);
    unmangle(buf2, struc);
    buf[0] = '\'';
    buf[1] = 0;
    strcat(buf, buf2);
    strcat(buf, "::");
    strcat(buf, buf1);
    strcat(buf, "'");
    global_flag++;
    basicerror(n, (void*)litlate(buf));
    global_flag--;
}

//-------------------------------------------------------------------------

void genmismatcherror(TYP *tp1, TYP *tp2)
{
        char buf[256], buf1[256], buf2[256];
        typenum(buf1, tp1);
        typenum(buf2, tp2);
        buf[0] = '\'';
        buf[1] = 0;
        strcat(buf, buf1);
        strcat(buf, "'");
        strcat(buf, " to ");
        strcat(buf, "'");
        strcat(buf, buf2);
        strcat(buf, "'");
        global_flag++;
        basicerror(ERR_CPPMISMATCH, (void*)litlate(buf));
        global_flag--;
}

//-------------------------------------------------------------------------

void doubleerr(int n, char *s1, char *s2)
{
    char buf1[256], buf2[256];
    char **data = xalloc(sizeof(char*) * 2);
    unmangle(buf1, s1);
    unmangle(buf2, s2);
    data[0] = litlate(buf1);
    data[1] = litlate(buf2);
    global_flag++;
    basicerror(n, (void*)data);
    global_flag--;
}

/*
 * various utilities for special case errors
 */
void expecttoken(int n, int *skimlist)
{
    if (skimlist)
        generror(ERR_PUNCT, n, skimlist);
    else
    {
        generror(ERR_INSERT, n, 0);
        getsym();
    }
}

//-------------------------------------------------------------------------

void generrorexp(int n, int data, int *skimlist)
{
    basicerror(n, (void*)data);
    if (skimlist)
        expskim(skimlist, FALSE);
    else
        getsym();
}

//-------------------------------------------------------------------------

void gensymerrorexp(int n, char *data)
{
    char buf1[256];
    global_flag++;
    if (data)
        unmangle(buf1, data);
    basicerror(n, (void*)litlate(buf1));
    global_flag--;
}

//-------------------------------------------------------------------------

void expecttokenexp(int n, int *skimlist)
{
    if (skimlist)
        generrorexp(ERR_PUNCT, n, skimlist);
    else
    {
        generrorexp(ERR_INSERT, n, 0);
        getsym();
    }
}
