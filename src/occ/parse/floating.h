#include <limits.h>
#ifndef M_LN2
#define M_LN2       0.693147180559945309417
#endif
#ifndef M_LN10
#define M_LN10      2.30258509299404568402
#endif
#define INTERNAL_FPF_PRECISION ((80/8)/sizeof(uf16))
#ifdef USE_LONGLONG
#ifdef BORLAND
    #define ULLONG_TYPE unsigned __int64
    #define LLONG_TYPE __int64
    #define LLONG_MIN _I64_MIN
    #define LLONG_MAX _I64_MAX
    #define ULLONG_MAX ((_I64_MAX << 1) + 1)
#else
    #define ULLONG_TYPE unsigned long long
    #define LLONG_TYPE long long
#endif
#else 
    #define ULLONG_TYPE unsigned long 
    #define LLONG_TYPE long
    #define ULLONG_MAX ULONG_MAX
#endif 
/*
** DEFINES
*/
#define uf8 unsigned char
#define uf16 unsigned short
#define uf32 unsigned long
#define uchar unsigned char
#define ulong unsigned long

#define MAX_EXP SHRT_MAX
#define MIN_EXP SHRT_MIN

#define IFPF_IS_ZERO 0
#define IFPF_IS_SUBNORMAL 1
#define IFPF_IS_NORMAL 2
#define IFPF_IS_INFINITY 3
#define IFPF_IS_NAN 4
#define IFPF_TYPE_COUNT 5

#define ZERO_ZERO                       0
#define ZERO_SUBNORMAL                  1
#define ZERO_NORMAL                     2
#define ZERO_INFINITY                   3
#define ZERO_NAN                        4

#define SUBNORMAL_ZERO                  5
#define SUBNORMAL_SUBNORMAL             6
#define SUBNORMAL_NORMAL                7
#define SUBNORMAL_INFINITY              8
#define SUBNORMAL_NAN                   9

#define NORMAL_ZERO                     10
#define NORMAL_SUBNORMAL                11
#define NORMAL_NORMAL                   12
#define NORMAL_INFINITY                 13
#define NORMAL_NAN                      14

#define INFINITY_ZERO                   15
#define INFINITY_SUBNORMAL              16
#define INFINITY_NORMAL                 17
#define INFINITY_INFINITY               18
#define INFINITY_NAN                    19

#define NAN_ZERO                        20
#define NAN_SUBNORMAL                   21
#define NAN_NORMAL                      22
#define NAN_INFINITY                    23
#define NAN_NAN                         24
#define OPERAND_ZERO                    0
#define OPERAND_SUBNORMAL               1
#define OPERAND_NORMAL                  2
#define OPERAND_INFINITY                3
#define OPERAND_NAN                     4

/*
** Following already defined in NMGLOBAL.H
**
#define _FPF_PRECISION 4
*/

/*
** TYPEDEFS
*/

typedef struct
{
        uf8 type;        /* Indicates, NORMAL, SUBNORMAL, etc. */
        uf8 sign;        /* Mantissa sign */
        long exp;      /* Signed exponent...no bias */
        uf16 mantissa[INTERNAL_FPF_PRECISION];
} FPF;

/*
** PROTOTYPES
*/
#ifdef __cplusplus
extern "C" {
#endif
#ifdef XXXXX
void SetupCPUEmFloatArrays(FPF *abase,
        FPF *bbase, FPF *cbase, ulong arraysize);
ulong DoEmFloatIteration(FPF *abase,
        FPF *bbase, FPF *cbase,
        ulong arraysize, ulong loops);
#endif
void SetFPFZero(FPF *dest,
                        uchar sign);
void SetFPFInfinity(FPF *dest,
                        uchar sign);
void SetFPFNaN(FPF *dest);
int IsMantissaZero(uf16 *mant);
int IsMantissaOne(uf16 *mant);
void Add16Bits(uf16 *carry,uf16 *a,uf16 b,uf16 c);
void Sub16Bits(uf16 *borrow,uf16 *a,uf16 b,uf16 c);
void ShiftMantLeft1(uf16 *carry,uf16 *mantissa);
void ShiftMantRight1(uf16 *carry,uf16 *mantissa);
void StickyShiftRightMant(FPF *ptr,int amount);
void normalize(FPF *ptr);
void denormalize(FPF *ptr,int minimum_exponent);
void RoundFPF(FPF *ptr);
void choose_nan(FPF *x,FPF *y,FPF *z,
                int intel_flag);
void AddSubFPF(uchar operation,FPF *x,
                FPF *y,FPF *z);
void MultiplyFPF(FPF *x,FPF *y,
                        FPF *z);
void DivideFPF(FPF *x,FPF *y, 
                        FPF *z);
void LongToFPF(long mylong,
                FPF *dest);
int FPFTensExponent(FPF *value);
void FPFMultiplyPowTen(FPF *value, int power);
char * FPFToString(char *dest,
                FPF *src);
void LongLongToFPF(FPF *value, LLONG_TYPE i);
void UnsignedLongLongToFPF(FPF *value, LLONG_TYPE i);
int FPFToLongDouble(unsigned char *dest, FPF *src);
void FPFTruncate(FPF *value, int bits, int maxexp, int minexp);
int LongDoubleToFPF(FPF *dest, unsigned char *src);
#ifdef __cplusplus
}
#endif
