/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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

/*
 * iopt.h
 *
 * icode optimization structures
 */
#include <set>
#include <list>
#include "containers.h"
#define DUMP_GCSE_INFO

/* the int types can be negative, which means 'signed' vs 'unsigned' */
#define ISZ_NONE 0
#define ISZ_BIT 1
#define ISZ_BOOLEAN 2
#define ISZ_UCHAR 3
#define ISZ_USHORT 4
#define ISZ_WCHAR 5
#define ISZ_U16 6
#define ISZ_UINT 7
#define ISZ_UNATIVE 8
#define ISZ_ULONG 9
#define ISZ_U32 10
#define ISZ_ULONGLONG 11
#define ISZ_ADDR 12
#define ISZ_FARPTR 13
#define ISZ_SEG 14
#define ISZ_REG 15
/* */
#define ISZ_STRING 16
#define ISZ_OBJECT 17
/* */
#define ISZ_FLOAT 18
#define ISZ_DOUBLE 19
#define ISZ_LDOUBLE 20

#define ISZ_IFLOAT 21
#define ISZ_IDOUBLE 22
#define ISZ_ILDOUBLE 23

#define ISZ_CFLOAT 24
#define ISZ_CDOUBLE 25
#define ISZ_CLDOUBLE 26

#define ISZ_BITINT 50

#define BITINTBITS (8 * sizeof(BITINT))

namespace Optimizer
{

template <class T>
class TestableList
{
  public:
    void clear()
    {
        set.clear();
        list.clear();
    }
    auto begin() { return list.begin(); }
    auto end() { return list.end(); }
    void push_back(T val)
    {
        set.insert(val);
        list.push_back(val);
    }
    void pop_back()
    {
        if (list.size())
        {
            set.erase(list.back());
            list.pop_back();
        }
    }
    void push(T val) { push_back(val); }
    void pop() { return pop_back(); }
    void push_front(T val)
    {
        set.insert(val);
        list.push_front(val);
    }
    void pop_front()
    {
        if (list.size())
        {
            set.erase(list.front());
            list.pop_front();
        }
    }
    auto top() { return list.back(); }
    auto test(T val) { return set.find(val) != set.end(); }
    bool empty() { return list.empty(); }

  private:
    std::set<T> set;
    std::list<T> list;
};
/*
 * basic blocks are kept in this type of structure
 * and marked with an i_block inn the icode
 */
#define BLOCKLIST_VISITED 1

enum vop
{
    vo_top,
    vo_bottom,
    vo_constant
};

typedef struct _value_of
{
    enum vop type;
    IMODE* imode;
} VALUEOF;
typedef struct _im_list
{
    struct _im_list* next;
    IMODE* im;
} IMODELIST;

typedef struct _limit_uses
{
    struct _limit_uses* next;
    int order;
    QUAD* ins;
    UBYTE gosubLevel;
    int ansmode : 1;
} LIMIT_USES;
typedef struct _pressure
{
    short floating;
    short cfloating;
    short address;
    short data;
    short ldata;
} PRESSURE;
typedef struct usesStrength
{
    IMODE* multiplier;
    int strengthName;
} USES_STRENGTH;

enum e_lptype
{
    LT_SINGLE,
    LT_MULTI,
    LT_ROOT,
    LT_BLOCK
};
struct Loop
{
    enum e_lptype type;
    int loopnum;
    struct Block* entry; /* will be the block for blocks */
    struct Loop* parent;
    struct _blocklist* successors;
};

typedef struct copieshash
{
    struct copiesshash* next;
    QUAD* ins;
} COPIESHASH;

typedef struct _normlist
{
    struct _normlist* next;
    IMODE* value;
    int level;
} NORMLIST;
struct TempInfo
{
    struct quad* storesUses;
    // originally InstructionUsesSsa[] was a linked list; the first element of the list (last thing inserted)
    // was checked in rewritex86.cpp.   When I changed it to a set and used the QUAD address as the set element type
    // this became wrong but the results were sometimes erratic, which could cause builds to fail.
    // so we introduced this field to bring things back to the original stable condition.
    QUAD* instructionUsesLast;
    Optimizer::SimpleExpression* enode;
    IMODE* newname;
    IMODE* newnameind;
    //	int limitUseCount;
    LIMIT_USES* limitUses;
    Loop* variantLoop;
    VALUEOF value;
    IMODE* inductionReplacement;
    BITINT* uses;
    BITINT* terms;
    BITINT* indTerms;
    IMODE* copy;
    NORMLIST* currentNormal;
    int strengthRename;
    bool ptUIV;
    enum
    {
        P_UNKNOWN,
        P_PTR,
        P_REAL
    } ptrMode;
    int preSSATemp;
    int postSSATemp;
    int partition;
    int dfstOrder;
    int temp;
    int inductionLoop; /* 0 = none, else outermost enclosing loop */
    int oldInductionVar;
    unsigned usedAsAddress : 1;
    unsigned usedAsFloat : 1;
    unsigned visiteddfst : 1;
    unsigned expressionRoot : 1;
    unsigned cachedForLoad : 1;
    unsigned cachedForStore : 1;
    unsigned liveAcrossFunctionCall : 1;
    unsigned liveAcrossBlock : 1;
    unsigned inductionInitVar : 1;
    unsigned inUse : 1;
    unsigned termClear : 1;
    unsigned degreed : 1;
    unsigned ircinitial : 1;
    char size;
};

typedef struct _exceedPressure
{
    struct _exceedPressure* next;
    Loop* l;
    int prio;
} EXCEED_PRESSURE;

struct Block
{
    int blocknum;
    /*        short dfstnum; */
    unsigned critical : 1;
    unsigned dead : 1;
    unsigned unuseThunk : 1;
    unsigned stopdfst : 1;
    unsigned visiteddfst : 1;
    unsigned globalChanged : 1;
    unsigned alwayslive : 1;
    short callcount;
    short preWalk;
    short postWalk;
    int temp;
    int idom;
    int pdom;
    int dfstOrder;
    int reversePostOrder;
    int spillCost;
    int nesting;
    struct _blocklist* dominates;
    struct _blocklist* dominanceFrontier;
    struct _blocklist *pred, *succ;
    struct _blocklist* loopGenerators;
    Loop* loopParent;
    Loop* inclusiveLoopParent;

    QUAD *head, *tail;
};

typedef struct _blocklist
{
    struct _blocklist* next;
    struct Block* block;
} BLOCKLIST;

enum e_fgtype
{
    F_NONE,
    F_TREE,
    F_DFSTREE,
    F_FORWARDEDGE,
    F_BACKEDGE,
    F_CROSSEDGE
};
}  // namespace Optimizer