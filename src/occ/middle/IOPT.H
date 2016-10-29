/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2016, David Lindauer, (LADSoft).
	All rights reserved.
	
	Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
	
	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the
	  following disclaimer.
	
	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.
	
	* Neither the name of LADSoft nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of LADSoft.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/*
 * iopt.h
 *
 * icode optimization structures
 */
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
#define ISZ_ULONG 8
#define ISZ_U32 9
#define ISZ_ULONGLONG 10
#define ISZ_ADDR 11
#define ISZ_FARPTR 12
#define ISZ_SEG 13
#define ISZ_REG 14
/* */
#define ISZ_FLOAT 15
#define ISZ_DOUBLE 16
#define ISZ_LDOUBLE 17

#define ISZ_IFLOAT 18
#define ISZ_IDOUBLE 19
#define ISZ_ILDOUBLE 20

#define ISZ_CFLOAT 21
#define ISZ_CDOUBLE 22
#define ISZ_CLDOUBLE 23

//#define TESTBITS
#define BITINTBITS (8 * sizeof(BITINT))
#ifdef TESTBITS
typedef struct _bitarray
{
	int count; 
	BITINT data[1];
} BITARRAY;
BITARRAY *allocbit(int size);
BITARRAY *tallocbit(int size);
BITARRAY *sallocbit(int size);
BITARRAY *aallocbit(int size);
BITARRAY *callocbit(int size);
BOOLEAN isset(BITARRAY *arr, int bit);
void setbit(BITARRAY *arr, int bit);
void clearbit(BITARRAY *arr, int bit);
void bitarrayClear(BITARRAY *arr, int count);

#define bits(x) ((x)->data)
#else
typedef BITINT BITARRAY;

#define lallocbit(size)  ((BITINT *)Alloc(((size) + (BITINTBITS-1))/BITINTBITS * sizeof(BITINT)))
#define allocbit(size)	 ((BITINT *)oAlloc(((size) +(BITINTBITS-1))/BITINTBITS * sizeof(BITINT)))
#define tallocbit(size)	 ((BITINT *)tAlloc(((size) +(BITINTBITS-1))/BITINTBITS * sizeof(BITINT)))
#define sallocbit(size)	 ((BITINT *)sAlloc(((size) +(BITINTBITS-1))/BITINTBITS * sizeof(BITINT)))
#define aallocbit(size)	 ((BITINT *)aAlloc(((size) +(BITINTBITS-1))/BITINTBITS * sizeof(BITINT)))
#define callocbit(size)	 ((BITINT *)cAlloc(((size) +(BITINTBITS-1))/BITINTBITS * sizeof(BITINT)))
#define isset(array,bit) ((array)[((unsigned)(bit))/BITINTBITS] & bittab[((unsigned)(bit)) & (BITINTBITS - 1)])
#define setbit(array,bit)   (array)[((unsigned)(bit))/BITINTBITS] |= bittab[((unsigned)(bit)) & (BITINTBITS - 1)]
#define clearbit(array,bit)   (array)[((unsigned)(bit))/BITINTBITS] &= ~bittab[((unsigned)(bit)) & (BITINTBITS - 1)]
#define bitarrayClear(array, size)  memset(array, 0 , ((size) + (BITINTBITS-1))/BITINTBITS * sizeof(BITINT))
#define bits(x) (x)
#endif
#define briggsClear(data)  ((data)->top = 0)
/*
 * basic blocks are kept in this type of structure
 * and marked with an i_block inn the icode
 */
#define BLOCKLIST_VISITED 1
typedef struct {
	unsigned short *indexes ;
	unsigned short *data;
	int size ;
	int top;
} BRIGGS_SET;

typedef struct _value_of
{
	enum vop { vo_top, vo_bottom, vo_constant } type;
	IMODE *imode;
} VALUEOF ;
typedef struct _ins_list
{
	struct _ins_list *next;
	QUAD *ins;
} INSTRUCTIONLIST;
typedef struct _im_list
{
	struct _im_list *next;
	IMODE *im;
} IMODELIST;
 
