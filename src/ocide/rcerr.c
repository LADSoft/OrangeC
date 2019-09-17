/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

/*
 * error handler
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>

#include "header.h"

extern HWND hwndFrame;
extern int errlineno;
extern char* errfile;
extern int lastch;
extern enum e_sym lastst;
extern char lastid[];
extern int lineno;
extern FILE* inclfile[10]; /* shared with preproc */
extern int incldepth;      /* shared with preproc */
extern char* infile;
extern char* inputBuffer;
extern jmp_buf errjump;

int diagcount = 0;
int referrorlvl = 3;
static char expectlist[] = {"###################################################:{}.#])#,;"};
static int errline;

int total_errors = 0;
void initerr(void)
{
    total_errors = 0;
    diagcount = 0;
    errline = 0;
}

void fatal(char* fmt, ...)
{
    char buf[256];
    va_list argptr;

    va_start(argptr, fmt);
#ifdef GUI
    vsprintf(buf, fmt, argptr);
    ExtendedMessageBox("Fatal RC File error", 0, buf);
#else
    printf("Fatal error: ");
    vprintf(fmt, argptr);
    fputc('\n', stdout);
#endif
    va_end(argptr);
    longjmp(errjump, 1);
}

//-------------------------------------------------------------------------

int printerr(char* buf, int errnum, void* data)
/*
 * subroutine gets the error code and returns whether it is an error or
 * warning
 */
{
    int errlvl = 0;
    switch (errnum)
    {
        case ERR_PUNCT:
            sprintf(buf, "Expected '%c'", expectlist[(int)data]);
            break;
        case ERR_INSERT:
            sprintf(buf, "Inserted '%c'", expectlist[(int)data]);
            break;
        case ERR_NEEDCHAR:
            sprintf(buf, "Expected '%c'", (int)data);
            break;
        case ERR_ILLCHAR:
            sprintf(buf, "Illegal character '%c'", (int)data);
            break;
        case ERR_NEEDCONST:
            sprintf(buf, "Constant value expected");
            break;
        case ERR_UNDEFINED:
            sprintf(buf, "Undefined symbol '%s'", (char*)data);
            break;
        case ERR_DUPSYM:
            sprintf(buf, "Duplicate symbol '%s'", (char*)data);
            break;
        case ERR_IDENTEXPECT:
            sprintf(buf, "Expected '%s'", (char*)data);
            break;
        case ERR_IDEXPECT:
            sprintf(buf, "Identifier expected");
            break;
        case ERR_PREPROCID:
            sprintf(buf, "Invalid preprocessor directive '%s'", (char*)data);
            break;
        case ERR_INCLFILE:
            sprintf(buf, "file name expected in #include directive");
            break;
        case ERR_CANTOPEN:
            sprintf(buf, "Cannot open file \"%s\" for read access", (char*)data);
            break;
        case ERR_EXPREXPECT:
            sprintf(buf, "Expression expected");
            break;
        case ERR_UNEXPECT:
            if (lastst == ident)
                sprintf(buf, "Unexpected '%s'", lastid);
            else
                sprintf(buf, "Unexpected '%c'", lastch);
            break;
        case ERR_PREPROCMATCH:
            sprintf(buf, "Unbalanced preprocessor directives");
            break;
        case ERR_MACROSUBS:
        case ERR_WRONGMACROARGS:
            sprintf(buf, "Macro substitution error");
            break;
        case ERR_ERROR:
            sprintf(buf, "User error: %s", (char*)data);
            break;
        case ERR_INTERP:
            sprintf(buf, "%s", (char*)data);
            break;
        case ERR_COMMENTMATCH:
            sprintf(buf, "File ended with comment in progress");
            break;
        case ERR_STRINGTOOBIG:
            sprintf(buf, "String constant too long");
            break;
        case ERR_CONSTTOOLARGE:
            sprintf(buf, "Numeric constant is too large");
            break;
        case ERR_INVALIDSTRING:
            sprintf(buf, "Invalid string operation");
            break;
        case ERR_PREPIG:
            sprintf(buf, "Preprocessor directive ignored");
            errlvl = 1;
            break;
        case ERR_CHAR4CHAR:
            sprintf(buf, "Character constant must be 1 to 4 characters");
            break;
        case ERR_USERERR:
            sprintf(buf, "User: %s", (char*)data);
            break;
        case ERR_USERWARN:
            sprintf(buf, "User: %s", (char*)data);
            errlvl = 1;
            break;
        case ERR_EXTRA_DATA_ON_LINE:
            sprintf(buf, "Extra data on line");
            break;
        case ERR_BEGIN_EXPECTED:
            sprintf(buf, "BEGIN expected");
            break;
        case ERR_END_EXPECTED:
            sprintf(buf, "END expected");
            break;
        case ERR_RESOURCE_ID_EXPECTED:
            sprintf(buf, "resource identifier expected");
            break;
        case ERR_STRING_EXPECTED:
            sprintf(buf, "string expected");
            break;
        case ERR_ACCELERATOR_CONSTANT_EXPECTED:
            sprintf(buf, "Accelerator key expected");
            break;
        case ERR_NO_ASCII_VIRTKEY:
            sprintf(buf, "ASCII/VIRTKEY keywords not allowed for string key type");
            break;
        case ERR_NOT_DIALOGEX:
            sprintf(buf, "Need DIALOGEX for this feature");
            break;
        case ERR_UNKNOWN_DIALOG_CONTROL_CLASS:
            sprintf(buf, "Unknown control type");
            break;
        case ERR_VERSIONINFO_TYPE_1:
            sprintf(buf, "VERSIONINFO id must be 1");
            break;
        case ERR_UNKNOWN_RESOURCE_TYPE:
            sprintf(buf, "Unknown Resource Type");
            break;
        case ERR_INVALIDCLASS:
            sprintf(buf, "class or ID expected");
            break;
        case ERR_FIXEDDATAEXPECTED:
            sprintf(buf, "Fixed version info expected");
            break;
        case ERR_BLOCK_EXPECTED:
            sprintf(buf, "BLOCK keyword expected");
            break;
        case ERR_INVALID_VERSION_INFO_TYPE:
            sprintf(buf, "Invalid version type block");
            break;
        default:
            sprintf(buf, "Error #%d", errnum);
            break;
    }
    return errlvl;
}

