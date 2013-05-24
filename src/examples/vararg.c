/*
 * this example shows how to use variable arguments to create a
 * wrapper around the colored conio functions.  It is a demonstration of
 * both variable arguments and simple text coloring
 *
 * note that the CONIO.H implementation in this compiler may NOT be
 * redirected, e.g. it is NOT stdout, it is something different entirely.
 *
 */
#include <stdio.h>
#include <stdarg.h>
#include <conio.h>
void ccprintf(int color, char *fmt, ... )
{
  char buf[512],buf2[512];
  va_list argptr;

  /* set the color */
  textcolor(color);

  /* stick the word hello in front of our format string */
  sprintf(buf,"\thello %s\n",fmt);

  /* now use variable args to print the specified args into the new format
   * string
   */
  va_start( argptr, fmt);
  vsprintf(buf2,buf,argptr);
  va_end(argptr);
  /*
   * now blit the string to the screen using the color-sensitive conio
   * print function
   */
  cprintf("%s",buf2);
}
main()
{
  ccprintf(LIGHTCYAN, "geez dave, %d",448);
}