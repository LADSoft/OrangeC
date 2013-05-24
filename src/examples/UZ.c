//
// UZ.C ||| Ultimate Un-ZIP for CC386 , compiles to DOS and Win32
// (CL) 2007 by DOS386, Ladsoft, Info-ZIP group, Phil Katz, maybe other contributors 
//

// Limitations and bugs: 
//
// - Only standard PKZIP 2.xx compatible ZIP's 
// - Compression methods: only Deflate and "Store"
// - Doesn't assign the file attributes to the output file

// History:
// 2013-04-27 Ladsoft
//
// add gzip format
// add decryption
// delete broken extracted files
// prompt if file exists
// add - /Q and /p command line switches
//
// 2007-12-24 DOS386:
// Optimized and simplified some code, fixed possible "internal" bug of reading beyond EOF
// Many comments added
// "i" and "val" renamed ;-)
// Re-arranged some code, got rid of 1 dirty jump and prepared removing the other one
//
// 2007-09-15 Ladsoft:
// Create directories
// Set file time
//
// 2007-09-09 DOS386:
// Fixed bugs (garbage text output on errors)
// Internal optimizations (fixed various internal duplicates, kicked relicts, "unsigned" some integers, 
//                                         introduced typedefs, reduced unuseful parameter passing)
// Functional improvements (more info, better text formatting, reject and report faulty ZIP's / files inside, "backed" all slashes) 
// Added ZIP , GZIP (not yet supported) , 7-ZIP (unsupported) detection
// Big improvements in commenting
//
// 2007-09-08 Ladsoft:
// Fixed "64KiB-bug" when extracting some ZIP's created with KZIP

// ToDo:
//
// - Recognize GZIP and 7-ZIP with SFX also
// - Recognize and support TAR

// ToDont
//
// - "new" ZIP (ZIP64, Deflate64, Wavpack, Rijndael, ...)
// - 7-ZIP support (use 7-ZIP ;-) )
// - RAR, ACE, ZOO, ... ;-)

#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <utime.h>

#pragma pack(1) /* "Structure packing" - required sy some compilers ? */

typedef unsigned char  UINT8  ;
typedef unsigned short UINT16 ;
typedef unsigned int   UINT32 ;
typedef short          SINT16 ;
typedef int            SINT32 ;

typedef struct
{
    UINT8 id1;
    UINT8 id2;
    UINT8 cm;
    UINT8 flg;
    UINT32 mtime;
    UINT8 xfl;
    UINT8 os;    
} GZIPHDR;  

#define FTEXT 1
#define FHCRC 2
#define FEXTRA 4
#define FNAME 8
#define FCOMMENT 16
#define FRES (0x20 | 0x40 | 0x80)

typedef struct
{
    UINT32 crc32;
    UINT32 size;
} GZIPTRLR;

typedef /* ZIPFILE */ struct {
    UINT32 sig;
    UINT16 xtractVersion;
    UINT16 bitFlags;
    UINT16 compressionMethod;
    UINT16 fileTime;
    UINT16 fileData;
    UINT32 CRC32;
    UINT32 compressedSize;
    UINT32 uncompressedSize;
    UINT16 filenamelen ;
    UINT16 secheadlen ;
} ZIPFILE ;

typedef /* ZIPDIR */ struct {
    UINT32 sig ; /* PK 1,2 "Central file header" */ 
    UINT32 res1 ;
    UINT16 res2 ;
    UINT16 compression ;
    UINT32 filetime ; /* date & time */
    UINT32 crc32 ;
    UINT32 internalsize ; /* compressed */
    UINT32 realsize ; /* uncompressed */
    UINT16 filenamelen ;
    UINT16 secheadlen ;
    UINT16 tertiaryheadlen ;
    UINT32 res3 ;
    UINT32 attrib ;
    UINT32 fileoffset ;
    char filename[64] ;
} ZIPDIR ;

typedef /* ZIPHEAD */ struct {
    UINT32 sig ; /* PK 5,6 - "End of central directory record" */
    UINT32 res1 ; 
    UINT16 res2 ;
    UINT16 filecount ;
    UINT32 dirlen ; 
    UINT32 diroffset ;
    UINT16 commentlen ;
} ZIPHEAD ;

/* JMP mystery */

jmp_buf inflate_badtab ;

/* GLOBAL FILE stuff */

FILE *FFzipfile,*FFoutfile ;

/* DIM global */

UINT32 CRCtab[256] ;
UINT32 currentcrc ;

UINT8  masktab[9] = { 0,1,3,7,0xF,0x1F,0x3F,0x7F,0xFF } ;
UINT8  tab7_rearrange[0x13] = { 0x10, 0x11, 0x12, 0 , 8 , 7 , 9 , 6 ,
    10,5,11,4,12,3,13,2,14,1,15 } ;
