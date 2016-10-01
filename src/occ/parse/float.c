#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
//#define USE_LONGLONG
#define TRUE 1
#define FALSE 0
#include "floating.h"
void diag(char *s);
#if defined(TEST) || defined(LIBFLOAT)
#define diag(s) puts(s)
#endif
//#undef USE_LONGLONG

static const int BigEndian = 0;
static FPF tensTab[10];
/*
** emfloat.c
** BYTEmark (tm)
** BYTE's Native Mode Benchmarks
** Rick Grehan, BYTE Magazine.
*/

static void FPFInit(void)
{
    int i;
    tensTab[0].mantissa[0] = 0xa000;
    for (i=1; i < INTERNAL_FPF_PRECISION; i++)
        tensTab[0].mantissa[i] = 0;
    tensTab[0].exp = 4;
    tensTab[0].type = IFPF_IS_NORMAL;
    for (i=1; i < 10 ;i++)
    {
        MultiplyFPF(&tensTab[0], &tensTab[i-1], &tensTab[i]);
    }
}
/***********************
** SetFPFZero **
************************
** Set an internal floating-point-format number to zero.
** sign determines the sign of the zero.
*/
void SetFPFZero(FPF *dest,
                        uchar sign)
{
int i;          /* Index */

dest->type=IFPF_IS_ZERO;
dest->sign=sign;
dest->exp=MIN_EXP;
for(i=0;i<INTERNAL_FPF_PRECISION;i++)
        dest->mantissa[i]=0;
return;
}

/***************************
** SetFPFInfinity **
****************************
** Set an internal floating-point-format number to infinity.
** This can happen if the exponent exceeds MAX_EXP.
** As above, sign picks the sign of infinity.
*/
void SetFPFInfinity(FPF *dest,
                        uchar sign)
{
int i;          /* Index */

dest->type=IFPF_IS_INFINITY;
dest->sign=sign;
dest->exp=MIN_EXP;
for(i=0;i<INTERNAL_FPF_PRECISION;i++)
        dest->mantissa[i]=0;
return;
}

/**********************
** SetFPFNaN **
***********************
** Set an internal floating-point-format number to Nan
** (not a number).  Note that we "emulate" an 80x87 as far
** as the mantissa bits go.
*/
void SetFPFNaN(FPF *dest)
{
int i;          /* Index */

dest->type=IFPF_IS_NAN;
dest->exp=MAX_EXP;
dest->sign=1;
dest->mantissa[0]=0x4000;
for(i=1;i<INTERNAL_FPF_PRECISION;i++)
        dest->mantissa[i]=0;

return;
}

int ValueIsOne(FPF *value)
{
    if (value->type != IFPF_IS_NORMAL)
        return 0;
    if (value->exp != 1)
        return 0;
    return IsMantissaOne(value->mantissa);
}
int FPFEQ(FPF *left, FPF *right)
{
    if (left->type != right->type)
        return 0;
    if (left->type == IFPF_IS_ZERO)
        return 1;
    if (left->sign != right->sign)
        return 0;
    if (left->type == IFPF_IS_INFINITY)
        return 1;
    if (left->type == IFPF_IS_NAN)
        return 0;
    if (left->exp != right->exp)
        return 0;
    return !memcmp(left->mantissa, right->mantissa, sizeof(right->mantissa));
}
int FPFGT(FPF *left, FPF *right)
{
    if (left->type == IFPF_IS_NAN || right->type == IFPF_IS_NAN)
        return 0;
    if (left->type == IFPF_IS_ZERO && right->type == IFPF_IS_ZERO)
        return 0;
    if (left->sign && !right->sign)
        return 0;
    if (!left->sign && right->sign)
        return 1;
    if (right->type == IFPF_IS_INFINITY && left->type == IFPF_IS_INFINITY)
        return 0;
    if (left->sign) {
        if (right->type == IFPF_IS_INFINITY)
            return 1;
        if (left->exp < right->exp)
            return 1;
        if (left->exp > right->exp)
            return 0;
        return (memcmp(left->mantissa, right->mantissa, sizeof(left->mantissa)) < 0);
    }
    if (left->type == IFPF_IS_INFINITY)
        return 1;
    if (left->exp > right->exp)
        return 1;
    if (left->exp < right->exp)
        return 0;
    return (memcmp(left->mantissa, right->mantissa, sizeof(left->mantissa)) > 0);
}
int FPFGTE(FPF *left, FPF *right)
{
    return FPFGT(left,right) || FPFEQ(left,right);
}
/*******************
** IsMantissaZero **
********************
** Pass this routine a pointer to an internal floating point format
** number's mantissa.  It checks for an all-zero mantissa.
** Returns 0 if it is NOT all zeros, !=0 otherwise.
*/
int IsMantissaZero(uf16 *mant)
{
int i;          /* Index */
int n;          /* Return value */

n=0;
for(i=0;i<INTERNAL_FPF_PRECISION;i++)
        n|=mant[i];

return(!n);
}
int IsMantissaOne(uf16 *mant)
{
    int i;
    if (mant[0] != 0x8000)
        return 0;
    for (i=1; i < INTERNAL_FPF_PRECISION; i++)
        if (mant[i])
            return 0;
    return 1;
}

/**************
** Add16Bits **
***************
** Add b, c, and carry.  Retult in a.  New carry in carry.
*/
void Add16Bits(uf16 *carry,
                uf16 *a,
                uf16 b,
                uf16 c)
{
uf32 accum;              /* Accumulator */

/*
** Do the work in the 32-bit accumulator so we can return
** the carry.
*/
accum=(uf32)b;
accum+=(uf32)c;
accum+=(uf32)*carry;
*carry=(uf16)((accum & 0x00010000) ? 1 : 0);     /* New carry */
*a=(uf16)(accum & 0xFFFF);       /* Result is lo 16 bits */
return;
}

/**************
** Sub16Bits **
***************
** Additive inverse of above.
*/
void Sub16Bits(uf16 *borrow,
                uf16 *a,
                uf16 b,
                uf16 c)
{
uf32 accum;              /* Accumulator */

accum=(uf32)b;
accum-=(uf32)c;
accum-=(uf32)*borrow;
*borrow=(uf32)((accum & 0x00010000) ? 1 : 0);    /* New borrow */
*a=(uf16)(accum & 0xFFFF);
return;
}

