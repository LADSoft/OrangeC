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
#pragma pack(1)

typedef struct {
    char *qualname;
    unsigned char data[2000];
} COLLATE_DATA;
typedef struct {
    char *qualname;
    unsigned char data[600];
} ABBREVIATED_COLLATE_DATA;
typedef struct {
    char *qualname;
    short flags;
    unsigned short ctypetab[256];
    unsigned char tolower[256];
    unsigned char toupper[256];    
} CTYPE_DATA;
typedef struct {
    char *qualname;
    short flags;
    unsigned short ctypetab[256];
    
} ABBREVIATED_CTYPE_DATA;
typedef struct {
    char *qualname;
    char shortdays[7][10];
    char longdays[7][20];
    char shortmonths[12][10];
    char longmonths[12][20];
    char am[10]; /* these should be considered as an array[2] */
    char pm[10];
    char dates[2][25] ; /* extended, short */
    char times[2][25]; /* military, */
    unsigned char ths[1000]; /* sequence of null-terminated strings */
} TIME_DATA;
typedef struct {
    char *qualname;
    char shortdays[7][10];
    char longdays[7][20];
    char shortmonths[12][10];
    char longmonths[12][20];
    char am[10];
    char pm[10];
    char dates[2][25] ; /* extended, short */
    char times[2][25]; /* military */
    unsigned char ths[560]; /* sequence of null-terminated strings */
} ABBREVIATED_TIME_DATA;
typedef struct {
    char *qualname;
   char int_curr_symbol[5];
   char currency_symbol[5];
   char mon_decimal_point[2];
   char mon_thousands_sep[5];
   char mon_grouping[5];
   char positive_sign[5];
   char negative_sign[5];
   char int_frac_digits;
   char frac_digits;
   char p_cs_precedes;
   char p_sep_by_space;
   char n_cs_precedes;
   char n_sep_by_space;
   char p_sign_posn;
   char n_sign_posn;    
   /* borland didn't have the following... not presently initialized */
   char int_p_cs_precedes;
   char int_p_sep_by_space;
   char int_n_cs_precedes;
   char int_n_sep_by_space;
   char int_p_sign_posn;
   char int_n_sign_posn;    
} MONETARY_DATA;
typedef struct {
    char *qualname;
   char decimal_point[2];
   char thousands_sep[10];
   char grouping[20];
    
} NUMERIC_DATA ;
typedef struct {
    char *qualname;
    char ychar[5];
    char nchar[5];    
} MESSAGE_DATA;

typedef struct {
    char *qualname;
    int len;
    char data[1];
} USERDEF_DATA;

typedef struct {
    char *unqual_name;
    char *qual_name;
    union {
        COLLATE_DATA *cd;
        ABBREVIATED_COLLATE_DATA *cda;
    } ;
    union {
        CTYPE_DATA *ct;
        ABBREVIATED_CTYPE_DATA *cta;
    };
    MONETARY_DATA *md;
    NUMERIC_DATA *nd;
    union {
        TIME_DATA *td;
        ABBREVIATED_TIME_DATA *tda;
    };
    MESSAGE_DATA *msgd;
    USERDEF_DATA *ud;
} LOCALE_HEADER ;

#define LOOKUP 1
#define REPLACE 2
#define REVERSE 2
#define EXPAND 4
#define CONTRACT 0x8
#define CONTROL 0x10
#define NORMAL 0x20
#define EOS 0x40
struct collstr {
    char stateflags;
    unsigned char *source ;
    unsigned char *dest;
    unsigned char *tbl;
    unsigned char *lvlflags;
    unsigned char *repltab;
    unsigned char *contrtab;
    int buflen;
    int fetched;
    int levels;
    int curlevel;
    unsigned char *grabptr;
    int grablen;
} ;

extern void *__locale_data[LC_LAST + 1];
unsigned char __nextWeight(struct collstr *db);

#pragma pack()
