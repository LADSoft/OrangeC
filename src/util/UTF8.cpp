#include "UTF8.h"
#include "ctype.h"
int UTF8::Decode(const char *str)
{
    return *str;
    /*
    if ((*str & 0x80) == 0)
    {
        return *str;
    }
    else if ((*str & 0xe0) == 0xc0)
    {
        if ((str[1] & 0xc0) == 0x80)
        {
            int rv;
            rv = ((str[0] & 0x1f) << 6) + (str[1] & 0x3f);
            return rv;
        }
    }
    else if ((*str & 0xf0) == 0xe0)
    {
        if ((str[1] & 0xc0) == 0x80 && (str[2] & 0xc0) == 0x80)
        {
            int rv;
            rv = ((str[0] & 0xf) << 12) + ((str[1] & 0x3f) << 6) + (str[2] & 0x3f);
            return rv;
        }
    }
    else if ((*str & 0xf8) == 0xf0)
    {
        if ((str[1] & 0xc0) == 0x80 && (str[2] & 0xc0) == 0x80 && (str[3] & 0xc0) == 0x80)
        {
            int rv;
            rv = ((str[0] & 0x7) << 18) + ((str[1] & 0x3f) << 12) 
                    + ((str[2] & 0x3f) << 6) + (str[3] & 0x3f);
            return rv;
        }
    }
    else
    {
        return 0;
    }
    */
}
int UTF8::Encode(char *dest, int val)
{
    dest[0] = static_cast<char>(val);
    return 1;
    /*
    if (val < 0x80)
    {
        dest[0] = val;
        return 1;
    }
    else if (val < (1 << (5 + 6)))
    {
        dest[0] = (val >> 6) | 0xc0;
        dest[1] = (val & 0x3f) | 0x80;
        return 2;
    }
    else if (val < (1 << (4 + 6 + 6)))
    {
        dest[0] = (val >> 12) | 0xe0;
        dest[1] = ((val >> 6) & 0x3f) | 0x80;
        dest[2] = (val & 0x3f) | 0x80;
        return 3;
    }
    else
    {
        dest[0] = (val >> 18) | 0xf0;
        dest[1] = ((val >> 12) & 0x3f) | 0x80;
        dest[2] = ((val >> 6) & 0x3f) | 0x80;
        dest[3] = (val & 0x3f) | 0x80;
        return 4;
    }
    */
}
int UTF8::CharSpan(const char *str)
{
	(void)str;
    return 1;
    /*
    if ((*str & 0x80) == 0)
    {
        return 1;
    }
    else if ((*str & 0xe0) == 0xc0)
    {
        return 2;
    }
    else if ((*str & 0xf0) == 0xe0)
    {
        return 3;
    }
    else if ((*str & 0xf8) == 0xf0)
    {
        return 4;
    }
    else
    {
        return 1;
    }
    */
}
int UTF8::Span(const char *str)
{
    int rv = 0;
    while (*str)
    {
        rv++;
        int n = CharSpan(str);
        for (int i=0; i < n; i++)
        {
            if (!*++str)
                break;
        }
    }
    return rv;
}
#include <fstream>
std::string UTF8::ToUpper(const std::string& val)
{
    std::string rv;
    const char *str = val.c_str();
    char buf[10];
    for (int i=0; i < val.size();)
    {
    /*
        if ((str[i] & 0x80) == 0)
        {
        (*/
            buf[0] = ToUpper(str[i]);
            buf[1] = 0;
            rv += buf;
            i++;
        /*
        }
        else
        {
            int spn = CharSpan(str+i);
            int v = Decode(str + i);
            int q = ToUpper(v);
            if (q == v)
            {
                memcpy(buf, str + i, spn);
                buf[spn] = 0;
            }
            else
            {
                buf[Encode(buf, q)] = 0;
            }
            rv += buf;
            i += spn;
        }
        */
    }
    return rv;
}
