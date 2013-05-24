/*
 *                 D3X(tm) DOS-Extender v0.90 unleash `g' alpha
 *                                stub manager
 *                                      
 *                      Copyright (c) 1998-2002 Borca Daniel
 *                                      
 *                               dborca@yahoo.com
 *                       http://www.geocities.com/dborca
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stubs.h"



#ifdef __GNUC__
#define NORET __attribute__((noreturn))
#else
#define NORET
#endif

#define _16_ *(word16 *)&
#define _32_ *(word32 *)&



static unsigned char d3x1[] = {
#include "d3x1.h"
};
static unsigned char d3xd[] = {
#include "d3xd.h"
};
static unsigned char d3xw[] = {
#include "d3xw.h"
};



static char *myself;

#ifdef DJGPP
char **__crt0_glob_function (char *_argument)         { (void)_argument; return 0; }
void   __crt0_load_environment_file (char *_app_name) { (void)_app_name; }
#endif

static int bname (const char *path);
static int chkexe (const char *file, exetype *type, int *offset);

static void cut_banners (void);
static void dump_extender (const char *filename, void *data, int size);
static void view_file (const char *file);
static void make_stubbed (const char *in, const char *out);
static void make_unstubbed (char *, char *);



/* usage:
 *  print mini-help
 */
static void NORET usage (void)
{
 printf("StubX stub manager v0.90.g\nCopyright (c) 1999-2001 Borca Daniel\n\n");
 printf("Usage: %s -d\n   or  %s -v executable\n   or  %s -{s|u} infile [outfile]\n\n", myself, myself, myself);
 printf("   -d  dump extenders\n   -v  view\n   -s  stubify\n   -u  unstubify\n");

 exit(0);
}



/* syntaxerr:
 *  bail with `syntax error'
 */
static void NORET syntaxerr (void)
{
 printf("Error 01: syntax error. Try %s -h\n", myself);

 exit(1);
}



/* main:
 *
 */
int main (int argc, char **argv)
{
 word32 opt;

 myself = &argv[0][bname(argv[0])];

 if ((argc<2)||(argc>4)) {
    syntaxerr();
 }

 opt = (_32_ argv[1][0])&0x00ffffff;
 switch (opt) {
        case 0x682d: /* -h */
             if (argc!=2) {
                syntaxerr();
             } else {
                usage();
             }
        case 0x642d: /* -d */
             if (argc!=2) {
                syntaxerr();
             } else {
                dump_extender("d3x1.exe", d3x1, sizeof(d3x1));
                dump_extender("d3xd.exe", d3xd, sizeof(d3xd));
                dump_extender("d3xw.exe", d3xw, sizeof(d3xw));
             }
             break;
        case 0x762d: /* -v */
             if (argc!=3) {
                syntaxerr();
             } else {
                view_file(argv[2]);
             }
             break;
        case 0x532d: /* -S */
             cut_banners();
        case 0x732d: /* -s */
             if ((argc!=3) && (argc!=4)) {
                syntaxerr();
             } else {
                make_stubbed(argv[2], (argc==4)?argv[3]:argv[2]);
             }
             break;
        case 0x752d: /* -u */
             if ((argc!=3) && (argc!=4)) {
                syntaxerr();
             } else {
                make_unstubbed(argv[2], (argc==4)?argv[3]:argv[2]);
             }
             break;
        default:
             syntaxerr();
 }

 return 0;
}



/* cut_banners:
 *  prevent banners in extenders
 */
static void cut_banners (void)
{
 _16_ d3x1[((_16_ d3x1[8])<<4) + sizeof(_D3X1_StubInfo) + 3] = 0x0beb;
 _16_ d3xd[((_16_ d3xd[8])<<4) + sizeof(_GO32_StubInfo) + 3] = 0x0beb;
 _16_ d3xw[((_16_ d3xw[8])<<4) + sizeof(_o2LE_StubInfo) + 3] = 0x0beb;
}



/* dump_extender:
 *
 */
static void dump_extender (const char *filename, void *data, int size)
{
 FILE *f;

 printf("Dumping (%s)...", filename);
 if ((f=fopen(filename, "wb")) != NULL) {
    if (fwrite(data, size, 1, f)) {
       printf(" done.\n");
    } else {
       printf("\nError 13: unable to write (%s)\n", filename);
    }
    fclose(f);
 } else {
    printf("\nError 10: unable to create (%s)\n", filename);
 }
}



/* analyze_d3x1
 *
 */
