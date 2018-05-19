/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

#ifndef PREPROC_H
#define PREPROC_H

typedef LLONG_TYPE PPINT;
typedef ULLONG_TYPE PPUINT;
#define TRUE 1
#define FALSE 0

/* ## sequences */
#define REPLACED_TOKENIZING ((unsigned char)-80)
/* left or right-hand size of a ## when an arg has been replaced by an empty string */
#define TOKENIZING_PLACEHOLDER ((unsigned char)-79)
#define STRINGIZING_PLACEHOLDER ((unsigned char)-78)
#define REPLACED_ALREADY ((unsigned char)-77)
#define MACRO_PLACEHOLDER ((unsigned char)-76)
 
#define issymchar(x) (((x) >= 0) && (isalnum(x) || (x) == '_'))
#define isstartchar(x) (((x) >= 0) && (isalpha(x) || (x) == '_'))

#define SYMBOL_NAME_LEN 256
#define MACRO_REPLACE_SIZE (128 * 1024)
#define MAX_PACK_DATA 256
#define DEFINELIST_MAX 256

#define STD_PRAGMA_FENV 1
#define STD_PRAGMA_FCONTRACT 2
#define STD_PRAGMA_CXLIMITED 4

/* struct for preprocessor if tracking */
typedef struct ifstruct
{
    struct ifstruct *next; /* next */
    short iflevel;
    BOOLEAN elsetaken;
    int line;
} IFSTRUCT;

typedef struct _includes_
{
    struct _includes_ *next;
    FILE	*handle;
    int     anonymousid;
    int 	fileindex;
    int		line;
    int		current;
    int 	ifskip;
    int		skiplevel;
    long        filesize;
    BOOLEAN	elsetaken;
    char	*data;
    unsigned char *lptr;
    int		pos;
    int		sysflags;
    BOOLEAN	sys_inc;
    IFSTRUCT *ifs;
    unsigned char 	*ibufPtr;
    int		inputlen;
    char	*fname;
    char    *linename;
    void    *linesHead, *linesTail;
    int     first:1;
    unsigned char inputline[MACRO_REPLACE_SIZE];
    unsigned char inputbuffer[32768];
} INCLUDES;
/* #define tracking */
typedef struct _defstruct
{
    char *name;
    char *string;
    int argcount;
    int line;
    char *file;
    char **args;
    int varargs: 1;
    int permanent: 1;
    int undefined : 1 ;
    int preprocessing: 1; /* true if is currently not a candidate for preprocessing (macros only( */
} DEFSTRUCT;

typedef struct _filelist
{
    struct _filelist *next;
    char *data;
    int hascode;
} FILELIST;

typedef struct _macbuflist
{
    char buf[MACRO_REPLACE_SIZE];
    struct _macbuflist *next; // MUST BE LAST!!!
} MACROLIST;

#endif /* PREPROC_H */
