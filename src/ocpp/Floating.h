/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#ifndef FLOATING_H
#define FLOATING_H

#include <string>
#include <cstring>

#define INTERNAL_FPF_PRECISION ((80 / 8) / sizeof(u16))
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

#define ZERO_ZERO 0
#define ZERO_SUBNORMAL 1
#define ZERO_NORMAL 2
#define ZERO_INFINITY 3
#define ZERO_NAN 4

#define SUBNORMAL_ZERO 5
#define SUBNORMAL_SUBNORMAL 6
#define SUBNORMAL_NORMAL 7
#define SUBNORMAL_INFINITY 8
#define SUBNORMAL_NAN 9

#define NORMAL_ZERO 10
#define NORMAL_SUBNORMAL 11
#define NORMAL_NORMAL 12
#define NORMAL_INFINITY 13
#define NORMAL_NAN 14

#define INFINITY_ZERO 15
#define INFINITY_SUBNORMAL 16
#define INFINITY_NORMAL 17
#define INFINITY_INFINITY 18
#define INFINITY_NAN 19

#define NAN_ZERO 20
#define NAN_SUBNORMAL 21
#define NAN_NORMAL 22
#define NAN_INFINITY 23
#define NAN_NAN 24
#define OPERAND_ZERO 0
#define OPERAND_SUBNORMAL 1
#define OPERAND_NORMAL 2
#define OPERAND_INFINITY 3
#define OPERAND_NAN 4

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
    FPF(const FPF& n)
    {
        exp = n.exp;
        memcpy(mantissa, n.mantissa, sizeof(mantissa));
        type = n.type;
        sign = n.sign;
    }
    virtual ~FPF() {}
    void SetZero(int sign);
    void Negate() { sign = !sign; }
    void SetInfinity(int sign);
    void SetNaN();
    bool ValueIsOne() const;
    bool ValueIsZero() const;
    bool operator==(const FPF& right);
    bool operator!=(const FPF& right) { return !operator==(right); }
    bool operator>(FPF& right);
    bool operator>=(FPF& right);
    bool operator<(FPF& right) { return !operator>=(right); }
    bool operator<=(FPF& right) { return !operator>(right); }

    void Normalize();
    void Denormalize(int minimum_exponent);
    void Round();

    FPF& operator+(const FPF& right) const { return AddSub('+', *new FPF, *this, right); }
    FPF& operator+=(const FPF& right) { return AddSub('+', *this, *this, right); }
    FPF& operator-(const FPF& right) const { return AddSub('-', *new FPF, *this, right); }
    FPF& operator-=(const FPF& right) { return AddSub('+', *this, *this, right); }
    FPF& operator*(const FPF& right) const { return Multiply(*new FPF, *this, right); }
    FPF& operator*=(const FPF& right) { return Multiply(*this, *this, right); };
    FPF& operator/(const FPF& right) const { return Divide(*new FPF, *this, right); };
    FPF& operator/=(const FPF& right) { return Divide(*this, *this, right); };
    FPF& operator=(const FPF& right)
    {
        exp = right.exp;
        memcpy(mantissa, right.mantissa, sizeof(mantissa));
        sign = right.sign;
        type = right.type;
        return *this;
    }
    FPF& operator=(long long right)
    {
        FromLongLong(right);
        return *this;
    }
    FPF& operator=(unsigned long long right)
    {
        FromUnsignedLongLong(right);
        return *this;
    }
    FPF& operator=(int right)
    {
        FromLongLong(right);
        return *this;
    }
    FPF& operator=(unsigned right)
    {
        FromUnsignedLongLong(right);
        return *this;
    }

    operator long long() const { return ToLongLong(); }
    operator std::string() const
    {
        std::string rv;
        ToString(rv);
        return rv;
    }
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
    void choose_nan(const FPF& x, const FPF& y, bool addition);
    bool IsMantissaZero() const;
    bool IsMantissaOne() const;
    void ShiftMantLeft1(u16* carry);
    void ShiftMantRight1(u16* carry);
    void StickyShiftRightMant(int amount);
    static void Add16Bits(u16* carry, u16* a, u16 b, u16 c);
    static void Sub16Bits(u16* borrow, u16* a, u16 b, u16 c);
    static FPF& AddSub(int flag, FPF& dest, const FPF& left, const FPF& right);
    static FPF& Multiply(FPF& dest, const FPF& left, const FPF& right);
    static FPF& Divide(FPF& dest, const FPF& left, const FPF& right);
    void FromLongLong(long long right);
    void FromUnsignedLongLong(unsigned long long right);
    long long ToLongLong() const;
    void ToString(std::string& dest) const;

  private:
    int exp; /* Signed exponent...no bias */
    u16 mantissa[INTERNAL_FPF_PRECISION];
    u8 type; /* Indicates, NORMAL, SUBNORMAL, etc. */
    u8 sign; /* Mantissa sign */
    static bool bigEndian;
};

#endif
