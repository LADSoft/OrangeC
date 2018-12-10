#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compareint (const void *left, const void *right)
{
    int *a = (int *)left, *b = (int *)right;
    if (*a < *b)
        return -1;
    else if (*a > *b)
        return 1;
    else 
        return 0;
}
char *list[] = { "Z","U","B","W","F","A", "0" };
struct ss
{
    int a, b,c;
} aa[] = { {'z'},{'u'},{'b'},{'w'},{'f'},{'a'}, {'0' } };
int compare(char **a,char **b)
{
	return(strcmp(*a,*b));
}
int compare2(struct ss *one, struct ss *two)
{
    if (one->a < two->a) return -1;
    if (one->a > two->a) return 1;
    return 0;        
}
int main()
{
     int arr[8] = { 8,7,9,2,3,4,1,6 };
     qsort(arr, 8, sizeof(int), compareint );
     for (int i=0; i < 8; i++)
        printf("%d ", arr[i]);
     printf("\n");
    qsort(list,7,sizeof (char *),compare);
    qsort(aa,7,sizeof (aa[0]),compare2);
    for (int i=0; i <7; i++)
        printf("%s %c\n",list[i],aa[i].a);
}