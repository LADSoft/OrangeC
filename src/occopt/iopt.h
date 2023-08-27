/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 */

/*
 * iopt.h
 *
 * icode optimization structures
 */
#include <set>

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

#define ISZ_TOVOIDSTAR 100
#define ISZ_TOINT 101

#define BITINTBITS (8 * sizeof(BITINT))

namespace Optimizer
{

/*
 * basic blocks are kept in this type of structure
 * and marked with an i_block inn the icode
 */
#define BLOCKLIST_VISITED 1

struct BriggsSet;

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
typedef std::set<QUAD*> InstructionList;
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
    struct usesStrength* next;
    IMODE* multiplier;
    int strengthName;
} USES_STRENGTH;
typedef struct inductionList
{
    struct inductionList* next;
    ILIST* vars;
} INDUCTION_LIST;

enum e_lptype
{
    LT_SINGLE,
    LT_MULTI,
    LT_ROOT,
    LT_BLOCK
};
struct Loop
{
    struct Loop* next;
    enum e_lptype type;
    int loopnum;
    struct Block* entry; /* will be the block for blocks */
    struct Loop* parent;
    LIST* contains;
    BITINT* invariantPhiList;
    struct _blocklist* successors;
    PRESSURE pressure;
    LIST* occurs;
    BriggsSet* through;
    BriggsSet* blocks;
    INDUCTION_LIST* inductionSets;
};

typedef struct copieshash
{
    struct copiesshash* next;
    QUAD* ins;
} COPIESHASH;

#define RF_NOT 1
#define RF_NEG 2
#define RF_SHIFT 4

typedef struct reshape_list
{
    struct reshape_list* next;
    struct reshape_list* distrib; /* distributive lists for multiplication */
    IMODE* im;
    IMODE* lastDistribName;
    short flags : 4; /* holds the RF values */
    short flags2 : 4;
    short distributed : 1;
    short genned : 1;
    short rporder;
} RESHAPE_LIST;
typedef struct
{
    enum i_ops op;
    RESHAPE_LIST* list;
    short count;
    IMODE* lastName;
} RESHAPE_EXPRESSION;

struct UIVOffset
{
    struct UIVOffset* next;
    int offset;
};

typedef struct _uiv
{
    IMODE* im;
    struct UIVOffset* offset;
    struct _uiv* alias;
    struct _uiv* base;
} UIV;
typedef struct _aliasName
{
    struct _aliasName* next;
    LIST* addresses;
    int byUIV;
    union
    {
        UIV* uiv;
        IMODE* name;
    } v;
} ALIASNAME;

typedef struct _aliasAddress
{
    struct _aliasAddress* next;
    struct _aliasAddress* merge;
    ALIASNAME* name;
    struct _aliaslist* pointsto;
    int offset;
    BITINT* modifiedBy;
    int processIndex;
} ALIASADDRESS;

typedef struct _aliaslist
{
    struct _aliaslist* next;
    ALIASADDRESS* address;
} ALIASLIST;

typedef struct _addrByName
{
    struct _addrByName* next;
    ALIASNAME* name;
    ALIASLIST* addresses;
} ADDRBYNAME;

typedef struct _normlist
{
    struct _normlist* next;
    IMODE* value;
    int level;
} NORMLIST;
struct TempInfo
{
    ILIST* renameStack;
    LIST* bdefines;
    LIST* idefines;
    LIST* iuses;
    struct quad* instructionDefines;
    struct quad* storesUses;
    struct Block* blockDefines;
    InstructionList* instructionUses;
    BITINT* conflicts;
    IMODE* spillVar;
    IMODE* spillAlias;
    Optimizer::SimpleExpression* enode;
    QUAD* spillTag;
    IMODE* newname;
    IMODE* newnameind;
    ILIST* elimPredecessors;
    ILIST* elimSuccessors;
    //	int limitUseCount;
    LIMIT_USES* limitUses;
    LIST* quietRegions;
    Loop* variantLoop;
    VALUEOF value;
    RESHAPE_EXPRESSION expression;
    IMODE* inductionReplacement;
    LIST* loadsIn;
    LIST* loadsOut;
    LIST* storesIn;
    LIST* storesOut;
    BITINT* workingMoves;
    USES_STRENGTH* sl;
    ALIASLIST* pointsto;
    BITINT* modifiedBy;
    BITINT* uses;
    BITINT* terms;
    BITINT* indTerms;
    IMODE* copy;
    NORMLIST* currentNormal;
    int strengthRename;
    struct _regclass* regClass;
    bool ptUIV;
    enum
    {
        P_UNKNOWN,
        P_PTR,
        P_REAL
    } ptrMode;
    int preSSATemp;
    int postSSATemp;
    int neighbors;
    int spillCost;
    int* rawSqueeze;
    int squeeze;
    int degree;
    int regCount;
    int color;
    int partition;
    int dfstOrder;
    int temp;
    int inductionLoop; /* 0 = none, else outermost enclosing loop */
    int oldInductionVar;
    unsigned spilled : 1;
    unsigned triedSpill : 1;
    unsigned spillTemp : 1;
    unsigned onstack : 1;
    unsigned usedAsAddress : 1;
    unsigned usedAsFloat : 1;
    unsigned visiteddfst : 1;
    unsigned expressionRoot : 1;
    unsigned doGlobal : 1; /* is being used, can do global opt */
    unsigned cachedForLoad : 1;
    unsigned cachedForStore : 1;
    unsigned precolored : 1;
    unsigned liveAcrossFunctionCall : 1;
    unsigned liveAcrossBlock : 1;
    unsigned inductionInitVar : 1;
    unsigned inUse : 1;
    unsigned iuTemp : 1;
    unsigned termClear : 1;
    unsigned degreed : 1;
    unsigned directSpill : 1;
    unsigned ircinitial : 1;
    unsigned spilling : 1;
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
    int critical : 1;
    int dead : 1;
    int unuseThunk : 1;
    int stopdfst : 1;
    int visiteddfst : 1;
    int onstack : 1;
    int globalChanged : 1;
    int alwayslive : 1;
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
    Loop* loopName;

    /*		struct _blocklist *defines; */
    BITINT* liveGen;
    BITINT* liveKills;
    BITINT* liveIn;
    BITINT* liveOut;
    QUAD *head, *tail;
    struct _blocklist* edgereached;
    LIST* occurs;
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
/*------------------------------------------------------------------------- */
/*
 * common code elimination uses this to track
 * all the gotos branching to a given label
 */
typedef struct _comgo
{
    char size;
    QUAD *head, *tail;
} COMGOREC;
/*
 * DAG structures
 */

typedef struct _list2
{
    struct _list2* next;
    int id;
    struct _l2data
    {
        IMODE* ans;
        QUAD* val;
    } data;
} LIST2;

/*-------------------------------------------------------------------------*/

typedef struct _list3
{
    struct _list3* next;
    IMODE* ans;
    LIST* decllist;
} LIST3;
}  // namespace Optimizer