/*******************
** ShiftMantLeft1 **
********************
** Shift a vector of 16-bit numbers left 1 bit.  Also provides
** a carry bit, which is shifted in at the beginning, and
** shifted out at the end.
*/
void ShiftMantLeft1(uf16 *carry,
                        uf16 *mantissa)
{
int i;          /* Index */
int new_carry;
uf16 accum;      /* Temporary holding placed */

for(i=INTERNAL_FPF_PRECISION-1;i>=0;i--)
{       accum=mantissa[i];
        new_carry=accum & 0x8000;       /* Get new carry */
        accum=accum<<1;                 /* Do the shift */
        if(*carry)
                accum|=1;               /* Insert previous carry */
        *carry=new_carry;
        mantissa[i]=accum;              /* Return shifted value */
}
return;
}

/********************
** ShiftMantRight1 **
*********************
** Shift a mantissa right by 1 bit.  Provides carry, as
** above
*/
void ShiftMantRight1(uf16 *carry,
                        uf16 *mantissa)
{
int i;          /* Index */
int new_carry;
uf16 accum;

for(i=0;i<INTERNAL_FPF_PRECISION;i++)
{       accum=mantissa[i];
        new_carry=accum & 1;            /* Get new carry */
        accum=accum>>1;
        if(*carry)
                accum|=0x8000;
        *carry=new_carry;
        mantissa[i]=accum;
}
return;
}


/*****************************
** StickyShiftMantRight **
******************************
** This is a shift right of the mantissa with a "sticky bit".
** I.E., if a carry of 1 is shifted out of the least significant
** bit, the least significant bit is set to 1.
*/
void StickyShiftRightMant(FPF *ptr,
                        int amount)
{
int i;          /* Index */
uf16 carry;      /* Self-explanatory */
uf16 *mantissa;

mantissa=ptr->mantissa;

if(ptr->type!=IFPF_IS_ZERO)     /* Don't bother shifting a zero */
{
        /*
        ** If the amount of shifting will shift everyting
        ** out of existence, then just clear the whole mantissa
        ** and set the lowmost bit to 1.
        */
        if(amount>=INTERNAL_FPF_PRECISION * 16)
        {
                for(i=0;i<INTERNAL_FPF_PRECISION-1;i++)
                        mantissa[i]=0;
                mantissa[INTERNAL_FPF_PRECISION-1]=1;
        }
        else
                for(i=0;i<amount;i++)
                {
                        carry=0;
                        ShiftMantRight1(&carry,mantissa);
                        if(carry)
                                mantissa[INTERNAL_FPF_PRECISION-1] |= 1;
                }
}
return;
}


/**************************************************
**         POST ARITHMETIC PROCESSING            **
**  (NORMALIZE, ROUND, OVERFLOW, AND UNDERFLOW)  **
**************************************************/

/**************
** normalize **
***************
** Normalize an internal-representation number.  Normalization
** discards empty most-significant bits.
*/
void normalize(FPF *ptr)
{
uf16     carry;

if (ptr->type == IFPF_IS_ZERO || ptr->type == IFPF_IS_INFINITY || ptr->type == IFPF_IS_NAN)
	return;
/*
** As long as there's a highmost 0 bit, shift the significand
** left 1 bit.  Each time you do this, though, you've
** gotta decrement the exponent.
*/
while ((ptr->mantissa[0] & 0x8000) == 0)
{
        carry = 0;
        ShiftMantLeft1(&carry, ptr->mantissa);
        ptr->exp--;
}
return;
}

/****************
** denormalize **
*****************
** Denormalize an internal-representation number.  This means
** shifting it right until its exponent is equivalent to
** minimum_exponent. (You have to do this often in order
** to perform additions and subtractions).
*/
void denormalize(FPF *ptr,
                int minimum_exponent)
{
long exponent_difference;

if (IsMantissaZero(ptr->mantissa))
{
        printf("Error:  zero significand in denormalize\n");
}

exponent_difference = ptr->exp-minimum_exponent;
if (exponent_difference < 0)
{
        /* 
        ** The number is subnormal
        */
        exponent_difference = -exponent_difference;
        if (exponent_difference >= (INTERNAL_FPF_PRECISION * 16))
        {
                /* Underflow */
                SetFPFZero(ptr, ptr->sign);
        }
        else
        {
                ptr->exp+=exponent_difference;
                StickyShiftRightMant(ptr, exponent_difference);
        }
}
return;
}


/*********************
** RoundFPF **
**********************
** Round an internal-representation number.
** The kind of rounding we do here is simplest...referred to as
** "chop".  "Extraneous" rightmost bits are simply hacked off.
*/
void RoundFPF(FPF *ptr)
{
/* int i; */

if (ptr->type == IFPF_IS_NORMAL ||
        ptr->type == IFPF_IS_SUBNORMAL)
{
        denormalize(ptr, MIN_EXP);
        if (ptr->type != IFPF_IS_ZERO)
        {
        
                /* clear the extraneous bits */
//                ptr->mantissa[INTERNAL_FPF_PRECISION-1] &= 0xfff8;
				if (IsMantissaZero(ptr->mantissa))
					SetFPFZero(ptr, ptr->sign);
/*              for (i=4; i<INTERNAL_FPF_PRECISION; i++)
                {
                        ptr->mantissa[i] = 0;
                }
*/
                /*
                ** Check for overflow
                */
                if (ptr->exp > MAX_EXP)
                {
                        SetFPFInfinity(ptr, ptr->sign);
                }
        }
}
return;
}

/*******************************************************
**  ARITHMETIC OPERATIONS ON INTERNAL REPRESENTATION  **
*******************************************************/

/***************
** choose_nan **
****************
** Called by routines that are forced to perform math on
** a pair of NaN's.  This routine "selects" which NaN is
** to be returned.
*/
void choose_nan(FPF *x,
                FPF *y,
                FPF *z,
                int intel_flag)
{
int i;

/*
** Compare the two mantissas,
** return the larger.  Note that we will be emulating
** an 80387 in this operation.
*/
for (i=0; i<INTERNAL_FPF_PRECISION; i++)
{
        if (x->mantissa[i] > y->mantissa[i])
        {
                memcpy((void *)z,(void *)x,sizeof(FPF));
                return;
        }
        if (x->mantissa[i] < y->mantissa[i])
        {
                memcpy((void *)z,(void *)y,sizeof(FPF));
                return;
        }
}

/* 
** They are equal
*/
if (!intel_flag)
        /* if the operation is addition */
        memcpy((void *)z,(void *)x,sizeof(FPF));
else
        /* if the operation is multiplication */
        memcpy((void *)z,(void *)y,sizeof(FPF));
return;
}


