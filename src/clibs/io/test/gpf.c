#include <stdio.h>

main()
{
	char buf[3003] ;
	FILE *in = fopen("input","r") ;
	FILE *out = fopen("cp","w") ;
	FILE *out2 = fopen("cpb","wb") ;
	
	while (1) {
		int l = fread(buf,1,3003,in) ;
		printf(":%d\n",l) ;
		if (l == 0)
			break ;
		printf("%d\n",fwrite(buf,1,l,out)) ;
		fwrite(buf,1,l,out2) ;
	}
	fclose(in) ;
	fclose(out) ;
	fclose(out2) ;
	
	in = fopen("input","rb") ;
	out = fopen("bcp","w") ;
	out2 = fopen("bcpb","wb") ;
	while (1) {
		int l = fread(buf,1,3003,in) ;
		if (l == 0)
			break ;
		fwrite(buf,1,l,out) ;
		fwrite(buf,1,l,out2) ;
	}
	fclose(in) ;
	fclose(out) ;
	fclose(out2) ;

	in = fopen("cpb","rb") ;
	out = fopen("wcp","w") ;
	out2 = fopen("wcpb","wb") ;
	while (1) {
		int l = fread(buf,1,3003,in) ;
		if (l == 0)
			break ;
		fwrite(buf,1,l,out) ;
		fwrite(buf,1,l,out2) ;
	}
	fclose(in) ;
	fclose(out) ;
	fclose(out2) ;
}