UINT32 accum,accum_count ;
UINT8  inflatebuf[1024];
SINT32 inflatepos ;

char   inflate_tab_1[0x140] ; /* #320 bytes */
UINT8  inflate_tab_2[32] ;
SINT16 inflate_tab_3[256] ;
SINT16 inflate_tab_4[256] ;
UINT16 inflate_tab_5[0x240] ; /* #576 bytes */
UINT16 inflate_tab_6[64] ;
UINT8  inflate_tab_7[0x13] ; /* #19 bytes */
UINT16 inflate_tab_8[0x116] ; /* #278 bytes */

UINT16 intermed_tab_1[16];
UINT16 intermed_tab_2[16];
UINT16 intermed_tab_3[0x140]; /* #320 bytes */

SINT32 if1_count, if2_count ;
UINT8  outputQueue[65536*2]; /* 128 KiB */
SINT32 outputPos = 65536;

SINT32 byte_count;

UINT8 vntype ;
UINT8 vnsfx  ;

UINT32 blockSize;


char password[256];
UINT32 keys[3];
int acceptAll;
int rejectAll;
/* global DIM's end */

/* proc */ void CreateCRCTab(void) {

    UINT32 i,j,k;
    
    for (i=0; i< 256; i++) {
        k=i ;
        for (j=0; j < 8; j++ ) {
            if (k & 1) {
                k >>= 1 ;
                k ^= 0xEDB88320;
            } else
            k >>= 1 ;
        }
        CRCtab[i] = k ;
    }
    
} /* endproc CreateCRCTab */

void increment_crc(UINT8 *data, SINT32 len)
{
    SINT32 i;
    for (i=0; i < len; i++)
    currentcrc = (currentcrc >> 8) ^ CRCtab[*data++ ^ (UINT8)currentcrc] ;
}

void ReadInflate(void)
{
    byte_count += 1024;
    blockSize = fread(inflatebuf,1,1024,FFzipfile) ;
    inflatepos = 0 ;
}
void ConditionalReadInflate(void)
{
    if (inflatepos == 1024) /* then */ 	ReadInflate() ;
}

SINT32 FlushQueue(SINT32 i)
{
    if (i || outputPos >= 65536*2) {
        increment_crc(outputQueue+65536,outputPos-65536) ;
        fwrite(outputQueue+65536,outputPos-65536,1,FFoutfile) ;
        memcpy(outputQueue,outputQueue+65536,65536) ;
        outputPos = 65536 ;
        return 1;
    }
    return 0;
}

void fetchbyte(void)
{
    ConditionalReadInflate();
    accum |= (inflatebuf[inflatepos++]) << 8 ;
    accum_count = 8 ;
}

void shr_n_bits(SINT32 count)
{
    if (count >= accum_count) {
        accum >>= accum_count ;
        count = count - accum_count ;
        fetchbyte() ;
    }
    accum >>= count ;
    accum_count -= count ;
}

UINT32 get_n_bits_b(SINT32 count)
{
    SINT32 rv = accum & masktab[count] ;
    shr_n_bits(count) ;
    return rv ;
}

UINT32 get_n_bits_w(SINT32 count)
{
    SINT32 rv = get_n_bits_b(8 ) ;
    rv += (get_n_bits_b(count-8) << 8) ;
    return rv ;
}

UINT32 get_n_bits(SINT32 count)
{
    if (count > 8) /* then */
      return get_n_bits_w(count) ;
    else
      return get_n_bits_b(count) ;
    /* endif */
}
    
