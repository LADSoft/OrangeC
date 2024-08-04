/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#ifndef __DIR_H
#define __DIR_H

#pragma pack(1)

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#define WILDCARDS 0x01
#define EXTENSION 0x02
#define FILENAME 0x04
#define DIRECTORY 0x08
#define DRIVE 0x10

#define MAXDRIVE 3

#define _A_NORMAL 0x00 /* Normal file, no attributes */
#define _A_RDONLY 0x01 /* Read only attribute */
#define _A_HIDDEN 0x02 /* Hidden file */
#define _A_SYSTEM 0x04 /* System file */
#define _A_VOLID 0x08  /* Volume label */
#define _A_SUBDIR 0x10 /* Directory */
#define _A_ARCH 0x20   /* Archive */

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RC_INVOKED
#ifndef _FFBLK_DEF
#    define _FFBLK_DEF
    struct ffblk
    {
        unsigned char ff_reserved[21];
        unsigned char ff_attrib; /* attribute byte for matched file */
        unsigned short ff_ftime; /* time of last write to file */
        unsigned short ff_fdate; /* date of last write to file */
        unsigned long ff_fsize;  /* size of file */
        char ff_name[13];        /* asciiz name of matched file */
        char ff_uu[256];
    };
#endif
#endif

#define MAXPATH 260
#define MAXDIR 256
#define MAXFILE 256
#define MAXEXT 256

#ifndef  RC_INVOKED
    int _RTL_FUNC _IMPORT findfirst(const char* __path, struct ffblk* __ffblk, int __attrib);
    int _RTL_FUNC _IMPORT findnext(struct ffblk* __ffblk);
    void _RTL_FUNC _IMPORT fnmerge(char* __path, const char* __drive, const char* __dir, const char* __name, const char* __ext);
    int _RTL_FUNC _IMPORT fnsplit(const char* __path, char* __drive, char* __dir, char* __name, char* __ext);
    int _RTL_FUNC _IMPORT getcurdir(int __drive, char* __directory);
    char* _RTL_FUNC _IMPORT getcwd(char* __buf, int __buflen);
    int _RTL_FUNC _IMPORT getdisk(void);
    char* _RTL_FUNC _IMPORT mktemp(char* __template);
    char* _RTL_FUNC _IMPORT searchpath(const char* __file);
    int _RTL_FUNC _IMPORT setdisk(int __drive);

    int _RTL_FUNC _IMPORT _chdir(const char* ZSTR);
    int _RTL_FUNC _IMPORT _wchdir(const wchar_t* ZSTR);
    char* ZSTR _RTL_FUNC _IMPORT _getcwd(char* ZSTR, int);
    int _RTL_FUNC _IMPORT _wmkdir(const wchar_t* ZSTR);
    int _RTL_FUNC _IMPORT _rmdir(const char* ZSTR);
    int _RTL_FUNC _IMPORT _wrmdir(const wchar_t* ZSTR);
#endif

#ifdef __cplusplus
};
#endif

#pragma pack()

#endif /* __DIR_H */