/**********************
** AddSubFPF **
***********************
** Adding or subtracting internal-representation numbers.
** -representation numbers pointed to by x and y are
** added/subtracted and the result returned in z.
*/
void AddSubFPF(uchar operation,
                FPF *x,
                FPF *y,
                FPF *z)
{
int exponent_difference;
uf16 borrow;
uf16 carry;
int i;
FPF locx,locy;  /* Needed since we alter them */

/*
** Following big switch statement handles the
** various combinations of operand types.
*/
switch ((x->type * IFPF_TYPE_COUNT) + y->type)
{
case ZERO_ZERO:
        memcpy((void *)z,(void *)x,sizeof(FPF));
        if (x->sign ^ y->sign ^ operation)
        {
                z->sign = 0; /* positive */
        }
        break;

case NAN_ZERO:
case NAN_SUBNORMAL:
case NAN_NORMAL:
case NAN_INFINITY:
case SUBNORMAL_ZERO:
case NORMAL_ZERO:
case INFINITY_ZERO:
case INFINITY_SUBNORMAL:
case INFINITY_NORMAL:
        memcpy((void *)z,(void *)x,sizeof(FPF));
        break;


case ZERO_NAN:
case SUBNORMAL_NAN:
case NORMAL_NAN:
case INFINITY_NAN:
        memcpy((void *)z,(void *)y,sizeof(FPF));
        break;

case ZERO_SUBNORMAL:
case ZERO_NORMAL:
case ZERO_INFINITY:
case SUBNORMAL_INFINITY:
case NORMAL_INFINITY:
        memcpy((void *)z,(void *)y,sizeof(FPF));
        z->sign ^= operation;
        break;

case SUBNORMAL_SUBNORMAL:
case SUBNORMAL_NORMAL:
case NORMAL_SUBNORMAL:
case NORMAL_NORMAL:
        /*
        ** Copy x and y to locals, since we may have
        ** to alter them.
        */
        memcpy((void *)&locx,(void *)x,sizeof(FPF));
        memcpy((void *)&locy,(void *)y,sizeof(FPF));

        /* compute sum/difference */
        exponent_difference = locx.exp-locy.exp;
        if (exponent_difference == 0)
        {
                /* 
                ** locx.exp == locy.exp
                ** so, no shifting required
                */
                if (locx.type == IFPF_IS_SUBNORMAL ||
                  locy.type == IFPF_IS_SUBNORMAL)
                        z->type = IFPF_IS_SUBNORMAL;
                else
                        z->type = IFPF_IS_NORMAL;

                /* 
                ** Assume that locx.mantissa > locy.mantissa
                */
                z->sign = locx.sign;
                z->exp= locx.exp;
        }
        else
                if (exponent_difference > 0)
                {
                        /*
                        ** locx.exp > locy.exp
                        */
                        StickyShiftRightMant(&locy,
                                 exponent_difference);
                        z->type = locx.type;
                        z->sign = locx.sign;
                        z->exp = locx.exp;
                }
                else    /* if (exponent_difference < 0) */
                {
                        /*
                        ** locx.exp < locy.exp
                        */
                        StickyShiftRightMant(&locx,
                                -exponent_difference);
                        z->type = locy.type;
                        z->sign = locy.sign ^ operation;
                        z->exp = locy.exp;
                }

                if (locx.sign ^ locy.sign ^ operation)
                {
                        /* 
                        ** Signs are different, subtract mantissas
                        */
                        borrow = 0;
                        for (i=(INTERNAL_FPF_PRECISION-1); i>=0; i--)
                                Sub16Bits(&borrow,
                                        &z->mantissa[i],
                                        locx.mantissa[i],
                                        locy.mantissa[i]);

                        if (borrow)
                        {
                                /* The y->mantissa was larger than the
                                ** x->mantissa leaving a negative
                                ** result.  Change the result back to
                                ** an unsigned number and flip the
                                ** sign flag.
                                */
                                z->sign = locy.sign ^ operation;
                                borrow = 0;
                                for (i=(INTERNAL_FPF_PRECISION-1); i>=0; i--)
                                {
                                        Sub16Bits(&borrow,
                                                &z->mantissa[i],
                                                0,
                                                z->mantissa[i]);
                                }
                        }
                        else
                        {
                                /* The assumption made above
                                ** (i.e. x->mantissa >= y->mantissa)
                                ** was correct.  Therefore, do nothing.
                                ** z->sign = x->sign;
                                */
                        }

                        if (IsMantissaZero(z->mantissa))
                        {
                                z->type = IFPF_IS_ZERO;
                                z->sign = 0; /* positive */
                        }
                        else
                                if (locx.type == IFPF_IS_NORMAL ||
                                         locy.type == IFPF_IS_NORMAL)
                                {
                                        normalize(z);
                                }
                }
                else
                {
                        /* signs are the same, add mantissas */
                        carry = 0;
                        for (i=(INTERNAL_FPF_PRECISION-1); i>=0; i--)
                        {
                                Add16Bits(&carry,
                                        &z->mantissa[i],
                                        locx.mantissa[i],
                                        locy.mantissa[i]);
                        }

                        if (carry)
                        {
                                z->exp++;
                                carry=0;
                                ShiftMantRight1(&carry,z->mantissa);
                                z->mantissa[0] |= 0x8000;
                                z->type = IFPF_IS_NORMAL;
                        }
                        else
                                if (z->mantissa[0] & 0x8000)
                                        z->type = IFPF_IS_NORMAL;
        }
        break;

case INFINITY_INFINITY:
        SetFPFNaN(z);
        break;

case NAN_NAN:
        choose_nan(x, y, z, 1);
        break;
}

/*
** All the math is done; time to round.
*/
RoundFPF(z);
return;
}


