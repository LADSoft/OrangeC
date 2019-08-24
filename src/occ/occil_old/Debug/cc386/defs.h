/* defs.h   */

#include    <stdio.h>

/* assert(): Enable one of these three. */
/* Note: This source doesn't use #elif directive to test preprocessor which
        can't recognize the directive.  */
#if     1   /* For the translator which can process <assert.h> properly.    */
#include    <assert.h>
#else
#if     0   /* Not to abort on error.   */
#define     assert( exp)    (exp) ? (void)0 : (void) fprintf( stderr,   \
        "Assertion failed: %s, from line %d of file %s\n",  \
        # exp, __LINE__, __FILE__)
#endif
#endif

extern int      strcmp( const char *, const char *);
extern size_t   strlen( const char *);
extern void     exit( int);

