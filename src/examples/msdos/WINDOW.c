/*
 * this example draws a window on the screen and puts a message in it
 *
 * then when a key is pressed it restores the screen
 *
 */
#include <conio.h>

main()
{
  unsigned char buf[30*10*2];

  /* get position */
  int x = wherex();
  int y = wherey();

  /* get what is on the screen */
  gettext(10,10,40,20,buf);

  /* make a small window */
  window(10,10,40,20);

  /* color is yellow on blue */
  textattr(YELLOW + (BLUE << 4));
  clrscr();

  /* draw a message */
  cprintf("Hello dave");

  /* wait for a key to be hit */
  while (!kbhit());
  getch();

  /* restore the original window for the puttext */
  window(0,0,25,80);

  /* now restore the screen the way it was */
  puttext(10,10,40,20,buf);

  /* put the cursor back */
  gotoxy(x,y);
}