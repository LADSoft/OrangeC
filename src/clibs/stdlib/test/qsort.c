/* 
   Copyright 1994-2003 Free Software Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
   USA

   You may contact the author at:

   mailto::camille@bluegrass.net

   or by snail mail at:

   David Lindauer
   850 Washburn Ave Apt 99
   Louisville, KY 40222
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
void _RTL_FUNC qsort(const void *base, size_t num, size_t width,
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
