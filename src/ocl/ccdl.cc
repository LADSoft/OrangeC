#ifdef XXXXX
#include "llp.h"
/* Symbolic constants for the access() function */

#define R_OK    4       /*  Test for read permission    */
#define W_OK    2       /*  Test for write permission   */
#define X_OK    1       /*  Test for execute permission */
#define F_OK    0       /*  Test for existence of file  */


int access(char *fname, int mode ) {
    DPMI_REGS regs;

	regs.h.dx = __nametodta(fname,0);
	regs.d.eax = 0;
	__doscall(0x43, &regs);
    if ( regs.h.flags & 1 ) /* CF == 1 */ {
    	return -1;
    }
    if (mode == W_OK && (regs.b.cl & 1) )
    	return -1;
	return 0;    
}

/*
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *%	  Copyright (C) 1988, by WATCOM Systems Inc. All rights     %
 *%	  reserved. No part of this software may be reproduced	    %
 *%	  in any form or by any means - graphic, electronic or	    %
 *%	  mechanical, including photocopying, recording, taping     %
 *%	  or information storage and retrieval systems - except     %
 *%	  with the written permission of WATCOM Systems Inc.	    %
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  Modified:	By:		Reason:
  ---------	---		-------
  23-nov-87	F.W.Crigger	Initial implementation
  19-may-88	F.W.Crigger	If name found in current directory, use getcwd
  25-may-88	F.W.Crigger	save and restore errno if we find the file.
  30-mar-90	J.B.Schueler	added PQNX support
  30-mar-90	J.B.Schueler	prevent searching root when PATH=path1;;path3
  30-mar-90	J.B.Schueler	prevent applying CWD when path already present
  14-dec-90	J.B.Schueler	QNX version has no direct.h (in unistd.h)
--------------------------------------------------------Version-11.0---------
  22-may-95	Greg Bentz	variety.h and dll linkage
  26-sep-95	M. Hildebrand	Added _wsearchenv.
*/
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <dir.h>
#include <string.h>

#define PATH_SEPARATOR '\\'
#define LIST_SEPARATOR ';'


void _searchenv( char *name, char *env_var, char *buffer ) {
	char *p, *p2;

	if( access( name, 0 ) == 0 ) {
	    p = buffer; 				/* JBS 90/3/30 */
	    for( ;; ) {
			if( name[0] == PATH_SEPARATOR ) break;
			if( name[0] == '.' ) break;
			getcwd( buffer, _MAX_PATH );
			p = &buffer[ strlen( buffer ) ];
			if( p[-1] != PATH_SEPARATOR ) *p++ = PATH_SEPARATOR;
			break;
		}
		strcpy( p, name );
		return;
	}
	p = getenv( env_var );
	if( p != NULL ) {
	    for(;;) {
		if( *p == '\0') break;
		p2 = buffer;
		while( *p ) {
		    if( *p == LIST_SEPARATOR ) break;
		    *p2++ = *p++;
		}
		/* check for zero-length prefix which represents CWD */
		if( p2 != buffer ) {			/* JBS 90/3/30 */
		    if( p2[-1] != PATH_SEPARATOR) {
			*p2++ = PATH_SEPARATOR;
		    }
		    *p2 = '\0';
		    strcat( p2, name );
		    /* check to see if file exists */
		    if( access( buffer, 0 ) == 0 ) {
				return;
		    }
		}
		if( *p == '\0' ) break;
		++p;
	    }
	}
	buffer[0] = '\0';
}
#endif