//-------------------------------------------------------------------------

void basicerror(int n, void* data)
/*
 * standard routine for putting out an error
 */
{
    char buf[512];
    int errlvl;
    ;
    errlvl = printerr(buf, n, data);
    if (!(errlvl & 1))
    {
        errline = lineno;
#ifdef GUI
        ExtendedMessageBox("Fatal RC File error", 0, "Error   %s(%d):  %s", errfile, errlineno, buf);
#else
        fprintf(stdout, "Error   %s(%d):  %s", errfile, errlineno, buf);
#endif
        total_errors++;
    }
    if (total_errors)
    {
        longjmp(errjump, 1);
    }
}

//-------------------------------------------------------------------------

void Error(char* string)
/*
 * some of the library functions required a generic error function
 *
 * we are remapping it to the C/C++ error routines
 */
{
    basicerror(ERR_INTERP, (void*)string);
}

//-------------------------------------------------------------------------

void generror(int n, int data)
/*
 * most errors come here
 */
{
    if (inputBuffer)
        basicerror(n, (void*)data);
}

//-------------------------------------------------------------------------

void gensymerror(int n, char* data)
/*
 * errors come here if the error has a symbol name
 */
{
    char buf[100];
    if (data)
        strcpy(buf, data);
    else
        buf[0] = 0;
    basicerror(n, (void*)rcStrdup(buf));
}

/*
 * various utilities for special case errors
 */
void expecttoken(int n) { generror(ERR_INSERT, n); }

//-------------------------------------------------------------------------

void generrorexp(int n, int data) { basicerror(n, (void*)data); }

//-------------------------------------------------------------------------

void gensymerrorexp(int n, char* data) { basicerror(n, (void*)rcStrdup(data)); }

//-------------------------------------------------------------------------

void expecttokenexp(int n) { generrorexp(ERR_INSERT, n); }
