#include <stdio.h>

main()
{
	FILE *in = fopen("input","r") ;
	FILE *out = fopen("cp","w") ;
	FILE *out2 = fopen("cpb","wb") ;

	setvbuf(in,0,_IONBF,0) ;
	setvbuf(out,0,_IONBF,0) ;
	setvbuf(out2,0,_IONBF,0) ;
	while (1) {
		int rv ;
		rv = fgetc(in) ;
		if (rv == EOF)
			break ;
		fputc(rv,out) ;
		fputc(rv,out2) ;
	}
	fclose(in) ;
	fclose(out) ;
	fclose(out2) ;
	
	in = fopen("input","rb") ;
	out = fopen("bcp","w") ;
	out2 = fopen("bcpb","wb") ;
	setvbuf(in,0,_IONBF,0) ;
	setvbuf(out,0,_IONBF,0) ;
	setvbuf(out2,0,_IONBF,0) ;
	while (1) {
		int rv = fgetc(in) ;
		if (rv == EOF)
			break ;
		fputc(rv,out) ;
		fputc(rv,out2) ;
	}
	fclose(in) ;
	fclose(out) ;
	fclose(out2) ;

	in = fopen("cpb","rb") ;
	out = fopen("wcp","w") ;
	out2 = fopen("wcpb","wb") ;
	setvbuf(in,0,_IONBF,0) ;
	setvbuf(out,0,_IONBF,0) ;
	setvbuf(out2,0,_IONBF,0) ;
	while (1) {
		int rv = fgetc(in) ;
		if (rv == EOF)
			break ;
		fputc(rv,out) ;
		fputc(rv,out2) ;
	}
	fclose(in) ;
	fclose(out) ;
	fclose(out2) ;
}