/************************
** MultiplyFPF **
*************************
** Two internal-representation numbers x and y are multiplied; the
** result is returned in z.
*/
void MultiplyFPF(FPF *x,
                        FPF *y,
                        FPF *z)
{
int i;
int j;
uf16 carry;
uf16 extra_bits[INTERNAL_FPF_PRECISION];
FPF locy;       /* Needed since this will be altered */
/*
** As in the preceding function, this large switch
** statement selects among the many combinations
** of operands.
*/
switch ((x->type * IFPF_TYPE_COUNT) + y->type)
{
case INFINITY_SUBNORMAL:
case INFINITY_NORMAL:
case INFINITY_INFINITY:
case ZERO_ZERO:
case ZERO_SUBNORMAL:
case ZERO_NORMAL:
        memcpy((void *)z,(void *)x,sizeof(FPF));
        z->sign ^= y->sign;
        break;

case SUBNORMAL_INFINITY:
case NORMAL_INFINITY:
case SUBNORMAL_ZERO:
case NORMAL_ZERO:
        memcpy((void *)z,(void *)y,sizeof(FPF));
        z->sign ^= x->sign;
        break;

case ZERO_INFINITY:
case INFINITY_ZERO:
        SetFPFNaN(z);
        break;

case NAN_ZERO:
case NAN_SUBNORMAL:
case NAN_NORMAL:
case NAN_INFINITY:
        memcpy((void *)z,(void *)x,sizeof(FPF));
        break;

case ZERO_NAN:
case SUBNORMAL_NAN:
case NORMAL_NAN:
case INFINITY_NAN:
        memcpy((void *)z,(void *)y,sizeof(FPF));
        break;


case SUBNORMAL_SUBNORMAL:
case SUBNORMAL_NORMAL:
case NORMAL_SUBNORMAL:
case NORMAL_NORMAL:
        /*
        ** Make a local copy of the y number, since we will be
        ** altering it in the process of multiplying.
        */
        memcpy((void *)&locy,(void *)y,sizeof(FPF));

        /*
        ** Check for unnormal zero arguments
        */
        if (IsMantissaZero(x->mantissa) || IsMantissaZero(y->mantissa))
                SetFPFInfinity(z, 0);

        /*
        ** Initialize the result
        */
        if (x->type == IFPF_IS_SUBNORMAL ||
            y->type == IFPF_IS_SUBNORMAL)
                z->type = IFPF_IS_SUBNORMAL;
        else
                z->type = IFPF_IS_NORMAL;

        z->sign = x->sign ^ y->sign;
        z->exp = x->exp + y->exp ;
        for (i=0; i<INTERNAL_FPF_PRECISION; i++)
        {
                z->mantissa[i] = 0;
                extra_bits[i] = 0;
        }

        for (i=0; i<(INTERNAL_FPF_PRECISION*16); i++)
        {
                /*
                ** Get rightmost bit of the multiplier
                */
                carry = 0;
                ShiftMantRight1(&carry, locy.mantissa);
                if (carry)
                {
                        /* 
                        ** Add the multiplicand to the product
                        */
                        carry = 0;
                        for (j=(INTERNAL_FPF_PRECISION-1); j>=0; j--)
                                Add16Bits(&carry,
                                        &z->mantissa[j],
                                        z->mantissa[j],
                                        x->mantissa[j]);
                }
                else
                {
                        carry = 0;
                }

                /* 
                ** Shift the product right.  Overflow bits get
                ** shifted into extra_bits.  We'll use it later
                ** to help with the "sticky" bit.
                */
                ShiftMantRight1(&carry, z->mantissa);
                ShiftMantRight1(&carry, extra_bits);
        }

        /*
        ** Normalize
        ** Note that we use a "special" normalization routine
        ** because we need to use the extra bits. (These are
        ** bits that may have been shifted off the bottom that
        ** we want to reclaim...if we can.
        */
        while ((z->mantissa[0] & 0x8000) == 0)
        {
                carry = 0;
                ShiftMantLeft1(&carry, extra_bits);
                ShiftMantLeft1(&carry, z->mantissa);
                z->exp--;
        }

        /*
        ** Set the sticky bit if any bits set in extra bits.
        */
        if (!IsMantissaZero(extra_bits))
        {
                z->mantissa[INTERNAL_FPF_PRECISION-1] |= 1;
        }
        break;

case NAN_NAN:
        choose_nan(x, y, z, 0);
        break;
}

/*
** All math done...do rounding.
*/
RoundFPF(z);
return;
}


