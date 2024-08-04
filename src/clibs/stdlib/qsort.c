/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <stdio.h>
#include <stdlib.h>

typedef int cmpfunc(const void*, const void*);
#define SWAP(x, y)          \
    unsigned char temp = x; \
    x = y;                  \
    y = temp;

static void quicksort(unsigned char* data, size_t width, cmpfunc* func, size_t elems)
{
    int staticheap[4096 * 4];

    int* heap = staticheap;
    if (elems >= 4096)
        heap = (int*)malloc(elems * sizeof(int) * 4 + 4 * sizeof(int));
    if (heap)
    {
        heap[0] = 0;
        heap[1] = (elems - 1) * width;
        size_t heapStart = 0;
        size_t heapSize = 2;
        while (heapStart < heapSize)
        {
            size_t pivot = 0;
            if (heap[heapStart] < heap[heapStart + 1])
            {
                unsigned low = heap[heapStart];
                unsigned high = heap[heapStart + 1];
                // pivot (Element to be placed at right position)
                unsigned char* pivotelem = data + high;

                size_t pivot = low - width;
                for (size_t j = low; j <= high - width; j += width)
                {
                    if (func(data + j, pivotelem) <= 0)
                    {
                        pivot += width;
                        if (pivot != j)
                        {
                            for (int k = 0; k < width; k++)
                            {
                                SWAP(data[pivot + k], data[j + k]);
                            }
                        }
                    }
                }
                pivot += width;
                for (int k = 0; k < width; k++)
                {
                    SWAP(data[pivot + k], pivotelem[k]);
                }
                heap[heapSize++] = heap[heapStart + 0];
                heap[heapSize++] = pivot - width;
                heap[heapSize++] = pivot + width;
                heap[heapSize++] = heap[heapStart + 1];
            }
            heapStart += 2;
        }
        if (heap != staticheap)
            free(heap);
    }
}

void _RTL_FUNC qsort(void* base, size_t num, size_t width, cmpfunc compare) { quicksort(base, width, compare, num); }
