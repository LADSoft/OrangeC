/*
 *                 D3X(tm) DOS-Extender v0.90 unleash `g' alpha
 *                              stub definitions
 *                                      
 *                      Copyright (c) 1998-2002 Borca Daniel
 *                                      
 *                               dborca@yahoo.com
 *                       http://www.geocities.com/dborca
 */

#ifndef STUBS_H_included
#define STUBS_H_included

typedef enum {
        X_UNKNOWN = -1,
        X_MZ,           /* regular MZ EXE                       */
        X_D3X1,         /* D3X1                                 */
        X_COFF,         /* DJGPP COFF                           */
        X_LE            /* LE linear executable (little endian) */
} exetype;

typedef unsigned char word8;
typedef unsigned short word16;
typedef unsigned long word32;

typedef struct {
        word16 sign;
        word16 partpag;
        word16 pagecnt;
        word16 relocnt;
        word16 hdrsize;
        word16 minmem;
        word16 maxmem;
        word16 reloss;
        word16 exesp;
        word16 chksum;
        word16 exeip;
        word16 relocs;
        word16 tabloff;
        word16 overlay;
} exe_hdr;

typedef struct {
        word32 sign;
        word32 hdrsize;
        word32 binsize;
        word32 admsize;
        word32 entry;
        word32 tos;
        char filler[8];
} d3x1_hdr;

typedef struct {
        char magic[16];
        word32 size;
        word32 minstack;
        word32 memory_handle;
        word32 initial_size;
        word16 minkeep;
        word16 ds_selector;
        word16 ds_segment;
        word16 psp_selector;
        word16 cs_selector;
        word16 env_size;
        word32 platform;
} _D3X1_StubInfo;

typedef struct {
        unsigned short f_magic;   /* magic number                          */
        unsigned short f_nscns;   /* number of sections                    */
        unsigned long f_timdat;   /* time & date stamp                     */
        unsigned long f_symptr;   /* file pointer to symtab                */
        unsigned long f_nsyms;    /* number of symtab entries              */
        unsigned short f_opthdr;  /* sizeof(optional hdr)                  */
        unsigned short f_flags;   /* flags                                 */
} coffheader;

typedef struct {
        unsigned short magic;     /* type of file                          */
        unsigned short vstamp;    /* version stamp                         */
        unsigned long tsize;      /* text size in bytes, padded to FW bdry */
        unsigned long dsize;      /* initialized data "  "                 */
        unsigned long bsize;      /* uninitialized data "   "              */
        unsigned long entry;      /* entry pt.                             */
        unsigned long text_start; /* base of text used for this file       */
        unsigned long data_start; /* base of data used for this file       */
} aoutheader;

typedef struct {
        char s_name[8];           /* section name                          */
        unsigned long s_paddr;    /* physical address, aliased s_nlib      */
        unsigned long s_vaddr;    /* virtual address                       */
        unsigned long s_size;     /* section size                          */
        unsigned long s_scnptr;   /* file ptr to raw data for section      */
        unsigned long s_relptr;   /* file ptr to relocation                */
        unsigned long s_lnnoptr;  /* file ptr to line numbers              */
        unsigned short s_nreloc;  /* number of relocation entries          */
        unsigned short s_nlnno;   /* number of line number entries         */
        unsigned long s_flags;    /* flags                                 */
} sectheader;

typedef struct {
        coffheader coff;
        aoutheader aout;
        sectheader text_sect;
        sectheader data_sect;
        sectheader bss_sect;
} coff_hdr;

typedef struct {
        char magic[16];
        word32 size;
        word32 minstack;
        word32 memory_handle;
        word32 initial_size;
        word16 minkeep;
        word16 ds_selector;
        word16 ds_segment;
        word16 psp_selector;
        word16 cs_selector;
        word16 env_size;
        char basename[8];
        char argv0[16];
        char dpmi_server[16];
} _GO32_StubInfo;

typedef struct {
        word32 LESignature;     /* must be 0x454C since order is little endian */
        word32 LEFormatLevel;   /* not really needed                           */
        word16 LECpuType;
        word16 LETargetSystem;  /* should be 01 (OS/2)                         */
        word32 LEModuleVersion; /* who cares...                                */
        word32 LEModuleType;    /* Must be flat                                */
        word32 LENumberPages;
        word32 LEEntrySection;
        word32 LEEntryOffset;
        word32 LEStackSection;
        word32 LEInitialESP;
        word32 LEPageSize;
        word32 LEBytesLastPage;
        word32 LEFixupSize;
        word32 LEFixupChecks;
        word32 LELoaderSize;
        word32 LELoaderChecks;
        word32 LEObjectTable;
        word32 LEObjectEntries;
        word32 LEObjectPMTable;
        word32 LEIterateData;
        word32 LEResourceTable;
        word32 LEResources;
        word32 LEResNamesTable;
        word32 LEEntryTable;
        word32 LEModuleDirTab;
        word32 LEModuleDirs;
        word32 LEFixupPageTab;
        word32 LEFixupRecords;
        word32 LEWeDontNeed[3]; /* getting bored                               */
        word32 LEPPchecksumTab;
        word32 LEDataPages;
        word32 LEPreloadPages;
        word32 LENonResTable;
        word32 LEWeDontNeed2[2];
        word32 LEAutoData;
        word32 LEDebugStart;
        word32 LEDebugSize;
        word32 LEPrelInstPages;
        word32 LEDemdInstPages;
        word32 LEExtraHeap;
        word32 LEReserved[3];
} le_hdr;

typedef struct {
        word32 OTVirtualSize;
        word32 OTRelocBase;
        word32 OTObjectFlags;
        word32 OTPageMapIndex;
        word32 OTPageMapSize;
        word32 OTReserved;
} obj_tbl;

typedef struct {
        char magic[16];
        word32 size;
        word32 minstack;
        word32 memory_handle;
        word32 initial_size;
        word16 minkeep;
        word16 ds_selector;
        word16 ds_segment;
        word16 psp_selector;
        word16 cs_selector;
        word16 env_size;
} _o2LE_StubInfo;

#endif
