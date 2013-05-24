/*
 * this example uses DPMI to initialize Mode 13h video mode and 
 * draws three lines on the screen in different colors
 */
#include <conio.h>
#include <dpmi.h>

#define PIXEL(x,y) (videobase + (x) + (y) *320)
/* this will point to the text area of video memory */
char * videobase;

void GetVidBase(void)
{
  ULONG base;
  SELECTOR temp;
  /* get the data segment selector */
  asm mov [temp],ds
  /* now get its base address */
  dpmi_get_sel_base(&base,temp);
  /* video base is address zero + 0xa0000 for VGA graphics area */
  videobase = (char *)(-base + 0xa0000);

    /* notice we do NOT change the DS selector limit.  The standard segments
   * are currently expand-down and changing the limit is not an option
   */
}
void ModeSet(int mode)
{
  DPMI_REGS regs;
  regs.b.ah = 0x0;
  regs.b.al = mode;
  regs.h.flags = 0x72;
  regs.h.ss = 0;
  regs.h.sp = 0;
  dpmi_simulate_real_interrupt(0x10,&regs);
}
main()
{
  int i;
  /* get a base pointer to video mem */
  GetVidBase();

  /* 320x200x16 graphics mode */
  ModeSet(0x13);

  /* now put a yellow line diagonally on the screen */
  for (i=0; i < 100; i++)
    *PIXEL(i,i) = YELLOW;
  /* now put a cyan line horizontally on the screen */
  for (i=0; i < 100; i++)
    *PIXEL(i,100) = LIGHTCYAN;
  /* now put a white line vertically on the screen */
  for (i=0; i < 100; i++)
    *PIXEL(100,i) = WHITE;

  /* wait for a key to be hit */
  while (!kbhit());
  getch();

  /* text mode */
  ModeSet(3);
}