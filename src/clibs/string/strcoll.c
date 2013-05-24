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
#include <locale.h>
#include <_locale.h>
#include <string.h>

int strcoll(const char *left, const char *right)
{
    COLLATE_DATA *cd = __locale_data[LC_COLLATE];
    struct collstr colldatleft, colldatright;
    colldatleft.levels = colldatright.levels = cd->data[2];
    colldatleft.lvlflags = colldatright.lvlflags = &cd->data[4];
    colldatleft.tbl = colldatright.tbl = &cd->data[15];
    for (colldatleft.curlevel =0, colldatright.curlevel = 0; colldatleft.curlevel < colldatleft.levels; 
                colldatleft.curlevel++, colldatright.curlevel++) {
        int chl, chr;
        int cachedrv=0, cacheddiff = 0;
        colldatleft.stateflags = colldatright.stateflags = 0;
        colldatleft.source = left;
        colldatright.source = right;
        colldatleft.fetched = colldatright.fetched = 0;
        chl = __nextWeight(&colldatleft) ;
        chr = __nextWeight(&colldatright);
        while (!((colldatleft.stateflags | colldatright.stateflags) & EOS)) {
            if (colldatleft.lvlflags[colldatleft.curlevel] & CONTROL) {
                int diff = colldatleft.fetched - colldatright.fetched;
                if (diff) {
                    if (!(colldatleft.lvlflags[colldatleft.curlevel] & REVERSE)) {
                        return diff ;
                    } else {
                        cacheddiff = diff;
                        cachedrv = (chr << 8) + chl;
                    }
                } else                 
                    goto join;
            } else {
                int rv ;
join:
                rv = chl - chr;
                if (rv) {
                    rv = (int)(char)rv ;
                    if (!(colldatleft.lvlflags[colldatleft.curlevel] & REVERSE))
                        return rv ;
                    else {
                        cachedrv = rv ; 
                    }
                }
            }
            chl = __nextWeight(&colldatleft) ;
            chr = __nextWeight(&colldatright);
        }
        if (chr-chl != 0) {
            if (colldatright.stateflags & EOS)
                return 0x7f7f;
            else
                return -0x7f7f;
        }
        if (cacheddiff)
            return cacheddiff;
        if (cachedrv)
            return cachedrv ;
    }
    return 0;
}
int _lstrcoll(const char *left, const char *right)
{
    return strcoll(left,right);
}
