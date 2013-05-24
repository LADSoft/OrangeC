/*
 *                 D3X(tm) DOS-Extender v0.90 unleash `g' alpha
 *                                utility file
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



static char *myself;

#ifdef DJGPP
char **__crt0_glob_function (char *_argument)         { (void)_argument; return 0; }
void   __crt0_load_environment_file (char *_app_name) { (void)_app_name; }
#endif

static int fix (char *infile, int array, int page, int newformat);



/* usage:
 *  print mini-help
 */
static void NORET usage (void)
{
 printf("Usage: %s [-e] [-p] [-n] executable\n\n   -e  generate C array (stdout)\n   -p  make output page aligned\n   -n  new format output header\n", myself);

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
 int i;
 char *infile = NULL;
 int array = 0, page = 0, newformat = 0;
 char *error;

 myself = argv[0];

 if (argc<2)
    syntaxerr();

 for (i=1;i<argc;i++) {
     if (!strcmp(argv[i], "-h")) {
        if (argc==2)
           usage();
        else
           syntaxerr();
     }
     if (!strcmp(argv[i], "-e")) {
        array = 1;
        continue;
     }
     if (!strcmp(argv[i], "-p")) {
        page = 1;
        continue;
     }
     if (!strcmp(argv[i], "-n")) {
        newformat = 1;
        continue;
     }
     if (!infile)
        infile = argv[i];
     else
        syntaxerr();
 }

 switch (i=fix(infile, array, page, newformat)) {
        case 0x02:
             error = "Error 02: no input file\n";
             break;
        case 0x10:
             error = "Error 10: unable to create (%s)\n";
             break;
        case 0x11:
             error = "Error 11: unable to open (%s)\n";
             break;
        case 0x12:
             error = "Error 12: unable to read (%s)\n";
             break;
        case 0x13:
             error = "Error 13: unable to write (%s)\n";
             break;
        case 0x20:
             error = "Error 20: not enough memory\n";
             break;
        case 0x40:
             error = "Error 40: invalid EXE (%s)\n";
             break;
        case 0x41:
             error = "Error 41: image not para aligned (%s)\n";
             break;
        default:
             error = "";
 }

 printf(error, infile);

 return i;
}



/* fix:
 *
 */
static int fix (char *infile, int array, int page, int newformat)
{
 FILE *f;
 exe_hdr inh, *outh;
 word32 newhdrlen, newsize, binsize;
 int sizeof_exe_hdr;

 if (infile==NULL) return 0x02;
 if ((f=fopen(infile, "rb"))==NULL) return 0x11;
 if (!fread(&inh, sizeof(exe_hdr), 1, f)) return 0x12;
 if (inh.sign!=0x5a4d) return 0x40;

 sizeof_exe_hdr = newformat?0x40:sizeof(exe_hdr);
 newhdrlen = (sizeof_exe_hdr + inh.relocnt*4 + 15)&~15;
 binsize = (inh.pagecnt<<9) - ((512 - inh.partpag)&511) - inh.hdrsize*16;
 if (page) {
    if (binsize&0xf) return 0x41;
    newhdrlen += (512 - (newhdrlen + binsize))&511;
 }
 newsize = binsize + newhdrlen;
 if ((outh=(exe_hdr *)malloc(newsize))==NULL) return 0x20;

 memset(outh, 0, newhdrlen);
 outh->sign = inh.sign;
 outh->relocnt = inh.relocnt;
 outh->hdrsize = newhdrlen/16;
 outh->minmem = inh.minmem;
 outh->maxmem = inh.maxmem;
 outh->reloss = inh.reloss;
 outh->exesp = inh.exesp;
 outh->exeip = inh.exeip;
 outh->relocs = inh.relocs;
 outh->tabloff = sizeof_exe_hdr;
 outh->pagecnt = newsize/512;
 if ((outh->partpag=newsize%512)) outh->pagecnt++;
 if (newformat) ((word32 *)outh)[15] = newsize;

 fseek(f, inh.tabloff, SEEK_SET);
 if (fread((word8 *)outh + outh->tabloff, 4, inh.relocnt, f)!=inh.relocnt) return 0x12;
 fseek(f, inh.hdrsize*16, SEEK_SET);
 if (!fread((word8 *)outh + newhdrlen, binsize, 1, f)) return 0x12;
 fclose(f);

 if (array) {
    word32 i;
    for (i=0;i<newsize;i++)
        fprintf(stdout, "0x%02x,%c", ((word8 *)outh)[i], ((i&15)==15)?'\n':' ');
 } else {
    if ((f=fopen(infile,"wb"))==NULL) return 0x10;
    if (!fwrite(outh, newsize, 1, f)) return 0x13;
    fclose(f);
 }

 return 0;
}
