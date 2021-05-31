#include <cstddef>
#include <cstdio>
#include <climits>
#include <stack>
template <class T>
size_t TreeToVine(T* root)
{
    
    size_t nodes = 0;
    T* tail = root;
    T* rest = tail->right;
    while (rest)
    {
        if (!rest->left)
        {
            tail = rest;
            rest = rest->right;
            nodes++;
        }
        else
        {
            T* temp = rest->left;
            rest->left = temp->right;
            temp->right = rest;
            rest = temp;
            tail->right = temp;
        }
    }
    return nodes;
}

size_t btlog2(size_t n)
{
    size_t i;
    for (i=0; i < INT_MAX; i++)
    {
        n >>= 1;
        if (!n)
            break;
    }
    return i;
}
template <class T>
void VineToTree(T* root, size_t size)
{
    int leaves = size + 1 - (1 << btlog2(size + 1));        
    printf("::%d\n", leaves);
    VineCompress(root, leaves);
    size -= leaves;
    printf("$$%d\n", size);
    while (size > 1)
    {
        size = size/2;
        VineCompress(root, size);
    }
}
template <class T>
void VineCompress(T* root, size_t count)
{
    T* scanner = root;
    for (int i=0; i < count; i++)
    {
        T* child = scanner->right;
        scanner->right = child->right;
        scanner = scanner->right;
        child->right = scanner->left;
        scanner->left = child;
    }
}
template <class T>
T* Rebalance(T* nd)
{
    T root = { };
    root.right = nd;
    size_t size = TreeToVine(root);
    VineToTree(root, size);
    return root.right;
}
//----------------------------------------------------------
struct node
{
	int data;
	node* left, *right;
};
node* mknode(int data, node*left, node*right)
{
	return new node { data, 	left, right } ;
}
void print(node *nd, int space = 0)
{
    if (nd == NULL)  
        return;  
  
    space += 4;  
  
    print(nd->right, space);  
  
    for (int i = 4; i < space; i++)  
        fputc(' ', stdout);
    printf("%d\n", nd->data);
    print(nd->left, space);  
}
node* test(node *nd)
{
	node root = { };
	root.right = nd;
	size_t size = TreeToVine(&root);
        VineToTree(&root, size);
        return root.right;
}
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