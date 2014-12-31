/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
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
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

char * _RTL_FUNC ecvt(double val, int len, int *decimal, int *sign)
{
    char *ecvtbuf = __getRtlData()->ecvt_buf;
    char *p = ecvtbuf, *q;
    int i;
    if (!len || !decimal || !sign)
    {
        errno = EINVAL;
        return NULL;
    }
    memset(ecvtbuf, 0, 100);
    if (len > 60)
        len = 60;
    sprintf(ecvtbuf, "%.*e", len-1, val);
    if (*p == '-')
    {
        *sign = 1;
        p++;
    }
    else
        *sign = 0;
    *decimal = 0;
    for (i=0; i < 100; i++)
    {
        if (p[i] == '.')
        {
            *decimal = i;
            strcpy(p+i, p+i+1);
            break;
        }
    }
    q = strchr(ecvtbuf, 'e');
    if (q)
    {
        *decimal += atoi(q+1);
        *q = 0;
    }
    return p;
}
char * _RTL_FUNC fcvt(double val, int len, int *decimal, int *sign)
{
    char *ecvtbuf = __getRtlData()->ecvt_buf;
    char *p = ecvtbuf, *q;
    int i;
    if (!len || !decimal || !sign)
    {
        errno = EINVAL;
        return NULL;
    }
    memset(ecvtbuf, 0, 100);
    if (len > 60)
        len = 60;
    if ((int)val)
        sprintf(ecvtbuf, "%.*f", len, val);
    else
        sprintf(ecvtbuf, "%.*e", len-1, val);
    if (*p == '-')
    {
        *sign = 1;
        p++;
    }
    else *sign = 0;
    *decimal = 0;
    for (i=0; i < 100; i++)
    {
        if (p[i] == '.')
        {
            *decimal = i;
            strcpy(p+i, p+i+1);
            break;
        }
    }
    q = strchr(ecvtbuf, 'e');
    if (q)
    {
        *decimal += atoi(q+1);
        *q = 0;
    }
    return p;
}
char * _RTL_FUNC gcvt(double val, int len, char *result)
{
    if (!len || !result)
    {
        errno = EINVAL;
        return NULL;
    }
    sprintf(result,"%.*g", len, val);
    return result;
}

char * _RTL_FUNC _ecvt(double val, int len, int *decimal, int *sign)
{
    return ecvt(val, len, decimal, sign);
}
char * _RTL_FUNC _fcvt(double val, int len, int *decimal, int *sign)
{
    return fcvt(val, len, decimal, sign);
}
char * _RTL_FUNC _gcvt(double val, int len, char * result)
{
    return gcvt(val, len, result);
}
