/*
 * this example uses DPMI to follow the (text mode) mouse around 
 * on the screen
 */
#include <conio.h>
#include <dpmi.h>

/*
 * main routine to call real mode mouse routine
 */
void MouseInt(DPMI_REGS *regs)
{
  regs->h.flags = 0x72;
  regs->h.ss = 0;
  regs->h.sp = 0;
  dpmi_simulate_real_interrupt(0x33,regs);
}
/* show mouse */
void MouseOn(void)
{
  DPMI_REGS regs;
  regs.h.ax = 1;
  MouseInt(&regs);
}
/* hide mouse */
void MouseOff(void)
{
  DPMI_REGS regs;
  regs.h.ax = 2;
  MouseInt(&regs);
}
/* initialize mouse */
void MouseInit(void)
{
  DPMI_REGS regs;
  regs.h.ax = 0;
  MouseInt(&regs);
}
/* get mouse position and button status */
void MousePos(int *x, int *y, int *buttons)
{
  DPMI_REGS regs;
  regs.h.ax = 3;
  MouseInt(&regs);
  *x = regs.h.cx;
  *y = regs.h.dx;
  *buttons = regs.h.bx & 3;
}
main()
{
  int x,y,buttons;

  /* init the mouse and clear the screen */
  MouseInit();
  clrscr();
  MouseOn();
  while (!kbhit()) {
    /* get mouse pos */
    MousePos(&x,&y,&buttons);

    /* adjust for crazy mouse coords */
    x = x / 8;
    y = y / 8;

    /* display the info */
    gotoxy(0,0);
    cprintf("X: %03d, Y: %03d, buttons: %d",x,y,buttons);
  }
  /* at the end reinitialize the mouse again */
  MouseInit();
}