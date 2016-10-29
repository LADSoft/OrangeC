/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef FLOATING_H
#define FLOATING_H

#include <string>
#include <string.h>

#define INTERNAL_FPF_PRECISION ((80/8)/sizeof(u16))
/*
** DEFINES
*/
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#define uchar unsigned char
#define ulong unsigned long

#define MAX_EXP 33000
#define MIN_EXP -33000

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

class FPF
{
public:
    FPF() { SetZero(0); }
    FPF(const FPF &n)
    {
        exp = n.exp;
        memcpy(mantissa, n.mantissa, sizeof(mantissa));
        type = n.type;
        sign = n.sign;
    }
    virtual ~FPF() { }
    void SetZero(int sign);
    void Negate() { sign = !sign; }
    void SetInfinity(int sign);
    void SetNaN();
    bool ValueIsOne() const ;
    bool ValueIsZero() const;
    bool operator ==(const FPF &right);
    bool operator !=(const FPF &right) { return ! operator==(right); }
    bool operator >(FPF &right);
    bool operator >=(FPF &right);
    bool operator <(FPF &right) { return !operator >=(right); }
    bool operator <=(FPF &right) { return !operator >(right); }

    void Normalize();
    void Denormalize(int minimum_exponent);
    void Round();

    FPF &operator +(const FPF &right) const { return AddSub('+', *new FPF, *this, right); }
    FPF &operator +=(const FPF &right) { return AddSub('+', *this, *this, right); }
    FPF &operator -(const FPF &right) const { return AddSub('-', *new FPF, *this, right); }
    FPF &operator -=(const FPF &right) { return AddSub('+', *this, *this, right); }
    FPF &operator *(const FPF &right) const { return Multiply(*new FPF, *this, right); }
    FPF &operator *=(const FPF &right){ return Multiply(*this, *this, right); };
    FPF &operator /(const FPF &right) const { return Divide(*new FPF, *this, right); };
    FPF &operator /=(const FPF &right) { return Divide(*this, *this, right); };
    FPF &operator =(const FPF &right)
    {
        exp = right.exp;
        memcpy(mantissa, right.mantissa, sizeof(mantissa));
        sign = right.sign;
        type = right.type;
        return *this;
    }
    FPF &operator =(L_INT right) { FromLongLong(right); return *this; }
    FPF &operator =(unsigned L_INT right) { FromUnsignedLongLong(right); return *this; }
    FPF &operator =(int right) { FromLongLong(right); return *this; }
    FPF &operator =(unsigned right) { FromUnsignedLongLong(right); return *this; }

    operator L_INT () const { return ToLongLong(); }
    operator std::string () const { std::string rv; ToString(rv); return rv; }
    void ToFloat(uchar dest[]) const;
    void ToDouble(uchar dest[]) const;
    void ToLongDouble(uchar dest[]) const;
    void Truncate(int bits, int maxexp, int minexp);

    void SetExp(int Exp) { exp = Exp; }
    int GetExp() const { return exp; }
    int TensExponent() const;
    void MultiplyPowTen(int power);
    static void SetBigEndian(bool be) { bigEndian = be; }
protected:
    void choose_nan(const FPF &x, const FPF &y, bool addition);
    bool IsMantissaZero() const;
    bool IsMantissaOne() const;
    void ShiftMantLeft1(u16 *carry);
    void ShiftMantRight1(u16 *carry);
    void StickyShiftRightMant(int amount);
    static void Add16Bits(u16 *carry,
                    u16 *a,
                    u16 b,
                    u16 c);
    static void Sub16Bits(u16 *borrow,
                    u16 *a,
                    u16 b,
                    u16 c);
    static FPF &AddSub(int flag, FPF &dest, const FPF &left, const FPF &right);
    static FPF &Multiply(FPF &dest, const FPF &left, const FPF &right);
    static FPF &Divide(FPF &dest, const FPF &left, const FPF &right);
    void FromLongLong(L_INT right);
    void FromUnsignedLongLong(unsigned L_INT right);
    L_INT ToLongLong() const ;
    void ToString(std::string &dest) const;
private:
        int exp;      /* Signed exponent...no bias */
        u16 mantissa[INTERNAL_FPF_PRECISION];
        u8 type;        /* Indicates, NORMAL, SUBNORMAL, etc. */
        u8 sign;        /* Mantissa sign */
        static bool bigEndian;
};

#endif

