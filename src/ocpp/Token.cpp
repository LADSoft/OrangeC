#include "UTF8.h"
#include "Token.h"

long long NumericToken::GetInteger() const
{
    if (parsedAsFloat)
    {
        return (long long)floatValue;
    }
    return intValue;
}

const FPF* NumericToken::GetFloat() const
{
    if (!parsedAsFloat)
        switch (type)
        {
            case t_int:
            case t_longint:
            case t_longlongint:
                const_cast<FPF&>(floatValue) = (long long)intValue;
                break;
            default:
                const_cast<FPF&>(floatValue) = (unsigned long long)intValue;
                break;
        }
    return &floatValue;
}

bool NumericToken::Start(const std::string& line) { return isdigit(line[0]) || (line[0] == '.' && isdigit(line[1])); }

int NumericToken::Radix36(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return INT_MAX;
}

long long NumericToken::GetBase(int b, char** ptr)
{
    long long i;
    int j;
    int errd = 0;
    i = 0;
    while ((j = Radix36(**ptr)) < b)
    {
        (*ptr)++;
        if (i > (llminus1 - j) / b)
            if (!errd)
            {
                Errors::Error("Constant too large");
                errd++;
            }
        i = i * b + j;
    }
    return i;
}

void NumericToken::GetFraction(int radix, char** ptr, FPF& floatValue)
{
    int pow = -1;
    while (Radix36(**ptr) < radix)
    {
        FPF temp, temp1;
        temp = Radix36(*(*ptr)++);
        if (radix == 10)
            temp.MultiplyPowTen(pow--);
        else
        {
            temp.SetExp(temp.GetExp() + 4 * pow--);
        }
        floatValue += temp;
    }
}

int NumericToken::GetExponent(char** ptr)
{
    bool neg = false;
    int intValue;
    if (**ptr == '-')
    {
        neg = true;
        (*ptr)++;
    }
    else
    {
        if (**ptr == '+')
            (*ptr)++;
    }
    intValue = GetBase(10, ptr);
    if (neg)
        intValue = -intValue;
    return intValue;
}

