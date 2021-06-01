#include <cstddef>
//#include <cstdio>
#include <climits>
#include <deque>
#include "dsw.h"

struct node
{
    int type;
    int treesize;
    node* left, *right;
};
node* mknode(int type, node*left, node*right)
{
	return new node { type, 0, 	left, right } ;
}
void fillprint(char arr[][120], node *nd, int row, int& col, int &maxrow)
{
    if (nd == NULL)  
        return;  
    if (maxrow < row+1)
        maxrow = row+1;
    fillprint(arr, nd->right, row+1, col, maxrow);  

    col++;
    char buf[256];
    sprintf(buf, "%c:%02d", nd->type, nd->treesize);
    memcpy(&arr[row][119 - col * 4], buf, 4);
    fillprint(arr, nd->left, row+1, col, maxrow);  
}
void print(node*nd)
{
    char buf[24][120];
    memset(buf,' ', sizeof(buf));
    for (int i=0; i < 24; i++)
        buf[i][119] = '\n';
    buf[23][119] = 0; 
    int rows = 0, col = 0;
    fillprint(buf, nd, 0, col, rows);	
    buf[rows][119] = 0; 
    printf("%s\n", buf);
}
node* scan(FILE* fil)
{
    node*top;
    char ch1;
    char buf[256];
    std::deque<node*> current;
    buf[0] = 0;
    fgets(buf, 256, fil);
    if (!buf[0])
        return false;
    sscanf(buf, " %c", & ch1);
    top = mknode(ch1, 0, 0);
    current.push_back(top);
    bool done = false;
    while (!done)
    {
        buf[0] = 0;
        fgets(buf, 256, fil);
        if (buf[0])
        {
            std::deque<node*> next;
            buf[strlen(buf)-1] = 0;
            int n = 0;
            for (auto c : current) 
            {
                int n2=0;
                if (sscanf(buf + n, " %c%n", &ch1, &n2) != 1)
                   break;
                n += n2;
                if (ch1 != '\'')
                {
                    c->left = mknode(ch1, 0, 0);
                    next.push_back(c->left);
                }
                if (sscanf(buf + n, " %c%n", &ch1, &n2) != 1)
                   break;
                n += n2;
                if (ch1 != '\'')
                {
                    c->right = mknode(ch1, 0, 0);
                    next.push_back(c->right);
                }
            }
            current = next;
        }
        else
        {
            done = true;
        }
    }
    return top;
}
bool toConsider(node* aa, node* bb)
{
    return bb->type == '+' || bb->type == '*';
}
int main()
{
        node* t = scan(stdin);
        print(t);
	printf("--------------------------\n");
    node root = { };
    root.right = t;
//    TreeToVineWrapper(&root, ToConsider);
//        print(root.right);

        print(Rebalance(t, toConsider));
}