/**********************
** DivideFPF **
***********************
** Divide internal FPF number x by y.  Return result in z.
*/
void DivideFPF(FPF *x, 
                        FPF *y, 
                        FPF *z)
{
int i;
int j;
uf16 carry;
uf16 extra_bits[INTERNAL_FPF_PRECISION];
FPF locx;       /* Local for x number */

/*
** As with preceding function, the following switch
** statement selects among the various possible
** operands.
*/
switch ((x->type * IFPF_TYPE_COUNT) + y->type)
{
case ZERO_ZERO:
case INFINITY_INFINITY:
        SetFPFNaN(z);
        break;

case ZERO_SUBNORMAL:
case ZERO_NORMAL:
        if (IsMantissaZero(y->mantissa))
        {
                SetFPFNaN(z);
                break;
        }

case ZERO_INFINITY:
case SUBNORMAL_INFINITY:
case NORMAL_INFINITY:
        SetFPFZero(z, x->sign ^ y->sign);
        break;

case SUBNORMAL_ZERO:
case NORMAL_ZERO:
        if (IsMantissaZero(x->mantissa))
        {
                SetFPFNaN(z);
                break;
        }

case INFINITY_ZERO:
case INFINITY_SUBNORMAL:
case INFINITY_NORMAL:
        SetFPFInfinity(z, 0);
        z->sign = x->sign ^ y->sign;
        break;

case NAN_ZERO:
case NAN_SUBNORMAL:
case NAN_NORMAL:
case NAN_INFINITY:
        memcpy((void *)z,(void *)x,sizeof(FPF));
        break;

case ZERO_NAN:
case SUBNORMAL_NAN:
case NORMAL_NAN:
case INFINITY_NAN:
        memcpy((void *)z,(void *)y,sizeof(FPF));
        break;

case SUBNORMAL_SUBNORMAL:
case NORMAL_SUBNORMAL:
case SUBNORMAL_NORMAL:
case NORMAL_NORMAL:
        /*
        ** Make local copy of x number, since we'll be
        ** altering it in the process of dividing.
        */
        memcpy((void *)&locx,(void *)x,sizeof(FPF));

        /* 
        ** Check for unnormal zero arguments
        */
        if (IsMantissaZero(locx.mantissa))
        {
                if (IsMantissaZero(y->mantissa))
                        SetFPFNaN(z);
                else
                        SetFPFZero(z, 0);
                break;
        }
        if (IsMantissaZero(y->mantissa))
        {
                SetFPFInfinity(z, 0);
                break;
        }

        /* 
        ** Initialize the result
        */
        z->type = x->type;
        z->sign = x->sign ^ y->sign;
        z->exp = x->exp - y->exp +
                        ((INTERNAL_FPF_PRECISION * 16 * 2));
        for (i=0; i<INTERNAL_FPF_PRECISION; i++)
        {
                z->mantissa[i] = 0;
                extra_bits[i] = 0;
        }

        while ((z->mantissa[0] & 0x8000) == 0)
        {
                carry = 0;
                ShiftMantLeft1(&carry, locx.mantissa);
                ShiftMantLeft1(&carry, extra_bits);

                /* 
                ** Time to subtract yet?
                */
                if (carry == 0)
                        for (j=0; j<INTERNAL_FPF_PRECISION; j++)
                        {
                                if (y->mantissa[j] > extra_bits[j])
                                {
                                        carry = 0;
                                        goto no_subtract;
                                }
                                if (y->mantissa[j] < extra_bits[j])
                                        break;
                        }
                /* 
                ** Divisor (y) <= dividend (x), subtract
                */
                carry = 0;
                for (j=(INTERNAL_FPF_PRECISION-1); j>=0; j--)
                        Sub16Bits(&carry,
                                &extra_bits[j],
                                extra_bits[j],
                                y->mantissa[j]);
                carry = 1;      /* 1 shifted into quotient */
        no_subtract:
                ShiftMantLeft1(&carry, z->mantissa);
                z->exp--;
        }
        break;

case NAN_NAN:
        choose_nan(x, y, z, 0);
        break;
}

/*
** Math complete...do rounding
*/
RoundFPF(z);
}
void LongLongToFPF(FPF *dest, LLONG_TYPE myllong)
{
int i;          /* Index */
uf16 myword;     /* Used to hold converted stuff */
/*
** Save the sign and get the absolute value.  This will help us
** with 64-bit machines, since we use only the lower 32
** bits just in case.
*/
if(myllong<0L)
{       dest->sign=1;
        myllong=0-myllong;
}
else
        dest->sign=0;
/*
** Prepare the destination floating point number
*/
dest->type=IFPF_IS_NORMAL;
for(i=0;i<INTERNAL_FPF_PRECISION;i++)
        dest->mantissa[i]=0;

/*
** See if we've got a zero.  If so, make the resultant FP
** number a true zero and go home.
*/
if(myllong==0)
{       dest->type=IFPF_IS_ZERO;
        dest->exp=0;
        return;
}

/*
** Not a true zero.  Set the exponent to 64 (internal FPFs have
** no bias) and load the low and high words into their proper
** locations in the mantissa.  Then normalize.  The action of
** normalizing slides the mantissa bits into place and sets
** up the exponent properly.
*/
dest->exp=64;
#ifdef USE_LONGLONG
myword=(uf16)((myllong >> 48) & 0xFFFFL);
dest->mantissa[0]=myword;
myword=(uf16)((myllong >> 32) & 0xFFFFL);
dest->mantissa[1]=myword;
#endif
myword=(uf16)((myllong >> 16) & 0xFFFFL);
dest->mantissa[2]=myword;
myword=(uf16)(myllong & 0xFFFFL);
dest->mantissa[3]=myword;
normalize(dest);
return;
}
void UnsignedLongLongToFPF(FPF *dest, LLONG_TYPE myllong)
{
int i;          /* Index */
uf16 myword;     /* Used to hold converted stuff */
        dest->sign=0;
/*
** Prepare the destination floating point number
*/
dest->type=IFPF_IS_NORMAL;
for(i=0;i<INTERNAL_FPF_PRECISION;i++)
        dest->mantissa[i]=0;

/*
** See if we've got a zero.  If so, make the resultant FP
** number a true zero and go home.
*/
if(myllong==0)
{       dest->type=IFPF_IS_ZERO;
        dest->exp=0;
        return;
}

/*
** Not a true zero.  Set the exponent to 64 (internal FPFs have
** no bias) and load the low and high words into their proper
** locations in the mantissa.  Then normalize.  The action of
** normalizing slides the mantissa bits into place and sets
** up the exponent properly.
*/
dest->exp=64;
#ifdef USE_LONGLONG
myword=(uf16)((myllong >> 48) & 0xFFFFL);
dest->mantissa[0]=myword;
myword=(uf16)((myllong >> 32) & 0xFFFFL);
dest->mantissa[1]=myword;
#endif
myword=(uf16)((myllong >> 16) & 0xFFFFL);
dest->mantissa[2]=myword;
myword=(uf16)(myllong & 0xFFFFL);
dest->mantissa[3]=myword;
normalize(dest);
return;
}
/* converts the exponent to base ten, doing truncation rounding 
 * we are using real floating point here but not depending on the format
 */