void ExpandTables(UINT16 *dest2, UINT16 *dest, UINT8 *source, SINT32 len)
{
    SINT32 i;
    SINT16 sum = 0,xlen = 0 ;
    UINT16 *idest ;
    
    if (!len) /* then */ return ;
    
    memset(intermed_tab_1,0,sizeof(intermed_tab_1)) ;
    
    for (i=0; i < len; i++) intermed_tab_1[source[i]] ++ ;
    
    intermed_tab_2[1] = 0 ;

    for (i=1; i < 16; i++) {
        sum += intermed_tab_1[i] ;
        sum <<=1 ;
        intermed_tab_2[i+1] = sum ;
    }
    
    if (sum) {
        sum = 0 ;
        for (i= 1; i < 16; i++) sum += intermed_tab_1[i] ;
        if (sum > 1) /* then */ longjmp(inflate_badtab,1) ; /* ########## */
    }
    
    for (i=0; i < len; i++)
    if (source[i] == 0)
        intermed_tab_3[i] = 0 ;
    else
        intermed_tab_3[i] = intermed_tab_2[source[i]]++ ;

    idest = intermed_tab_3 ;

    for (i=0; i < len; i++) {
        sum = source[i] -1;
        if (sum <= 0)
        idest++ ;
        else {
            SINT16 shift = *idest ;
            SINT16 shift1 = 0 ;
            do {
                shift1 <<= 1 ;
                if (shift & 1)
                shift1 |= 1 ;
                shift >>= 1 ;
            } while (--sum && shift != 0) ;
            shift1 <<= 1 ;
            if (shift & 1)
            shift1 |= 1 ;
            shift1 <<= sum ;
            *(idest)++ = shift1 ;
        }
    }

    memset(dest,0,256*2) ;
    idest = intermed_tab_3 ;
    source += len - 1 ;
    idest += len-1 ;
    for (i = len-1 ; i >= 0; i--) {
        if ((sum = *source--) != 0) {
            if (sum > 8) {
                        SINT16 *xdest = dest + (*idest & 0xFF) ;
                        SINT32 j,shift=1,mask =*idest >> 8 ;
                        sum -= 8 ;
                        for (j = 0; j < sum; j++) {
                            UINT32 val ;
                            if (*xdest == 0) {
                        *xdest = ~xlen ;
                        dest2[xlen] = dest2[xlen+1]= 0;
                        xlen += 2 ;
                    }
                    xdest = ~*xdest + dest2;
                    if (mask & shift)
                    xdest ++ ;
                    shift <<= 1;
                }
                *xdest = i ;
            } else {
                SINT32 pos = *idest ;
                sum = 1 << sum;
                do {
                    dest[pos] = i ;
                    pos += sum ;
                } while (pos < 0x100) ;
            }
        }
        idest-- ;
    }
} /* endfunction ExpandTables */

/* proc */ void FixedTabs (void) {

        memset(inflate_tab_1        , 8, 0x90) ;
        memset(inflate_tab_1 + 0x90 , 9, 0x70) ;
        memset(inflate_tab_1 + 0x100, 7, 0x18) ;
        memset(inflate_tab_1 + 0x118, 8,  0x8) ;
        if2_count = 32 ; /* GLOBAL SINT32 */
        memset(inflate_tab_2, 5, 32) ;
        if1_count = 0x120 ; /* GLOBAL SINT32 */

} /* endproc */
        
UINT32 fancymove(SINT16 b, SINT16 *tab, SINT32 *count)
{
    SINT16 c;
    *count = 0 ;
    shr_n_bits(8) ;
    c = accum & 0xFF ;
    do {
        b = ~b ;
        (*count)++ ;
        b &= ~1 ;
        if (c & 1)
        b |= 1 ;
        c >>= 1 ;
    } while ((b = tab[b]) < 0) ;
    return b ;
}

UINT32 consulttabs1(void)
{
    SINT16 b = (accum & 0xff) ;
    SINT32 count,shift ;
    if ((b = inflate_tab_3[b]) >= 0)
      shr_n_bits(count = inflate_tab_1[b]) ;
    else {
        b = fancymove(b,inflate_tab_5,&count) ;
        shr_n_bits(count) ;
    }
    if ( b < 0x109)
    return b ;
    if (b == 0x11d) {
        return 0x200 ;
    }
    b -= 0x101 ;
    shift = (b >> 2) - 1 ;
    b = (((b & 3) + 4) << shift) + 0x101 ;
    b += get_n_bits(shift) ;
    return b ;
}

UINT32 consulttabs2(void)
{
    SINT16 b = (accum & 0xff) ;
    SINT32 count,shift ;
    if ((b = inflate_tab_4[b]) >= 0)
    shr_n_bits(count = inflate_tab_2[b]) ;
    else {
        b = fancymove(b,inflate_tab_6,&count) ;
        shr_n_bits(count) ;
    }
    if ((b & 0xff) >= 4) {
        shift = (b & 0xff)/2 -1;
        b = ((b & 1) + 2 ) << shift ;
        b += get_n_bits(shift) ;
    }
    return b ;
}

