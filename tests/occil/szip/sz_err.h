// SZ error messages
#ifndef ERR_H
#define ERR_H

extern int data_error(int errnum);

//#ifdef DEBUG                // give last 8 bits too 
//#define sz_error(x) data_error(x)
//#else
//#define sz_error(x) data_error((x)&~0xff)
//#endif
#define sz_error(x) do{fprintf(stderr,"Error #%x\n",x); abort();}while(0)

// those are ok:
#define NOMEM			0x6500
#define SZ_NOMEM_HASH		0x6502
#define SZ_NOMEM_SORT		0x6503

// those are a bug:
#define UNEXPECTED		0x6600
#define SZ_NOTCYCLIC		0x6601
#define SZ_NOTFOUND			0x6602
#define SZ_NOTIMPLEMENTED	0x6603
#define SZ_DOUBLEINDIRECT   0x6604
#define AR_OUTSTANDING		0x6605

#endif
