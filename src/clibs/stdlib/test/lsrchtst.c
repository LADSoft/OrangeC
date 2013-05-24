typedef struct {
	char name[20];
	int n;
} X;
X list3[] = { {"A",12},{"B",13},{"C",14},{"D",15},{"E",16},{"F",17},{"G",18} };
int compare(X *a,X *b)
{
	return(strcmp(&a->name,&b->name));
}
void *lfind(char *a,X*b,int *c,int d,int (*comp)());
void main(void)
{
	int num = 7 ;
	printf("%d %d\n",((X *)lfind("A",list3,&num,sizeof(X),compare))->n,num);
	num = 7 ;
	printf("%d %d\n",((X *)lfind("B",list3,&num,sizeof(X),compare))->n,num);
	num = 7 ;
	printf("%d %d\n",((X *)lfind("C",list3,&num,sizeof(X),compare))->n,num);
	num = 7 ;
	printf("%d %d\n",((X *)lfind("D",list3,&num,sizeof(X),compare))->n,num);
	num = 7 ;
	printf("%d %d\n",((X *)lfind("E",list3,&num,sizeof(X),compare))->n,num);
	num = 7 ;
	printf("%d %d\n",((X *)lfind("F",list3,&num,sizeof(X),compare))->n,num);
	num = 7 ;
	printf("%d %d\n",((X *)lfind("G",list3,&num,sizeof(X),compare))->n,num);

}