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
#include <stdlib.h>
#include <memory.h>

void _RTL_FUNC quicksort(const void *base, int lo, int hi, size_t width,
                        int (*compare)(const void *elem1, const void *elem2), char *buf)
{

    if (hi > lo) {
        int i=lo, j=hi;
        char * celem = buf + width;
        memcpy(celem,((char *)base) + width * ((lo + hi) / 2),width) ;

        while (i <= j)
        {
            while (i <= hi && (*compare)((char *)base + i * width, celem) <0) i++; 
            while (j >= lo && (*compare)((char *)base + j * width, celem) >0) j--; 
            if (i<=j)
            {
                memcpy(buf, (char *)base + i * width, width) ;
                memcpy((char *)base + i * width, (char *)base + j * width, width) ;
                memcpy((char *)base + j * width, buf, width) ;
                i++; j--;
            }
        }
        if (lo<j) quicksort(base, lo, j,width,compare,buf);
        if (i<hi) quicksort(base, i, hi,width,compare,buf);
    } 

}
void _RTL_FUNC qsort(void *base, size_t num, size_t width,
                        int (*compare)(const void *elem1, const void *elem2))
{
    char data[512];
    char *buf ;

    if (width <= sizeof(data)/2)
        buf = data;
    else {
        buf = malloc(width) ;
        if (!buf)
            return ;
    }
    
    quicksort(base,0,num-1,width, compare,buf) ;

    if (buf != data)
        free(buf) ;
}
