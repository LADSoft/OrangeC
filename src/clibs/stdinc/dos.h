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

#if !defined(__DOS_H)
#    define __DOS_H

#    pragma pack(1)

#        include <stddef.h>

#    define FA_NORMAL 0x00 /* Normal file, no attributes */
#    define FA_RDONLY 0x01 /* Read only attribute */
#    define FA_HIDDEN 0x02 /* Hidden file */
#    define FA_SYSTEM 0x04 /* System file */
#    define FA_LABEL 0x08  /* Volume label */
#    define FA_DIREC 0x10  /* Directory */
#    define FA_ARCH 0x20   /* Archive */

#    define _A_NORMAL 0x00 /* Normal file, no attributes */
#    define _A_RDONLY 0x01 /* Read only attribute */
#    define _A_HIDDEN 0x02 /* Hidden file */
#    define _A_SYSTEM 0x04 /* System file */
#    define _A_VOLID 0x08  /* Volume label */
#    define _A_SUBDIR 0x10 /* Directory */
#    define _A_ARCH 0x20   /* Archive */

#    define SEEK_CUR 1
#    define SEEK_END 2
#    define SEEK_SET 0

#    define _HARDERR_IGNORE 0 /* ignore error */
#    define _HARDERR_RETRY 1  /* retry the operation */
#    define _HARDERR_ABORT 2  /* abort program */
#    define _HARDERR_FAIL 3   /* fail the operation */

