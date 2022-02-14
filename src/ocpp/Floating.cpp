/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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

#include "Utils.h"
#include "Floating.h"
#include <iostream>
#include <fstream>
#include <climits>

FPF FPF::tensTab[10];
bool FPF::initted;

bool FPF::bigEndian = false;

void FPF::Init()
{
    if (!initted)
    {
        initted = true;
        int i;
        tensTab[0].mantissa[0] = 0xa000;
        for (i = 1; i < INTERNAL_FPF_PRECISION; i++)
            tensTab[0].mantissa[i] = 0;
        tensTab[0].exp = 4;
        tensTab[0].type = IFPF_IS_NORMAL;
        for (i = 1; i < 10; i++)
        {
            tensTab[i] = tensTab[0] * tensTab[i - 1];
        }
    }
}

void FPF::SetZero(int sign)
{
    type = IFPF_IS_ZERO;
    this->sign = sign;
    exp = MIN_EXP;
    memset(mantissa, 0, sizeof(mantissa));
}
void FPF::SetInfinity(int sign)
{
    type = IFPF_IS_INFINITY;
    this->sign = sign;
    exp = MIN_EXP;
    memset(mantissa, 0, sizeof(mantissa));
}
void FPF::SetNaN()
{
    type = IFPF_IS_NAN;
    exp = MAX_EXP;
    sign = 1;
    memset(mantissa, 0, sizeof(mantissa));
    mantissa[0] = 0x4000;
}

bool FPF::ValueIsOne() const
{
    if (type != IFPF_IS_NORMAL)
        return false;
    if (exp != 1)
        return false;
    //    if (sign)
    //        return false;
    return IsMantissaOne();
}
bool FPF::ValueIsZero() const { return (type == IFPF_IS_ZERO); }
bool FPF::operator==(const FPF& right)
{
    if (type != right.type)
        return false;
    if (sign != right.sign)
        return false;
    if (type == IFPF_IS_INFINITY || type == IFPF_IS_ZERO)
        return true;
    if (type == IFPF_IS_NAN)
        return false;
    if (exp != right.exp)
        return false;
    return !memcmp(mantissa, right.mantissa, sizeof(mantissa));
}
bool FPF::operator>(FPF& right)
{
    if (type == IFPF_IS_NAN || right.type == IFPF_IS_NAN)
        return false;
    if (type == IFPF_IS_ZERO && right.type == IFPF_IS_ZERO)
        return false;
    if (sign && !right.sign)
        return false;
    if (!sign && right.sign)
        return true;
    if (right.type == IFPF_IS_INFINITY && type == IFPF_IS_INFINITY)
        return false;
    if (sign)
    {
        if (right.type == IFPF_IS_INFINITY)
            return true;
        if (exp < right.exp)
            return true;
        if (exp > right.exp)
            return false;
        return (memcmp(mantissa, right.mantissa, sizeof(mantissa)) < 0);
    }
    if (type == IFPF_IS_INFINITY)
        return true;
    if (exp > right.exp)
        return true;
    if (exp < right.exp)
        return false;
    return (memcmp(mantissa, right.mantissa, sizeof(mantissa)) > 0);
}
bool FPF::operator>=(FPF& right) { return *this > right || *this == right; }
bool FPF::IsMantissaZero() const
{

    int n = 0;
    for (int i = 0; i < INTERNAL_FPF_PRECISION; i++)
        n |= mantissa[i];

    return (!n);
}
bool FPF::IsMantissaOne() const
{
    if (mantissa[0] != 0x8000)
        return false;
    for (int i = 1; i < INTERNAL_FPF_PRECISION; i++)
        if (mantissa[i])
            return false;
    return true;
}

void FPF::Add16Bits(u16f* carry, u16f* a, u16f b, u16f c)
{

    u32f accum = b + c + *carry;
    *carry = (accum >> 16) & 1;
    *a = (u16f)accum;
}
void FPF::Sub16Bits(u16f* borrow, u16f* a, u16f b, u16f c)
{
    u32f accum = (u32f)b - (u32f)c - (u32f)*borrow;
    *borrow = (accum >> 16) & 1;
    *a = (u16f)accum;
}

void FPF::ShiftMantLeft1(u16f* carry)
{

    for (int i = INTERNAL_FPF_PRECISION - 1; i >= 0; i--)
    {
        u16f accum = mantissa[i];
        u16f new_carry = accum >> 15;
        accum <<= 1;
        accum |= *carry;
        *carry = new_carry;
        mantissa[i] = accum;
    }
}

