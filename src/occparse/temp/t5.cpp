#include <cstddef>
#include <cstdio>
#include <climits>
#include <stack>	
#include <functional>

template <class T>
void Color(T* root, std::function<int(node*, node*)>& consider)
{
     std::stack<T*> stk;
     stk.push(root);
     int color = root.color;
     int index = 1;

}
//----------------------------------------------------------
struct node
{
    int type;
    int size;
    node* left, *right;
};
node* mknode(int type, node*left, node*right)
{
	return new node { type, 0, 	left, right } ;
}
void print(node *nd, int space = 0)
{
    if (nd == NULL)  
        return;  
  
    space += 4;  
  
    print(nd->right, space);  
  
    for (int i = 4; i < space; i++)  
        fputc(' ', stdout);
    printf("%d:%d\n", nd->type, nd->size);
    print(nd->left, space);  
}
bool toConsider(node* aa, node* bb)
{
    return bb->type == 0 || bb->type == 1;
}
std::function<bool(node*, node*)> ToConsider = toConsider;
int main()
{
	node *tt1 = mknode(0, mknode(0, mknode(3, mknode(3, 0, 0), 0), 0), 0);	
	node *tt2 = mknode(0, 0, mknode(0, 0, mknode(3, 0, mknode(3, 0, 0))));	
	node *tt3 = mknode(0, mknode(0, mknode(0, mknode(5, 0, 0), mknode(5, 0, 0)), mknode(5, 0, 0)), mknode(1, mknode(1, mknode(1, mknode(5, 0, 0), mknode(5, 0, 0)), mknode(1, mknode(5, 0, 0), mknode(5, 0, 0))), mknode(2, mknode(2, 0, 0) , mknode(2, 0, 0))));	
        node *tt4 = mknode(5, 0, 0);
        for (int i=0; i < 10; i++)
            tt4 = mknode(0, tt4, mknode(5, 0, 0));
        node *tt5 = mknode(5, 0, 0);
        for (int i=0; i < 10; i++)
            tt5 = mknode(0, mknode(5, 0, 0), tt5);
	print(tt1);
	printf("---\n");
	print(tt2);
	printf("---\n");
	print(tt3);
	printf("---\n");
	print(tt4);
	printf("---\n");
	print(tt5);
	printf("--------------------------\n");
#if 0
        node rt = { 0, 0, tt1 };
        int n = TreeToVine(&rt); 
        printf("::%d\n", n);
	print(rt.right);
        rt = { 0, 0, tt2 };
        n = TreeToVine(&rt); 
        printf("::%d\n", n);
	print(rt.right);
        rt = { 0, 0, tt3 };
        n = TreeToVine(&rt); 
        printf("::%d\n", n);
	print(rt.right);
#endif
	print(test(tt1));
	printf("---\n");
	print(test(tt2));
	printf("---\n");
	print(test(tt3));
	printf("---\n");
	print(test(tt4));
	printf("---\n");
	print(test(tt5));
}