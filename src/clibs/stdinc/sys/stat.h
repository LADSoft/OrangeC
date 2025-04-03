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

/*  stat.h

    Definitions used for file status functions
*/

#pragma GCC system_header

#if !defined(__STAT_H)
#    define __STAT_H

#    pragma pack(1)

#    include <_defs.h>
#    include <sys/types.h>

#    ifndef __cplusplus
#        ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#            define _WCHAR_T_DEFINED
#        endif
#    endif

/* Traditional names for bits in st_mode.
 */
#    define S_IFMT 0xF000  /* file type mask */
#    define S_IFDIR 0x4000 /* directory */
#    define S_IFIFO 0x1000 /* FIFO special */
#    define S_IFCHR 0x2000 /* character special */
#    define S_IFBLK 0x3000 /* block special */
#    define S_IFREG 0x8000 /* or just 0x0000, regular */
/*#define S_IREAD  0x0100  owner may read */
/*#define S_IWRITE 0x0080  owner may write */
/*#define S_IEXEC  0x0040  owner may execute <directory search> */

/* POSIX file type test macros.  The parameter is an st_mode value.
 */
#    define S_ISDIR(m) (!!((m)&S_IFDIR))
#    define S_ISCHR(m) (!!((m)&S_IFCHR))
#    define S_ISBLK(m) (!!((m)&S_IFBLK))
#    define S_ISREG(m) (!!((m)&S_IFREG))
#    define S_ISFIFO(m) (!!((m)&S_IFIFO))

/* owner permission */
#    define S_IRWXU 0000700
#    define S_IRUSR 0000400
#    define S_IWUSR 0000200
#    define S_IXUSR 0000100
#    define S_IREAD 0000400
#    define S_IWRITE 0000200
#    define S_IEXEC 0000100

/* group permission.  same as owner's on PC and PenPoint*/
#    define S_IRWXG 0000070
#    define S_IRGRP 0000040
#    define S_IWGRP 0000020
#    define S_IXGRP 0000010

/* other permission.  same as owner's on PC and PenPoint*/
#    define S_IRWXO 0000007
#    define S_IROTH 0000004
#    define S_IWOTH 0000002
#    define S_IXOTH 0000001

/* setuid, setgid, and sticky.	always false on PC */
#    define S_ISUID 0004000
#    define S_ISGID 0002000
#    define S_ISVTX 0001000

