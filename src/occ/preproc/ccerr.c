/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"
#include <stdarg.h>

#define ERROR			1
#define WARNING			2
#define TRIVIALWARNING	4
#define ANSIERROR 	8
#define ANSIWARNING 16

extern COMPILER_PARAMS cparams ;
extern char infile[256];
extern int preprocLine;
extern char *preprocFile;

SYMBOL *theCurrentFunc;

static LIST *listErrors;
int currentpreprocLine;
static char *currentpreprocFile;

static struct {
    char *name;
    int	level;
} errors[] = {
{ "Unknown error", ERROR },
{"Too many errors or warnings", ERROR },
{ "Constant too large",  ERROR },
{"Expected constant", ERROR },
{"Invalid constant", ERROR },
{"Invalid floating point constant", ERROR },
{"Invalid character constant", ERROR },
{"Unterminated character constant", ERROR },
{"Invalid string constant", ERROR },
{"Unterminated string constant", ERROR },
{"String constant too long", ERROR },
{"Syntax error: %c expected", ERROR },
{"Syntax error: string constant expected", ERROR},
{"Expected constant or address", ERROR},
{"Expected integer type", ERROR },
{"Expected integer expression", ERROR },
{"Identifier expected", ERROR},
{"Multiple declaration of '%s'", ERROR },
{"Undefined identifier '%s'", ERROR },
{"Too many identififiers in type", ERROR },
{"Unexpected end of file", ERROR },
{"File not terminated with End Of Line character", TRIVIALWARNING},
{"Nested Comments", TRIVIALWARNING},
{"Non-terminated comment in file started at line %d", WARNING},
{"Non-terminated preprocessor conditional in include file started at line %d", ERROR},
{"#elif without #if", ERROR},
{"#else without #if", ERROR},
{"#endif without #if", ERROR},
{"Macro substitution error", ERROR},
{"Incorrect macro argument in call to %s", ERROR},
{"Unexpected end of line in directive", ERROR},
{"Unknown preprocessor directive %s", ERROR},
{"#error: %s", ERROR},
{"Expected include file name", ERROR},
{"Cannot open include file \"%s\"", ERROR},
{"Invalid macro definition", ERROR},
{"Redefinition of macro '%s' changes value", ANSIWARNING},
{"#error: %s", ERROR},
{"#warning: %s", WARNING},
{"Previous declaration of '%s' here", WARNING},
} ;
int total_errors;
int diagcount ;

void errorinit(void)
{
    total_errors = diagcount = 0;
    currentpreprocFile = NULL;
}

static void printerr(int err, char *file, int line, ...)
{
    char buf[256];
    char infunc[256];
    char *listerr;
    char nameb[265], *name = nameb;
    
    if (!file)
        file = "unknown";
    strcpy(nameb, file);
    if (strchr(infile, '\\') != 0 || strchr(infile, ':') != 0)
    {
        name = fullqualify(nameb);
    }
    if (total_errors > cparams.prm_maxerr)
        return;
    if (err != ERR_TOO_MANY_ERRORS && err != ERR_UNDEFINED_IDENTIFIER &&
        currentpreprocFile && !strcmp(currentpreprocFile, file) && 
        line == currentpreprocLine)
        return;
    if (err >= sizeof(errors)/sizeof(errors[0]))
    {
        sprintf(buf, "Error %d", err);
    }
    else
    {
        va_list arg;	
        va_start(arg, line);
        vsprintf(buf, errors[err].name, arg);
        va_end(arg);
    }
    if ((errors[err].level & ERROR) || (cparams.prm_ansi && (errors[err].level & ANSIERROR)))
    {
        if (!cparams.prm_quiet)
            printf("Error   ");
        listerr = "ERROR";
        total_errors++;
        currentpreprocFile = file;
        currentpreprocLine = line;
    }
    else
    {
        if (!cparams.prm_warning)
            return;
        if (errors[err].level & TRIVIALWARNING)
            if (!cparams.prm_extwarning)
                return;
        if (!cparams.prm_quiet)
            printf("Warning ");
        listerr = "WARNING";
    }
        infunc[0] = 0;
    if (!cparams.prm_quiet)
        printf(" %s(%d):  %s%s\n", name, line, buf, infunc);
}
void pperror(int err, int data)
{
    printerr(err, preprocFile, preprocLine, data);
}
void pperrorstr(int err, char *str)
{
    printerr(err, preprocFile, preprocLine, str);
}
void preverror(int err, char *name, char *origfile, int origline)
{
    printerr(err, preprocFile, preprocLine, name);
    if (origfile && origline)
        printerr(ERR_PREVIOUS, origfile, origline, name);
}
void error(int err)
{
    printerr(err, preprocFile, preprocLine);
}
void errorint(int err, int val)
{
    printerr(err, preprocFile, preprocLine, val);
}
void errorstr(int err, char *val)
{
    printerr(err, preprocFile, preprocLine, val);
}
void errorsym(int err, SYMBOL *sym)
{
    char buf[256];
    strcpy(buf, sym->name);
    printerr(err, preprocFile, preprocLine, buf);
}