#define LB2_10 (M_LN10 / M_LN2)
int FPFTensExponent(FPF *value)
{
    return (int)((float)value->exp/LB2_10);
}
/* multiply by a power of ten */
void FPFMultiplyPowTen(FPF *value, int power)
{
    FPF temp,mul ;
    int i;
    if (!tensTab[0].mantissa[0])
        FPFInit();
    temp.sign = 0;
 /* 10^x = 5^x * 2^x*/
    if (value->type == IFPF_IS_ZERO || value->type == IFPF_IS_NAN || value->type == IFPF_IS_INFINITY)
        return;
    if (power < 0) { /* constant 0.2 */
        if (power < - 20)
        {
            value->exp += power ;
    	    for (i=0; i < INTERNAL_FPF_PRECISION; i++)
	            temp.mantissa[i] = 0xCCCC;
            power = - power;
            temp.exp = -2 ;
            temp.type = IFPF_IS_NORMAL;
        }
        else
        {
            power = - power;
            if (power > 10)
            {
                DivideFPF(value, &tensTab[9], &temp);
                *value = temp;
                power -= 10;
            }
            DivideFPF(value, &tensTab[power-1], &temp);
            *value = temp;
            return;
        }
    } else if (power > 0) { /* constant 5 */
        value->exp += power ;
        temp.mantissa[0] = 0xa000;
        for (i=1; i < INTERNAL_FPF_PRECISION; i++)
            temp.mantissa[i] = 0;
        temp.exp = 3;
        temp.type = IFPF_IS_NORMAL;
    } else { /* multiply by 10^0 is a nop */
        return ;
    }
    mul = temp;
    while (power) {
        FPF internal;
    	if (power & 1) { 
            MultiplyFPF(value, &mul, &internal);
            *value = internal;
        }
        MultiplyFPF (&mul, &mul, &internal);
        mul = internal;
        power >>= 1 ;
    }
    RoundFPF(value);
}
/************************
** FPFToString **
*************************
** FOR DEBUG PURPOSES
** This routine converts an internal floating point representation
** number to a string.  Used in debugging the package.
** Returns length of converted number.
** NOTE: dest must point to a buffer big enough to hold the
**  result.  Also, this routine does append a null (an effect
**  of using the sprintf() function).  It also returns
**  a length count.
** NOTE: This routine returns 5 significant digits.  Thats
**  about all I feel safe with, given the method of
**  conversion.  It should be more than enough for programmers
**  to determine whether the package is properly ported.
*/
char * FPFToString(char *dest,
                FPF *src)
{
    char *old = dest;
	if (src->type == IFPF_IS_NAN)
	{
		if (src->sign)
			*dest++ = '-';
		strcpy(dest, "nan");
	}
	else if (src->type == IFPF_IS_INFINITY)
	{
		if (src->sign)
			*dest++ = '-';
		strcpy(dest, "inf");
	}
	else if (src->type == IFPF_IS_ZERO)
	{
		if (src->sign)
			*dest++ = '-';
		strcpy(dest, "0.0");
	}
	else
	{
	    uf16 mantissa[INTERNAL_FPF_PRECISION];
	    FPF temp = *src;
	    int power = FPFTensExponent(&temp);
	    uf16 carry;
	    int val,val1,i,j,c;
	    FPFMultiplyPowTen(&temp,-power);
	    val = 0;
	    if (temp.sign)
	        *dest++ = '-';
	    if (temp.exp > 0) {
	        while (temp.exp--) {
	            carry = 0;
	            ShiftMantLeft1(&carry,temp.mantissa);
	            val <<= 1;
	            val |= !!carry;
	        }
	        *dest++ = val + '0';
	        *dest++ = '.' ;
	    }
	    else {
	        *dest++ = '0';
	        *dest++ = '.';
	        while (temp.exp++) {
	            carry = 0;
	            ShiftMantRight1(&carry, temp.mantissa);
	        }
	    }
	    c = *(dest-2) != '0';
	    for (i=0; i < 24 - c; i++) {
	        carry = 0;
	        val = 0;
	        ShiftMantLeft1(&carry,temp.mantissa);
	        val <<= 1;
	        val |= !!carry;
	        val1 = val ;
	        memcpy(mantissa,temp.mantissa, INTERNAL_FPF_PRECISION * sizeof(uf16));
	        carry = 0;
	        ShiftMantLeft1(&carry,temp.mantissa);
	        val <<= 1;
	        val |= !!carry;
	        carry = 0;
	        ShiftMantLeft1(&carry,temp.mantissa);
	        val <<= 1;
	        val |= !!carry;
	        val += val1 ;
	        carry = 0;
	        for (j= INTERNAL_FPF_PRECISION-1; j >=0; j--) {
	            Add16Bits(&carry, temp.mantissa + j, mantissa[j], temp.mantissa[j]);
	        }
	        val += !!carry;
	        *dest++ = '0' + val;
	    }
	    if (*(dest-1) > '4') {
	        char *t = dest-2;
	        while (t >= old) {
	            if (*t < '0') {
	                t--;
	                continue;
	            }
	            (*t)++;
	            if (*t != '9'+1)
	                break;
	            *t-- = '0';
	        }
	    }
	    dest--;
	    if (power) {
	       *dest++ = 'E';
	        if (power < 0) {
	            power = - power;
	            *dest++ = '-';
	        } else
	            *dest++ = '+';
	        sprintf(dest,"%d",power);
	    } else
	        *dest = 0 ;
	}
    return old;
}

