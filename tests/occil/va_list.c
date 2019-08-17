// Example from: 
// https://en.wikibooks.org/wiki/C%2B%2B_Programming/Code/Standard_C_Library/Functions/va_arg

#include <stdio.h>
#include <stdarg.h>

void my_printf( char *format, ... ) {
  va_list argptr;             

  va_start( argptr, format );          

  while( *format != '\0' ) {
    // string
    if( *format == 's' ) {
      char* s = va_arg( argptr, char * );
      printf( "Printing a string: %s\n", s );
    }
    // character
    else if( *format == 'c' ) {
      char c = (char) va_arg( argptr, int );
      printf( "Printing a character: %c\n", c );
      break;
    }
    // integer
    else if( *format == 'd' ) {
      int d = va_arg( argptr, int );
      printf( "Printing an integer: %d\n", d );
    }          

    *format++;
  }            

  va_end( argptr );
}    

int main( void ) {             

  my_printf( "sdc", "This is a string", 29, 'X' );         
  return( 0 );
}