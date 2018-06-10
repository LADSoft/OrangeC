// example for debugging global variables

typedef struct
{
    int i;
    long l;
    char text[10];
} test;

test t1;

int ii;

int main()
{
    test t2;
    test *pt1 = &t1;
    int *pii = &ii;
    
    ii = 27;
    *pii = *pii + ii;
    
    t1.i = 10;
    t1.l = 200;
    strcpy(t1.text, "Hello");
    
    pt1->i = 15;

    ii = *pii + *pii;
        
    t2.i = 10;
    t2.l = 200;
    strcpy(t2.text, "Hello");
    
    return 0;
}