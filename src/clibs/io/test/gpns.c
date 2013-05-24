#include <stdio.h>

main()
{
	char buf[256] ;
	FILE *in = fopen("input","r") ;
	FILE *out = fopen("cp","w") ;
	FILE *out2 = fopen("cpb","wb") ;
	
	setvbuf(in,0,_IONBF,0) ;
	setvbuf(out,0,_IONBF,0) ;
	setvbuf(out2,0,_IONBF,0) ;
	while (1) {
//		asm int 3 ;
		if (fgets(buf,256,in) != buf)
			break ;
		fputs(buf,out) ;
		fputs(buf,out2) ;
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
		if (fgets(buf,256,in) != buf)
			break ;
		fputs(buf,out) ;
		fputs(buf,out2) ;
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
		if (fgets(buf,256,in) != buf)
			break ;
		fputs(buf,out) ;
		fputs(buf,out2) ;
	}
	fclose(in) ;
	fclose(out) ;
	fclose(out2) ;
}