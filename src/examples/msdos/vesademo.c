//
// This example uses DPMI to initialize VESA truecolor 24/32 bpp
// LFB video mode and draws nice garbage on the screen
//

// --------------------------------------------------------
// INT $31 / AX = $0800
//   BX:CX  = physical address of memory
//   SI:DI  = size of region to map (bytes)
//   if function successful: Carry flag = clear
//   BX:CX  = linear address that can be used to access the
//   physical memory
// --------------------------------------------------------
// INT 10 - VESA HyperVGA BIOS (VBE) - GET HyperVGA INFORMATION
//   AX = 4F00h
//   ES:DI -> buffer for HyperVGA information (see #00077)
// Return: AL = 4Fh if function supported
//   AH = status
//      00h successful ES:DI buffer filled
//      01h failed
// Offset  Size    Description     (Table 00077)
// 00h  4 BYTEs   (ret) signature ("VESA")
//                (call) VESA 2.0 request signature ("VBE2")
// 04h    WORD    VESA version number (one-digit minor version -- 0102h = v1.2)
// 06h    DWORD   pointer to OEM name "761295520" for ATI
// 0Ah    DWORD   capabilities flags (see #00078)
// 0Eh    DWORD   pointer to list of supported VESA and OEM video modes
//                (list of words terminated with FFFFh)
// 12h    WORD    total amount of video memory in 64K blocks
// 14h    WORD    OEM software version (BCD, high byte = major, low byte = minor)
// 16h    DWORD   pointer to vendor name
// 1Ah    DWORD   pointer to product name
// 1Eh    DWORD   pointer to product revision string
//Bitfields for VESA capabilities:
//Bit(s)  Description     (Table 00078)
// 0      DAC can be switched into 8-bit mode
// 1      non-VGA controller
// 2      programmed DAC with blank bit (i.e. only during blanking interval)
//INT 10 - VESA SuperVGA BIOS - GET SuperVGA MODE INFORMATION
//        AX = 4F01h
//        CX = SuperVGA video mode (see #04082 for bitfields)
//        ES:DI -> 256-byte buffer for mode information (see #00079)
//Return: AL = 4Fh if function supported
//        AH = status
//            00h successful ES:DI buffer filled
//            01h failed
//Bit(s)  Description     (Table 04082)
// 15     preserve display memory on mode change
// 14     (VBE v2.0+) use linear (flat) frame buffer
// 8-0    video mode number (0xxh are non-VESA modes, 1xxh are VESA-defined)
//Format of VESA SuperVGA mode information:
//Offset  Size    Description     (Table 00079)
// 00h    WORD    mode attributes (see #00080)
// 02h    BYTE    window attributes, window A (see #00081)
// 03h    BYTE    window attributes, window B (see #00081)
// 04h    WORD    window granularity in KB
// 06h    WORD    window size in KB
// 08h    WORD    start segment of window A (0000h if not supported)
// 0Ah    WORD    start segment of window B (0000h if not supported)
// 0Ch    DWORD   -> FAR window positioning function (equivalent to AX=4F05h)
// 10h    WORD    bytes per scan line
//---remainder is optional for VESA modes in v1.0/1.1, needed for OEM modes---
// 12h    WORD    width in pixels (graphics) or characters (text)
// 14h    WORD    height in pixels (graphics) or characters (text)
// 16h    BYTE    width of character cell in pixels
// 17h    BYTE    height of character cell in pixels
// 18h    BYTE    number of memory planes
// 19h    BYTE    number of bits per pixel
// 1Ah    BYTE    number of banks (CRAP)
// 1Bh    BYTE    memory model type (see #00082)
// 1Ch    BYTE    size of bank in KB
// 1Dh    BYTE    number of image pages (less one) that will fit in video RAM
// 1Eh    BYTE    reserved (00h for VBE 1.0-2.0, 01h for VBE 3.0)
//---VBE v1.2+ ---
// 1Fh    BYTE    red mask size
// 20h    BYTE    red field position
// 21h    BYTE    green mask size
// 22h    BYTE    green field size
// 23h    BYTE    blue mask size
// 24h    BYTE    blue field size
// 25h    BYTE    reserved mask size
// 26h    BYTE    reserved mask position
// 27h    BYTE    direct color mode info
//---VBE v2.0+ ---
// 28h    DWORD   physical address of linear video buffer
// 2Ch    DWORD   pointer to start of offscreen memory
// 30h    WORD    KB of offscreen memory
//Bitfields for VESA SuperVGA mode attributes:
//Bit(s)  Description     (Table 00080)
// 0      mode supported by present hardware configuration
// 1      optional information available (must be =1 for VBE v1.2+)
// 2      BIOS output supported
// 3      set if color, clear if monochrome
// 4      set if graphics mode, clear if text mode
//---VBE v2.0+ ---
// 5      mode is not VGA-compatible
// 6      bank-switched mode not supported
// 7      linear framebuffer mode supported
// 8      double-scan mode available (e.g. 320x200 and 320x240)
//INT 10 - VESA SuperVGA BIOS - SET SuperVGA VIDEO MODE
//        AX = 4F02h
//        BX = new video mode (see #04082,#00083,#00084)
//Return: AL = 4Fh if function supported
//        AH = status
//            00h successful |  01h failed
// --------------------------------------------------------
// INT 21 - DOS 2+ - ALLOCATE MEMORY
//      AH = 48h
//      BX = number of paragraphs to allocate
// Return: CF clear if successful
//          AX = segment of allocated block
//         CF set on error
//          AX = error code (07h,08h) (see #01680 at AH=59h/BX=0000h)
//          BX = size of largest available block
// --------------------------------------------------------
// INT 21 - DOS 2+ - FREE MEMORY
//      AH = 49h
//      ES = segment of block to free
// Return: CF clear if successful
//         CF set on error
//          AX = error code (07h,09h) (see #01680 at AH=59h/BX=0000h)