#    ifdef __cplusplus
extern "C"
{
#    endif
#if !defined(__CRTDLL_DLL) && !defined(__MSVCRT_DLL) && !defined(__MSIL__)
struct _stat32
{
        int st_dev;
        short st_ino;
        short st_mode;
        int st_nlink;
        short st_uid;
        short st_gid;
        int st_rdev;
    off_t         st_size;
    __time_t_32     st_atime;
    __time_t_32     st_mtime;
    __time_t_32     st_ctime;
};

struct _stat32i64
{
        int st_dev;
        short st_ino;
        short st_mode;
        int st_nlink;
        short st_uid;
        short st_gid;
        int st_rdev;
    __int64        st_size;
    __time_t_32     st_atime;
    __time_t_32     st_mtime;
    __time_t_32     st_ctime;
};

struct _stat64i32
{
        int st_dev;
        short st_ino;
        short st_mode;
        int st_nlink;
        short st_uid;
        short st_gid;
        int st_rdev;
    off_t         st_size;
    __time_t_64     st_atime;
    __time_t_64     st_mtime;
    __time_t_64     st_ctime;
};

struct _stat64
{
        int st_dev;
        short st_ino;
        short st_mode;
        int st_nlink;
        short st_uid;
        short st_gid;
        int st_rdev;
    __int64        st_size;
    __time_t_64     st_atime;
    __time_t_64     st_mtime;
    __time_t_64     st_ctime;
};

#define __stat64 _stat64 // For legacy compatibility
#endif

    struct stat
    {
        int st_dev;
        short st_ino;
        short st_mode;
        int st_nlink;
        short st_uid;
        short st_gid;
        int st_rdev;
        off_t st_size;
        time_t st_atime;
        time_t st_mtime;
        time_t st_ctime;
    };

/* Define MS compatible names
 */
#    define _S_IFMT S_IFMT
#    define _S_IFDIR S_IFDIR
#    define _S_IFIFO S_IFIFO
#    define _S_IFCHR S_IFCHR
#    define _S_IFBLK S_IFBLK
#    define _S_IFREG S_IFREG
#    define _S_IREAD S_IREAD
#    define _S_IWRITE S_IWRITE
#    define _S_IEXEC S_IEXEC

    struct _stat
    {
#    ifdef __CRTDLL_DLL
#pragma pack(1)
        short st_dev;
        short st_ino;
        short st_mode;
        int st_nlink;
        short st_uid;
        short st_gid;
        short st_rdev;
        long st_size;
        long st_atime;
        long st_mtime;
        long st_ctime;
#pragma pack()
#    else
    int st_dev;
    short st_ino;
    short st_mode;
    int st_nlink;
    short st_uid;
    short st_gid;
    int st_rdev;
    long st_size;
    long st_atime;
    long st_mtime;
    long st_ctime;
#    endif
    };

#if !defined(__CRTDLL_DLL) && !defined(__MSVCRT_DLL) && !defined(__MSIL__)
#ifdef _USE_32BIT_TIME_T
    #define _fstat      _fstat32
    #define _fstati64   _fstat32i64
    #define _stat       _stat32
    #define _stati64    _stat32i64
    #define _wstat      _wstat32
    #define _wstati64   _wstat32i64
#else
    #define _fstat      _fstat64i32
    #define _fstati64   _fstat64
    #define _stat       _stat64i32
    #define _stati64    _stat64
    #define _wstat      _wstat64i32
    #define _wstati64   _wstat64
#endif

int _RTL_FUNC _fstat32(
     int             _FileHandle,
     struct _stat32* _Stat
    );

int _RTL_FUNC _fstat32i64(
     int                _FileHandle,
     struct _stat32i64* _Stat
    );

int _RTL_FUNC _fstat64i32(
     int                _FileHandle,
     struct _stat64i32* _Stat
    );

int _RTL_FUNC _fstat64(
     int             _FileHandle,
     struct _stat64* _Stat
    );

int _RTL_FUNC _stat32(
      char const*     _FileName,
      struct _stat32* _Stat
    );

int _RTL_FUNC _stat32i64(
      char const*        _FileName,
      struct _stat32i64* _Stat
    );

int _RTL_FUNC _stat64i32(
      char const*        _FileName,
      struct _stat64i32* _Stat
    );

int _RTL_FUNC _stat64(
      char const*     _FileName,
      struct _stat64* _Stat
    );

int _RTL_FUNC _wstat32(
      wchar_t const*  _FileName,
      struct _stat32* _Stat
    );

int _RTL_FUNC _wstat32i64(
      wchar_t const*     _FileName,
      struct _stat32i64* _Stat
    );

int _RTL_FUNC _wstat64i32(
      wchar_t const*     _FileName,
      struct _stat64i32* _Stat
    );

int _RTL_FUNC _wstat64(
      wchar_t const*  _FileName,
      struct _stat64* _Stat
    );
#ifndef _DEFINING_STAT
    #ifdef _USE_32BIT_TIME_T

        static __inline int _RTL_FUNC fstat(int const _FileHandle, struct stat* const _Stat)
        {
//            _STATIC_ASSERT(sizeof(struct stat) == sizeof(struct _stat32));
            return _fstat32(_FileHandle, (struct _stat32*)_Stat);
        }

        static __inline int _RTL_FUNC stat(char const* const _FileName, struct stat* const _Stat)
        {
//            _STATIC_ASSERT(sizeof(struct stat) == sizeof(struct _stat32));
            return _stat32(_FileName, (struct _stat32*)_Stat);
        }

    #else

        static __inline int _RTL_FUNC fstat(int const _FileHandle, struct stat* const _Stat)
        {
//            _STATIC_ASSERT(sizeof(struct stat) == sizeof(struct _stat64i32));
            return _fstat64i32(_FileHandle, (struct _stat64i32*)_Stat);
        }
        static __inline int _RTL_FUNC stat(char const* const _FileName, struct stat* const _Stat)
        {
//            _STATIC_ASSERT(sizeof(struct stat) == sizeof(struct _stat64i32));
            return _stat64i32(_FileName, (struct _stat64i32*)_Stat);
        }

    #endif
#endif
#else
    int _RTL_FUNC _IMPORT _fstat(int __handle, struct _stat* __statbuf);
    int _RTL_FUNC _IMPORT fstat(int __handle, struct stat* __statbuf);
    int _RTL_FUNC _IMPORT _wstat(const wchar_t* __path, struct _stat* __statbuf);
    int _RTL_FUNC _IMPORT _stat(const char* __path, struct _stat* __statbuf);
    int _RTL_FUNC _IMPORT stat(const char* __path, struct stat* __statbuf);
    int _RTL_FUNC _IMPORT chmod(const char* ZSTR __path, int __amode);
    int _RTL_FUNC _IMPORT fchmod(int __handle, int __amode);
    int _RTL_FUNC _IMPORT mkdir(const char* ZSTR __path, int __amode);
#endif


#    ifdef __cplusplus
};
#    endif

#    pragma pack()

#endif /* __STAT_H */