typedef struct _limit_uses
{
	struct _limit_uses *next;
	int order;
	QUAD *ins;
	UBYTE gosubLevel;
	int ansmode:1;
} LIMIT_USES;
typedef struct _pressure {
	short floating;
	short cfloating;
	short address;
	short data;
	short ldata;
} PRESSURE;
typedef struct usesStrength {
	struct usesStrength *next;
	IMODE *multiplier;
	int strengthName;
} USES_STRENGTH;
typedef struct inductionList
{
	struct inductionList *next;
	LIST *vars;
} INDUCTION_LIST;

typedef struct _loop
{
	struct _loop *next;
	enum e_lptype { LT_SINGLE, LT_MULTI, LT_ROOT, LT_BLOCK } type;
	int loopnum;
	struct _block *entry; /* will be the block for blocks */
	struct _loop *parent;
	LIST *contains;	
    BITARRAY *invariantPhiList;
	struct _blocklist *successors;
	PRESSURE pressure;
	LIST *occurs;
	BRIGGS_SET *through;
	BRIGGS_SET *blocks;
	INDUCTION_LIST *inductionSets;
} LOOP;

typedef struct copieshash
{
	struct copiesshash	*next;
	QUAD 				*ins;
} COPIESHASH;

#define RF_NOT 1
#define RF_NEG 2
#define RF_SHIFT 4

typedef struct reshape_list
{
	struct reshape_list *next;
	struct reshape_list *distrib; /* distributive lists for multiplication */
	IMODE *im;
	IMODE *lastDistribName;
	short flags:4; /* holds the RF values */
	short flags2: 4;
	short distributed: 1;
	short genned : 1;
	short rporder;
} RESHAPE_LIST;
typedef struct
{
	enum i_ops op;
	RESHAPE_LIST *list;
	short count;
	IMODE *lastName;
} RESHAPE_EXPRESSION;

typedef struct _uiv
{
	IMODE *im;
	struct UIVOffset
	{
		struct UIVOffset *next;
		int offset;
	} *offset;
	struct _uiv *alias;
	struct _uiv *base;
} UIV;
typedef struct _aliasName
{
	struct _aliasName *next;
	LIST *addresses;
	int byUIV;
	union
	{
		UIV *uiv;
		IMODE *name;
	} v;
} ALIASNAME;

typedef struct _aliasAddress
{
	struct _aliasAddress *next;
	struct _aliasAddress *merge;
	ALIASNAME *name;
	struct _aliaslist *pointsto;
	int offset;
    BITINT *modifiedBy;
    int processIndex;
} ALIASADDRESS;

typedef struct _aliaslist
{
	struct _aliaslist *next;
	ALIASADDRESS *address;
} ALIASLIST;

