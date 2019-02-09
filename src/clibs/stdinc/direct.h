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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#ifndef __DIRECT_H
#define __DIRECT_H

#ifndef __STDDEF_H
#include <stddef.h>
#endif


#ifdef __cplusplus
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
};
#endif

#endif //__DIRECT_H