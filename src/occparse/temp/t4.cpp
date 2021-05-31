#include <cstddef>
//#include <cstdio>
#include <climits>
#include <stack>	
#include <functional>
template <class T>
T* TreeToVine(T* nd, std::function<bool(T*, T*)>& consider)
{ 
    T root = {};
    root.right = nd;
    T* tail = &root;
    T* rest = tail->right;
    int type = nd->type;
    while (rest)
    {	
        if (!rest->left)
        {
            tail = rest;
            rest = rest->right;
            root.size++;
        }
        else if ((rest->left->type != type && (rest->left->left || rest->left->right)) ||	
                 (rest->right && (rest->right->left || rest->right->right) && rest->right->type != type))

        {
             T troot =  { };
             troot.right = rest->left;
             TreeToVineWrapper(&troot, consider);
             rest->left = troot.right;
             troot.right = rest->right;
             TreeToVineWrapper(&troot, consider);
             rest->right = troot.right;
             break;
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
    root.right->size = root.size;
    return root.right;
}
template <class T>
void TreeToVineWrapper(T* root, std::function<bool(T*, T*)>& consider)
{
     T* tail = root;
     std::stack<T*> stk;
     stk.push(tail);
     while (!stk.empty())
     {
          T* temp = stk.top();
          stk.pop();
	   if (temp->right)
           {
              if (consider(temp->right, temp->right))
              {
                  temp->right = TreeToVine(temp->right, consider);
              }
              else
              {
                  temp->right->size = -1;
                  stk.push(temp->right);
              }
          }
          if (temp->left)
          {
              if (consider(temp->left, temp->left))
              {
                  temp->left = TreeToVine(temp->left, consider);
              }
              else
              {
                  temp->left->size = -1;
                  stk.push(temp->left);
              }
          }
     }
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
void VineToTree(T* root)
{
    size_t size = root->right->size;
    int leaves = size + 1 - (1 << btlog2(size + 1));        
    VineCompress(root, leaves);
    size -= leaves;
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
void VineToTreeWrapper(T* root)
{
    std::stack<T*> work;
    std::stack<T*> stk;
    work.push(root); 
    while(!work.empty())
    {
        T* cur = work.top();
        work.pop();
        if (cur->right)
            work.push(cur->right);
        if (cur->left)
            work.push(cur->left);
        if ((cur->right && cur->right->size > 0) || (cur->left && cur->left->size > 0))
        {
            stk.push(cur);
        }
    }
    while (!stk.empty())
    {
         T* cur = stk.top();
         if (cur->left && cur->left->size > 0)
         {
             T* temp = cur->left;
             cur->left = cur->right;
             cur->right = temp;
             VineToTree(cur);
             temp = cur->left;
             cur->left = cur->right;
             cur->right = temp;
         }
         if (cur->right && cur->right->size > 0)
             VineToTree(cur);
         stk.pop();
    }
}
template <class T>
T* Rebalance(T* nd, std::function<bool(T*, T*)>& consider)
{
    T root = { };
    root.right = nd;
    TreeToVineWrapper(&root, consider);
    VineToTreeWrapper(&root);
    return root.right;
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
void fillprint(char arr[][120], node *nd, int row, int& col, int &maxrow)
{
    if (nd == NULL)  
        return;  
    if (maxrow < row+1)
        maxrow = row+1;
    fillprint(arr, nd->right, row+1, col, maxrow);  

    col++;
    char buf[256];
    sprintf(buf, "%c:%02d", nd->type, nd->size);
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
std::function<bool(node*, node*)> ToConsider = toConsider;
int main()
{
        node* t = scan(stdin);
        print(t);
	printf("--------------------------\n");
        print(Rebalance(t, ToConsider));
}