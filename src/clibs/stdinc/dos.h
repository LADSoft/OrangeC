/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#if !defined(__DOS_H)
#define __DOS_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#define FA_NORMAL   0x00        /* Normal file, no attributes */
#define FA_RDONLY   0x01        /* Read only attribute */
#define FA_HIDDEN   0x02        /* Hidden file */
#define FA_SYSTEM   0x04        /* System file */
#define FA_LABEL    0x08        /* Volume label */
#define FA_DIREC    0x10        /* Directory */
#define FA_ARCH     0x20        /* Archive */

#define _A_NORMAL   0x00        /* Normal file, no attributes */
#define _A_RDONLY   0x01        /* Read only attribute */
#define _A_HIDDEN   0x02        /* Hidden file */
#define _A_SYSTEM   0x04        /* System file */
#define _A_VOLID    0x08        /* Volume label */
#define _A_SUBDIR   0x10        /* Directory */
#define _A_ARCH     0x20        /* Archive */

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#define _HARDERR_IGNORE 0   /* ignore error */
#define _HARDERR_RETRY  1   /* retry the operation */
#define _HARDERR_ABORT  2   /* abort program */
#define _HARDERR_FAIL   3   /* fail the operation */

#ifdef __cplusplus
namespace std {
extern "C" {
#endif

struct  dfree
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
    unsigned char   ti_min;     /* Minutes */
    unsigned char   ti_hour;    /* Hours */
    unsigned char   ti_hund;    /* Hundredths of seconds */
    unsigned char   ti_sec;     /* Seconds */
};

struct date
{
    int     da_year;        /* Year - 1980 */
    char    da_day;     /* Day of the month */
    char    da_mon;     /* Month (1 = Jan) */
};

struct dostime_t
{
    unsigned char hour;         /* Hours */
    unsigned char minute;       /* Minutes */
    unsigned char second;       /* Seconds */
    unsigned char hsecond;      /* Hundredths of seconds */
};

struct dosdate_t
{
    unsigned char day;      /* 1-31 */
    unsigned char month;    /* 1-12 */
    unsigned int  year;     /* 1980 - 2099 */
    unsigned char dayofweek;/* 0 - 6 (0=Sunday) */
};

/* I will reorganize this properly later, do, don't depend on the size
 * field for now.  And DON'T depend on ordering of fields either
 */
struct find_t
{
    unsigned char   reserved[21];
    unsigned char   attrib;            /* attribute byte for matched file */
    unsigned short  wr_time;           /* time of last write to file */
    unsigned short  wr_date;           /* date of last write to file */
    unsigned short  size[2];           /* size of file */
    char            name[13];         /* asciiz name of matched file */
    char	    uu[256];
};

extern  unsigned char    _RTL_DATA _osmajor;
extern  unsigned char    _RTL_DATA _osminor;


unsigned   _RTL_FUNC _dos_getdiskfree(unsigned __drive,
                                                struct diskfree_t *__dtable);
int        _RTL_FUNC _dos_getdrive(int *__drive) ;
int        _RTL_FUNC _dos_setdrive(int __drive, int *__ndrives);
void       _RTL_FUNC _dos_getdate(struct dosdate_t *__datep);
unsigned   _RTL_FUNC _dos_getfileattr(const char *__filename,
                                                unsigned *__attrib);

unsigned   _RTL_FUNC _dos_getftime(int __fd, unsigned *__date, unsigned *__time);
int        _RTL_FUNC _dos_getpwd(char *__buf, int __drive) ;
void       _RTL_FUNC _dos_gettime(struct dostime_t *__timep);
unsigned   _RTL_FUNC _dos_setdate(struct dosdate_t *__datep);
unsigned   _RTL_FUNC _dos_setfileattr(const char *__filename,
                                                unsigned __attrib);
int        _RTL_FUNC _dos_setpwd(char *__buf) ;
unsigned   _RTL_FUNC _dos_settime(struct dostime_t *__timep);
int        _RTL_FUNC _dos_findfirst(char *__string, int __attr, struct find_t *__buf) ;
int        _RTL_FUNC _dos_findnext(struct find_t *__buf) ;
unsigned   _RTL_FUNC _dos_open(char *__name,unsigned __mode, int *__fd);
unsigned   _RTL_FUNC _dos_close(int __fd);
unsigned   _RTL_FUNC _dos_setftime(int __fd, unsigned __date, unsigned __time);
unsigned   _RTL_FUNC _dos_creat(const char *__pathP, unsigned __attr,
                                          int *__fd);
unsigned   _RTL_FUNC _dos_creatnew(const char *__pathP, unsigned __attr,
                                             int *__fd);
void       _RTL_FUNC disable(void);
void       _RTL_FUNC enable(void);
void	   _RTL_FUNC _disable(void);
void 	   _RTL_FUNC _enable(void);
void       _RTL_FUNC getdfree(unsigned char __drive,
                                        struct dfree *__dtable);
int        _RTL_FUNC _getdrive(void);
long       _RTL_FUNC dostounix(struct date *__d, struct time *__t);
void       _RTL_FUNC unixtodos(long __time, struct date *__d,
                                         struct time *__t);
int        _RTL_FUNC unlink(const char *__path);
unsigned   _RTL_FUNC _dos_read(int __fd, void *__buf, unsigned __len,
                                         unsigned *__nread);
unsigned   _RTL_FUNC _dos_write(int __fd, const void *__buf, unsigned __len,
                                          unsigned *__nread );
void       _RTL_FUNC getdate(struct date *__datep);
void       _RTL_FUNC gettime(struct time *__timep);
void       _RTL_FUNC setdate(struct date *__datep);
void       _RTL_FUNC settime( struct time *__timep);
int        _RTL_FUNC bdos(int __func, unsigned __regdx, int __regal);

int        _RTL_FUNC sleep(int __seconds) ;

#if defined(__cplusplus)
void     _RTL_FUNC  _harderr(void (*__fptr)(unsigned __deverr,
                            unsigned __doserr, unsigned *__hdr));
#else
void     _RTL_FUNC  _harderr(void (*__fptr)());
#endif

void     _RTL_FUNC  _hardresume(int __axret);
void     _RTL_FUNC  _hardretn(int __retn);

#define _disable()      _genbyte((unsigned char)(0xfa)) /* MSC name */
#define disable()       _genbyte((unsigned char)(0xfa)) /* MSC name */
#define _enable()       _genbyte((unsigned char)(0xfb)) /* MSC name */
#define enable()        _genbyte((unsigned char)(0xfb)) /* MSC name */
#define geninterrupt(i) _genbyte(0xCD); _genbyte(i)      /* Interrupt instruction */

extern int        _RTL_DATA _argc, _RTL_DATA __argc ;
extern char **    _RTL_DATA _argv,** _RTL_DATA __argv ;
extern char **	  _RTL_DATA _environ ;

#ifdef __cplusplus
};
};
#endif