#    ifdef __cplusplus
extern "C"
{
#    endif

    struct dfree
    {
        unsigned df_avail;
        unsigned df_total;
        unsigned df_bsec;
        unsigned df_sclus;
    };

    struct diskfree_t
    {
        unsigned total_clusters;
        unsigned avail_clusters;
        unsigned sectors_per_cluster;
        unsigned bytes_per_sector;
    };

    struct time
    {
        unsigned char ti_min;  /* Minutes */
        unsigned char ti_hour; /* Hours */
        unsigned char ti_hund; /* Hundredths of seconds */
        unsigned char ti_sec;  /* Seconds */
    };

    struct date
    {
        int da_year; /* Year - 1980 */
        char da_day; /* Day of the month */
        char da_mon; /* Month (1 = Jan) */
    };

    struct dostime_t
    {
        unsigned char hour;    /* Hours */
        unsigned char minute;  /* Minutes */
        unsigned char second;  /* Seconds */
        unsigned char hsecond; /* Hundredths of seconds */
    };

    struct dosdate_t
    {
        unsigned char day;       /* 1-31 */
        unsigned char month;     /* 1-12 */
        unsigned int year;       /* 1980 - 2099 */
        unsigned char dayofweek; /* 0 - 6 (0=Sunday) */
    };

    /* I will reorganize this properly later, do, don't depend on the size
     * field for now.  And DON'T depend on ordering of fields either
     */
    struct find_t
    {
        unsigned char reserved[21];
        unsigned char attrib;   /* attribute byte for matched file */
        unsigned short wr_time; /* time of last write to file */
        unsigned short wr_date; /* date of last write to file */
        unsigned short size[2]; /* size of file */
        char name[13];          /* asciiz name of matched file */
        char uu[256];
    };

    extern unsigned char _RTL_DATA _osmajor;
    extern unsigned char _RTL_DATA _osminor;

    unsigned _RTL_FUNC _IMPORT _dos_getdiskfree(unsigned __drive, struct diskfree_t* __dtable);
    int _RTL_FUNC _IMPORT _dos_getdrive(int* __drive);
    int _RTL_FUNC _IMPORT _dos_setdrive(int __drive, int* __ndrives);
    void _RTL_FUNC _IMPORT _dos_getdate(struct dosdate_t* __datep);
    unsigned _RTL_FUNC _IMPORT _dos_getfileattr(const char* ZSTR __filename, unsigned* __attrib);

    unsigned _RTL_FUNC _IMPORT _dos_getftime(int __fd, unsigned* __date, unsigned* __time);
    int _RTL_FUNC _IMPORT _dos_getpwd(char* ZSTR __buf, int __drive);
    void _RTL_FUNC _IMPORT _dos_gettime(struct dostime_t* __timep);
    unsigned _RTL_FUNC _IMPORT _dos_setdate(struct dosdate_t* __datep);
    unsigned _RTL_FUNC _IMPORT _dos_setfileattr(const char* ZSTR __filename, unsigned __attrib);
    int _RTL_FUNC _IMPORT _dos_setpwd(char* ZSTR __buf);
    unsigned _RTL_FUNC _IMPORT _dos_settime(struct dostime_t* __timep);
    int _RTL_FUNC _IMPORT _dos_findfirst(char* ZSTR __stringValue, int __attr, struct find_t* __buf);
    int _RTL_FUNC _IMPORT _dos_findnext(struct find_t* __buf);
    unsigned _RTL_FUNC _IMPORT _dos_open(char* ZSTR __name, unsigned __mode, int* __fd);
    unsigned _RTL_FUNC _IMPORT _dos_close(int __fd);
    unsigned _RTL_FUNC _IMPORT _dos_setftime(int __fd, unsigned __date, unsigned __time);
    unsigned _RTL_FUNC _IMPORT _dos_creat(const char* ZSTR __pathP, unsigned __attr, int* __fd);
    unsigned _RTL_FUNC _IMPORT _dos_creatnew(const char* ZSTR __pathP, unsigned __attr, int* __fd);
    void _RTL_FUNC _IMPORT disable(void);
    void _RTL_FUNC _IMPORT enable(void);
    void _RTL_FUNC _IMPORT _disable(void);
    void _RTL_FUNC _IMPORT _enable(void);
    void _RTL_FUNC _IMPORT getdfree(unsigned char __drive, struct dfree* __dtable);
    int _RTL_FUNC _IMPORT _getdrive(void);
    long _RTL_FUNC _IMPORT dostounix(struct date* __d, struct time* __t);
    void _RTL_FUNC _IMPORT unixtodos(long __time, struct date* __d, struct time* __t);
    int _RTL_FUNC _IMPORT unlink(const char* ZSTR __path);
    unsigned _RTL_FUNC _IMPORT _dos_read(int __fd, void* __buf, unsigned __len, unsigned* __nread);
    unsigned _RTL_FUNC _IMPORT _dos_write(int __fd, const void* __buf, unsigned __len, unsigned* __nread);
    void _RTL_FUNC _IMPORT getdate(struct date* __datep);
    void _RTL_FUNC _IMPORT gettime(struct time* __timep);
    void _RTL_FUNC _IMPORT setdate(struct date* __datep);
    void _RTL_FUNC _IMPORT settime(struct time* __timep);
    int _RTL_FUNC _IMPORT bdos(int __func, unsigned __regdx, int __regal);

    int _RTL_FUNC _IMPORT sleep(int __seconds);

#    if defined(__cplusplus)
    void _RTL_FUNC _IMPORT _harderr(void (*__fptr)(unsigned __deverr, unsigned __doserr, unsigned* __hdr));
#    else
void _RTL_FUNC _IMPORT _harderr(void (*__fptr)());
#    endif

    void _RTL_FUNC _IMPORT _hardresume(int __axret);
    void _RTL_FUNC _IMPORT _hardretn(int __retn);

#    define _disable() _genbyte((unsigned char)(0xfa)) /* MSC name */
#    define disable() _genbyte((unsigned char)(0xfa))  /* MSC name */
#    define _enable() _genbyte((unsigned char)(0xfb))  /* MSC name */
#    define enable() _genbyte((unsigned char)(0xfb))   /* MSC name */
#    define geninterrupt(i) \
        _genbyte(0xCD);     \
        _genbyte(i) /* Interrupt instruction */

    extern int _RTL_DATA _argc, _RTL_DATA __argc;
    extern char *ZSTR *_RTL_DATA _argv, **_RTL_DATA __argv;
    extern char* ZSTR* _RTL_DATA _environ;

#    ifdef __cplusplus
};
#    endif

#    define MK_FP(__s, __o) ((void _far*)(((void _seg*)(__s)) + ((void _near*)(__o))))
#    define _MK_FP(__s, __o) MK_FP(__s, __o)

#    define FP_SEG(__p) ((unsigned)(void _seg*)(__p))
#    define _FP_SEG(__p) ((unsigned)(void _seg*)(__p))

#    define FP_OFF(__p) ((unsigned)(__p))
#    define _FP_OFF(__p) ((unsigned)(__p))

#    pragma pack()

#endif /* __DOS_H */