void FPF::ShiftMantRight1(u16f* carry)
{

    for (int i = 0; i < INTERNAL_FPF_PRECISION; i++)
    {
        u16f accum = mantissa[i];
        u16f new_carry = accum & 1;
        accum = accum >> 1;
        accum |= *carry << 15;
        *carry = new_carry;
        mantissa[i] = accum;
    }
}
void FPF::StickyShiftRightMant(int amount)
{
    u16f carry; /* Self-explanatory */
    if (type != IFPF_IS_ZERO)
    {
        if (amount >= INTERNAL_FPF_PRECISION * 16)
        {
            memset(mantissa, 0, sizeof(mantissa));
            mantissa[INTERNAL_FPF_PRECISION - 1] = 1;
        }
        else
        {
            for (int i = 0; i < amount; i++)
            {
                carry = 0;
                ShiftMantRight1(&carry);
                mantissa[INTERNAL_FPF_PRECISION - 1] |= carry;
            }
        }
    }
}

void FPF::Normalize()
{
    if (type == IFPF_IS_ZERO || type == IFPF_IS_INFINITY || type == IFPF_IS_NAN)
        return;

    while ((mantissa[0] & 0x8000) == 0)
    {
        u16f carry = 0;
        ShiftMantLeft1(&carry);
        exp--;
    }
    return;
}

void FPF::Denormalize(int minimum_exponent)
{
    int diff;

    if (IsMantissaZero())
    {
        std::cout << "Error:  zero significand in denormalize" << std::endl;
    }

    diff = exp - minimum_exponent;
    if (diff < 0)
    {
        diff = -diff;
        if (diff >= (INTERNAL_FPF_PRECISION * 16))
        {
            SetZero(sign);
        }
        else
        {
            exp += diff;
            StickyShiftRightMant(diff);
        }
    }
}
void FPF::Round()
{
    if (type == IFPF_IS_NORMAL || type == IFPF_IS_SUBNORMAL)
    {
        Denormalize(MIN_EXP);
        if (type != IFPF_IS_ZERO)
        {
            if (IsMantissaZero())
                SetZero(sign);
            if (exp > MAX_EXP)
            {
                SetInfinity(sign);
            }
        }
    }
}

void FPF::choose_nan(const FPF& x, const FPF& y, bool addition)
{
    for (int i = 0; i < INTERNAL_FPF_PRECISION; i++)
    {
        if (x.mantissa[i] > y.mantissa[i])
        {
            *this = x;
            return;
        }
        if (x.mantissa[i] < y.mantissa[i])
        {
            *this = y;
            return;
        }
    }
    if (addition)
        *this = y;
    else
        *this = x;
}

