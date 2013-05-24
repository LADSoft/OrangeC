#include "afx.h"


void parse_params( char *env, char *cfgfile, char *argv[], int argc ) {
    int i;

    if ( env ) {
        char *val = getenv( env );
        if ( val )
            parse_param_line( val );
    }
    if ( cfgfile ) {
        char *cfg = xfind( cfgfile, getrundir());
#if DEBUG
        printf( "*** ConfigFile '%s'\n", cfg );
#endif
        parse_param_file( cfg );
    }
    for (i = 1; i < argc; i++)
        parse_param( argv[i] );
}


void parse_param_line( char *s ) {
    static char a[ LINE_MAX ];
    int i;

    assert( s != NULL );
    while(1) {
        while ( *s && isspace(*s) )	/* skip spaces */
            s++;
        if ( !*s )	/* end of line */
            return;
        if ( *s == '\"') {	/* " */
            s++;
            i = 0;
            while ( *s && *s != '\"' )
                a[i++] = *s, s++;
            a[i] = 0;
            parse_param(a);
            if (!*s)
                return;
            s++;
        } else {
            i = 0;
            while ( *s && !isspace(*s))
                a[i++] = *s, s++;
            a[i] = 0;
            parse_param(a);
        }
    }
}

void scan_env(char *output,char *string)
{
   char name[256],*p ;
   while (*string) {
      if (*string == '%') {
         p = name ;
         string++ ;
         while (*string && *string != '%')
            *p++ = *string++ ;
         if (*string)
            string++ ;
         *p = 0 ;
         p = getenv(name) ;
         if (p) {
            strcpy(output,p) ;
            output += strlen(output) ;
         }

      } else
         *output++ = *string++ ;
   }
   *output = 0 ;
}

void parse_param_file( char *file ) {
   static char line1[ LINE_MAX ];
   static char line[ LINE_MAX ] ;
    FILE *fp;
    assert( file != NULL );
   fp = fopen( file, "rt" );
    if (fp) {
        while( fgets( line, LINE_MAX-1, fp ) ) {
            int len = strlen( line );
            if ( len > 0 && line[len-1] == '\n' )
                line[len-1] = '\0';
         scan_env(line1, line) ;
         parse_param_line( line1 );
        }
        fclose( fp );
    } else {
        perror( file );
    }
}


void parse_param( char *param ) {
    assert( param != NULL );
    if ( param[0] == '@' ) {
        parse_param_file( param+1 );
    } else if ( param[0] == '-' || param[0] == '/' || param[0] == '+' ) {
        option( param );
    } else {
        source( param );
    }
}