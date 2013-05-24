main()
{
   int a ;
   char buf[10 ];
   int hand = open("hi1",0) ;
   a = read(hand,buf,10) ;
   printf("%d",a) ;

}