static void analyze_d3x1 (const char *file, int offset)
{
 FILE *f;
 _D3X1_StubInfo d3x1_si;
 exe_hdr header;
 d3x1_hdr subheader;
 char tmp[32];

 if ((f=fopen(file, "rb"))==NULL) {
    printf("Error 11: unable to open (%s)\n", file);
    exit(0x11);
 }

 if (offset) {
    if (!fread(&header, sizeof(header), 1, f)) {
       printf("Error 12: unable to read (%s)\n", file);
       fclose(f);
       exit(0x12);
    }
    fseek(f, header.hdrsize*16, SEEK_SET);
    if (!fread(&d3x1_si, sizeof(d3x1_si), 1, f)) {
       printf("Error 12: unable to read (%s)\n", file);
       fclose(f);
       exit(0x12);
    }
    if (!memcmp(d3x1_si.magic, "d3x1stub", 8)) {
       printf("StubInfo:\n");
       strncpy(tmp, d3x1_si.magic, 16); tmp[16] = '\0';
       printf("\tmagic\t\t= %s\n", tmp);
       printf("\tstubinfo size\t= %lxh\n", d3x1_si.size);
       printf("\ttransferbuffer\t= %d\n", d3x1_si.minkeep);
    }
    fseek(f, offset, SEEK_SET);
 }

 if (!fread(&subheader, sizeof(subheader), 1, f)) {
    printf("Error 12: unable to read (%s)\n", file);
    fclose(f);
    exit(0x12);
 }
 printf("D3X1 hdr:\n");
 printf("\tbinary size\t= %lu\n", subheader.binsize);
 printf("\tadditional mem\t= %lu\n", subheader.admsize);
 printf("\tentry point\t= 0x%08lx\n", subheader.entry);
 printf("\ttop of stack\t= 0x%08lx\n", subheader.tos);

 fclose(f);
}



/* analyze_coff
 *
 */
static void analyze_coff (const char *file, int offset)
{
 FILE *f;
 _GO32_StubInfo go32_si;
 exe_hdr header;
 coff_hdr subheader;
 char tmp[32];

 if ((f=fopen(file, "rb"))==NULL) {
    printf("Error 11: unable to open (%s)\n", file);
    exit(0x11);
 }

 if (offset) {
    if (!fread(&header, sizeof(header), 1, f)) {
       printf("Error 12: unable to read (%s)\n", file);
       fclose(f);
       exit(0x12);
    }
    fseek(f, header.hdrsize*16, SEEK_SET);
    if (!fread(&go32_si, sizeof(go32_si), 1, f)) {
       printf("Error 12: unable to read (%s)\n", file);
       fclose(f);
       exit(0x12);
    }
    if (!memcmp(go32_si.magic, "go32stub", 8)) {
       printf("StubInfo:\n");
       strncpy(tmp, go32_si.magic, 16); tmp[16] = '\0';
       printf("\tmagic\t\t= %s\n", tmp);
       printf("\tstubinfo size\t= %lxh\n", go32_si.size);
       printf("\tstack size\t= 0x%08lx\n", go32_si.minstack);
       printf("\ttransferbuffer\t= %d\n", go32_si.minkeep);
       strncpy(tmp, go32_si.basename, 8); tmp[8] = '\0';
       printf("\trun file\t= %s\n", tmp[0]?tmp:"<self>");
       strncpy(tmp, go32_si.argv0, 16); tmp[16] = '\0';
       printf("\targv0\t\t= %s\n", tmp[0]?tmp:"<default>");
       printf("\tdpmi server\t= %s\n", go32_si.dpmi_server[0]?go32_si.dpmi_server:"<none>");
    }
    fseek(f, offset, SEEK_SET);
 }

 if (!fread(&subheader, sizeof(subheader), 1, f)) {
    printf("Error 12: unable to read (%s)\n", file);
    fclose(f);
    exit(0x12);
 }

 printf("COFF hdr:\n");
 if (subheader.coff.f_flags&2) {
    printf("\ttext\t\t= %lu\n", subheader.text_sect.s_size);
    printf("\tdata\t\t= %lu\n", subheader.data_sect.s_size);
    printf("\tbss\t\t= %lu\n", subheader.bss_sect.s_size);
    printf("\tstart eip\t= text:0x%08lx\n", subheader.aout.entry - subheader.text_sect.s_vaddr);
 } else {
    printf("\ttype\t\t= unresolved\n");
 }

 fclose(f);
}



