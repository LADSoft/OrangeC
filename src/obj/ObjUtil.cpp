/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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

#define _CRT_SECURE_NO_WARNINGS  

#include "ObjUtil.h"
#include <strstream>
#include <iomanip>
#include <stdio.h>
/*
 * C++ stream objects are not used here, because these functions are called
 * a lot and the resultant memory allocations fragment memory to such an extent
 * that it really slows down linker and librarian operations...
 */
ObjString ObjUtil::ToHex(ObjInt value, int width)
{
    char buf[256], buf2[256];
    int n;
    sprintf(buf, "%X", value);
    n = strlen(buf);
    if (n < width)
    {
        for (int i=n; i < width; i++)
        {
            buf2[i-n] = '0';
        }
        strcpy(buf2 + width - n, buf);
        return buf2;
    }
    return buf;
}
ObjString ObjUtil::ToDecimal(ObjInt value, int width)
{
    char buf[256], buf2[256];
    int n;
    sprintf(buf, "%d", value);
    n = strlen(buf);
    if (n < width)
    {
        for (int i=n; i < width; i++)
        {
            buf2[i-n] = '0';
        }
        strcpy(
               buf2 + width - n, buf);
        return buf2;
    }
    return buf;
}
ObjInt ObjUtil::FromHex(const char *data, int *pos, int width)
{
    return FromInt(data, pos, width, 16);
}
ObjInt ObjUtil::FromDecimal(const char *data, int *pos, int width)
{
    return FromInt(data, pos, width, 10);
}
ObjInt ObjUtil::FromBinary(const char *data, int *pos, int width)
{
    return FromInt(data, pos, width, 2);
}
ObjInt ObjUtil::FromOctal(const char *data, int *pos, int width)
{
    return FromInt(data, pos, width, 8);
}
ObjInt ObjUtil::FromInt(const char *data, int *pos, int width, int radix)
{
    int n = 0;
    if (pos)
        n = *pos;
    if (!width)
    {
        width = 10000;
    }
    ObjInt val = 0;
    while (width && data[n])
    {
        int ch = data[n];
        if (ch >= 'a' && ch <= 'z')
            ch = ch - 'a' + 'A';
        ch -= '0';
        if (ch < 0)
            break;
        if (ch > 9)
        {
            ch -= 7;
            if (ch < 10)
                break;
        }
        if (ch >= radix)
            break;
        val *= radix;
        val += ch;
        n++, width--;
    }
    if (pos)
        (*pos) = n;
    return val;
}