FPF FPF::AddSub(int mode, const FPF& x, const FPF& y)
{
    int diff;
    FPF dest;
    FPF locx, locy; /* Needed since we alter them */

    /*
    ** Following big switch statement handles the
    ** various combinations of operand types.
    */
    switch ((x.type * IFPF_TYPE_COUNT) + y.type)
    {
        case ZERO_ZERO:
            dest = x;
            if (x.sign ^ y.sign ^ (mode == '-'))
            {
                dest.sign = 0; /* positive */
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
            dest = x;
            break;

        case ZERO_NAN:
        case SUBNORMAL_NAN:
        case NORMAL_NAN:
        case INFINITY_NAN:
            dest = y;
            break;

        case ZERO_SUBNORMAL:
        case ZERO_NORMAL:
        case ZERO_INFINITY:
        case SUBNORMAL_INFINITY:
        case NORMAL_INFINITY:
            dest = y;
            dest.sign ^= (mode == '-');
            break;

        case SUBNORMAL_SUBNORMAL:
        case SUBNORMAL_NORMAL:
        case NORMAL_SUBNORMAL:
        case NORMAL_NORMAL:
            locx = x;
            locy = y;

            diff = x.exp - y.exp;
            if (diff == 0)
            {
                if (x.type == IFPF_IS_SUBNORMAL || y.type == IFPF_IS_SUBNORMAL)
                    dest.type = IFPF_IS_SUBNORMAL;
                else
                    dest.type = IFPF_IS_NORMAL;

                dest.sign = x.sign;
                dest.exp = x.exp;
            }
            else
            {
                if (diff > 0)
                {
                    locy.StickyShiftRightMant(diff);
                    dest.type = x.type;
                    dest.sign = x.sign;
                    dest.exp = x.exp;
                }
                else /* if (diff < 0) */
                {
                    locx.StickyShiftRightMant(-diff);
                    dest.type = y.type;
                    dest.sign = y.sign ^ (mode == '-');
                    dest.exp = y.exp;
                }
            }
            if (locx.sign ^ locy.sign ^ (mode == '-'))
            {
                u16f borrow = 0;
                for (int i = (INTERNAL_FPF_PRECISION - 1); i >= 0; i--)
                    Sub16Bits(&borrow, &dest.mantissa[i], locx.mantissa[i], locy.mantissa[i]);

                if (borrow)
                {
                    dest.sign = locy.sign ^ (mode == '-');
                    borrow = 0;
                    for (int i = (INTERNAL_FPF_PRECISION - 1); i >= 0; i--)
                        Sub16Bits(&borrow, &dest.mantissa[i], 0, dest.mantissa[i]);
                }

                if (dest.IsMantissaZero())
                {
                    dest.type = IFPF_IS_ZERO;
                    dest.sign = 0; /* positive */
                }
                else if (locx.type == IFPF_IS_NORMAL || locy.type == IFPF_IS_NORMAL)
                {
                    dest.Normalize();
                }
            }
            else
            {
                u16f carry = 0;
                for (int i = (INTERNAL_FPF_PRECISION - 1); i >= 0; i--)
                    Add16Bits(&carry, &dest.mantissa[i], locx.mantissa[i], locy.mantissa[i]);

                if (carry)
                {
                    dest.exp++;
                    carry = 0;
                    dest.ShiftMantRight1(&carry);
                    dest.mantissa[0] |= 0x8000;
                    dest.type = IFPF_IS_NORMAL;
                }
                else if (dest.mantissa[0] & 0x8000)
                    dest.type = IFPF_IS_NORMAL;
            }
            break;

        case INFINITY_INFINITY:
            dest.SetNaN();
            break;
        case NAN_NAN:
            dest.choose_nan(x, y, true);
            break;
    }

    dest.Round();
    return dest;
}

FPF FPF::Multiply(const FPF& x, const FPF& y)
{
    FPF dest;
    FPF temp, locy, locx;

    /*
    ** As in the preceding function, this large switch
    ** statement selects among the many combinations
    ** of operands.
    */
    switch ((x.type * IFPF_TYPE_COUNT) + y.type)
    {
        case INFINITY_SUBNORMAL:
        case INFINITY_NORMAL:
        case INFINITY_INFINITY:
        case ZERO_ZERO:
        case ZERO_SUBNORMAL:
        case ZERO_NORMAL:
            dest = x;
            dest.sign ^= y.sign;
            break;

        case SUBNORMAL_INFINITY:
        case NORMAL_INFINITY:
        case SUBNORMAL_ZERO:
        case NORMAL_ZERO:
            dest = y;
            dest.sign ^= x.sign;
            break;
        case ZERO_INFINITY:
        case INFINITY_ZERO:
            dest.SetNaN();
            break;

        case NAN_ZERO:
        case NAN_SUBNORMAL:
        case NAN_NORMAL:
        case NAN_INFINITY:
            dest = x;
            break;
        case ZERO_NAN:
        case SUBNORMAL_NAN:
        case NORMAL_NAN:
        case INFINITY_NAN:
            dest = y;
            break;
        case SUBNORMAL_SUBNORMAL:
        case SUBNORMAL_NORMAL:
        case NORMAL_SUBNORMAL:
        case NORMAL_NORMAL:
            locy = y;
            locx = x;
            if (x.IsMantissaZero() || y.IsMantissaZero())
                dest.SetInfinity(0);
            if (x.type == IFPF_IS_SUBNORMAL || y.type == IFPF_IS_SUBNORMAL)
                dest.type = IFPF_IS_SUBNORMAL;
            else
                dest.type = IFPF_IS_NORMAL;
            dest.sign = x.sign ^ y.sign;
            dest.exp = x.exp + y.exp;
            memset(dest.mantissa, 0, sizeof(dest.mantissa));

            for (int i = 0; i < (INTERNAL_FPF_PRECISION * 16); i++)
            {
                u16f carry = 0;
                locy.ShiftMantRight1(&carry);
                if (carry)
                {
                    carry = 0;
                    for (int j = (INTERNAL_FPF_PRECISION - 1); j >= 0; j--)
                        Add16Bits(&carry, &dest.mantissa[j], dest.mantissa[j], locx.mantissa[j]);
                }
                else
                {
                    carry = 0;
                }
                dest.ShiftMantRight1(&carry);
                temp.ShiftMantRight1(&carry);
            }
            while ((dest.mantissa[0] & 0x8000) == 0)
            {
                u16f carry = 0;
                temp.ShiftMantLeft1(&carry);
                dest.ShiftMantLeft1(&carry);
                dest.exp--;
            }
            if (!temp.IsMantissaZero())
            {
                dest.mantissa[INTERNAL_FPF_PRECISION - 1] |= 1;
            }
            break;

        case NAN_NAN:
            dest.choose_nan(x, y, false);
            break;
    }

    dest.Round();
    return dest;
}

FPF FPF::Divide(const FPF& x, const FPF& y)
{
    FPF dest;
    FPF temp, locx;

    switch ((x.type * IFPF_TYPE_COUNT) + y.type)
    {
        case ZERO_ZERO:
        case INFINITY_INFINITY:
            dest.SetNaN();
            break;

        case ZERO_SUBNORMAL:
        case ZERO_NORMAL:
            if (y.IsMantissaZero())
            {
                dest.SetNaN();
                break;
            }
            /* fall through */
        case ZERO_INFINITY:
        case SUBNORMAL_INFINITY:
        case NORMAL_INFINITY:
            dest.SetZero(x.sign ^ y.sign);
            break;

        case SUBNORMAL_ZERO:
        case NORMAL_ZERO:
            if (x.IsMantissaZero())
            {
                dest.SetNaN();
                break;
            }
            // fallthrough
        case INFINITY_ZERO:
        case INFINITY_SUBNORMAL:
        case INFINITY_NORMAL:
            dest.SetInfinity(x.sign ^ y.sign);
            break;

        case NAN_ZERO:
        case NAN_SUBNORMAL:
        case NAN_NORMAL:
        case NAN_INFINITY:
            dest = x;
            break;

        case ZERO_NAN:
        case SUBNORMAL_NAN:
        case NORMAL_NAN:
        case INFINITY_NAN:
            dest = y;
            break;

        case SUBNORMAL_SUBNORMAL:
        case NORMAL_SUBNORMAL:
        case SUBNORMAL_NORMAL:
        case NORMAL_NORMAL:
            locx = x;
            if (x.IsMantissaZero())
            {
                if (y.IsMantissaZero())
                    dest.SetNaN();
                else
                    dest.SetZero(0);
                break;
            }
            if (y.IsMantissaZero())
            {
                dest.SetInfinity(x.sign ^ y.sign);
                break;
            }
            dest.type = x.type;
            dest.sign = x.sign ^ y.sign;
            dest.exp = x.exp - y.exp + ((INTERNAL_FPF_PRECISION * 16 * 2));
            memset(dest.mantissa, 0, sizeof(dest.mantissa));
            // relies on constructor setting temp.mantissa to 0
            while ((dest.mantissa[0] & 0x8000) == 0)
            {
                bool do_sub = true;
                u16f carry = 0;
                locx.ShiftMantLeft1(&carry);
                temp.ShiftMantLeft1(&carry);
                if (carry == 0)
                    for (int j = 0; j < INTERNAL_FPF_PRECISION; j++)
                    {
                        if (y.mantissa[j] > temp.mantissa[j])
                        {
                            carry = 0;
                            do_sub = false;
                            break;
                        }
                        if (y.mantissa[j] < temp.mantissa[j])
                            break;
                    }
                if (do_sub)
                {
                    carry = 0;
                    for (int i = (INTERNAL_FPF_PRECISION - 1); i >= 0; i--)
                        Sub16Bits(&carry, &temp.mantissa[i], temp.mantissa[i], y.mantissa[i]);
                    carry = 1;
                }
                dest.ShiftMantLeft1(&carry);
                dest.exp--;
            }
            break;

        case NAN_NAN:
            dest.choose_nan(x, y, false);
            break;
    }

    dest.Round();
    return dest;
}
void FPF::FromLongLong(long long myllong)
{
    u16f myword; /* Used to hold converted stuff */
    unsigned long long temp = (unsigned long long)myllong;
    if (myllong < 0L)
    {
        sign = 1;
        if (myllong != LLONG_MIN)
            temp = (unsigned long long)-myllong;
    }
    else
    {
        sign = 0;
    }
    type = IFPF_IS_NORMAL;
    memset(mantissa, 0, sizeof(mantissa));

    /*
    ** See if we've got a zero.  If so, make the resultant FP
    ** number a true zero and go home.
    */
    if (myllong == 0)
    {
        type = IFPF_IS_ZERO;
        exp = 0;
    }
    else
    {
        exp = 64;
#ifdef LLONG_MAX
        myword = (u16f)((temp >> 48) & 0xFFFFL);
        mantissa[0] = myword;
        myword = (u16f)((temp >> 32) & 0xFFFFL);
        mantissa[1] = myword;
#endif
        myword = (u16f)((temp >> 16) & 0xFFFFL);
        mantissa[2] = myword;
        myword = (u16f)(temp & 0xFFFFL);
        mantissa[3] = myword;
        Normalize();
    }
}
void FPF::FromUnsignedLongLong(unsigned long long myllong)
{
    u16f myword; /* Used to hold converted stuff */
    sign = 0;
    type = IFPF_IS_NORMAL;
    memset(mantissa, 0, sizeof(mantissa));

    /*
    ** See if we've got a zero.  If so, make the resultant FP
    ** number a true zero and go home.
    */
    if (myllong == 0)
    {
        type = IFPF_IS_ZERO;
        exp = 0;
    }
    else
    {
        exp = 64;
#ifdef LLONG_MAX
        myword = (u16f)((myllong >> 48) & 0xFFFFL);
        mantissa[0] = myword;
        myword = (u16f)((myllong >> 32) & 0xFFFFL);
        mantissa[1] = myword;
#endif
        myword = (u16f)((myllong >> 16) & 0xFFFFL);
        mantissa[2] = myword;
        myword = (u16f)(myllong & 0xFFFFL);
        mantissa[3] = myword;
        Normalize();
    }
}
/* converts the exponent to base ten, doing truncation rounding
 * we are using real floating point here but not depending on the format
 */
#define LB2_10 (M_LN10 / M_LN2)
int FPF::TensExponent() const { return (int)((float)exp / LB2_10); }
/* multiply by a power of ten */
void FPF::MultiplyPowTen(int power)
{
    FPF temp, mul;
    int i;
    Init();
    temp.sign = 0;
    switch (type)
    {
        case IFPF_IS_ZERO:
        case IFPF_IS_NAN:
        case IFPF_IS_INFINITY:
            break;
        default:
            if (power < 0)
            { /* constant 0.2 */
                if (power < -20)
                {
                    exp += power;
                    for (i = 0; i < INTERNAL_FPF_PRECISION; i++)
                        temp.mantissa[i] = 0xCCCC;
                    power = -power;
                    temp.exp = -2;
                    temp.type = IFPF_IS_NORMAL;
                }
                else
                {
                    power = -power;
                    if (power > 10)
                    {
                        temp = *this / tensTab[9];
                        *this = temp;
                        power -= 10;
                    }
                    temp = *this / tensTab[power - 1];
                    *this = temp;
                    return;
                }
            }
            else if (power > 0)

            { /* constant 5 */
                exp += power;
                temp.mantissa[0] = 0xa000;
                temp.exp = 3;
                temp.type = IFPF_IS_NORMAL;
            }
            else
            {
                return;  // 10 ^ 0 = 1, nop;
            }
            mul = temp;
            while (power)
            {
                FPF internal;
                if (power & 1)
                {
                    internal = *this * mul;
                    *this = internal;
                }
                internal = mul * mul;
                mul = internal;
                power >>= 1;
            }
            Round();
            break;
    }
}
void FPF::ToString(std::string& dest) const
{
    dest.erase();
    if (type == IFPF_IS_NAN)
    {
        if (sign)
            dest += "-";
        dest += "nan";
    }
    else if (type == IFPF_IS_INFINITY)
    {
        if (sign)
            dest += "-";
        dest += "inf";
    }
    else if (type == IFPF_IS_ZERO)
    {
        if (sign)
            dest += "-";
        dest = "0.0";
    }
    else
    {
        FPF temp = *this;
        int power = temp.TensExponent();
        temp.MultiplyPowTen(-power);
        if (temp.sign)
            dest += "-";
        if (temp.exp > 0)
        {
            int val = 0;
            while (temp.exp--)
            {
                u16f carry = 0;
                temp.ShiftMantLeft1(&carry);
                val <<= 1;
                val |= carry;
            }
            dest.insert(dest.size(), 1, val + '0');
            dest += ".";
        }
        else
        {
            dest += "0.";
            while (temp.exp++)
            {
                u16f carry = 0;
                temp.ShiftMantRight1(&carry);
            }
        }
        int c = 0;
        if (dest.size() >= 3 && dest[dest.size() - 2] == '0')
            c = 1;
        for (int i = 0; i < 24 - c; i++)
        {
            u16f carry = 0;
            u16f val = 0;
            temp.ShiftMantLeft1(&carry);
            val <<= 1;
            val |= carry;
            u16f val1 = val;
            FPF temp1 = temp;
            carry = 0;
            temp.ShiftMantLeft1(&carry);
            val <<= 1;
            val |= carry;
            carry = 0;
            temp.ShiftMantLeft1(&carry);
            val <<= 1;
            val |= carry;
            val += val1;
            carry = 0;
            for (int j = INTERNAL_FPF_PRECISION - 1; j >= 0; j--)
            {
                Add16Bits(&carry, temp.mantissa + j, temp1.mantissa[j], temp.mantissa[j]);
            }
            val += carry;
            dest.insert(dest.size(), 1, val + '0');
        }
        if (dest[dest.size() - 1] > '4')
        {
            int n = dest.size() - 2;
            while (n >= 0)
            {
                if (dest[n] < '0')
                {
                    n--;
                    continue;
                }
                dest[n]++;
                if (dest[n] != '9' + 1)
                    break;
                dest[n--] = '0';
            }
        }

        if (power)
        {
            if (power < 0)
            {
                power = -power;
                dest += "E-";
            }
            else
            {
                dest += "E+";
            }

            dest += Utils::NumberToString(power);
        }
    }
}

long long FPF::ToLongLong() const
{
    FPF stemp = *this;
    int exp = stemp.exp;
    long long rv = 0;
#ifdef LLONG_MAX
    int maxExp = 63;
#else
    int maxExp = 31;
#endif
    if (stemp.type == IFPF_IS_ZERO)
        return 0;
    switch (stemp.type)
    {
        case IFPF_IS_INFINITY:
        case IFPF_IS_NAN:
#ifdef LLONG_MAX
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
            if (stemp.exp < 0)
                return 0;
            if (stemp.exp > maxExp)
            {
#ifdef LLONG_MAX
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
            }
            while (stemp.exp++ != 64)
            {
                u16f carry = 0;
                stemp.ShiftMantRight1(&carry);
            }
            for (int i = 0; i < 4; i++)
            {
                rv = rv << 16;
                rv |= stemp.mantissa[i];
            }
            if (stemp.sign)
                rv = -rv;
            break;
    }
    return rv;
}
void FPF::ToFloat(uchar dest[]) const
{
    u32f val;
    if (type == IFPF_IS_ZERO)
        val = 0;
    else
    {
        if (type == IFPF_IS_INFINITY)
            val = 0x7f800000;
        else if (type == IFPF_IS_NAN)
            val = 0x7fc00000;
        else
        {
            if (exp <= -126)
            {
                val = ((mantissa[0]) << 7) + (mantissa[1] >> 9);
            }
            else
            {
                val = ((mantissa[0] & 0x7fff) << 8) + (mantissa[1] >> 8);
            }
            if (exp > 129 || exp < -126)
            {
                if (exp < -126 && exp >= -149)
                {
                    int n = -(exp + 126);
                    u32f r = !!(val & (1 << (n - 1)));
                    val >>= n;
                    val += r;
                }
                else
                    printf("FPFToFloat: invalid exponent");
            }
            else
                val |= (exp + 126) << 23;
        }
    }
    if (sign)
        val |= 0x80000000L;
    if (bigEndian)
    {
        dest[0] = val >> 24;
        dest[1] = (val >> 16) & 0xff;
        dest[2] = (val >> 8) & 0xff;
        dest[3] = (val >> 0) & 0xff;
    }
    else
    {
        dest[3] = val >> 24;
        dest[2] = (val >> 16) & 0xff;
        dest[1] = (val >> 8) & 0xff;
        dest[0] = (val >> 0) & 0xff;
    }
}
void FPF::ToDouble(uchar dest[]) const
{
    u32f val[2];
    if (type == IFPF_IS_ZERO)
        val[0] = val[1] = 0;
    else
    {
        if (type == IFPF_IS_INFINITY)
            val[0] = 0x7ff00000, val[1] = 0;
        else if (type == IFPF_IS_NAN)
            val[0] = 0x7ff80000, val[1] = 0;
        else
        {
            if (exp <= -1022)
            {
                val[0] = ((mantissa[0]) << 4) + (mantissa[1] >> 12);
                val[1] = (mantissa[1] << 20) + (mantissa[2] << 4) + (mantissa[3] >> 12);
            }
            else
            {
                val[0] = ((mantissa[0] & 0x7fff) << 5) + (mantissa[1] >> 11);
                val[1] = (mantissa[1] << 21) + (mantissa[2] << 5) + (mantissa[3] >> 11);
            }
            if (exp > 1025 || exp < -1022)
            {
                if (exp < -1022 && exp >= -1074)
                {
                    int n = -(exp + 1022);
                    u32f c = val[0], r, t;
                    if (n < 32)
                    {
                        r = !!(val[1] & (1 << (n - 1)));
                        val[0] >>= n;
                        val[1] >>= n;
                        val[1] |= (c & ~(1 << n)) << (32 - n);
                    }
                    else
                    {
                        if (n == 32)
                            r = !!(val[1] & (1 << 31));
                        else
                            r = !!(c & (1 << (n - 33)));
                        val[0] = 0;
                        val[1] = c >> (n - 32);
                    }
                    t = val[1];
                    t += r;
                    if (t < val[1])
                        val[0]++;
                    val[1] = t;
                }
                else
                    printf("FPFToDouble: invalid exponent");
            }
            else
                val[0] |= (exp + 1022) << 20;
        }
    }
    if (sign)
        val[0] |= 0x80000000L;
    if (bigEndian)
    {
        dest[0] = val[0] >> 24;
        dest[1] = (val[0] >> 16) & 0xff;
        dest[2] = (val[0] >> 8) & 0xff;
        dest[3] = (val[0] >> 0) & 0xff;
        dest[4] = val[1] >> 24;
        dest[5] = (val[1] >> 16) & 0xff;
        dest[6] = (val[1] >> 8) & 0xff;
        dest[7] = (val[1] >> 0) & 0xff;
    }
    else
    {
        dest[7] = val[0] >> 24;
        dest[6] = (val[0] >> 16) & 0xff;
        dest[5] = (val[0] >> 8) & 0xff;
        dest[4] = (val[0] >> 0) & 0xff;
        dest[3] = val[1] >> 24;
        dest[2] = (val[1] >> 16) & 0xff;
        dest[1] = (val[1] >> 8) & 0xff;
        dest[0] = (val[1] >> 0) & 0xff;
    }
}
void FPF::ToLongDouble(uchar dest[]) const
{
    u32f val[3];
    /* have to or in the high bit in case infinity or nan*/
    if (type == IFPF_IS_ZERO)
        val[0] = val[1] = val[2] = 0;
    else
    {
        if (type == IFPF_IS_INFINITY)
            val[0] = 0x7fff, val[1] = 0x80000000, val[2] = 0;
        else if (type == IFPF_IS_NAN)
        {
            val[0] = 0x7fff;
            val[1] = 0xc0000000;
            val[2] = 0;
        }
        else
        {
            val[0] = 0;
            val[1] = (mantissa[0] << 16) + mantissa[1];
            val[2] = (mantissa[2] << 16) + mantissa[3];
            if (exp > 16385 || exp < -16382)
                if (exp < -16382 && exp >= -16446)
                {
                    int n = -(exp + 16381);
                    u32f c = val[1], r, t;
                    if (n < 32)
                    {
                        r = !!(val[2] & (1 << (n - 1)));
                        val[1] >>= n;
                        val[2] >>= n;
                        val[2] |= (c & ~(1 << n)) << (32 - n);
                    }
                    else if (n < 64)
                    {
                        if (n == 32)
                            r = !!(val[2] & (1 << 31));
                        else
                            r = !!(c & (1 << (n - 33)));
                        val[1] = 0;
                        val[2] = c >> (n - 32);
                    }
                    else
                    {
                        if (n == 64)
                            r = !!(c & (1 << 31));
                        else
                            r = 0;
                        val[1] = 0;
                        val[2] = 0;
                    }
                    t = val[2];
                    t += r;
                    if (t < val[2])
                        val[1]++;
                    val[2] = t;
                }
                else
                    printf("FPFToLongDouble: invalid exponent");
            else
                val[0] = exp + 16382;
        }
    }
    if (sign)
        val[0] |= 0x8000L;

    if (bigEndian)
    {
        dest[0] = val[0] >> 8;
        dest[1] = val[0] & 0xff;
        dest[2] = val[1] >> 24;
        dest[3] = (val[1] >> 16) & 0xff;
        dest[4] = (val[1] >> 8) & 0xff;
        dest[5] = (val[1] >> 0) & 0xff;
        dest[6] = val[2] >> 24;
        dest[7] = (val[2] >> 16) & 0xff;
        dest[8] = (val[2] >> 8) & 0xff;
        dest[9] = (val[2] >> 0) & 0xff;
    }
    else
    {
        dest[9] = val[0] >> 8;
        dest[8] = val[0] & 0xff;
        dest[7] = val[1] >> 24;
        dest[6] = (val[1] >> 16) & 0xff;
        dest[5] = (val[1] >> 8) & 0xff;
        dest[4] = (val[1] >> 0) & 0xff;
        dest[3] = val[2] >> 24;
        dest[2] = (val[2] >> 16) & 0xff;
        dest[1] = (val[2] >> 8) & 0xff;
        dest[0] = (val[2] >> 0) & 0xff;
    }
}
void FPF::Truncate(int bits, int maxexp, int minexp)
{
    switch (type)
    {
        case IFPF_IS_NAN:
        case IFPF_IS_INFINITY:
            return;
        case IFPF_IS_ZERO:
            return;
        case IFPF_IS_NORMAL:
        case IFPF_IS_SUBNORMAL:
            Normalize();
            if (exp > maxexp)
                SetInfinity(sign);
            else if (exp < minexp)
            {
                if (exp + bits < minexp)
                    SetZero(0);
                else
                {
                    type = IFPF_IS_SUBNORMAL;
                }
            }
            else
            {
                int i, v;
                bool rounding = false;
                int mask = 0x8000, mask2 = 0x8000;
                for (i = 1; i < bits % 16; i++)
                {
                    mask >>= 1;
                    mask2 >>= 1;
                    mask |= 0x8000;
                }
                if (mantissa[bits / 16] & mask2)
                    rounding = true;
                mantissa[bits / 16] &= mask;
                v = mask2;

                if (rounding)
                {
                    int n;

                    /* do an add of 1 to the LSB */
                    for (i = bits / 16; i >= 0; i--)
                    {
                        n = mantissa[i] + v;
                        mantissa[i] = n;
                        if (n < 0x10000)
                            break;
                        v = 1;
                    }
                    /* overflow????? */
                    if (n >= 0x10000)
                    {
                        exp++;
                        /* exponent too large ? */
                        if (exp >= maxexp)
                        {
                            /* yes leave it alone */
                            SetInfinity(sign);
                        }
                        else
                        {
                            /* else shift mantissa right and increment exponent... */
                            u16f carry = 1; /* high bit should be one */
                            ShiftMantRight1(&carry);
                        }
                    }
                }
            }
            break;
    }
}
#ifdef TEST
int main()
{
    int i;
    char buf[256];
    float f;
    double d;
    long double l;
    FPF one, two, three;
    int val;
    long long aa;
    LongLongToFPF(&one, -1976543);
    LongLongToFPF(&two, 100000000);
    DivideFPF(&one, &two, &three);
    /*    val = FPFTensExponent(&three);*/
    /*    printf("%d\n",val);*/
    /*    FPFMultiplyPowTen(&one, -2);*/
    /*    printf("%d\n",memcmp(&one,&three,sizeof(one)));*/
    FPFToString(buf, &three);

    printf("%s\n", buf);
    FPFToFloat(&f, &one);
    FPFToDouble(&d, &one);
    FPFToLongDouble(&l, &one);
    aa = FPFToLongLong(&one);
    printf("%f %f %Lf %ld", f, d, l, aa);
    for (i = 0; i < INTERNAL_FPF_PRECISION; i++)
        printf("%x ", one.mantissa[i]);
    printf("\n%d\n", one.type);
    FPFTruncate(&one, 10, 4, -4);
    for (i = 0; i < INTERNAL_FPF_PRECISION; i++)
        printf("%x ", one.mantissa[i]);
    printf("\n%d\n", one.type);
    LongLongToFPF(&one, 1);
    printf("%d\n", ValueIsOne(&one));
    printf("%d\n", FPFGTE(&one, &two));
    printf("%d\n", FPFGTE(&two, &one));
}
#endif
