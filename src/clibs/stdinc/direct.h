/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#pragma GCC system_header

#ifndef __DIRECT_H
#define __DIRECT_H

#    include <stddef.h>

#ifndef __STAT_H
#    include <sys/stat.h>
#endif

#ifndef RC_INVOKED
#ifdef __cplusplus
extern "C"
{
#endif
    struct _diskfree_t
    {
        unsigned total_clusters;
        unsigned avail_clusters;
        unsigned sectors_per_cluster;
        unsigned bytes_per_sector;
    };

    int _RTL_FUNC _IMPORT _chdrive(int __drive);
    char* ZSTR _RTL_FUNC _IMPORT _getdcwd(int __drive, char* ZSTR __buf, int __len);
    unsigned _RTL_FUNC _IMPORT _getdiskfree(unsigned, struct _diskfree_t*);
    int _RTL_FUNC _IMPORT _getdrive(void);
    unsigned long _RTL_FUNC _IMPORT _getdrives(void);

    char* ZSTR _RTL_FUNC _IMPORT getcwd(char* ZSTR, int);

    int _RTL_FUNC _IMPORT _chdir(const char* ZSTR);
    char* ZSTR _RTL_FUNC _IMPORT _getcwd(char* ZSTR, int);
    int _RTL_FUNC _IMPORT _mkdir(const char* ZSTR);
    int _RTL_FUNC _IMPORT _rmdir(const char* ZSTR);

#ifdef __cplusplus
};
#endif
#endif
#endif  //__DIRECT_H