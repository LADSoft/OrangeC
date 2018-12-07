#include <stdio.h>
#include <stdlib.h>
int compare(const void *left, const void *right)
{
    int *a = (int *)left, *b = (int *)right;
    if (*a < *b)
        return -1;
    else if (*a > *b)
        return 1;
    else 
        return 0;
}
int main()
{
     int arr[8] = { 8,7,9,2,3,4,1,6 };
     qsort(arr, 8, sizeof(int), compare);
     for (int i=0; i < 8; i++)
        printf("%d ", arr[i]);
}