#include <stdio.h> /* Studio ??? */
#include <conio.h> /* CowNio ??? */
#include <i86.h>
#include <dpmi.h>
#include <string.h>

//#include <string.h>
//#include <memory.h> /* Don't uncomment, compiler becomes even more mad */

typedef unsigned char  UINT8  ;
typedef unsigned short UINT16 ;
typedef unsigned int   UINT32 ;

UINT8  vidbuf[3145728];               
/* 800*600*3=1440000 | 1024*768*4=3MiB=3145728 */

UINT8  dosmirbuf[1024];
UINT8  mlbuf[100];     /* Intermediate candidates, up to 100  */
UINT8  WINNERBUF[48];  /* Final winners, up to 4 per 12 bytes */
UINT8  COLORBUF[1024];

DPMI_REGS regs;

UINT8  VBPREF32,VBPREFHI;
/* ^^^ Prefer 32bpp from 24bpp | prefer 1024x768 from 800x600 */

UINT8  BRUTEFORCE,BFSUCC,BFMODE,MMODEV,MMODE0123,BFBPP,MMBPP;
UINT16 BFXX,BFYY,BFBPSL,MMXX,MMYY,MMBPSL,PLOTXX,PLOTYY;
UINT32 BFPHYSLFB,MMPHYSLFB,MMLIN,MMSIZE;

UINT16 PL1,PL2,VRAM; /* VESA info */
UINT32 DosMem,PLLIN;
UINT8  VBFAIL,VESAVER,MODES1,MODES2;

/* proc */ void Empty (void) {

  UINT32 qqq=0;
  while (1) {
    dosmirbuf[qqq]=0;
    qqq=qqq+1;
    if (qqq==1024) /* then */ break;
  } /* endwhile | loop */

} /* endproc */ 

/* proc */ void Eol (void) {
    printf ("\r\n");
    _delay(100);
} /* endproc */
/* proc */ void SmiGood (void) {
    printf (":-)");
} /* endproc */
/* proc */ void SmiEvil (void) {
    printf (":-(");
} /* endproc */