/* analyze_le
 *
 */
static void analyze_le (const char *file, int offset)
{
 FILE *f;
 le_hdr header;
 obj_tbl *table;
 word32 mem;
 int i;

 if ((f=fopen(file, "rb"))==NULL) {
    printf("Error 11: unable to open (%s)\n", file);
    exit(0x11);
 }
 fseek(f, offset, SEEK_SET);

 if (!fread(&header, sizeof(header), 1, f)) {
    printf("Error 12: unable to read (%s)\n", file);
    fclose(f);
    exit(0x12);
 }

 if ((table=(obj_tbl *)malloc(header.LEObjectEntries*sizeof(obj_tbl)))==NULL) {
    printf("Error 20: not enough memory\n");
    fclose(f);
    exit(0x20);
 }
 fseek(f, offset+header.LEObjectTable, SEEK_SET);
 if (!fread(table, sizeof(obj_tbl), header.LEObjectEntries, f)) {
    printf("Error 12: unable to read (%s)\n", file);
    free(table);
    fclose(f);
    exit(0x12);
 }

 printf("LE hdr  :\n");
 printf("\tloader size\t= 0x%lx\n", header.LELoaderSize);
 printf("\tdata offset\t= 0x%08lx\n", header.LEDataPages);
 printf("\tentry point\t= %08lx:%08lx\n", header.LEEntrySection, header.LEEntryOffset);
 printf("\ttop of stack\t= %08lx:%08lx\n", header.LEStackSection, header.LEInitialESP);
 for (mem=0, i=0;(unsigned)i<header.LEObjectEntries;i++) {
     mem += (table[i].OTVirtualSize + 4095)&~4095;
     printf("\tobject %04d\t= (%08lx+%08lx)", i+1, table[i].OTRelocBase, table[i].OTVirtualSize);
     if (table[i].OTObjectFlags&1) printf(" readable");
     if (table[i].OTObjectFlags&2) printf(" writable");
     if (table[i].OTObjectFlags&4) printf(" executbl");
     if (table[i].OTObjectFlags&0x2000) printf(" big");
     printf("\n");
 }
 printf("\tprogram mem\t= 0x%lx\n", mem);

 free(table);
 fclose(f);
}



/* view_file
 *
 */
static void view_file (const char *file)
{
 int result, offset;
 exetype type;

 result = chkexe(file, &type, &offset);

 switch (result) {
        case 0:
             printf("Filename: %s\nType    : %s", file, offset?"stubbed ":"");
             switch (type) {
                    case X_UNKNOWN:
                         printf("<unknown>\n");
                         break;
                    case X_MZ:
                         printf("MZ\n");
                         break;
                    case X_D3X1:
                         printf("D3X1\n");
                         analyze_d3x1(file, offset);
                         break;
                    case X_COFF:
                         printf("COFF\n");
                         analyze_coff(file, offset);
                         break;
                    case X_LE:
                         printf("LE\n");
                         analyze_le(file, offset);
                         break;
             }
             break;
        case 0x11:
             printf("Error 11: unable to open (%s)\n", file);
             exit(0x11);
        case 0x12:
             printf("Error 12: unable to read (%s)\n", file);
             exit(0x12);
 }
}



/* build_d3x1:
 *  build D3X1 from MZ file
 */
static void build_d3x1 (const char *in, const char *out)
{
 FILE *f;
 exe_hdr header;
 d3x1_hdr subheader;
 word8 *buffer;

 if ((f=fopen(in, "rb"))==NULL) {
    printf("\nError 11: unable to open (%s)\n", in);
    exit(0x11);
 }
 if (!fread(&header, sizeof(header), 1, f)) {
    printf("\nError 12: unable to read (%s)\n", in);
    fclose(f);
    exit(0x12);
 }

 subheader.sign = 0x31583344; /* D3X1 */
 subheader.hdrsize = sizeof(d3x1_hdr);
 subheader.binsize = ((header.pagecnt<<9) - ((512 - header.partpag)&511)) - header.hdrsize*16;
 subheader.admsize = header.minmem*16;
 subheader.entry = header.relocs*16 + header.exeip;
 subheader.tos = header.reloss*16 + header.exesp;
 memset(&subheader.filler, 0, sizeof(subheader.filler));
 if (header.relocnt) {
    printf("\nError 42: relocation items detected\n");
    fclose(f);
    exit(0x42);
 }
 if (subheader.tos==0) {
    printf("\nError 43: input executable has no stack\n");
    fclose(f);
    exit(0x43);
 }
 if ((buffer=(word8 *)malloc(subheader.binsize))==NULL) {
    printf("\nError 20: not enough memory\n");
    fclose(f);
    exit(0x20);
 }

 fseek(f, header.hdrsize*16, SEEK_SET);
 if (!fread(buffer, subheader.binsize, 1, f)) {
    printf("\nError 12: unable to read (%s)\n", in);
    free(buffer);
    fclose(f);
    exit(0x12);
 }
 fclose(f);

 if ((f=fopen(out, "wb"))==NULL) {
    printf("\nError 10: unable to create (%s)\n", out);
    free(buffer);
    exit(0x10);
 }
 if (!fwrite(&subheader, sizeof(subheader), 1, f)) {
    printf("\nError 13: unable to write (%s)\n", out);
    fclose(f);
    free(buffer);
    exit(0x13);
 }
 if (!fwrite(buffer, subheader.binsize, 1, f)) {
    printf("\nError 13: unable to write (%s)\n", out);
    fclose(f);
    free(buffer);
    exit(0x13);
 }
 fclose(f);

 free(buffer);
 printf(" done.\n");
}



