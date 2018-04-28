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
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#ifndef __DIRECT_H
#define __DIRECT_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif


#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif
struct _diskfree_t
{
    unsigned total_clusters;
    unsigned avail_clusters;
    unsigned sectors_per_cluster;
    unsigned bytes_per_sector;
};

int     _RTL_FUNC _IMPORT _chdrive(int __drive);
char *  _RTL_FUNC _IMPORT _getdcwd(int __drive, char *__buf, int __len);
unsigned _RTL_FUNC _IMPORT _getdiskfree(unsigned, struct _diskfree_t *);
int     _RTL_FUNC _IMPORT _getdrive(void);
unsigned long _RTL_FUNC _IMPORT _getdrives(void);

int _RTL_FUNC _IMPORT chdir(const char *);
char * _RTL_FUNC _IMPORT getcwd(char *, int);
int _RTL_FUNC _IMPORT mkdir(const char *);
int _RTL_FUNC _IMPORT rmdir(const char *);

int _RTL_FUNC _IMPORT _chdir(const char *);
char * _RTL_FUNC _IMPORT _getcwd(char *, int);
int _RTL_FUNC _IMPORT _mkdir(const char *);
int _RTL_FUNC _IMPORT _rmdir(const char *);


#ifdef __cplusplus
} ;
};
#endif

#endif /* __DIRECT_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__DIRECT_H_USING_LIST)
#define __DIRECT_H_USING_LIST
    using __STD_NS_QUALIFIER _chdrive;
    using __STD_NS_QUALIFIER _getdcwd;
    using __STD_NS_QUALIFIER _getdrive;
    using __STD_NS_QUALIFIER _getdrives;
    using __STD_NS_QUALIFIER _getdiskfree;
    using __STD_NS_QUALIFIER chdir;
    using __STD_NS_QUALIFIER _chdir;
    using __STD_NS_QUALIFIER getcwd;
    using __STD_NS_QUALIFIER _getcwd;
    using __STD_NS_QUALIFIER mkdir;
    using __STD_NS_QUALIFIER _mkdir;
    using __STD_NS_QUALIFIER rmdir;
    using __STD_NS_QUALIFIER _rmdir;
    
#endif
