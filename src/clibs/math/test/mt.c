#include <math.h>
#include <stdio.h>

main()
{
	double cst = .124;
	double a;
	a = 0.9999999999;
	printf("%f\n",a);
	a = 9.9999999999;
	printf("%f\n",a);
	a = 0.00432156;
	printf("%f\n",a);
	printf("%e\n",a);
	printf("%f\n",sin(M_PI));
	printf("%f\n",cos(M_PI));
	printf("%f\n",tan(M_PI));
	printf("%f\n",sin(M_PI/3));
	printf("%f\n",cos(M_PI/3));
	printf("%f\n",tan(M_PI/3));
	printf("%f\n",asin(sin(M_PI/3)));
	printf("%f\n",acos(cos(M_PI/3)));
	printf("%f\n",atan(tan(M_PI/3)));
	
	printf("\n%f\n",exp(1.0));
	printf("%f\n",exp(2.0));
	printf("%f\n",pow(5.0,3.0));
	printf("%f\n",pow10(5));

	printf("\n%f\n",log(M_E*M_E));
	printf("%f\n",log10(100000));
	printf("%f\n",sqrt(64));

	printf("\n%f\n",floor(64.2222));
	printf("%f\n",floor(64.6222));
	printf("%f\n",floor(-64.2222));
	printf("%f\n",floor(-64.6222));
	printf("%f\n",ceil(64.2222));
	printf("%f\n",ceil(64.6222));
	printf("%f\n",ceil(-64.2222));
	printf("%f\n",ceil(-64.6222));

	printf("\n%f %f\n",cosh(cst),(exp(cst)+exp(-cst))/2);
	printf("%f %f\n",sinh(cst),(exp(cst)-exp(-cst))/2);
	printf("%f %f\n",tanh(cst),(exp(cst)-exp(-cst))/(exp(cst)+exp(-cst)));

	printf("\n%f\n",fabs(-4.23));	
	printf("%f\n",fabs(4.23));	

	printf("\n%f\n",atan2(tan(M_PI/3),1));
  {
		int resi;
		double resf = frexp(29.775e42, &resi);
		printf("\n%f %d\n",resf,resi);	
		printf("%f\n",ldexp(resf,resi));
	}
  {
		double resf1;
		double resf = modf(29.775, &resf1);
		printf("\n%f %f\n",resf,resf1);	
	}
	printf("%f\n",fmod(29.775,10));
	printf("\n%f\n",hypot(3,4));
}