void NumericToken::GetFloating(FPF& floatValue, int radix, char** ptr)
{
    FPF fradix;
    fradix = radix;
    while (**ptr && Radix36(**ptr) < radix)
    {
        floatValue *= fradix;
        FPF temp;
        temp = Radix36(*(*ptr)++);
        floatValue += temp;
    }
    if (**ptr == '.')
    {
        (*ptr)++;
        GetFraction(radix, ptr, floatValue);
        intValue = 0;
    }
    if (**ptr == 'e' || **ptr == 'E' || **ptr == 'p' || **ptr == 'P')
    {
        (*ptr)++;
        int intValue = GetExponent(ptr);
        /* floating point too large goes to infinity... */
        if (radix == 2)
        {
            floatValue.SetExp(floatValue.GetExp() + intValue);
        }
        else
        {
            floatValue.MultiplyPowTen(intValue);
        }
    }
}
int NumericToken::GetNumber(const char** ptr)
{
    char buf[256];
    char* p = buf;
    int radix = 10;
    int floatradix = 0;
    bool hasdot = false;
    bool floating = false;
    if (!isdigit(**ptr) && **ptr != '.')
        return INT_MIN;
    if (**ptr == '.' && !isdigit(*(*ptr + 1)))
        return INT_MIN;
    if (**ptr == '0')
    {
        (*ptr)++;
        if (**ptr == 'x' || **ptr == 'X')
        {
            (*ptr)++;
            radix = 16;
        }
        else
            radix = 8;
    }
    else if (**ptr == '$')
    {
        radix = 16;
        (*ptr)++;
    }
    while (Radix36(**ptr) < radix || Radix36(**ptr) < 16)
    {
        *p++ = **ptr;
        (*ptr)++;
    }
    if (**ptr == '.')
    {
        hasdot = true;
        if (radix == 8)
            radix = 10;
        *p++ = **ptr;
        (*ptr)++;
        while (Radix36(**ptr) < radix)
        {
            *p++ = **ptr;
            (*ptr)++;
        }
    }
    if ((**ptr == 'e' || **ptr == 'E') && radix != 16)
        radix = floatradix = 10;
    else if ((**ptr == 'p' || **ptr == 'P') && radix == 16)
    {
        floatradix = 2;
    }
    else if (radix == 16 && hasdot)
    {
        Errors::Error("Invalid floating point constant");
    }

    if (floatradix)
    {
        *p++ = **ptr;
        (*ptr)++;
        if (**ptr == '-' || **ptr == '+')
        {
            *p++ = **ptr;
            (*ptr)++;
        }
        while (Radix36(**ptr) < 10)
        {
            *p++ = **ptr;
            (*ptr)++;
        }
    }

    *p = 0;
    if (!floatradix && radix != 16)
    {
        char* q = buf;
        if (**ptr == 'H' || **ptr == 'h')
        {
            radix = 16;
            (*ptr)++;
            while (*q)
                if (Radix36(*q++) >= 16)
                    return INT_MIN;
        }
    }
    p = buf;
    /* at this point the next char is any qualifier after the number*/

    if (hasdot || floatradix)
    {
        if (floatradix == 0)
            floatradix = radix;
        GetFloating(floatValue, floatradix, &p);
        floating = true;
    }
    else
    {
        if (Radix36(*p) < radix)
            intValue = GetBase(radix, &p);
        else
        {
            intValue = 0;
        }
    }
    if (!floating)
    {
        type = t_int;
        if (**ptr == 'i')
        {
            if (!ansi && (*ptr)[1] == '6' && (*ptr)[2] == '4')
            {
                (*ptr)++;
                (*ptr)++;
                (*ptr)++;
                type = t_longlongint;
            }
        }
        else if (**ptr == 'U' || **ptr == 'u')
        {
            (*ptr)++;
            type = t_unsignedint;
            if (**ptr == 'i')
            {
                if (!ansi && (*ptr)[1] == '6' && (*ptr)[2] == '4')
                {
                    (*ptr)++;
                    (*ptr)++;
                    (*ptr)++;
                    type = t_unsignedlonglongint;
                }
            }
            else if (**ptr == 'L' || **ptr == 'l')
            {
                (*ptr)++;
                type = t_unsignedlongint;
                if (c99 && (**ptr == 'L' || **ptr == 'l'))
                {
                    type = t_unsignedlonglongint;
                    (*ptr)++;
                }
            }
        }
        else if (**ptr == 'L' || **ptr == 'l')
        {
            type = t_longint;
            (*ptr)++;
            if (c99 && (**ptr == 'L' || **ptr == 'l'))
            {
                type = t_longlongint;
                (*ptr)++;
                if (**ptr == 'U' || **ptr == 'u')
                {
                    type = t_unsignedlonglongint;
                    (*ptr)++;
                }
            }

            else if (**ptr == 'U' || **ptr == 'u')
            {
                type = t_unsignedlongint;
                (*ptr)++;
            }
        }
        if (type == t_int) /* no qualifiers */
        {
            if (intValue > INT_MAX)
            {
                type = t_unsignedint;
                if (radix == 10 || (L_UINT)intValue > UINT_MAX)
                {
                    type = t_longint;
                    if (intValue > LONG_MAX)
                    {
                        type = t_unsignedlongint;
                        if (radix == 10 || (L_UINT)intValue > ULONG_MAX)
                        {
                            if (radix == 10)
                            {
                                type = t_longlongint;
                            }
                            else
                                type = t_unsignedlonglongint;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (**ptr == 'F' || **ptr == 'f')
        {
            type = t_float;
            (*ptr)++;
            floatValue.Truncate(FLT_MANT_DIG, FLT_MAX_EXP, FLT_MIN_EXP);
        }
        else if (**ptr == 'L' || **ptr == 'l')
        {
            type = t_longdouble;
            (*ptr)++;
            floatValue.Truncate(LDBL_MANT_DIG, LDBL_MAX_EXP, LDBL_MIN_EXP);
        }
        else
        {
            type = t_double;
            floatValue.Truncate(DBL_MANT_DIG, DBL_MAX_EXP, DBL_MIN_EXP);
        }
        parsedAsFloat = true;
    }
    if (TokenizerSettings::Instance()->GetSymbolCheckFunction()(*ptr, false))
    {
        Errors::Error("Invalid constant value");
        while (**ptr && TokenizerSettings::Instance()->GetSymbolCheckFunction()(*ptr, false))
        {
            int n = UTF8::CharSpan(*ptr);
            for (int i = 0; i < n && **ptr; i++)
                (*ptr)++;
        }
    }
    return type;
}
void NumericToken::Parse(std::string& line)
{
    const char* p = line.c_str();
    GetNumber(&p);
    SetChars(line.substr(0, p - line.c_str()));
    line.erase(0, p - line.c_str());
}