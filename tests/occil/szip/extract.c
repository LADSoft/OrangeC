/* extract.c - extracts buggy datablocks for szip 1.10 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifndef unix
#include <io.h>
#include <fcntl.h>
#endif

void usage()
{	fprintf(stderr,"extract [blocksize] originalfile corruptfile logfile [errorblockfile]\n");
    fprintf(stderr,"the blocksize must be the same as used with szip (example: -b1)\n");
	exit(2);
}

int readnum(char *s, int min, int max)
{	int j=0;
	while (isdigit(*s))
	{	j=10*j+*s-'0';
		s++;
	}
	if (j<min || j>max)
		usage();
	return j;
}


void bugfound(unsigned char *buffer, size_t bufsize, char *logfilename,
char *blockfilename)
{   FILE *blockfil;
    size_t writebytes;
    blockfil = fopen( logfilename, "ab" );
    fprintf(blockfil,"%s\n",blockfilename);
    fclose(blockfil);
    blockfil = fopen( blockfilename, "wb" );
#ifndef unix
    setmode( fileno( blockfil ), O_BINARY );
#endif
    writebytes = fwrite( (char *)buffer, 1, (size_t)bufsize, blockfil);
    fclose(blockfil);
    if (writebytes==bufsize)
        exit(1);
    else
        exit(2);
}


size_t blocksize=1703936;

int main( int argc, char *argv[] )
{   unsigned char *infilename=NULL, *outfilename=NULL,
        *blockfilename=NULL, *logfilename=NULL, *buffer;
    FILE *infil, *outfil;
    size_t i, bufsize;

    for (i=1; i<(unsigned)argc; i++)
	{	char *s=argv[i];
	    if (*s == '-')
		{	s++;
			if (*s=='b')
                blocksize = (100000*readnum(s+1,1,41)+0x7fff) & 0x7fff8000L;
            else
                usage();
        }
		else if (infilename == NULL)
			infilename = s;
		else if (outfilename == NULL)
			outfilename = s;
		else if (logfilename == NULL)
			logfilename = s;
		else if (blockfilename == NULL)
			blockfilename = s;
		else
			usage();
	}
    if (logfilename==NULL)
        usage();

    infil = fopen( infilename, "rb" );
    outfil = fopen( outfilename, "rb" );

#ifndef unix
    setmode( fileno( infil ), O_BINARY );
    setmode( fileno( outfil ), O_BINARY );
#endif

    buffer = (unsigned char*) malloc(blocksize);
    do
    {   bufsize = fread( (char *)buffer, 1, (size_t)blocksize, infil);
        for (i=0; i<bufsize; i++)
            if (buffer[i] != getc(outfil))
            {   fclose(infil);
                fclose(outfil);
                bugfound(buffer,bufsize,logfilename,blockfilename);
            }
    } while (bufsize==blocksize);
    fclose(infil);
    fclose(outfil);
    return 0;
}
