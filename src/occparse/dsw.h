/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 *
 */

// dayton-stout-warren algorithm, modified to handle arbitrary expression trees
// the original algorithm rebalanced an entire Binary search tree
// this modification only rebalances subtrees that are rebalanceable
// for example '+' and '*' nodes are rebalanceable but '/' nodes are not.
//

#include <stack>
template <class T, class F>
void TreeToVineWrapper(T* root, F* consider);
template <class T>
void VineCompress(T* root, size_t count);

template <class T, class F>
T* TreeToVine(T* nd, F* consider)
{
    T root = {};
    root.right = nd;
    T* tail = &root;
    T* rest = tail->right;
    decltype(nd->type) type = nd->type;
    while (rest)
    {
        if (!rest->left)
        {
            tail = rest;
            rest = rest->right;
            root.treesize++;
        }
        else if ((rest->left->type != type && (rest->left->left || rest->left->right)) ||
                 (rest->right && (rest->right->left || rest->right->right) && rest->right->type != type))

        {
            T troot = {};
            troot.right = rest->left;
            TreeToVineWrapper(&troot, consider);
            rest->left = troot.right;
            troot.right = rest->right;
            TreeToVineWrapper(&troot, consider);
            rest->right = troot.right;
            root.treesize++;
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
    root.right->treesize = root.treesize;
    return root.right;
}
template <class T, class F>
void TreeToVineWrapper(T* root, F* consider)
{
    static std::stack<T*> stk;
    while (stk.size())
        stk.pop();
    T* tail = root;
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
                temp->right->treesize = -1;
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
                temp->left->treesize = -1;
                stk.push(temp->left);
            }
        }
    }
}
template <class T>
T btlog2(T n)
{
    T i;
    for (i = 0; i < INT_MAX; i++)
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
    size_t size = root->right->treesize;
    int leaves = size + 1 - (1 << btlog2(size + 1));
    VineCompress(root, leaves);
    size -= leaves;
    while (size > 1)
    {
        size = size / 2;
        VineCompress(root, size);
    }
}
template <class T>
void VineCompress(T* root, size_t count)
{
    T* scanner = root;
    for (int i = 0; i < count; i++)
    {
        T* child = scanner->right;
        scanner->right = child->right;
        scanner = scanner->right;
        if (scanner)
        {
            child->right = scanner->left;
            scanner->left = child;
        }
        else
        {
            // this is to handle problems when errors are detected in the source
            break;
        }
    }
}

template <class T>
void VineToTreeWrapper(T* root)
{
    std::stack<T*> work;
    std::stack<T*> stk;
    work.push(root);
    while (!work.empty())
    {
        T* cur = work.top();
        work.pop();
        if (cur->right)
            work.push(cur->right);
        if (cur->left)
            work.push(cur->left);
        if ((cur->right && cur->right->treesize > 0) || (cur->left && cur->left->treesize > 0))
        {
            stk.push(cur);
        }
    }
    while (!stk.empty())
    {
        T* cur = stk.top();
        if (cur->left && cur->left->treesize > 0)
        {
            T* temp = cur->left;
            cur->left = cur->right;
            cur->right = temp;
            VineToTree(cur);
            temp = cur->left;
            cur->left = cur->right;
            cur->right = temp;
        }
        if (cur->right && cur->right->treesize > 0)
            VineToTree(cur);
        stk.pop();
    }
}
template <class T, class F>
T* Rebalance(T* nd, F* consider)
{
    T root = {};
    root.right = nd;
    TreeToVineWrapper(&root, consider);
    VineToTreeWrapper(&root);
    return root.right;
}