/* function */ UINT32 Inflate() {

    SINT32 count, scount, iii, len, valx, shift, ia, vala, b ;
    UINT8 *xdest ;
    UINT8 Type, LastBlock ;
    char *src ;
    
    LastBlock  = 0 ; 
    byte_count = -1024 ; /* Is __GLOBAL__ SINT32 */

    if (setjmp(inflate_badtab)) /* then */ { /* ########## */
        printf(" Bad Table ") ;
        return 0; /* Mallicious */
    } /* endif */

    ReadInflate() ;

    ConditionalReadInflate();
    accum = inflatebuf[inflatepos++] ;
    ConditionalReadInflate();
    accum |= inflatebuf[inflatepos++] << 8 ;
    accum_count = 8 ;

    while (1) /* do */ /* Main Inflate loop */ {
    
      LastBlock = accum & 1 ; /* Pick just 1 bit */
      shr_n_bits(1) ;
    
      Type = get_n_bits(2) ; /* Type of next block: 00-uncomp 01-fixed 10-dynamic 11-error  */
      
      if (Type==0) /* then */ /* Uncompressed block, copy data in the clear */ {
      
          if (accum_count != 8) /* then */ shr_n_bits(accum_count) ; 
          /* Skip up to 7 junk ^^^ bits, align to byte */
          count = accum ; /* Pick block size */ 
          ConditionalReadInflate() ;
          scount = inflatebuf[inflatepos++] ; /* Pick redundant size */
          ConditionalReadInflate() ;
          scount |= inflatebuf[inflatepos++] << 8 ; /* Even more */
          if (count + scount == 0xFFFF) /* then */ {
                do {
                    ConditionalReadInflate() ;
                    outputQueue[outputPos++] = inflatebuf[inflatepos++];
                    FlushQueue(0) ;
                } while (--count) ; /* Copying block data */
                if (LastBlock==0) /* then */ {
                     ConditionalReadInflate();
                    accum = inflatebuf[inflatepos++] ;
                    ConditionalReadInflate();
                    accum |= inflatebuf[inflatepos++] << 8 ;
                    accum_count = 8 ; /* Prepare further reading */   
                } /* endif */
          } else {
                longjmp(inflate_badtab,1) ; /* Block is bad  */ /* ########## */
          } /* endif */
      
      } /* endif */ /* Type 0 */
        
      if (Type==1) /* then */ /* 01 - "fixed" default tabs, rare, small TEXT files */ {
          FixedTabs() ;
      } /* endif */ 

      if (Type==2) /* then */ /* 10 - compressed tabs */ {
      
          if1_count = get_n_bits(5) + 0x101 ;
          if2_count = get_n_bits(5) + 1 ;
          memset(inflate_tab_7,0,0x13) ;
          len = get_n_bits(4) + 4 ;
          for (iii=0; iii < len; iii++) {
            inflate_tab_7[tab7_rearrange[iii]] = get_n_bits(3) ;
          } /* next iii */
          ExpandTables(tab7_rearrange+len,inflate_tab_8,inflate_tab_7,0x13) ;
          xdest = inflate_tab_1 ;
          for (iii=if1_count + if2_count; iii > 0; ) {
            valx = inflate_tab_8[accum & 0xff] ;
            shr_n_bits(inflate_tab_7[valx]) ;
            if (valx < 0x10) {
                *xdest++ = valx ;
                iii-- ;
            } else {
                switch(valx) {
                case 0x10:
                    valx = get_n_bits(2) + 3 ;
                    shift = *(xdest-1) ;
                    break ;
                case 0x11:
                    valx = get_n_bits(3) + 3 ;
                    shift = 0 ;
                    break ;
                default:
                    valx = get_n_bits(7) + 11 ;
                    shift = 0 ;
                    break ;
                }
                iii -=valx ;
                if (iii < 0) /* then */ longjmp(inflate_badtab,1) ; /* ########## */
                memset(xdest,shift,valx) ;
                xdest += valx ;
            } /* endif */
          }  /* next iii */
          memcpy(inflate_tab_2,inflate_tab_1+if1_count,if2_count) ;
          
      } /* endif */ /* Type 2 */

      if (Type==3) /* then */ /* Error, no such block defined */ {
          longjmp(inflate_badtab,1) ; /* ########## */
      } /* endif */ /* Type 2 */
     
      if (Type!=0) /* then */ {
      
        ExpandTables(inflate_tab_5,inflate_tab_3,inflate_tab_1,if1_count) ;
        ExpandTables(inflate_tab_6,inflate_tab_4,inflate_tab_2,if2_count) ;
    
        while (1) /* do */ /* Block Inflate loop running on fixed or dynamic tabs */ {
            vala = consulttabs1() ;
            if (!(vala & 0xFF00)) /* then */{
                outputQueue[outputPos++] = vala & 0xFF ;
                FlushQueue(0) ;
            } else {
                if (vala == 0x100)
                break ;
                vala -= 0xFE ;
                b = consulttabs2() ;
                src = outputQueue + outputPos - 1 - b ;
                for (ia=0 ; ia < vala ; ia++) {
                    outputQueue[outputPos++] = *src++ ;
                    if (FlushQueue(0)) /* then */ src -= 65536;
                } /* next ia */
            } /* endif */
        } /* endwhile | loop */ /* Block Inflate loop running on fixed or dynamic tabs */
      
      } /* endif */ /* Type!=0 */
      
      if (LastBlock==1) /* then */ break;

    } /* endwhile | loop */ /* Main Inflate loop */

    FlushQueue(1) ;
    
    // relocate to the next position in the stream
    fseek(FFzipfile, inflatepos - blockSize-(accum_count/8+1), SEEK_CUR);
    return 1 ; /* GOOD */
    
} /* endfunction Inflate */