UINT32 main(UINT32 argc, UINT8 **argv) {

  UINT32 qqa32;
  UINT16 qqb16;
  UINT8  qqc8,qqd8;
   
  VBFAIL=0 ; VBPREF32=0 ; VBPREFHI=0 ;
 
  Eol();
  printf ("Hello DOSSER dude !!!");
  Eol();

  /* COLOR transation buffer */
  qqa32=0 ; qqc8=255 ; qqd8=255 ;
  while (1) {
    COLORBUF[qqa32]=qqc8;
    qqa32=qqa32+1;
    if (qqa32==1024) /* then */ break;
    qqc8=qqc8+qqd8;                      /* 255 to 55  step -1 | 200 */
    if (qqa32==200) /* then */ qqd8=2;   /* 55  to 255 step  2 | 100 */
    if (qqa32==300) /* then */ qqd8=253; /* 255 to 6   step -3 | 83  */
    if (qqa32==383) /* then */ qqd8=1;   /* 6   to 255 step  1 | 249 */
    if (qqa32==632) /* then */ qqd8=255; /* 255 to 59  step -1 | 196 */
    if (qqa32==828) /* then */ qqd8=1;   /* 59  to 254 step  1 | 195 */
  } /* endwhile | loop */

  if (argc>=2) /* then */ {
    qqc8=strcmp(argv[1],"PREF32");
    if (qqc8==0) /* then */ VBPREF32=1;
    qqc8=strcmp(argv[1],"PREFHI");
    if (qqc8==0) /* then */ VBPREFHI=1;
  } /* endif */
  if (argc>=3) /* then */ {
    qqc8=strcmp(argv[2],"PREF32");
    if (qqc8==0) /* then */ VBPREF32=1;
    qqc8=strcmp(argv[2],"PREFHI");
    if (qqc8==0) /* then */ VBPREFHI=1;
  } /* endif */

  printf ("DOS Alloc : ");
  regs.h.ax = 0x4800 ; /* Alloc */
  regs.h.bx = 0x0040 ; /* 1 KiB */
  regs.h.flags = 0x72 ;
  regs.h.ss = 0 ;
  regs.h.sp = 0 ;
  dpmi_simulate_real_interrupt(0x21,&regs);
  qqb16  =  regs.h.flags ;
  DosMem =  regs.h.ax ;
  if ((qqb16&1)==1) /* then */ {
    VBFAIL=1;
    printf ("failed ");
    SmiEvil() ;
  } else {
    printf ("OK: %d",DosMem);
  };
  Eol();

  if (VBFAIL==0) /* then */ {
   
    Empty();
    dosmirbuf[0]=86;
    dosmirbuf[1]=66;
    dosmirbuf[2]=69;
    dosmirbuf[3]=50;
    memcpy((void *)(DosMem*16),dosmirbuf,1024);

    regs.h.ax    = 0x4F00; /* VESA : Capturing info block */
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    regs.h.es = DosMem;
    regs.h.di = 0;
    dpmi_simulate_real_interrupt(0x10,&regs);
    qqb16 =  regs.h.ax ;
    if (qqb16!=0x004F) /* then */ {
      VBFAIL=1;
      printf ("VBE2 failure ");
      SmiEvil() ; Eol() ;
    };

  } /* endif */

  if (VBFAIL==0) /* then */ {
   
    memcpy(dosmirbuf,(void *)(DosMem*16),512);
    VESAVER=dosmirbuf[5];
    qqa32=0;
    while (1) {
      qqc8=dosmirbuf[qqa32];
      if ((qqc8<32) || (qqc8>125)) /* then */ qqc8=46;
      printf ("%c",qqc8);
      qqa32=qqa32+1;
      if (qqa32==512) /* then */ break;
    }; /* endwhile | loop */
    Eol();
   
    qqc8=4; /* Silly "pointers" */
    while (1) {
      qqb16=dosmirbuf[qqc8]+256*dosmirbuf[qqc8+1];
      printf (">%X-%X<",qqc8,qqb16);
      qqc8=qqc8+2;
      if (qqc8==34) /* then */ break;
    }; /* endwhile | loop */
    Eol();

    PL1  =dosmirbuf[14]+dosmirbuf[15]*256;
    PL2  =dosmirbuf[16]+dosmirbuf[17]*256;
    VRAM =dosmirbuf[18]+dosmirbuf[19]*256; /* 64 KiB units */
    PLLIN=(UINT32)PL1+PL2*16;

    printf ("Buffer: %d -> ",DosMem*16);
    printf ("List  : %d",PLLIN);
    Eol();
    printf ("VESA Version: %d | VRAM : %d MiB",VESAVER,VRAM/16);
    Eol();
    if (VESAVER<2) /* then */ {
      VBFAIL=1;
      printf ("At least VESA 2.0 required ");
      SmiEvil() ; Eol() ;
    };

  } /* endif */

  if (VBFAIL==0) /* then */ {

    MODES1=0;MODES2=0;
    while (1) {
      memcpy(&qqb16,(void *)(PLLIN+MODES1*2),2); /* PEEK */
      if ((MODES1==100) || (qqb16==65535)) /* then */ break;
      printf (">%d<",qqb16);
      MODES1=MODES1+1;
      if ((qqb16>255) && (qqb16<511)) /* then */ {
        mlbuf[MODES2]=qqb16 & 255;
        MODES2=MODES2+1;
      } /* endif */
    }; /* endwhile | loop */
    mlbuf[MODES2]=255; /* Terminate list */
    Eol();
   
    printf ("Modes : %d -> %d",MODES1,MODES2);
    if (MODES2==0) /* then */ {
      VBFAIL=1;
      printf (" - no usable mode candidates ");
      SmiEvil() ;
    } /* endif */
    Eol() ;
  } /* endif */

  if (VBFAIL==0) /* then */ {

    BRUTEFORCE=0 ; BFSUCC=0;
    qqc8=0;
    while (1) {
      WINNERBUF[qqc8]=0;
      qqc8=qqc8+1;
      if (qqc8==48) /* then */ break;
    } /* endwhile | loop */
    printf ("mode xx yy bpp bpsl $physlfb");
    Eol();
 
    while (1) {
      BFMODE=mlbuf[BRUTEFORCE];
      if (BFMODE==255) /* then */ break; /* The end */   
      Empty();
      memcpy((void *)(DosMem*16),dosmirbuf,1024);
      regs.h.ax = 0x4F01 ;     /* VESA */
      regs.h.cx = 256+BFMODE;  /* Mode */ 
      regs.h.flags = 0x72;
      regs.h.ss = 0;
      regs.h.sp = 0;
      regs.h.es = DosMem; /* ES:DI */
      regs.h.di = 0;
      dpmi_simulate_real_interrupt(0x10,&regs);
      qqb16 =  regs.h.ax ;
      if (qqb16!=0x004F) /* then */ {
        VBFAIL=1;
        printf ("Mode info call failed ");
        SmiEvil() ; Eol() ;
        break;
      } else {
        memcpy(dosmirbuf,(void *)(DosMem*16),256); /* $10,$12/13,$14/15,$19,$28 */
        BFBPSL = dosmirbuf[16]+dosmirbuf[17]*256;
        BFXX   = dosmirbuf[18]+dosmirbuf[19]*256;
        BFYY   = dosmirbuf[20]+dosmirbuf[21]*256;
        BFBPP  = dosmirbuf[25];
        memcpy(&BFPHYSLFB,&dosmirbuf[40],4);
        printf ("%d %d %d %d %d $%X ",BFMODE,BFXX,BFYY,BFBPP,BFBPSL,BFPHYSLFB);
        if ((BFXX==800) && (BFYY==600) && (BFBPP==24)) /* then */ {
          SmiGood();
          WINNERBUF[0] = BFBPP;
          WINNERBUF[1] = BFMODE;
          memcpy(&WINNERBUF[2] ,&BFBPSL,   2); 
          memcpy(&WINNERBUF[4] ,&BFPHYSLFB,4); 
          memcpy(&WINNERBUF[8] ,&BFXX,     2); 
          memcpy(&WINNERBUF[10],&BFYY,     2); 
          BFSUCC=BFSUCC+1;
        }; /* endif */
        if ((BFXX==800) && (BFYY==600) && (BFBPP==32)) /* then */ {
          SmiGood();
          WINNERBUF[12] = BFBPP;
          WINNERBUF[13] = BFMODE;
          memcpy(&WINNERBUF[14] ,&BFBPSL,   2); 
          memcpy(&WINNERBUF[16] ,&BFPHYSLFB,4); 
          memcpy(&WINNERBUF[20] ,&BFXX,     2); 
          memcpy(&WINNERBUF[22] ,&BFYY,     2); 
          BFSUCC=BFSUCC+1;
        }; /* endif */
        if ((BFXX==1024) && (BFYY==768) && (BFBPP==24)) /* then */ {
          SmiGood();
          WINNERBUF[24] = BFBPP;
          WINNERBUF[25] = BFMODE;
          memcpy(&WINNERBUF[26] ,&BFBPSL,   2); 
          memcpy(&WINNERBUF[28] ,&BFPHYSLFB,4); 
          memcpy(&WINNERBUF[32] ,&BFXX,     2); 
          memcpy(&WINNERBUF[34] ,&BFYY,     2); 
          BFSUCC=BFSUCC+1;
        }; /* endif */
        if ((BFXX==1024) && (BFYY==768) && (BFBPP==32)) /* then */ {
          SmiGood();
          WINNERBUF[36] = BFBPP;
          WINNERBUF[37] = BFMODE;
          memcpy(&WINNERBUF[38] ,&BFBPSL,   2); 
          memcpy(&WINNERBUF[40] ,&BFPHYSLFB,4); 
          memcpy(&WINNERBUF[44] ,&BFXX,     2); 
          memcpy(&WINNERBUF[46] ,&BFYY,     2); 
          BFSUCC=BFSUCC+1;
        }; /* endif */
        Eol();
      }; /* endif */
      BRUTEFORCE=BRUTEFORCE+1;
    } /* endwhile | loop */

    if (BFSUCC==0) /* then */ {
      printf ("No usable modes found ");
      SmiEvil();Eol();
      VBFAIL=1;
    }; /* endif */

  }; /* endif */

  if (VBFAIL==0) /* then */ {
   
    printf ("We have %d modes ",BFSUCC);
    SmiGood() ; Eol() ;
    MMODE0123=0 ;
    if                    (WINNERBUF[0] ==0)  /* then */ MMODE0123=1;
    if ((MMODE0123==1) && (WINNERBUF[12]==0)) /* then */ MMODE0123=2;
    if ((MMODE0123==2) && (WINNERBUF[24]==0)) /* then */ MMODE0123=3;
    if ((VBPREF32==1) && (MMODE0123==0) && (WINNERBUF[12]!=0)) /* then */ MMODE0123=1;
    if ((VBPREF32==1) && (MMODE0123==2) && (WINNERBUF[36]!=0)) /* then */ MMODE0123=3;
    if ((VBPREFHI==1) && (MMODE0123==0) && (WINNERBUF[24]!=0)) /* then */ MMODE0123=2;
    if ((VBPREFHI==1) && (MMODE0123==1) && (WINNERBUF[36]!=0)) /* then */ MMODE0123=3;
    MMODEV =           WINNERBUF[MMODE0123*12+1];
    MMBPP  =           WINNERBUF[MMODE0123*12];
    memcpy(&MMBPSL,   &WINNERBUF[MMODE0123*12+2] ,2); 
    memcpy(&MMPHYSLFB,&WINNERBUF[MMODE0123*12+4] ,4); 
    memcpy(&MMXX,     &WINNERBUF[MMODE0123*12+8] ,2); 
    memcpy(&MMYY,     &WINNERBUF[MMODE0123*12+10],2); 
    MMSIZE=MMBPSL*MMYY;
    printf ("Selected mode ");
    printf ("%d %d %d %d %d %d %d $%X",MMODE0123,MMODEV,MMXX,MMYY,MMBPP,MMBPSL,MMSIZE,MMPHYSLFB);
    Eol();
    printf ("Map now ... ");
    asm {
      push esi
      push edi
      push ebx
      mov  ax,0x0800
      mov  ebx,[MMPHYSLFB] /* BX:CX physical address of memory, BX is high */
      mov  ecx,ebx
      shr  ebx,16
      mov  esi,[MMSIZE] /* SI:DI size of region to map (bytes), SI is high */
      mov  edi,esi
      shr  esi,16
      int  0x31
      jnc  blah    /* OK */
      xor  ebx,ebx /* return 0 */
      xor  ecx,ecx
blah: shl  ebx,16
      mov  bx,cx   /* Now we have 32 bits in EBX */
      mov  [MMLIN],ebx
      pop  ebx
      pop  edi
      pop  esi
    } 
    if (MMLIN>1048576) /* then */ {
      printf ("Done: $%X -> $%X ... p.a.k. (ESC to abort) ",MMPHYSLFB,MMLIN); 
      SmiGood();
      Eol();
      while (!kbhit());
      qqc8=getch();
      Eol();
      if (qqc8==27) /* then */ VBFAIL=1;
    } else {
      printf ("Mapping failed - Vi$ta/XP NTVDM crap rulez ");
      SmiEvil() ; Eol() ;
      VBFAIL=1 ;
    } /* endif */
  }; /* endif */

  if (VBFAIL==0) /* then */ {
 
    regs.h.ax = 0x4F02 ;             /* VESA set */
    regs.h.bx = 0x4100 + MMODEV ;    /* LFB Mode */ 
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    dpmi_simulate_real_interrupt(0x10,&regs);
    qqb16 =  regs.h.ax ;
    if (qqb16!=0x004F) /* then */ {
      VBFAIL=1;
      printf ("Mode set failed for no acceptable reason ");
      SmiEvil();Eol();
    }; /* endif */

  }; /* endif */

  if (VBFAIL==0) /* then */ {

      /* have it !!! */
      PLOTXX=0 ; PLOTYY=0 ;
      while (1) {
        qqa32=(MMBPP/8)*PLOTXX+PLOTYY*MMBPSL ;
        vidbuf[qqa32  ] = COLORBUF[((UINT32)768 *PLOTXX/MMXX+256*PLOTYY/MMYY)];
        vidbuf[qqa32+1] = COLORBUF[((UINT32)512 *PLOTXX/MMXX+512*PLOTYY/MMYY)];
        vidbuf[qqa32+2] = COLORBUF[((UINT32)256 *PLOTXX/MMXX+768*PLOTYY/MMYY)];
        PLOTXX=PLOTXX+1 ;
        if (PLOTXX==MMXX) /* then */ {
          PLOTXX=0 ; PLOTYY=PLOTYY+1 ;
        } /* endif */
        if (PLOTYY==MMYY) /* then */ break;
      }
      memcpy ((void *)(MMLIN),vidbuf,MMSIZE);
      while (!kbhit());
      getch();

      regs.h.ax    = 3    ; /* Restore text mode */
      regs.h.flags = 0x72 ;
      regs.h.ss    = 0 ;
      regs.h.sp    = 0 ;
      dpmi_simulate_real_interrupt(0x10,&regs);

      printf ("Unrecoverable end of fun occurred here ") ;
      SmiGood() ; Eol() ; Eol();     
      printf ("Commandline parameters: PREF32 (bits) PREFHI (1024x768)");
      Eol() ; Eol();
      printf ("(CL) 2007-12-26 | ABUSE at your own RISK !!!") ;
      Eol() ;
 
  }; /* endif */

  regs.h.ax    = 0x4900 ; /* Free memory */
  regs.h.flags = 0x72 ;
  regs.h.ss = 0;
  regs.h.sp = 0;
  regs.h.es = DosMem;
  dpmi_simulate_real_interrupt(0x21,&regs); /* Don't care about errors */

  return 0;

} /* endmain */
