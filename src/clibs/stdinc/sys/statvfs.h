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

/*  statvs.h

    Definitions used for file status functions
*/

#pragma GCC system_header

#if !defined(__STATVFS_H)
#    define __STATVFS_H

#    pragma pack(1)

#    include <_defs.h>
#    include <sys/types.h>

#define ST_RDONLY 1
#define ST_NOSUID 2

#    ifdef __cplusplus
extern "C"
{

#    endif
struct statvfs
{
    unsigned long f_bsize;    //File system block size. 
    unsigned long f_frsize;   //Fundamental file system block size. 
    fsblkcnt_t    f_blocks;   //Total number of blocks on file system in units of f_frsize. 
    fsblkcnt_t    f_bfree;    //Total number of free blocks. 
    fsblkcnt_t    f_bavail;   //Number of free blocks available to 
                             //non-privileged process. 
    fsfilcnt_t    f_files;    //Total number of file serial numbers. 
    fsfilcnt_t    f_ffree;    //Total number of free file serial numbers. 
    fsfilcnt_t    f_favail;   //Number of file serial numbers available to 
                             //non-privileged process. 
    unsigned long f_fsid;     //File system ID. 
    unsigned long f_flag;     //Bit mask of f_flag values. 
    unsigned long f_namemax;  //Maximum filename length. 
};
    int _RTL_FUNC _IMPORT statvfs(const char* restrict __name, struct statvfs* restrict __statbuf);
    int _RTL_FUNC _IMPORT fstatvfs(int __handle, struct statvfs* __statbuf);

#    ifdef __cplusplus
};
#    endif

#    pragma pack()

#endif /* __STATVFS_H */