/* proc */ void ExtractStored(UINT32 len) {

    char buf[2048] ;
    UINT32 qq7 , bytes ;
    
    while (1) /* do */ {

        qq7 = len ;
        if (qq7>2048) /* then */ qq7=2048 ;
        
        bytes = fread(buf,1,qq7,FFzipfile) ;
        increment_crc(buf,bytes) ;
        len -= bytes ;
        fwrite(buf,1,bytes,FFoutfile) ;
        if (len==0) /* then */ break ;

    } ; /* endwhile */
    return ;

} /* endproc ExtractStored */

/* function */ UINT32 FindZipHeader(ZIPHEAD *head, UINT32 sfxOffset) {

    UINT32 offset = 0, foundofs = 0 , i , bytes ;
    char buf[1024] ;
    
    bytes = fread(buf,1,1024,FFzipfile) ;
    if (bytes>100) /* then */ {
      bytes -= 4 ;
    } else {
      bytes=0 ; /* Failure, can't be a valid ZIP */
    } /* endif */
    
    while (bytes) {
        for (i=0; i< bytes; i++)
        if (*(UINT32 *)(buf+i) == 0x06054B50) /* PK 5,6 */ {
            foundofs = offset + i ;
        }
        *(UINT32 *)(buf) = * (UINT32 *)(buf+bytes) ;
        offset += bytes ;
        bytes = fread(buf+4,1,1020,FFzipfile) ;
    } /* endwhile */
    
    if (foundofs) /* then */ {
      fseek(FFzipfile,foundofs+ sfxOffset,SEEK_SET) ;
      fread(head,1,sizeof(ZIPHEAD),FFzipfile) ;
      return 1 ; /* GOOD */
    } else {
      return 0 ; /* Mallicious */
    } /* endif */
    
} /* endfunction FindZipHeader */