typedef struct _normlist
{
	struct _normlist *next;
	IMODE *value;
	int level;
} NORMLIST;
typedef struct
{
	LIST *renameStack;
	LIST *bdefines;
	LIST *idefines;
	LIST *iuses;
	struct quad *instructionDefines;
	struct quad *storesUses;
	struct _block *blockDefines;
	INSTRUCTIONLIST *instructionUses;
	BITINT *conflicts;
	IMODE *spillVar;
    IMODE *spillAlias;
	EXPRESSION *enode;
	QUAD *spillTag;
	IMODE *newname;
	IMODE *newnameind;
	LIST *elimPredecessors;
	LIST *elimSuccessors;
//	int limitUseCount;
	LIMIT_USES *limitUses;
	LIST *quietRegions;
	LOOP *variantLoop;
	VALUEOF value;
	RESHAPE_EXPRESSION expression;
	IMODE *inductionReplacement;
	LIST *loadsIn;
	LIST *loadsOut;
	LIST *storesIn;
	LIST *storesOut;
	BITARRAY *workingMoves;
	USES_STRENGTH *sl;
	ALIASLIST *pointsto;
    BITINT *modifiedBy;
    BITINT *uses;
	BITINT *terms;
	IMODE *copy;
	NORMLIST *currentNormal;
	int strengthRename;
	struct _regclass *regClass;
	BOOLEAN ptUIV;
	enum { P_UNKNOWN, P_PTR, P_REAL } ptrMode;
	int preSSATemp;
	int postSSATemp;
	int neighbors ;
	int spillCost;
	int *rawSqueeze;
	int squeeze;
	int degree;
	int regCount;
	int color;
	int partition;
	int dfstOrder;
	int temp;
	int inductionLoop; /* 0 = none, else outermost enclosing loop */
	int oldInductionVar;
	unsigned spilled:1;
	unsigned spillTemp:1;
	unsigned onstack:1;
	unsigned usedAsAddress:1 ;
	unsigned usedAsFloat:1;
	unsigned visiteddfst:1;
	unsigned expressionRoot: 1;
	unsigned doGlobal:1 ; /* is being used, can do global opt */
	unsigned cachedForLoad:1;
	unsigned cachedForStore:1;
	unsigned precolored : 1;
	unsigned liveAcrossFunctionCall : 1;
	unsigned liveAcrossBlock : 1;
	unsigned dontoptimize : 1;
	unsigned inductionInitVar: 1;
	unsigned inUse: 1;
	unsigned iuTemp : 1;
	unsigned termClear : 1;
	unsigned degreed : 1;
	unsigned directSpill:1;
    unsigned ircinitial:1;
    unsigned spilling : 1;
	char size;
} TEMP_INFO;


typedef struct _exceedPressure
{
	struct _exceedPressure *next;
	LOOP *l;
	int prio;
} EXCEED_PRESSURE;

typedef struct _blocklist
{
    struct _blocklist *next;
    struct _block
    {
        short blocknum;
/*        short dfstnum; */
        int critical: 1;
		int dead: 1;
		int unuseThunk: 1;
		int stopdfst:1;
		int visiteddfst:1;
		int onstack:1;
		int globalChanged:1;
        int alwayslive:1;
        short callcount;
		short preWalk ;
		short postWalk;
		int temp;
		int idom;
		int pdom;
		int dfstOrder;
		int reversePostOrder;
		int spillCost;
		int nesting;
		struct _blocklist *dominates;
		struct _blocklist *dominanceFrontier;
		struct _blocklist * pred, *succ;
		struct _blocklist *loopGenerators;
		LOOP *loopParent;
		LOOP *inclusiveLoopParent;
		LOOP *loopName;
		
/*		struct _blocklist *defines; */
		BITINT *liveGen;
		BITINT *liveKills;
		BITINT *liveIn;
		BITINT *liveOut;		
		QUAD *head, *tail;
		struct _blocklist *edgereached;
		LIST *occurs;
    }  *block;
} BLOCKLIST;

enum e_fgtype { F_NONE, F_TREE, F_DFSTREE, F_FORWARDEDGE, F_BACKEDGE, F_CROSSEDGE };
/*------------------------------------------------------------------------- */


typedef struct _block BLOCK;
/*
 * common code elimination uses this to track
 * all the gotos branching to a given label
 */
typedef struct _comgo
{
    char size;
    QUAD *head,  *tail;
} COMGOREC;
/*
 * DAG structures
 */

typedef struct _daglist
{
    struct _daglist *next;
    UBYTE *key;
    UBYTE *rv;
} DAGLIST;

typedef struct _list2
{
    struct _list2 *next;
    int id;
    struct _l2data
    {
        IMODE *ans;
        QUAD *val;
    } data;
}LIST2;


/*-------------------------------------------------------------------------*/


typedef struct _list3
{
    struct _list3 *next;
    IMODE *ans;
    LIST *decllist;
} LIST3;