LLONG_TYPE FPFToLongLong(FPF *src)
{
	FPF stemp = *src;
	int exp = stemp.exp;
    LLONG_TYPE temp;
    uf16 tmant[INTERNAL_FPF_PRECISION];
    int i;
    if (stemp.type == IFPF_IS_ZERO)
        return 0;
    switch(stemp.type) {
        case IFPF_IS_INFINITY:
        case IFPF_IS_NAN:
#ifdef USE_LONGLONG
        if (stemp.sign)
            return LLONG_MIN;
        else
            return LLONG_MAX;
#else
        if (stemp.sign)
            return LONG_MIN;
        else
            return LONG_MAX;
#endif
        case IFPF_IS_ZERO:
        case IFPF_IS_SUBNORMAL:
            return 0;
        case IFPF_IS_NORMAL:
        memcpy(tmant,stemp.mantissa,sizeof(tmant));
#ifdef USE_LONGLONG
        if (stemp.exp > 63)
        {
            if (stemp.sign)
                return LLONG_MIN;
            else
                return LLONG_MAX;
        }
        if (stemp.exp < 0)
            return 0;
        while (stemp.exp++ != 64) {
            uf16 carry= 0;
            ShiftMantRight1(&carry, tmant);
        }
        temp = 0;
        for (i=0 ; i < 4; i++) {
            temp = temp << 16;
            temp |= tmant[i];
        }
#else
        if (stemp.exp > 31)
            if (stemp.sign)
                return LONG_MIN;
            else
                return LONG_MAX;
        if (stemp.exp < 0)
            return 0;
        while (exp++ != 32) {
            uf16 carry= 0;
            ShiftMantRight1(&carry, tmant);
        }
        temp = 0;
        for (i=0 ; i < 2; i++) {
            temp = temp << 16;
            temp |= tmant[i];
        }
#endif            
        if (stemp.sign)
            temp = - temp;
        return temp;
        
    }
    return 0;
}
int FPFToFloat(unsigned char *dest, FPF *src)
{
    uf32 val ;
      FPFTruncate(src, 24, 128, -126);
	if (src->type == IFPF_IS_ZERO)
		val = 0 ;
	else 
	{
		if (src->type == IFPF_IS_INFINITY)
			val = 0x7f800000;
		else if (src->type == IFPF_IS_NAN)
			val = 0x7fc00000;
		else {
			if (src->exp <= -126)
			{
				val = ((src->mantissa[0]) << 7) + (src->mantissa[1] >> 9) ;
			}
			else
			{
				val = ((src->mantissa[0] & 0x7fff) << 8) + (src->mantissa[1] >> 8) ;
			}
		    if (src->exp > 129 || src->exp < - 126)
			{
				if (src->exp < -126 && src->exp >= -149)
				{
					int n = -(src->exp + 126);
					uf32 r = !!(val & (1 << (n - 1)));
					val >>= n;
					val += r;
				}
				else diag("FPFToFloat: invalid exponent");
			}
			else
			    val |= (src->exp + 126) << 23;
		}
	}
    if (src->sign)
        val |= 0x80000000L;
    if (BigEndian) {
        dest[0] = val >> 24;
        dest[1] = (val >> 16) & 0xff ;
        dest[2] = (val >> 8) & 0xff ;
        dest[3] = (val >> 0) & 0xff ;
    } else {
        dest[3] = val >> 24;
        dest[2] = (val >> 16) & 0xff ;
        dest[1] = (val >> 8) & 0xff ;
        dest[0] = (val >> 0) & 0xff ;
    }
    return 1;
}
int FPFToDouble(unsigned char *dest, FPF *src)
{
    uf32 val[2] ;
      FPFTruncate(src, 53, 1024, -1022);
	if (src->type == IFPF_IS_ZERO)
		val[0] = val[1] = 0 ;
	else 
	{
		if (src->type == IFPF_IS_INFINITY)
			val[0] = 0x7ff00000, val[1] = 0;
		else if (src->type == IFPF_IS_NAN)
			val[0] = 0x7ff80000, val[1] = 0;
	    else {
			if (src->exp <= -1022)
			{
			    val[0] = ((src->mantissa[0]) << 4) + (src->mantissa[1] >> 12) ;
			    val[1] = (src->mantissa[1] << 20) + (src->mantissa[2] << 4) + (src->mantissa[3] >> 12) ;
			}
			else
			{
			    val[0] = ((src->mantissa[0] & 0x7fff) << 5) + (src->mantissa[1] >> 11) ;
			    val[1] = (src->mantissa[1] << 21) + (src->mantissa[2] << 5) + (src->mantissa[3] >> 11) ;
			}
	        if (src->exp > 1025 || src->exp < - 1022)
			{
				if (src->exp < -1022 && src->exp >= -1074)
				{
					int n = -(src->exp + 1022);
					uf32 c = val[0], r, t;
					if (n < 32)
					{
						r = !!(val[1] & (1 << (n - 1)));
						val[0] >>= n;
						val[1] >>= n;
						val[1] |= (c & ~(1 << n)) << (32 - n);
					}
					else
					{
						if (n == 32) r = !!(val[1] & (1 << 31));
						else r = !!(c & (1 << (n - 33)));
						val[0] = 0;
						val[1] = c >> (n - 32);
					}
					t = val[1];
					t += r;
					if (t < val[1]) val[0]++;
					val[1] = t;
				}
				else diag("FPFToDouble: invalid exponent");
			}
			else
		        val[0] |= (src->exp + 1022) << 20;
	    }
	}
    if (src->sign)
        val[0] |= 0x80000000L;
    if (BigEndian) {
        dest[0] = val[0] >> 24;
        dest[1] = (val[0] >> 16) & 0xff ;
        dest[2] = (val[0] >> 8) & 0xff ;
        dest[3] = (val[0] >> 0) & 0xff ;
        dest[4] = val[1] >> 24;
        dest[5] = (val[1] >> 16) & 0xff ;
        dest[6] = (val[1] >> 8) & 0xff ;
        dest[7] = (val[1] >> 0) & 0xff ;
    } else {
        dest[7] = val[0] >> 24;
        dest[6] = (val[0] >> 16) & 0xff ;
        dest[5] = (val[0] >> 8) & 0xff ;
        dest[4] = (val[0] >> 0) & 0xff ;
        dest[3] = val[1] >> 24;
        dest[2] = (val[1] >> 16) & 0xff ;
        dest[1] = (val[1] >> 8) & 0xff ;
        dest[0] = (val[1] >> 0) & 0xff ;
    }
    return 1;
}
int FPFToLongDouble(unsigned char *dest, FPF *src)
{
    uf32 val[3] ;
    FPFTruncate(src, 64, 16384, -16382);
    /* have to or in the high bit in case infinity or nan*/
	if (src->type == IFPF_IS_ZERO)
		val[0] = val[1] = val[2] = 0 ;
	else 
	{
		if (src->type == IFPF_IS_INFINITY)
			val[0] = 0x7fff, val[1] = 0x80000000, val[2] = 0;
		else if (src->type == IFPF_IS_NAN)
		{
			val[0] = 0x7fff;
			val[1] = 0xc0000000;
			val[2] = 0;
		}
	    else {
			val[0] = 0;
		    val[1] = (src->mantissa[0] << 16) + src->mantissa[1];
		    val[2] = (src->mantissa[2] << 16) + src->mantissa[3];
	        if (src->exp > 16385 || src->exp < - 16382)
				if (src->exp < -16382 && src->exp >= -16446)
				{
					int n = -(src->exp + 16381);
					uf32 c = val[1], r, t;
					if (n < 32)
					{
						r = !!(val[2] & (1 << (n - 1)));
						val[1] >>= n;
						val[2] >>= n;
						val[2] |= (c & ~(1 << n)) << (32 - n);
					}
					else if (n < 64)
					{
						if (n == 32) r = !!(val[2] & (1 << 31));
						else r = !!(c & (1 << (n - 33)));
						val[1] = 0;
						val[2] = c >> (n - 32);
					}
					else
					{
						if (n == 64) r = !!(c & (1 << 31));
						else r = 0;
						val[1] = 0;
						val[2] = 0;
					}
					t = val[2];
					t += r;
					if (t < val[2]) val[1]++;
					val[2] = t;
				}
				else diag("FPFToLongDouble: invalid exponent");
			else
		        val[0] = src->exp + 16382;
	    }
	}
    if (src->sign)
        val[0] |= 0x8000L;
        
    if (BigEndian) {
        dest[0] = val[0] >> 8;
        dest[1] = val[0] & 0xff;
        dest[2] = val[1] >> 24;
        dest[3] = (val[1] >> 16) & 0xff ;
        dest[4] = (val[1] >> 8) & 0xff ;
        dest[5] = (val[1] >> 0) & 0xff ;
        dest[6] = val[2] >> 24;
        dest[7] = (val[2] >> 16) & 0xff ;
        dest[8] = (val[2] >> 8) & 0xff ;
        dest[9] = (val[2] >> 0) & 0xff ;
    } else {
        dest[9] = val[0] >> 8;
        dest[8] = val[0] & 0xff;
        dest[7] = val[1] >> 24;
        dest[6] = (val[1] >> 16) & 0xff ;
        dest[5] = (val[1] >> 8) & 0xff ;
        dest[4] = (val[1] >> 0) & 0xff ;
        dest[3] = val[2] >> 24;
        dest[2] = (val[2] >> 16) & 0xff ;
        dest[1] = (val[2] >> 8) & 0xff ;
        dest[0] = (val[2] >> 0) & 0xff ;
    }
    return 1;
}
int LongDoubleToFPF(FPF *dest, unsigned char *src)
{
    int i, exp;
    if (BigEndian) {
        for (i = 0; i < INTERNAL_FPF_PRECISION - 1; i++)
            dest->mantissa[i] = *((uf16 *)src + i + 1);
        dest->mantissa[i] = 0;
        exp = *(uf16 *)src;
    } else {
        for (i = 0; i < INTERNAL_FPF_PRECISION - 1; i++)
            dest->mantissa[i] = *((uf16 *)src + INTERNAL_FPF_PRECISION - 2 - i);
        dest->mantissa[i] = 0;
        exp = *((uf16 *)src + INTERNAL_FPF_PRECISION - 1);
    }
    dest->sign = !!(exp & 0x8000);
    exp &= 0x7fff;
    if (exp == 0) {
       if (IsMantissaZero(dest->mantissa)) {
           dest->type = IFPF_IS_ZERO;
           dest->exp = exp;
       }
       else {
           dest->type = IFPF_IS_SUBNORMAL;
           dest->exp = exp - 16381;
           FPFTruncate(dest, 64, MAX_EXP, MIN_EXP);
       }
    }
    else if (exp == 0x7fff)
    {
       if (dest->mantissa[0] == 0x8000 &&
           (dest->mantissa[1] | dest->mantissa[2] | dest->mantissa[3]) == 0)
           dest->type = IFPF_IS_INFINITY;
       else
           dest->type = IFPF_IS_NAN;
       dest->exp = exp;
    }
    else {
       dest->type = IFPF_IS_NORMAL;
       dest->exp = exp - 16382;
    }
    return 1;
}
void FPFTruncate(FPF *value, int bits, int maxexp, int minexp)
{
    switch(value->type) {
        case IFPF_IS_NAN:
        case IFPF_IS_INFINITY:
            return;
        case IFPF_IS_ZERO:
            return;
        case IFPF_IS_NORMAL:
        case IFPF_IS_SUBNORMAL:
			normalize(value);
            if (value->exp > maxexp)
                SetFPFInfinity(value, value->sign);
            else if (value->exp < minexp) {
                if (value->exp + bits < minexp)
                    SetFPFZero(value, 0);
                else {
/*
                    while (value->exp < minexp) {
                        uf16 carry = 0;
                        ShiftMantRight1(&carry, value->mantissa);
                        value->exp++ ;
                    }
                    if (IsMantissaZero(value->mantissa))
                        SetFPFZero(value,0);
                    else
*/
                        value->type = IFPF_IS_SUBNORMAL;
                }
            } else {
                int i, v;
				int rounding = FALSE;
                /*if (bits % 16)*/ {
                    int mask = 0x8000, mask2 = 0x8000;
                    for (i= 1; i <= bits % 16; i++) {
                        mask >>= 1;
						mask2 >>= 1;
                        mask |= 0x8000;
                    }
					if (value->mantissa[bits/16] & mask2)
						rounding = TRUE;
                    value->mantissa[bits /16] &= mask;
					v = mask2;
                }
/*
				else
					v = 0x8000;
*/
/* Needed if "& mask2", above, is changed back to "& ~mask"
                for (i = bits / 16 + 1; i < INTERNAL_FPF_PRECISION; i++)
				{
					if (value->mantissa[i])
						rounding ++;
                    value->mantissa[i] = 0;
				}
*/
				if (rounding)
				{
					int n;

					/* do an add of 1 to the LSB */
					for (i= bits/16; i >= 0; i--)
					{
						n =	value->mantissa[i] + v;
						value->mantissa[i] = n;
						if (n < 0x10000)
							break;
						v = 1;
					}
					// overflow????? 
					if (n >= 0x10000)
					{
						value->exp++;
						/* exponent too large ? */
						if (value->exp >= maxexp)
						{
							/* yes leave it alone */
							SetFPFInfinity(value, value->sign);
						}
						else
						{
							/* else shift mantissa right and increment exponent... */
							uf16 carry = 1 ; /* high bit should be one */
							ShiftMantRight1(&carry, &value->mantissa[0]);
						}
					}
				}
            }
            break ;
    }
}
#ifdef TEST
void main()
{
    int i;
    char buf[256];
    float f;
    double d;
    long double l;
    FPF one,two,three ;
    int val;
    LLONG_TYPE aa ;
    LongLongToFPF(&one,-1976543);
    LongLongToFPF(&two,100000000);
    DivideFPF(&one, &two, &three);
/*    val = FPFTensExponent(&three);*/
/*    printf("%d\n",val);*/
/*    FPFMultiplyPowTen(&one, -2);*/
/*    printf("%d\n",memcmp(&one,&three,sizeof(one)));*/
    FPFToString(buf,&three);
    printf("%s\n",buf);
    FPFToFloat(&f,&one);
    FPFToDouble(&d,&one);
    FPFToLongDouble(&l, &one);
    LongDoubleToFPF(&three, &l);
    FPFToString(buf,&three);
    printf("%s\n",buf);
    aa = FPFToLongLong(&one) ;
    printf("%f %f %Lf %ld ", f,d,l, aa);
    for (i=0; i < INTERNAL_FPF_PRECISION; i++)
        printf("%x ",one.mantissa[i]);
    printf("\n%d\n",one.type);
    FPFTruncate(&one,10,4,-4);
    for (i=0; i < INTERNAL_FPF_PRECISION; i++)
        printf("%x ",one.mantissa[i]);
    printf("\n%d\n",one.type);
    LongLongToFPF(&one,1);
    printf("%d\n",ValueIsOne(&one));
    printf("%d\n",FPFGTE(&one,&two));
    printf("%d\n",FPFGTE(&two,&one));
}
#endif
