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

#if !defined(__BIOS_H)
#define __BIOS_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif


/* diskinfo_t structure for _bios_disk() */

struct diskinfo_t
{
    unsigned drive, head, track, sector, nsectors;
    void *buffer;
};

/* cmd values for _bios_disk() */

#define _DISK_RESET     0   /* controller hard reset */
#define _DISK_STATUS    1   /* status of last operation */
#define _DISK_READ      2   /* read sectors */
#define _DISK_WRITE     3   /* write sectors */
#define _DISK_VERIFY    4   /* verify sectors */
#define _DISK_FORMAT    5   /* format track */

unsigned _RTL_FUNC _IMPORT _bios_disk(unsigned __cmd, struct diskinfo_t *__dinfo);
int      _RTL_FUNC _IMPORT biosdisk(int __cmd, int __drive, int __head, int __track,
                            int __sector, int __nsects, void *__buffer);


/* cmd values for _bios_keybrd() */

#define _KEYBRD_READ            0       /* read key */
#define _NKEYBRD_READ           0x10    /* read key - enhanced */
#define _KEYBRD_READY           1       /* check key ready */
#define _NKEYBRD_READY          0x11    /* check key ready - enhanced */
#define _KEYBRD_SHIFTSTATUS     2       /* get shift status */
#define _NKEYBRD_SHIFTSTATUS    0x12    /* get shift status - enhanced */

/* cmd values for _bios_printer() */

#define _PRINTER_WRITE  0       /* send a byte to printer */
#define _PRINTER_INIT   1       /* initialize printer */
#define _PRINTER_STATUS 2       /* read printer status */

/* cmd values for _bios_serialcom() */

#define _COM_INIT       0       /* set communication parms to a byte */
#define _COM_SEND       1       /* send a byte to port */
#define _COM_RECEIVE    2       /* read character from port */
#define _COM_STATUS     3       /* get status of port */

/* byte values for _COM_INIT cmd of _bios_serialcom() */

#define _COM_CHR7       0x02    /* 7 data bits */
#define _COM_CHR8       0x03    /* 8 data bits */
#define _COM_STOP1      0x00    /* 1 stop bit */
#define _COM_STOP2      0x04    /* 2 stop bits */
#define _COM_NOPARITY   0x00    /* no parity */
#define _COM_EVENPARITY 0x18    /* even parity */
#define _COM_ODDPARITY  0x08    /* odd parity */
#define _COM_110        0x00    /* 110 baud */
#define _COM_150        0x20    /* 150 baud */
#define _COM_300        0x40    /* 300 baud */
#define _COM_600        0x60    /* 600 baud */
#define _COM_1200       0x80    /* 1200 baud */
#define _COM_2400       0xa0    /* 2400 baud */
#define _COM_4800       0xc0    /* 4800 baud */
#define _COM_9600       0xe0    /* 9600 baud */

unsigned _RTL_FUNC _IMPORT _bios_keybrd(unsigned __cmd);
unsigned _RTL_FUNC _IMPORT _bios_printer(unsigned __cmd, unsigned __port, unsigned __abyte);
unsigned _RTL_FUNC _IMPORT _bios_serialcom(unsigned __cmd, unsigned __port, unsigned __abyte);

int      _RTL_FUNC _IMPORT bioscom(int __cmd, char __abyte, int __port);
int      _RTL_FUNC _IMPORT bioskey(int __cmd);
int      _RTL_FUNC _IMPORT biosprint(int __cmd, int __abyte, int __port);

/* cmd values for _bios_timeofday() */

#define _TIME_GETCLOCK  0   /* get clock count */
#define _TIME_SETCLOCK  1   /* set clock count */

/* Constants for biostime() */
#define _BIOS_CLOCKS_PER_SEC 18.2
#define _BIOS_CLK_TCK        _BIOS_CLOCKS_PER_SEC

/* register structure definitions for int86(), int86x() */

#ifndef _REG_DEFS
#define _REG_DEFS

#undef __FILLER

/* dword registers */

struct DWORDREGS {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int esi;
    unsigned int edi;
    unsigned int cflag;
};
#define __FILLER(a) unsigned short a;