/* stubify:
 *
 */
static void stubify (const char *in, int offset, word8 *stubaddress, int stubsize, exetype type, const char *out)
{
 FILE *f;
 word32 size;
 word8 *buffer;

 if ((offset|stubsize)==0) goto done;

 if ((f=fopen(in, "rb"))==NULL) {
    printf("\nError 11: unable to open (%s)\n", in);
    exit(0x11);
 }
 fseek(f, 0, SEEK_END);
 size = ftell(f) - offset;
 fseek(f, offset, SEEK_SET);
 if ((buffer=(word8 *)malloc(size))==NULL) {
    printf("\nError 20: not enough memory\n");
    fclose(f);
    exit(0x20);
 }
 if (!fread(buffer, size, 1, f)) {
    printf("\nError 12: unable to read (%s)\n", in);
    free(buffer);
    fclose(f);
    exit(0x12);
 }
 fclose(f);

 if (type==X_LE) {
    word8 *p;
    le_hdr *b;
    word32 i, before, after;

    printf(" patching LE...");
    b = (le_hdr *)buffer;
    p = buffer + b->LEObjectTable + b->LELoaderSize;
    before = b->LEDataPages - offset;
    for (i=0;i<(size - (b->LEObjectTable + b->LELoaderSize));i++) {
        if (p[i]) {
           before = b->LEObjectTable + b->LELoaderSize + i;
           break;
        }
    }
    after = (((stubsize + b->LEObjectTable + b->LELoaderSize)+511)&~511) - stubsize;
    if (after!=before) {
       if (after<before) {
          memmove(&buffer[after], &buffer[before], size-before);
       } else {
          if ((p=(word8 *)realloc(buffer, size+after-before))==NULL) {
             printf("\nError 21: not enough memory\n");
             free(buffer);
             exit(0x21);
          }
          buffer = p;
          b = (le_hdr *)buffer;
          memmove(&buffer[after], &buffer[before], size-before);
          memset(&buffer[before], 0, after-before);
       }
       size += after - before;
       if (b->LENonResTable) b->LENonResTable += stubsize + after - b->LEDataPages;
    }
    b->LEDataPages = stubsize + after;
 }

 if (offset)
    printf(" removing old stub...");

 if ((f=fopen(out, "wb"))==NULL) {
    printf("\nError 10: unable to create (%s)\n", out);
    free(buffer);
    exit(0x10);
 }
 if (stubsize) {
    if (!fwrite(stubaddress, stubsize, 1, f)) {
       printf("\nError 13: unable to write (%s)\n", out);
       fclose(f);
       free(buffer);
       exit(0x13);
    }
 }
 if (!fwrite(buffer, size, 1, f)) {
    printf("\nError 13: unable to write (%s)\n", out);
    fclose(f);
    free(buffer);
    exit(0x13);
 }
 fclose(f);

 free(buffer);
done:
 printf(" done.\n");
}



/* make_stubbed:
 *
 */
