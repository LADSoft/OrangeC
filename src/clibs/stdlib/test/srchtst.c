typedef struct {
	char name[20];
	int n;
} X;
X list1[] = { {"A",1},{"B",2},{"C",3},{"D",4},{"E",5} };
X list2[] = { {"A",6},{"B",7},{"C",8},{"D",9},{"E",10},{"F",11} };
X list3[] = { {"A",12},{"B",13},{"C",14},{"D",15},{"E",16},{"F",17},{"G",18} };
int compare(X *a,X *b)
{
	return(strcmp(&a->name,&b->name));
}
void *bsearch(char *a,X*b,int c,int d,int (*comp)());
void main(void)
{
	printf("%d\n",((X *)bsearch("A",list1,5,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("B",list1,5,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("C",list1,5,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("D",list1,5,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("E",list1,5,sizeof(X),compare))->n);

	printf("%d\n",((X *)bsearch("A",list2,6,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("B",list2,6,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("C",list2,6,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("D",list2,6,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("E",list2,6,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("F",list2,6,sizeof(X),compare))->n);

	printf("%d\n",((X *)bsearch("A",list3,7,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("B",list3,7,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("C",list3,7,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("D",list3,7,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("E",list3,7,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("F",list3,7,sizeof(X),compare))->n);
	printf("%d\n",((X *)bsearch("G",list3,7,sizeof(X),compare))->n);

	printf("%d\n",bsearch("Z",list1,5,sizeof(X),compare));
	printf("%d\n",bsearch("1",list1,5,sizeof(X),compare));
	printf("%d\n",bsearch("Z",list2,6,sizeof(X),compare));
	printf("%d\n",bsearch("1",list2,6,sizeof(X),compare));
	printf("%d\n",bsearch("Z",list3,7,sizeof(X),compare));
	printf("%d\n",bsearch("1",list3,7,sizeof(X),compare));

}