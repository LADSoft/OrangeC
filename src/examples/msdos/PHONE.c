/*
 * This example shows how to use i86.h facilities fo play sounds.
 */
#include <i86.h>

main()
{
   int i,j,k ;
   for (i=0; i < 2; i++) {
      for (j=0; j <2; j++)
         for (k=0; k < 6; k++) {
            _sound(597) ;
            _delay(55) ;
            _sound(745) ;
            _delay(55) ;
         }
      _nosound() ;
      _delay(1800) ;
   }
            
}