UINT32 createdir(char *path)
{
    char *p = path;
    char s;
    while ((p = strchr(p, '\\')) != 0)
    {
        if (p != path && p[-1] != '.')
        {
            s = *p;
            *p = 0;
            mkdir(path);
            *p++ = s;
        }
    }
    return 0;
}
UINT32 nextcrc(UINT32 currentcrc, UINT8 ch)
{
    return (currentcrc >> 8) ^ CRCtab[ch ^ (UINT8)currentcrc] ;
}
void updatekeys(UINT8 ch)
{
    keys[0] = nextcrc(keys[0], ch);
    keys[1] = keys[1] + (keys[0] & 255);
    keys[1] = keys[1] * 134775813 + 1;
    keys[2] = nextcrc(keys[2], keys[1] >> 24);
}
UINT32 decryptbyte(void)
{
    UINT16 temp = keys[2] | 2;
    return (temp * (temp ^ 1)) >> 8;
}
UINT32 InitializeKeys(int crc)
{
    int l = strlen(password), i;
    char buf[12];
    keys[0] = 305419896;
    keys[1] = 591751049;
    keys[2] = 878082192;
    for (i=0; i < l; i++)
        updatekeys(password[i]);
    if (fread(buf, 1, 12, FFzipfile) != 12)
        return 1;
    for (i=0; i < 12; i++)
    {
        buf[i] ^= decryptbyte();
        updatekeys(buf[i]);
    }
    return (crc >> 24) != buf[11]; // sanity check, returns 0 for success
}
UINT32 Decrypt(char *name , int len, int crc)
{
    FILE *fil;
    int i;
    if (!password[0])
        return 1;
    fil = fopen(name, "wb");
    if (!fil)
    {
        return 1;
    }
    if (InitializeKeys(crc))
    {
        fclose(fil);
        unlink(name);
        return 1;
    }
    
    for (i=0; i < len; i++)
    {
        int ch = fgetc(FFzipfile);
        if (ch == -1)
        {
            fclose(fil);
            unlink(name);
            return 1;
        }
         ch ^= decryptbyte();
         updatekeys(ch);
         if (fputc(ch, fil) == -1)
        {
            fclose(fil);
            unlink(name);
            return 1;
        }
    }
    fclose(fil);
    return 0;
}
UINT32 DoExistsPrompt(char *name)
{
    FILE *fil;
    int rv = 1;
    if (acceptAll)
        return 1;
    if (rejectAll)
        return 0;
    fil = fopen(name, "rb");
    if (fil)
    {
        UINT32 done = 0;
        fclose(fil);
        // exists...
        printf("%s exists.  Overwrite (Y/N/A/S)? ", name); 
        while (!done)
        {
            int n;
            done = 1;
            switch(n = getch())
            {
                case 'Y': case 'y':
                    rv = 1;
                    break;
                case 'N': case 'n':
                    rv = 0;
                    break;
                case 'S': case 's': case 0x1b:
                    rejectAll = 1;
                    rv = 0;
                    break;
                case 'A': case 'a':
                    acceptAll = 1;
                    rv = 1;
                    break;
                default:
                    done = 0;
                    break;
            }
            if (done)
            {
                printf("%c\n", n);
            }
        }
    }
    return rv;
}
UINT32 unzipOneFile(char *path, ZIPDIR *direntry, UINT32 sfxOffset) {
    static char *temp="$$uztmp.$$$";
    FILE *cachedFile;
    ZIPFILE fileentry ;
    UINT32  rv ;
    UINT8   qq5 ,qq6 ;
    char    pathandname[128];
    char    buf[256] ;
    direntry->filename[direntry->filenamelen] = 0 ;
    strcpy(pathandname,direntry->filename);
    
    qq5=0 ;
    while (1) /* do */ {
      qq6=pathandname[qq5] ;       /* "backing" possible slashes in PATH from ZIP !!! */ 
      if (qq6==0) /* then */ break ;   /* $2F=#47="/" $5C=#92="\" */
      if (qq6==47) /* then */ qq6=92 ;
      pathandname[qq5]=qq6 ;
      qq5=qq5+1 ; 
    } ; /* endwhile */

    strcpy(buf,path) ; strcat(buf,pathandname) ;
    rv = DoExistsPrompt(buf);

    printf ("File: #%s %s# ",path,pathandname) ;

    if (!rv)
    {
        printf("SKIPPED\r\n");
        return 0;
    }
    if (direntry->attrib & 0x10) // directory
    {
        printf("(Directory)\r\n");
        return createdir(buf);
    }
	else
		createdir(buf);
    fseek(FFzipfile,direntry->fileoffset + sfxOffset,SEEK_SET) ;
    fread(&fileentry,1,sizeof(fileentry),FFzipfile) ; /* Load "Local file header" */ 
    fseek(FFzipfile,direntry->fileoffset + sizeof(ZIPFILE) +
    fileentry.filenamelen + fileentry.secheadlen + sfxOffset, SEEK_SET) ;

    FFoutfile = fopen(buf,"wb") ; 
    /* Result : ||| (>0) : GOOD ||| 0 : Mallicious */ /* "FFoutfile" is a __GLOBAL__ one */
    
    if (!FFoutfile) /* then */ {
        printf ("Can't open output file :-(") ;
        return 1; /* Mallicious */
    } /* endif */
    
    currentcrc = 0xFFFFFFFF ;
    qq5=direntry->compression ;

    if (fileentry.bitFlags & 1)
    {
        if (Decrypt(temp, fileentry.compressedSize, direntry->crc32))
        {
            printf("bad password (SKIPPED)\r\n");
            return 0;
        }
        cachedFile = FFzipfile;
        FFzipfile = fopen(temp, "rb");
        if (!FFzipfile)
        {
            FFzipfile = cachedFile;
            printf ("Can't open temp file :-(") ;
            return 1;
        }
    }
    if (qq5 == 8) /* then */ {
      printf ("Deflated ") ; /* 8 = Deflate-32 */
      rv = Inflate() ; /* Result : ||| 0 : Mallicious ||| 1 : GOOD */
      rv = (!rv) ; /* Result : ||| 1 : Mallicious ||| 0 : GOOD */
    } ; /* endif */
    if (qq5 == 0) /* then */ {
      printf ("Stored ");
      ExtractStored(direntry->internalsize) ; rv=0 ;
    } ; /* endif */
    if ((qq5 != 0) && (qq5 != 8)) /* then */ {
      rv=2;
      printf ("Unsupported compression method (%d) :-(",qq5);	 
    } ; /* endif */

    if (fileentry.bitFlags & 1)
    {
        fclose (FFzipfile);
        FFzipfile = cachedFile;
        unlink(temp);
    }

    setftime(fileno(FFoutfile), &direntry->filetime);
    fclose(FFoutfile) ;

    if (rv==0) /* then */ {
      if (~currentcrc != direntry->crc32) /* then */ {
        printf ("CRC FAILED :-(") ; rv = 1 ;
      } else {
        printf ("OK");
      } ; /* endif */
    } /* endif */

    if(rv)
        unlink(buf);
    printf("\r\n") ;
    return rv ; /* 0: GOOD ||| 1: Mallicious tables/CRC/path ||| 2: Unsupported comp */ 

} /* endfunction unzipOneFile */ 
UINT32 gunzip(char *path, GZIPHDR *hdr, char *inName)
{
    GZIPTRLR trail;
    char name[256];
    char comment[1024];
    char buf[256];
    int i;
    int rv;
    UINT16 hcrc;
    name[0] = 0;
    comment[0] = 0;
    if (hdr->id1 != 0x1f || hdr->id2 != 0x8b || hdr->cm != 8 || (hdr->flg & FRES))
    {
        printf("invalid file format");
        return 1;
    }
    if (hdr->flg & FEXTRA)
    {
        UINT16 len;
        // read the length
        if (fread(&len, 1, 2, FFzipfile) != 2)
            return 1;
        // discard the data
        for (i ==0 ; i < len; i++)
            if (fgetc(FFzipfile) == -1)
                return 1;
    }
    if (hdr->flg & FNAME)
    {
        char *p = name;
        int n;
        do
        {
            n = fgetc(FFzipfile);
            if (n == -1)
                return 1;
            *p++ = n;
        } while (n);
    }
    if (hdr->flg & FCOMMENT)
    {
        char *p = comment;
        int n;
        do
        {
            n = fgetc(FFzipfile);
            if (n == -1)
                return 1;
            *p++ = n;
        } while (n);
    }
    if (hdr->flg & FHCRC)
    {
        if (fread(&hcrc, 1, 2, FFzipfile) != 2)
            return 1;
    }
    if (name[0])
    {
        strcpy(buf, path);
        strcat(buf, name);
    }
    else
    {
        int l = strlen(inName);
        if (l > 3 && !stricmp(inName + l - 3, ".gz"))
        {
            strcpy(buf, inName);
            buf[l-3] = 0;
        }
        else
        {
            printf("input file requires .gz suffix");
            return 1;
        }
            
    }
    rv = DoExistsPrompt(buf);
    printf ("File: #%s# Deflated ", buf) ;
    if (!rv)
    {
        printf("SKIPPED\r\n");
        return 0;
    }

	createdir(buf);

    FFoutfile = fopen(buf,"wb") ; 
    /* Result : ||| (>0) : GOOD ||| 0 : Mallicious */ /* "FFoutfile" is a __GLOBAL__ one */
    if (!FFoutfile) /* then */ {
        printf ("Can't open output file :-(") ;
        return 1; /* Mallicious */
    } /* endif */
    currentcrc = 0xFFFFFFFF ;

    rv = Inflate() ; /* Result : ||| 0 : Mallicious ||| 1 : GOOD */
    rv = (!rv) ; /* Result : ||| 1 : Mallicious ||| 0 : GOOD */

    if (rv==0) /* then */ {
        if (fread(&trail, 1, sizeof(trail), FFzipfile) != sizeof(trail))
        {
            printf("missing trailer");
            return 1;
        }
      if (~currentcrc != trail.crc32) /* then */ {
        printf ("CRC FAILED :-(") ; rv = 1 ;
      } else {
        printf ("OK");
      } ; /* endif */
    } /* endif */

  
    if (hdr->mtime)
    {
        struct utimbuf xx;
        xx.actime = hdr->mtime;
        xx.modtime = hdr->mtime;
        _futime(fileno(FFoutfile), &xx);
    }
    fclose(FFoutfile);
    if(rv)
    {
        unlink(buf);
    }   
    printf("\r\n") ;
    return rv ; /* 0: GOOD ||| 1: Mallicious tables/CRC/path ||| 2: Unsupported comp */ 
}
UINT32 unzip(char *filename, char *path) {

    char *p ;
    UINT32 sfxOffset = 0;
    ZIPHEAD head ;
    ZIPDIR direntry ;
    UINT32 qq2,qq3,qq4 , ofs ;
    char buf7[32] ;
        
    FFzipfile = fopen(filename,"rb") ; 
    /* Result: ||| 0 : Mallicious ||| (>0) : Good */ /* "FFzipfile" is a __GLOBAL__ file handle/pointer */

    if (!FFzipfile) /* then */ {
        printf("Cannot open input file #%s# :-(", filename) ;
        vntype=99 ;
    } else {	
        qq2 = fread(buf7,1,32,FFzipfile) ;
        if (qq2!=32) /* then */ {
          printf ("File is faulty/empty/ridiculously small :-(");
          vntype=99 ;
        } else {	
		  if (buf7[0] == 'M' && buf7[1] == 'Z')
		  {
			fseek(FFzipfile, -8, SEEK_END);
	    	qq2 = fread(buf7,1,8,FFzipfile) ;
			if (buf7[0] != 'L' || buf7[1] != 'S')
			{
				printf("Invalid file format");
			    vntype = 99;
			}
			else
			{
				sfxOffset = *(UINT32 *)(buf7 + 4);
				fseek(FFzipfile, sfxOffset, SEEK_SET);
		    	qq2 = fread(buf7,1,32,FFzipfile) ;
		        if (qq2!=32) /* then */ {
			      printf ("File is faulty/empty/ridiculously small :-(");
		    	  vntype=99 ;
				}
			}
		  }
		  if (vntype != 99)
		  {
              vntype=0 ; vnsfx=0 ; /* "vntype" : ||| 1:ZIP 2:7-ZIP 3:GZIP */
              if (*(UINT32 *)(buf7) == 0x04034B50) /* 4: PK 3,4 */ /* then */ vntype = 1 ;
              if (*(UINT32 *)(buf7) == 0xAFBC7A37) /* 6: 7z $BC,$AF */ /* then */ {
                vntype = 2 ;
                putchar (55) ; putchar (45) ;
              } ; /* endif */	
              buf7[3]=0;
              if (*(UINT32 *)(buf7) == 0x00088B1F) /* 3: $1F,$8B,8 */ /* then */ {
                vntype =3 ;
                putchar (71) ;
              } ; /* endif */	
              if (vntype==0) /* then */ {
                vnsfx=1;	  
              } else {
                printf ("ZIP file format detected");
                printf ("\r\n\r\n") ;
               } ; /* endif */
          } ; /* endif */
        } ; /* endif */
    } /* endif */

    if (vntype <= 1) /* zip */
    {
        
        p = strrchr(path,'\\') ; /* Search __LAST__ occur of 1 byte in string, returns pointer to found char */ 
        if (!p && path[0] || p && *(p+1) != 0) /* then */ strcat(path,"\\") ; 
        
        fseek(FFzipfile,sfxOffset,SEEK_SET) ; /* Set absolute position to begin again */
        if (!FindZipHeader(&head, sfxOffset)) /* then */ return 1 ; /* Mallicious */
        ofs = head.diroffset ; qq4=head.filecount ; qq3=0 ;
        
        while (1) /* do */ {
        
            fseek(FFzipfile,ofs + sfxOffset,SEEK_SET) ; /* Set absolute position to central DIR */
            fread(&direntry,1,sizeof(direntry),FFzipfile) ;
            qq2=unzipOneFile(path,&direntry, sfxOffset); /* Result: ||| 0 : GOOD ||| (>0) : Mallicious */ 
            if (qq2!=0) /* then */ return 1 ; /* Mallicious */
    
            ofs += 0x2E + direntry.secheadlen + 
            direntry.tertiaryheadlen + direntry.filenamelen;
    
            qq3=qq3+1 ; if (qq3==qq4) /* then */ break;
        
        } ; /* endwhile | loop */
        fclose(FFzipfile) ;
    }
    else if (vntype == 3) /* gzip */
    {
        p = strrchr(path,'\\') ; /* Search __LAST__ occur of 1 byte in string, returns pointer to found char */ 
        if (!p && path[0] || p && *(p+1) != 0) /* then */ strcat(path,"\\") ; 
        
        fseek(FFzipfile,sfxOffset,SEEK_SET) ; /* Set absolute position to begin again */
        qq3=0 ;
        while (1) /* do */ {
            GZIPHDR hdr;
            if (fread(&hdr, 1, sizeof(hdr), FFzipfile) == 0)
                return 0;
            if (gunzip(path, &hdr, filename))
                return 1;
            qq3 = qq3 + 1;
        }
        fclose(FFzipfile) ;
        return 1;
    }
    else
    {
        fclose(FFzipfile) ;
        return 1;
    }
    fclose(FFzipfile) ;
    return 0 ; /* GOOD */
    
} /* endfunction unzip */ 

