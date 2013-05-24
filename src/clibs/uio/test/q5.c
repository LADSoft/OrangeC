#include <sys\stat.h>
#include <fcntl.h>
char buf[10000] ;
main()
{
   int handin,handout,a ;
   handin = open("decl.c",O_TEXT) ;
   _fmode = O_BINARY ;
   handout = creat("temp",S_IWRITE) ;
   printf("%d",handout) ;
   do {
      a = read(handin,buf,10000) ;
      if (a > 0)
         write(handout,buf,a) ;
   } while (a > 0) ;
   close(handin) ;
   close(handout) ;
}