#define MK_FP(__s,__o) ((void _far *)(((void _seg *)(__s)) + ((void _near *)(__o))))
#define _MK_FP(__s,__o)  MK_FP(__s,__o)

#define FP_SEG(__p)  ((unsigned)(void _seg *)(__p))
#define _FP_SEG(__p) ((unsigned)(void _seg *)(__p))

#define FP_OFF(__p)  ((unsigned)(__p))
#define _FP_OFF(__p) ((unsigned)(__p))

#pragma pack()

#endif /* __DOS_H */

#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__DOS_H_USING_LIST)
#define __DOS_H_USING_LIST
    using std::_dos_getdiskfree;
    using std::_dos_getdrive;
    using std::_dos_setdrive;
    using std::_dos_getdate;
    using std::_dos_getfileattr;
    using std::_dos_getftime;
    using std::_dos_getpwd;
    using std::_dos_gettime;
    using std::_dos_setdate;
    using std::_dos_setfileattr;
    using std::_dos_setpwd;
    using std::_dos_settime;
    using std::_dos_findfirst;
    using std::_dos_findnext;
    using std::_dos_open;
    using std::_dos_close;
    using std::_dos_setftime;
    using std::_dos_creat;
    using std::_dos_creatnew;
    using std::disable;
    using std::enable;
    using std::_disable;
    using std::_enable;
    using std::getdfree;
    using std::_getdrive;
    using std::dostounix;
    using std::unixtodos;
    using std::unlink;
    using std::_dos_read;
    using std::_dos_write;
    using std::getdate;
    using std::gettime;
    using std::setdate;
    using std::settime;
    using std::bdos;
    using std::sleep;
    using std::dfree;
    using std::diskfree_t;
    using std::time;
    using std::date;
    using std::dostime_t;
    using std::dosdate_t;
    using std::find_t;
    using std::_osmajor;
    using std::_osminor;
    using std::_argc;
    using std::__argc;
    using std::_argv;
    using std::__argv;
    using std::_harderr;
    using std::_hardretn;
    using std::_hardresume;
#endif
