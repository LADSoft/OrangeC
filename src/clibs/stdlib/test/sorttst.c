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
void main(void)
{
	int i;
	qsort(list,7,sizeof (char *),compare);
	qsort(aa,7,sizeof (aa[0]),compare2);
	for (i=0; i <7; i++)
		printf("%s %c\n",list[i],aa[i].a);
}