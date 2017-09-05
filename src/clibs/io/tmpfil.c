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
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <dir.h>
#include "io.h"
#include "libp.h"

char *_RTL_FUNC tmpnam(char *buf)
{
    int n = 0;
    if (!buf)
        buf = &__getRtlData()->tmpfilnam;
    __ll_enter_critical();
    do
    {
        sprintf(buf, "tmp%05d.$$$", n++);
    }
    while (!access(buf, 0) && n <= TMP_MAX);
    __ll_exit_critical();
    if (n <= TMP_MAX)
        return buf;
    return NULL;		
}
char *_RTL_FUNC tempnam(char *dir, char *prefix)
{
    int n = 0;
    char buf[MAXPATH], *p;
    char *s = getenv("TMP");
    struct ffblk ff;
    if (!s || findfirst(s, &ff, 0xff))
    {
        s = dir;
        if (!s || findfirst(s, &ff, 0xff))
        {
            s = buf;
            buf[0] = 0;
        }
    }
    strcpy(buf, s);
    s = buf;
    p = s + strlen(s);
    if (p != s && p[-1] != '\\')
        *p++ = '\\';
    __ll_enter_critical();
    do
    {
        sprintf(p, "%s%05d", prefix, n++);
    }
    while (!access(buf, 0) && n <= TMP_MAX);
    __ll_exit_critical();
    if (n <= TMP_MAX)
        return strdup(buf);	
    return NULL;
}
char *_RTL_FUNC _tempnam(char *dir, char *prefix)
{
    return tempnam(dir, prefix);
}
FILE *_RTL_FUNC tmpfile(void)
{
    char *aa;
    FILE *rv;
    int len;
    
    aa = tempnam(NULL, "tmf");
    if (__insertTempName(aa))
    {
        free(aa);
        return 0;
    }
    rv = fopen(aa,"wb+");
    free(aa);
    return rv;
}