static void make_stubbed (const char *in, const char *out)
{
 int result, offset;
 exetype type;

 result = chkexe(in, &type, &offset);

 switch (result) {
        case 0:
             switch (type) {
                    case X_UNKNOWN:
                         printf("Error 40: unknown input type\n");
                         exit(0x40);
                    case X_MZ:
                         if (offset) {
                            printf("Error 41: unsupported input type\n");
                            exit(0x41);
                         }
                         printf("Building D3X1...");
                         build_d3x1(in, out);
                         in = out;
                    case X_D3X1:
                         printf("Stubifying D3X1...");
                         stubify(in, offset, d3x1, sizeof(d3x1), type, out);
                         break;
                    case X_COFF:
                         printf("Stubifying COFF...");
                         stubify(in, offset, d3xd, sizeof(d3xd), type, out);
                         break;
                    case X_LE:
                         printf("Stubifying LE...");
                         stubify(in, offset, d3xw, sizeof(d3xw), type, out);
                         break;
             }
             break;
        case 0x11:
             printf("Error 11: unable to open (%s)\n", in);
             exit(0x11);
        case 0x12:
             printf("Error 12: unable to read (%s)\n", in);
             exit(0x12);
 }
}



/* make_unstubbed:
 *
 */
static void make_unstubbed (char *in, char *out)
{
 int result, offset;
 exetype type;

 result = chkexe(in, &type, &offset);

 switch (result) {
        case 0:
             switch (type) {
                    case X_UNKNOWN:
                         printf("Error 40: unknown input type\n");
                         exit(0x40);
                    case X_MZ:
                         printf("Error 41: unsupported input type\n");
                         exit(0x41);
                    case X_D3X1:
                         printf("Un-stubifying D3X1...");
                         stubify(in, offset, NULL, 0, type, out);
                         break;
                    case X_COFF:
                         printf("Un-stubifying COFF...");
                         stubify(in, offset, NULL, 0, type, out);
                         break;
                    case X_LE:
                         printf("Un-stubifying LE...");
                         stubify(in, offset, NULL, 0, type, out);
                         break;
             }
             break;
        case 0x11:
             printf("Error 11: unable to open (%s)\n", in);
             exit(0x11);
        case 0x12:
             printf("Error 12: unable to read (%s)\n", in);
             exit(0x12);
 }
}



/* bname:
 *  return basename of the file
 */
static int bname (const char *path)
{
 int i;

 for (i=0; path[i]; i++) ;

 for (; (i>=0) && (path[i]!=':') && (path[i]!='\\') && (path[i]!='/'); i--) ;

 return i+1;
}



/* chk:
 *  return executable type, based on signature
 */
static exetype chk (word32 sig)
{
 switch (sig) {
        case 0x31583344:                        /* D3X1 */
             return X_D3X1;
        case 0x0000454c:                        /* LE little endian */
             return X_LE;
        default:
             switch (sig&0xffff) {
                    case 0x014c:                /* COFF */
                         return X_COFF;
                    case 0x5a4d:                /* MZ */
                         return X_MZ;
                    default:
                         return X_UNKNOWN;
             }
 }
}



/* chkexe:
 *  check executable
 */
static int chkexe (const char *file, exetype *type, int *offset)
{
 FILE *f;
 exe_hdr header;
 word32 header2[16];
 word32 sig;
 word32 pos, size;

 *type = X_UNKNOWN;
 *offset = 0;

 if ((f=fopen(file, "rb"))==NULL) {
    /* unable to open */
    return 0x11;
 }
 fseek(f, 0, SEEK_END);
 size = ftell(f);
 fseek(f, 0, SEEK_SET);

 if (!fread(&header, sizeof(header), 1, f)) {
    fclose(f);
    /* unable to read */
    return 0x12;
 }

 pos = 0;
 if (header.sign==0x5a4d) {                             /* MZ */
    pos = (header.pagecnt<<9) - ((512 - header.partpag)&511);
    if (size==pos) {
       *type = X_MZ;
       fclose(f);
       return 0;
    }
    if ((header.hdrsize>=4)&&(header.tabloff>=0x40)) {  /* newformat EXE */
       fseek(f, 0, SEEK_SET);
       if (fread(&header2, sizeof(header2), 1, f)) {
          fseek(f, header2[15], SEEK_SET);
          if (fread(&sig, sizeof(sig), 1, f)) {
             *offset = header2[15];
             if ((*type=chk(sig))!=X_UNKNOWN) {
                fclose(f);
                return 0;
             }
          }
       }
    }
 }

 fseek(f, pos, SEEK_SET);
 if (!fread(&sig, sizeof(sig), 1, f)) {
    fclose(f);
    /* unable to read */
    return 0x12;
 }

 *offset = pos;
 *type = chk(sig);

 fclose(f);
 return 0;
}