UINT32 ParseOptions(UINT32 argc, char *argv[])
{
    int i;
    for (i=1; i < argc; i++)
    {
        int changed = 0;
        if (argv[i][0] == '/' || argv[i][0] == '-')
        {
            if (argv[i][1] == 'Q')
            {
                changed = 1;
                acceptAll = 1;
            }
            else if (argv[i][1] == 'p')
            {
                changed = 1;
                strcpy(password, argv[i]+2);
            }
            else
            {
                return 1;
            }
        }
        if (changed)
        {
            memcpy(argv + i, argv + i + 1, (argc - i - 1) * sizeof(char *));
            argc--;
            i--;
        }
    }
    return argc;
}
UINT32 main (UINT32 argc, char *argv[]) {

    char arg1file[256] ;
    char arg2path[256] ;
    UINT8 qq1 ;
    
    strcpy(arg1file,__DATE__) ;
    strcat(arg1file, " ") ;
    strcat(arg1file,__TIME__) ;
    
    printf ("\r\nUltimate Un-ZIP %s\r\n\r\n",arg1file);
    
    CreateCRCTab() ;
    
    argc = ParseOptions(argc, argv);
    if (argc == 1 || argc > 3) /* then */ {
    
      printf ("Usage: UZ [/Q] [/ppassword] file [path]") ; qq1=1 ;
    
    } else { 
      strcpy(arg1file,argv[1]) ;

      if (argc == 3) /* then */
        strcpy(arg2path,argv[2]) ;
      else
        arg2path[0] = 0 ;
      /* endif */  

      qq1 = unzip(arg1file,arg2path) ; /* 0 : GOOD ||| 1 : Mallicious */ 
    } ; /* endif */
    
    printf ("\r\n\r\n") ;
    return qq1;	/* 0 : GOOD ||| 1 : Mallicious */

} /* endmain */
