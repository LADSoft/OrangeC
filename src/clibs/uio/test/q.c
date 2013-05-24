#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <errno.h>

extern _uihandles[20] ;
char name[7] ;
int time = 0x00050048 ;
int main()
{                     
   char buf[10];
   int len,i ;  
   int hand = creat("hi",S_IREAD),hand1 ;
   write(hand,"10\n",3) ;
   close(hand) ;
   hand = creat("hi1",S_IWRITE) ;
   write(hand,"10\n",3) ;
   chsize(hand,44) ;
   printf("filelen: %d\n",filelength(hand)) ;
   close(hand) ;
   hand = open("hi",O_RDONLY|O_BINARY) ;
   printf("eof: %d\n",eof(hand)) ;
   len = read(hand,buf,10) ;
   buf[len] = 0 ;
   printf("%d: %s\n",len,buf) ;
   printf("eof: %d\n",eof(hand)) ;
   close (hand) ;
   hand = creat("hi2",S_IWRITE) ;
   write(hand,"hi",2) ;
   printf("tell: %d\n",tell(hand)) ;
   printf("isatty: %d %d\n",isatty(0),isatty(hand));
   close(hand) ;
   for (i=0; i < 4; i++) {
      sprintf(name,"hiXXXXXX") ;
      mktemp(name) ;
      close(creat(name,S_IWRITE)) ;
   }
   hand = open("hi1",0) ;
   printf("hand: %d",hand) ;
   setftime(hand,&time) ;
   getftime(hand,&time) ;
   printf("%x\n",time) ;
   printf("%d ",setmode(hand,O_BINARY)) ;
   printf("%d ",setmode(hand,O_TEXT)) ;
   printf("%d ",setmode(hand,O_BINARY)) ;
   printf("%d ",setmode(hand,O_BINARY | O_TEXT)) ;
   printf("%d\n",setmode(hand,O_TEXT)) ;
   printf("%d ",lseek(hand,10,SEEK_SET)) ;
   printf("%d ",lseek(hand,10,SEEK_CUR)) ;
   printf("%d ",lseek(hand,-5,SEEK_END)) ;
   printf("%d\n",tell(hand)) ;
   dup2(hand,17) ;
   close(hand) ;
   printf("%d ",lseek(17,-10,SEEK_CUR)) ;
   close(17) ;
}