/* word registers */

struct WORDREGS {
    unsigned short ax;  __FILLER(_1)
    unsigned short bx;  __FILLER(_2)
    unsigned short cx;  __FILLER(_3)
    unsigned short dx;  __FILLER(_4)
    unsigned short si;  __FILLER(_5)
    unsigned short di;  __FILLER(_6)
    unsigned int cflag;
};

/* byte registers */

struct BYTEREGS {
    unsigned char al, ah;  __FILLER(_1)
    unsigned char bl, bh;  __FILLER(_2)
    unsigned char cl, ch;  __FILLER(_3)
    unsigned char dl, dh;  __FILLER(_4)
};

/* general purpose registers union - overlays the corresponding dword,
 * word, and byte registers.
 */

union REGS {
    struct DWORDREGS x;
    struct WORDREGS  w;
    struct BYTEREGS  h;
};
#define _REGS REGS

/* segment registers */

struct SREGS {
    unsigned short es, cs, ss, ds;
    unsigned short fs, gs;
};
#define _SREGS SREGS


/* intr structs */

struct REGPACKB {
    unsigned char al, ah;  __FILLER(_1)
    unsigned char bl, bh;  __FILLER(_2)
    unsigned char cl, ch;  __FILLER(_3)
    unsigned char dl, dh;  __FILLER(_4)
};

struct REGPACKW {
    unsigned short ax;  __FILLER(_1)
    unsigned short bx;  __FILLER(_2)
    unsigned short cx;  __FILLER(_3)
    unsigned short dx;  __FILLER(_4)
    unsigned short bp;  __FILLER(_5)
    unsigned short si;  __FILLER(_6)
    unsigned short di;  __FILLER(_7)
    unsigned short ds;
    unsigned short es;
    unsigned short fs;
    unsigned short gs;
    unsigned int flags;
};

struct REGPACKX {
    unsigned int   eax, ebx, ecx, edx, ebp, esi, edi;
    unsigned short ds, es, fs, gs;
    unsigned int   flags;
};

union REGPACK {
    struct REGPACKB h;
    struct REGPACKW w;
    struct REGPACKX x;
};

#endif /* _REG_DEFS */

unsigned _RTL_FUNC _IMPORT _bios_equiplist(void);
unsigned _RTL_FUNC _IMPORT _bios_memsize(void);
unsigned _RTL_FUNC _IMPORT _bios_timeofday(unsigned __cmd, long *__timeval);

int      _RTL_FUNC _IMPORT biosequip(void);
int      _RTL_FUNC _IMPORT biosmemory(void);
long     _RTL_FUNC _IMPORT biostime(int __cmd, long __newtime);

int      _RTL_FUNC _IMPORT _int386(int __intno,
                         union REGS *__inregs,
                         union REGS *__outregs );

#ifdef __cplusplus
}
}
#endif

#pragma pack()

#endif  /* __BIOS_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__BIOS_H_USING_LIST)
#define __BIOS_H_USING_LIST
    using __STD_NS_QUALIFIER _bios_disk;
    using __STD_NS_QUALIFIER biosdisk;
    using __STD_NS_QUALIFIER _bios_keybrd;
    using __STD_NS_QUALIFIER _bios_printer;
    using __STD_NS_QUALIFIER _bios_serialcom;
    using __STD_NS_QUALIFIER bioscom;
    using __STD_NS_QUALIFIER bioskey;
    using __STD_NS_QUALIFIER biosprint;
    using __STD_NS_QUALIFIER _bios_equiplist;
    using __STD_NS_QUALIFIER _bios_memsize;
    using __STD_NS_QUALIFIER _bios_timeofday;
    using __STD_NS_QUALIFIER biosequip;
    using __STD_NS_QUALIFIER biosmemory;
    using __STD_NS_QUALIFIER biostime;
    using __STD_NS_QUALIFIER _int386;
    using __STD_NS_QUALIFIER diskinfo_t;
    using __STD_NS_QUALIFIER REGS;
    using __STD_NS_QUALIFIER SREGS;
    using __STD_NS_QUALIFIER REGPACK;
#endif
