#include <stdio.h>

main()
{
	char buf[256] ;
	FILE *in = fopen("input","r") ;
	FILE *out = fopen("cp","w") ;
	FILE *out2 = fopen("cpb","wb") ;
	
	while (1) {
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