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
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
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
