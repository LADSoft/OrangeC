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
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */
#include <stdio.h>
#include <stdlib.h>

typedef int cmpfunc(void *, void *);

static size_t partition(unsigned char *data, size_t width, cmpfunc *func, size_t low, size_t high)
{
    // pivot (Element to be placed at right position)
    unsigned char *pivot = data + high;

    size_t i = low - width;
    for (size_t j = low; j <= high-width; j += width)
    {
        // If current element is smaller than or
        // equal to pivot
        if (func(data + j, pivot) <= 0)
        {
            i += width;    // increment index of smaller element
            if (i != j)
            {
                for (int k = 0; k < width; k++)
                {
                    unsigned char temp = data[i + k];
                    data[i + k] = data[j + k];
                    data[j + k] = temp;
                }
            }
        }
    }
    i += width;
    for (int k = 0; k < width; k++)
    {
        unsigned char temp = data[i + k];
        data[i + k] = pivot[k];
        pivot[k] = temp;
    }
    return i;
}
static void quicksort(void *data, size_t width, cmpfunc *func, size_t high)
{
    int staticheap[4096 * 4];

    int *heap = staticheap;
    if (high >= 4096)
        heap  = (int *)malloc(high*sizeof(int) * 4);
    if (heap)
    {
        heap[0] = 0;
        heap[1] = (high-1) * width;
        size_t heapStart = 0;
        size_t heapSize = 2;
        while (heapStart < heapSize)
        {
            size_t pivot=0;
            if (heap[heapStart] < heap[heapStart + 1])
            {
                pivot = partition((unsigned char *)data, width, func, heap[heapStart], heap[heapStart + 1]);
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

void _RTL_FUNC qsort(void* base, size_t num, size_t width, int (*compare)(const void* elem1, const void* elem2))
{
    quicksort(base, width, compare, num);
}
