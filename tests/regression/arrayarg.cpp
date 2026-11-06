void foo2(int *) { }
void foo1(int a[]) {}
void foo(int**) {}
void bar(int a[]) { foo(&a);}
void bar1(int a[]) { foo1(a);}
void bar2(int a[]) { foo2(a); }
int main()
{
}