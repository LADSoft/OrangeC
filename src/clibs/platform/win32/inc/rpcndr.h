#ifndef __RPCNDR_H_VERSION__
#define __RPCNDR_H_VERSION__  (500)
#endif

#ifndef _RPCNDR_H
#define _RPCNDR_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows RPC Network Data Representation definitions */

#include <pshpack8.h>
#include <basetsd.h>
#include <rpcnsip.h>
/* #include <rpcsal.h> */

#ifdef __cplusplus
extern "C" {
#endif


#define NDR_CHAR_REP_MASK  0x0000000FUL
#define NDR_INT_REP_MASK  0x000000F0UL
#define NDR_FLOAT_REP_MASK  0x0000FF00UL
#define NDR_LITTLE_ENDIAN  0x00000010UL
#define NDR_BIG_ENDIAN  0x00000000UL
#define NDR_IEEE_FLOAT  0x00000000UL
#define NDR_VAX_FLOAT  0x00000100UL
#define NDR_IBM_FLOAT  0x00000300UL
#define NDR_ASCII_CHAR  0x00000000UL
#define NDR_EBCDIC_CHAR  0x00000001UL
#define NDR_LOCAL_DATA_REPRESENTATION   0x00000010UL
#define NDR_LOCAL_ENDIAN  NDR_LITTLE_ENDIAN

#if (_WIN32_WINNT >= 0x600)
#define TARGET_IS_NT60_OR_LATER  1
#else
#define TARGET_IS_NT60_OR_LATER  0
#endif

#if (_WIN32_WINNT >= 0x501)
#define TARGET_IS_NT51_OR_LATER  1
#else
#define TARGET_IS_NT51_OR_LATER  0
#endif

#if (_WIN32_WINNT >= 0x500)
#define TARGET_IS_NT50_OR_LATER  1
#else
#define TARGET_IS_NT50_OR_LATER  0
#endif

#if (defined(_WIN32_DCOM) || _WIN32_WINNT >= 0x0400)
#define TARGET_IS_NT40_OR_LATER  1
#else
#define TARGET_IS_NT40_OR_LATER  0
#endif

#define TARGET_IS_NT351_OR_WIN95_OR_LATER   1

#define small char
typedef unsigned char byte;
typedef unsigned char boolean;
typedef byte cs_byte;

#ifndef _HYPER_DEFINED
#define _HYPER_DEFINED
#if (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
#define  hyper __int64
#define MIDL_uhyper  unsigned __int64
#else
typedef double  hyper;
typedef double MIDL_uhyper;
#endif
#endif /* _HYPER_DEFINED */

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
#ifndef _SIZE_T
#ifdef __RPC_WIN64__
typedef unsigned __int64 size_t;
#else
typedef unsigned int size_t;
#endif
#define _SIZE_T_DEFINED
#define _SIZE_T
#endif
#endif

#ifdef __RPC_WIN32__
#define __RPC_CALLEE  __stdcall
#endif

#ifndef __MIDL_USER_DEFINED
#define midl_user_allocate  MIDL_user_allocate
#define midl_user_free  MIDL_user_free
#define __MIDL_USER_DEFINED
#endif

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

#define RPC_VAR_ENTRY  __cdecl

#if defined(_M_IX86) || defined(_M_AMD64) || defined(_M_IA64)
#define __MIDL_DECLSPEC_DLLIMPORT  __declspec(dllimport)
#define __MIDL_DECLSPEC_DLLEXPORT  __declspec(dllexport)
#else
#define __MIDL_DECLSPEC_DLLIMPORT
#define __MIDL_DECLSPEC_DLLEXPORT
#endif

#ifdef CONST_VTABLE
#define CONST_VTBL const
#else
#define CONST_VTBL
#endif

#ifndef DECLSPEC_SELECTANY
#define DECLSPEC_SELECTANY
#endif

#ifndef DECLSPEC_UUID
#define DECLSPEC_UUID(x)
#endif

#define MIDL_INTERFACE(x)  struct DECLSPEC_UUID(x)

#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)  EXTERN_C const IID itf

#define NDRSContextValue(hContext)  (&(hContext)->userContext)

#define cbNDRContext  20

#define byte_from_ndr(source,target)  { *(target) = *(*(char * *)&(source)->Buffer)++; }
#define byte_array_from_ndr(Source,LowerIndex,UpperIndex,Target)  { NDRcopy((((char*)(Target))+(LowerIndex)), (Source)->Buffer, (unsigned int)((UpperIndex)-(LowerIndex))); *(unsigned long *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); }
#define boolean_from_ndr(source,target)  { *(target) = *(*(char**)&(source)->Buffer)++; }
#define boolean_array_from_ndr(Source,LowerIndex,UpperIndex,Target)  { NDRcopy((((char*)(Target))+(LowerIndex)), (Source)->Buffer, (unsigned int)((UpperIndex)-(LowerIndex))); *(unsigned long *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); }
#define small_from_ndr(source,target)  { *(target) = *(*(char**)&(source)->Buffer)++; }
#define small_from_ndr_temp(source,target,format)  { *(target) = *(*(char**)(source))++; }
#define small_array_from_ndr(Source,LowerIndex,UpperIndex,Target)  { NDRcopy((((char*)(Target))+(LowerIndex)), (Source)->Buffer, (unsigned int)((UpperIndex)-(LowerIndex))); *(unsigned long *)&(Source)->Buffer += ((UpperIndex)-(LowerIndex)); }

#if defined(__RPC_WIN32__) || defined(__RPC_WIN64__)
#define MIDL_ascii_strlen(string)  strlen(string)
#define MIDL_ascii_strcpy(target,source)  strcpy(target,source)
#define MIDL_memset(s,c,n)  memset(s,c,n)
#endif

#define _midl_ma1(p,cast)  *(*(cast**)&p)++
#define _midl_ma2(p,cast)  *(*(cast**)&p)++
#define _midl_ma4(p,cast)  *(*(cast**)&p)++
#define _midl_ma8(p,cast)  *(*(cast**)&p)++

#define _midl_unma1(p,cast)  *((cast*)p)++
#define _midl_unma2(p,cast)  *((cast*)p)++
#define _midl_unma3(p,cast)  *((cast*)p)++
#define _midl_unma4(p,cast)  *((cast*)p)++

#define _midl_fa2(p)  (p = (RPC_BUFPTR)((ULONG_PTR)(p+1)&~0x1))
#define _midl_fa4(p)  (p = (RPC_BUFPTR)((ULONG_PTR)(p+3)&~0x3))
#define _midl_fa8(p)  (p = (RPC_BUFPTR)((ULONG_PTR)(p+7)&~0x7))

#define _midl_addp(p,n)  (p+=n)

#define _midl_marsh_lhs(p,cast)  *(*(cast**)&p)++
#define _midl_marsh_up(mp,p)  *(*(unsigned long **)&mp)++ = (unsigned long)p
#define _midl_advmp(mp)  *(*(unsigned long **)&mp)++
#define _midl_unmarsh_up(p)  (*(*(unsigned long **)&p)++)

#define NdrMarshConfStringHdr(p,s,l)  (_midl_ma4(p,unsigned long) = s, _midl_ma4(p,unsigned long) = 0, _midl_ma4(p, unsigned long) = l)
#define NdrUnMarshConfStringHdr(p,s,l)  ((s=_midl_unma4(p,unsigned long),(_midl_addp(p,4)),(l=_midl_unma4(p,unsigned long))
#define NdrMarshCCtxtHdl(pc,p)  (NDRCContextMarshall((NDR_CCONTEXT)pc, p),p+20)
#define NdrUnMarshCCtxtHdl(pc,p,h,drep)  (NDRCContextUnmarshall((NDR_CONTEXT)pc,h,p,drep), p+20)
#define NdrUnMarshSCtxtHdl(pc, p,drep)  (pc = NdrSContextUnMarshall(p,drep))
#define NdrMarshSCtxtHdl(pc,p,rd)   (NdrSContextMarshall((NDR_SCONTEXT)pc,p,(NDR_RUNDOWN)rd)
#define NdrFieldOffset(s,f)  (LONG_PTR)(&(((s*)0)->f))
#define NdrFieldPad(s,f,p,t)  ((unsigned long)(NdrFieldOffset(s,f) - NdrFieldOffset(s,p)) - sizeof(t))
#define NdrFcShort(s)  (unsigned char)(s&0xFF),(unsigned char)(s>>8)
#define NdrFcLong(s)  (unsigned char)(s&0xFF),(unsigned char)((s&0x0000FF00)>>8),(unsigned char)((s&0x00FF0000)>>16),(unsigned char)(s>>24)

#define RPC_BAD_STUB_DATA_EXCEPTION_FILTER  \
    ((RpcExceptionCode() == STATUS_ACCESS_VIOLATION)  || \
     (RpcExceptionCode() == STATUS_DATATYPE_MISALIGNMENT) || \
     (RpcExceptionCode() == RPC_X_BAD_STUB_DATA) || \
     (RpcExceptionCode() == RPC_S_INVALID_BOUND))

#define NDR_SHAREABLE __inline

#define USER_MARSHAL_CB_SIGNATURE  'USRC'

#define USER_CALL_CTXT_MASK(f)  ((f)&0x00FF)
#define USER_CALL_AUX_MASK(f)  ((f)&0xFF00)
#define GET_USER_DATA_REP(f)  ((f)>>16)

#define USER_CALL_IS_ASYNC  0x0100
#define USER_CALL_NEW_CORRELATION_DESC  0x0200

#define USER_MARSHAL_FC_BYTE  1
#define USER_MARSHAL_FC_CHAR  2
#define USER_MARSHAL_FC_SMALL  3
#define USER_MARSHAL_FC_USMALL  4
#define USER_MARSHAL_FC_WCHAR  5
#define USER_MARSHAL_FC_SHORT  6
#define USER_MARSHAL_FC_USHORT  7
#define USER_MARSHAL_FC_LONG  8
#define USER_MARSHAL_FC_ULONG  9
#define USER_MARSHAL_FC_FLOAT  10
#define USER_MARSHAL_FC_HYPER  11
#define USER_MARSHAL_FC_DOUBLE  12

typedef void *NDR_CCONTEXT;

typedef struct {
    void *pad[2];
    void *userContext;
} *NDR_SCONTEXT;

typedef void (__RPC_USER *NDR_RUNDOWN)(void*);
typedef void (__RPC_USER *NDR_NOTIFY_ROUTINE)(void);
typedef void (__RPC_USER *NDR_NOTIFY2_ROUTINE)(boolean);

typedef struct _SCONTEXT_QUEUE {
    unsigned long NumberOfObjects;
    NDR_SCONTEXT *ArrayOfObjects;
} SCONTEXT_QUEUE, *PSCONTEXT_QUEUE;

typedef unsigned long error_status_t;

struct _MIDL_STUB_MESSAGE;
struct _MIDL_STUB_DESC;
struct _FULL_PTR_XLAT_TABLES;

typedef unsigned char *RPC_BUFPTR;
typedef unsigned long RPC_LENGTH;

typedef void (__RPC_USER *EXPR_EVAL)(struct _MIDL_STUB_MESSAGE*);
typedef const unsigned char *PFORMAT_STRING;

typedef struct {
    long Dimension;
    unsigned long *BufferConformanceMark;
    unsigned long *BufferVarianceMark;
    unsigned long *MaxCountArray;
    unsigned long *OffsetArray;
    unsigned long *ActualCountArray;
} ARRAY_INFO, *PARRAY_INFO;

typedef struct _NDR_PIPE_DESC *PNDR_PIPE_DESC;
typedef struct _NDR_PIPE_MESSAGE *PNDR_PIPE_MESSAGE;
typedef struct _NDR_ASYNC_MESSAGE *PNDR_ASYNC_MESSAGE;
typedef struct _NDR_CORRELATION_INFO *PNDR_CORRELATION_INFO;

#if !defined(__RPC_WIN64__)
#include <pshpack4.h>
#endif

typedef struct _MIDL_STUB_MESSAGE {
    PRPC_MESSAGE RpcMsg;
    unsigned char *Buffer;
    unsigned char *BufferStart;
    unsigned char *BufferEnd;
    unsigned char *BufferMark;
    unsigned long BufferLength;
    unsigned long MemorySize;
    unsigned char *Memory;
    int IsClient;
    int ReuseBuffer;
    unsigned char *AllocAllNodesMemory;
    unsigned char *AllocAllNodesMemoryEnd;
    int IgnoreEmbeddedPointers;
    unsigned char *PointerBufferMark;
    unsigned char fBufferValid;
    unsigned char uFlags;
    ULONG_PTR MaxCount;
    unsigned long Offset;
    unsigned long ActualCount;
    void *(__RPC_API *pfnAllocate)(size_t);
    void (__RPC_API *pfnFree)(void *);
    unsigned char *StackTop;
    unsigned char *pPresentedType;
    unsigned char *pTransmitType;
    handle_t SavedHandle;
    const struct _MIDL_STUB_DESC *StubDesc;
    struct _FULL_PTR_XLAT_TABLES *FullPtrXlatTables;
    unsigned long FullPtrRefId;
    unsigned long ulUnused1;
    int fInDontFree:1;
    int fDontCallFreeInst:1;
    int fInOnlyParam:1;
    int fHasReturn:1;
    int fHasExtensions:1;
    int fHasNewCorrDesc:1;
    int fUnused:10;
    unsigned long dwDestContext;
    void *pvDestContext;
    NDR_SCONTEXT *SavedContextHandles;
    long ParamNumber;
    struct IRpcChannelBuffer *pRpcChannelBuffer;
    PARRAY_INFO pArrayInfo;
    unsigned long *SizePtrCountArray;
    unsigned long *SizePtrOffsetArray;
    unsigned long *SizePtrLengthArray;
    void *pArgQueue;
    unsigned long dwStubPhase;
    PNDR_PIPE_DESC pPipeDesc;
    PNDR_ASYNC_MESSAGE pAsyncMsg;
    PNDR_CORRELATION_INFO pCorrInfo;
    unsigned char *pCorrMemory;
    void *pMemoryList;
    ULONG_PTR w2kReserved[5];
} MIDL_STUB_MESSAGE, *PMIDL_STUB_MESSAGE;

#if !defined(__RPC_WIN64__)
#include <poppack.h>
#endif

typedef void * (__RPC_API *GENERIC_BINDING_ROUTINE)(void*);
typedef void (__RPC_API *GENERIC_UNBIND_ROUTINE)(void*,unsigned char*);

typedef struct _GENERIC_BINDING_ROUTINE_PAIR {
    GENERIC_BINDING_ROUTINE pfnBind;
    GENERIC_UNBIND_ROUTINE pfnUnbind;
} GENERIC_BINDING_ROUTINE_PAIR, *PGENERIC_BINDING_ROUTINE_PAIR;

typedef struct __GENERIC_BINDING_INFO {
    void *pObj;
    unsigned int Size;
    GENERIC_BINDING_ROUTINE pfnBind;
    GENERIC_UNBIND_ROUTINE pfnUnbind;
} GENERIC_BINDING_INFO, *PGENERIC_BINDING_INFO;

typedef void (__RPC_USER *XMIT_HELPER_ROUTINE)(PMIDL_STUB_MESSAGE);

typedef struct _XMIT_ROUTINE_QUINTUPLE {
    XMIT_HELPER_ROUTINE pfnTranslateToXmit;
    XMIT_HELPER_ROUTINE pfnTranslateFromXmit;
    XMIT_HELPER_ROUTINE pfnFreeXmit;
    XMIT_HELPER_ROUTINE pfnFreeInst;
} XMIT_ROUTINE_QUINTUPLE, *PXMIT_ROUTINE_QUINTUPLE;

typedef unsigned long (__RPC_USER *USER_MARSHAL_SIZING_ROUTINE)(unsigned long*,unsigned long,void *);
typedef unsigned char *(__RPC_USER *USER_MARSHAL_MARSHALLING_ROUTINE)(unsigned long*,unsigned char*,void*);
typedef unsigned char *(__RPC_USER *USER_MARSHAL_UNMARSHALLING_ROUTINE)(unsigned long*,unsigned char*,void*);
typedef void (__RPC_USER *USER_MARSHAL_FREEING_ROUTINE)(unsigned long*,void*);

typedef struct _USER_MARSHAL_ROUTINE_QUADRUPLE {
    USER_MARSHAL_SIZING_ROUTINE pfnBufferSize;
    USER_MARSHAL_MARSHALLING_ROUTINE pfnMarshall;
    USER_MARSHAL_UNMARSHALLING_ROUTINE pfnUnmarshall;
    USER_MARSHAL_FREEING_ROUTINE pfnFree;
} USER_MARSHAL_ROUTINE_QUADRUPLE;

typedef enum _USER_MARSHAL_CB_TYPE {
    USER_MARSHAL_CB_BUFFER_SIZE,
    USER_MARSHAL_CB_MARSHALL,
    USER_MARSHAL_CB_UNMARSHALL,
    USER_MARSHAL_CB_FREE
} USER_MARSHAL_CB_TYPE;

typedef struct _USER_MARSHAL_CB {
    unsigned long Flags;
    PMIDL_STUB_MESSAGE pStubMsg;
    PFORMAT_STRING pReserve;
    unsigned long Signature;
    USER_MARSHAL_CB_TYPE CBType;
    PFORMAT_STRING pFormat;
    PFORMAT_STRING pTypeFormat;
} USER_MARSHAL_CB;

typedef struct _MALLOC_FREE_STRUCT {
    void * (__RPC_USER *pfnAllocate)(size_t);
    void (__RPC_USER *pfnFree)(void*);
} MALLOC_FREE_STRUCT;

typedef struct _COMM_FAULT_OFFSETS {
    short CommOffset;
    short FaultOffset;
} COMM_FAULT_OFFSETS;

typedef struct _MIDL_STUB_DESC {
    void *RpcInterfaceInformation;
    void *(__RPC_API *pfnAllocate)(size_t);
    void (__RPC_API *pfnFree)(void*);
    union {
        handle_t *pAutoHandle;
        handle_t *pPrimitiveHandle;
        PGENERIC_BINDING_INFO pGenericBindingInfo;
    } IMPLICIT_HANDLE_INFO;
    const NDR_RUNDOWN *apfnNdrRundownRoutines;
    const GENERIC_BINDING_ROUTINE_PAIR *aGenericBindingRoutinePairs;
    const EXPR_EVAL *apfnExprEval;
    const XMIT_ROUTINE_QUINTUPLE *aXmitQuintuple;
    const unsigned char *pFormatTypes;
    int fCheckBounds;
    unsigned long Version;
    MALLOC_FREE_STRUCT *pMallocFreeStruct;
    long MIDLVersion;
    const COMM_FAULT_OFFSETS *CommFaultOffsets;
    const USER_MARSHAL_ROUTINE_QUADRUPLE *aUserMarshalQuadruple;
    const NDR_NOTIFY_ROUTINE *NotifyRoutineTable;
    ULONG_PTR mFlags;
    ULONG_PTR Reserved3;
    ULONG_PTR Reserved4;
    ULONG_PTR Reserved5;
} MIDL_STUB_DESC;

typedef const MIDL_STUB_DESC *PMIDL_STUB_DESC;

typedef void *PMIDL_XMIT_TYPE;

typedef struct _MIDL_FORMAT_STRING {
    short Pad;
    unsigned char Format[];
} MIDL_FORMAT_STRING;

typedef void (__RPC_API *STUB_THUNK)(PMIDL_STUB_MESSAGE);
typedef long (__RPC_API *SERVER_ROUTINE)();

typedef struct  _MIDL_SERVER_INFO_ {
    PMIDL_STUB_DESC pStubDesc;
    const SERVER_ROUTINE *DispatchTable;
    PFORMAT_STRING ProcString;
    const unsigned short *FmtStringOffset;
    const STUB_THUNK *ThunkTable;
    PFORMAT_STRING LocalFormatTypes;
    PFORMAT_STRING LocalProcString;
    const unsigned short *LocalFmtStringOffset;
} MIDL_SERVER_INFO, *PMIDL_SERVER_INFO;

typedef struct _MIDL_STUBLESS_PROXY_INFO {
    PMIDL_STUB_DESC pStubDesc;
    PFORMAT_STRING ProcFormatString;
    const unsigned short *FormatStringOffset;
    PFORMAT_STRING LocalFormatTypes;
    PFORMAT_STRING LocalProcString;
    const unsigned short *LocalFmtStringOffset;
} MIDL_STUBLESS_PROXY_INFO;

typedef MIDL_STUBLESS_PROXY_INFO *PMIDL_STUBLESS_PROXY_INFO;

typedef union _CLIENT_CALL_RETURN {
    void *Pointer;
    LONG_PTR Simple;
} CLIENT_CALL_RETURN;

typedef enum {
    XLAT_SERVER = 1,
    XLAT_CLIENT
} XLAT_SIDE;

typedef struct _FULL_PTR_TO_REFID_ELEMENT {
    struct _FULL_PTR_TO_REFID_ELEMENT *Next;
    void *Pointer;
    unsigned long RefId;
    unsigned char State;
} FULL_PTR_TO_REFID_ELEMENT, *PFULL_PTR_TO_REFID_ELEMENT;

typedef struct _FULL_PTR_XLAT_TABLES {
    struct {
        void **XlatTable;
        unsigned char *StateTable;
        unsigned long NumberOfEntries;
    } RefIdToPointer;
    struct {
        PFULL_PTR_TO_REFID_ELEMENT *XlatTable;
        unsigned long NumberOfBuckets;
        unsigned long HashMask;
    } PointerToRefId;
    unsigned long NextRefId;
    XLAT_SIDE XlatSide;
} FULL_PTR_XLAT_TABLES, *PFULL_PTR_XLAT_TABLES;

CLIENT_CALL_RETURN RPC_VAR_ENTRY NdrClientCall2(PMIDL_STUB_DESC,PFORMAT_STRING,...);
CLIENT_CALL_RETURN RPC_VAR_ENTRY NdrClientCall(PMIDL_STUB_DESC,PFORMAT_STRING,...);
CLIENT_CALL_RETURN RPC_VAR_ENTRY NdrAsyncClientCall(PMIDL_STUB_DESC,PFORMAT_STRING,...);
CLIENT_CALL_RETURN RPC_VAR_ENTRY NdrDcomAsyncClientCall(PMIDL_STUB_DESC,PFORMAT_STRING,...);

typedef enum {
    STUB_UNMARSHAL,
    STUB_CALL_SERVER,
    STUB_MARSHAL,
    STUB_CALL_SERVER_NO_HRESULT
} STUB_PHASE;

typedef enum {
    PROXY_CALCSIZE,
    PROXY_GETBUFFER,
    PROXY_MARSHAL,
    PROXY_SENDRECEIVE,
    PROXY_UNMARSHAL
} PROXY_PHASE;

struct IRpcStubBuffer;

typedef void *RPC_SS_THREAD_HANDLE;
typedef void *__RPC_API RPC_CLIENT_ALLOC(size_t);
typedef void __RPC_API RPC_CLIENT_FREE(void*);

typedef struct _NDR_USER_MARSHAL_INFO_LEVEL1 {
    void *Buffer;
    unsigned long BufferSize;
    void *(__RPC_API * pfnAllocate)(size_t);
    void (__RPC_API * pfnFree)(void *);
    struct IRpcChannelBuffer * pRpcChannelBuffer;
    ULONG_PTR Reserved[5];
} NDR_USER_MARSHAL_INFO_LEVEL1;

typedef struct _NDR_USER_MARSHAL_INFO {
    unsigned long InformationLevel;
    union {
        NDR_USER_MARSHAL_INFO_LEVEL1 Level1;
    };
} NDR_USER_MARSHAL_INFO;

RPCRTAPI RPC_BINDING_HANDLE RPC_ENTRY NDRCContextBinding(NDR_CCONTEXT);
RPCRTAPI void RPC_ENTRY NDRCContextMarshall(NDR_CCONTEXT,void *);
RPCRTAPI void RPC_ENTRY NDRCContextUnmarshall(NDR_CCONTEXT *,RPC_BINDING_HANDLE,void*,unsigned long);
RPCRTAPI void RPC_ENTRY NDRCContextUnmarshall2(NDR_CCONTEXT*,RPC_BINDING_HANDLE,void*,unsigned long);
RPCRTAPI void RPC_ENTRY NDRSContextMarshall(NDR_SCONTEXT,void *,NDR_RUNDOWN);
RPCRTAPI NDR_SCONTEXT RPC_ENTRY NDRSContextUnmarshall(void *, unsigned long);
RPCRTAPI void RPC_ENTRY NDRSContextMarshallEx(RPC_BINDING_HANDLE,NDR_SCONTEXT,void*,NDR_RUNDOWN);
RPCRTAPI void RPC_ENTRY NDRSContextMarshall2( RPC_BINDING_HANDLE,NDR_SCONTEXT,void*,NDR_RUNDOWN,void*,unsigned long);
RPCRTAPI NDR_SCONTEXT RPC_ENTRY NDRSContextUnmarshallEx(RPC_BINDING_HANDLE,void*,unsigned long);
RPCRTAPI NDR_SCONTEXT RPC_ENTRY NDRSContextUnmarshall2(RPC_BINDING_HANDLE,void*,unsigned long,void*,unsigned long);
RPCRTAPI void RPC_ENTRY RpcSsDestroyClientContext(void * *);
RPCRTAPI void RPC_ENTRY NDRcopy(void*,void*,unsigned int);
RPCRTAPI size_t RPC_ENTRY MIDL_wchar_strlen(wchar_t*);
RPCRTAPI void RPC_ENTRY MIDL_wchar_strcpy(void*,wchar_t*);
RPCRTAPI void RPC_ENTRY char_from_ndr(PRPC_MESSAGE,unsigned char*);
RPCRTAPI void RPC_ENTRY char_array_from_ndr(PRPC_MESSAGE,unsigned long,unsigned long,unsigned char*);
RPCRTAPI void RPC_ENTRY short_from_ndr(PRPC_MESSAGE,unsigned short*);
RPCRTAPI void RPC_ENTRY short_array_from_ndr(PRPC_MESSAGE,unsigned long,unsigned long,unsigned short*);
RPCRTAPI void RPC_ENTRY short_from_ndr_temp(unsigned char**,unsigned short*,unsigned long);
RPCRTAPI void RPC_ENTRY long_from_ndr(PRPC_MESSAGE,unsigned long*);
RPCRTAPI void RPC_ENTRY long_array_from_ndr(PRPC_MESSAGE,unsigned long,unsigned long,unsigned long*);
RPCRTAPI void RPC_ENTRY long_from_ndr_temp(unsigned char**,unsigned long*,unsigned long);
RPCRTAPI void RPC_ENTRY enum_from_ndr(PRPC_MESSAGE,unsigned int*);
RPCRTAPI void RPC_ENTRY float_from_ndr(PRPC_MESSAGE,void*);
RPCRTAPI void RPC_ENTRY float_array_from_ndr(PRPC_MESSAGE,unsigned long,unsigned long,void*);
RPCRTAPI void RPC_ENTRY double_from_ndr(PRPC_MESSAGE,void*);
RPCRTAPI void RPC_ENTRY double_array_from_ndr(PRPC_MESSAGE,unsigned long,unsigned long,void*);
RPCRTAPI void RPC_ENTRY hyper_from_ndr(PRPC_MESSAGE,hyper*);
RPCRTAPI void RPC_ENTRY hyper_array_from_ndr(PRPC_MESSAGE,unsigned long,unsigned long,hyper*);
RPCRTAPI void RPC_ENTRY hyper_from_ndr_temp(unsigned char**,hyper*,unsigned long);
RPCRTAPI void RPC_ENTRY data_from_ndr(PRPC_MESSAGE,void*,char*,unsigned char);
RPCRTAPI void RPC_ENTRY data_into_ndr(void*,PRPC_MESSAGE,char*,unsigned char);
RPCRTAPI void RPC_ENTRY tree_into_ndr(void*,PRPC_MESSAGE,char*,unsigned char);
RPCRTAPI void RPC_ENTRY data_size_ndr(void*,PRPC_MESSAGE,char*,unsigned char);
RPCRTAPI void RPC_ENTRY tree_size_ndr(void*,PRPC_MESSAGE,char*,unsigned char);
RPCRTAPI void RPC_ENTRY tree_peek_ndr(PRPC_MESSAGE,unsigned char**,char*,unsigned char);
RPCRTAPI void* RPC_ENTRY midl_allocate(size_t);
RPCRTAPI void RPC_ENTRY NdrSimpleTypeMarshall(PMIDL_STUB_MESSAGE,unsigned char*,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrPointerMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrSimpleStructMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantStructMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantVaryingStructMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrComplexStructMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrFixedArrayMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantArrayMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantVaryingArrayMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrVaryingArrayMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrComplexArrayMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrNonConformantStringMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantStringMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrEncapsulatedUnionMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrNonEncapsulatedUnionMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrByteCountPointerMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrXmitOrRepAsMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrUserMarshalMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrInterfacePointerMarshall(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrClientContextMarshall(PMIDL_STUB_MESSAGE,NDR_CCONTEXT,int);
RPCRTAPI void RPC_ENTRY NdrServerContextMarshall(PMIDL_STUB_MESSAGE,NDR_SCONTEXT,NDR_RUNDOWN);
RPCRTAPI void RPC_ENTRY NdrServerContextNewMarshall(PMIDL_STUB_MESSAGE,NDR_SCONTEXT,NDR_RUNDOWN,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrSimpleTypeUnmarshall(PMIDL_STUB_MESSAGE,unsigned char*,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrRangeUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI void RPC_ENTRY NdrCorrelationInitialize(PMIDL_STUB_MESSAGE,unsigned long*,unsigned long,unsigned long);
RPCRTAPI void RPC_ENTRY NdrCorrelationPass(PMIDL_STUB_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrCorrelationFree(PMIDL_STUB_MESSAGE);
RPCRTAPI unsigned char* RPC_ENTRY NdrPointerUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrSimpleStructUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantStructUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantVaryingStructUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrComplexStructUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrFixedArrayUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantArrayUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantVaryingArrayUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrVaryingArrayUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrComplexArrayUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrNonConformantStringUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrConformantStringUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrEncapsulatedUnionUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrNonEncapsulatedUnionUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrByteCountPointerUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrXmitOrRepAsUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrUserMarshalUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI unsigned char* RPC_ENTRY NdrInterfacePointerUnmarshall(PMIDL_STUB_MESSAGE,unsigned char**,PFORMAT_STRING,unsigned char);
RPCRTAPI void RPC_ENTRY NdrClientContextUnmarshall(PMIDL_STUB_MESSAGE,NDR_CCONTEXT*,RPC_BINDING_HANDLE);
RPCRTAPI NDR_SCONTEXT RPC_ENTRY NdrServerContextUnmarshall(PMIDL_STUB_MESSAGE);
RPCRTAPI NDR_SCONTEXT RPC_ENTRY NdrContextHandleInitialize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI NDR_SCONTEXT RPC_ENTRY NdrServerContextNewUnmarshall(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrPointerBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrSimpleStructBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantStructBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantVaryingStructBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrComplexStructBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrFixedArrayBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantArrayBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantVaryingArrayBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrVaryingArrayBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrComplexArrayBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantStringBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrNonConformantStringBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrEncapsulatedUnionBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrNonEncapsulatedUnionBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrByteCountPointerBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrXmitOrRepAsBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrUserMarshalBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrInterfacePointerBufferSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrContextHandleSize(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrPointerMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrSimpleStructMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrConformantStructMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrConformantVaryingStructMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrComplexStructMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrFixedArrayMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrConformantArrayMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrConformantVaryingArrayMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrVaryingArrayMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrComplexArrayMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrConformantStringMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrNonConformantStringMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrEncapsulatedUnionMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrNonEncapsulatedUnionMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrXmitOrRepAsMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrUserMarshalMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned long RPC_ENTRY NdrInterfacePointerMemorySize(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrPointerFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrSimpleStructFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantStructFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantVaryingStructFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrComplexStructFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrFixedArrayFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantArrayFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConformantVaryingArrayFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrVaryingArrayFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrComplexArrayFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrEncapsulatedUnionFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrNonEncapsulatedUnionFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrByteCountPointerFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrXmitOrRepAsFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrUserMarshalFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrInterfacePointerFree(PMIDL_STUB_MESSAGE,unsigned char*,PFORMAT_STRING);
RPCRTAPI void RPC_ENTRY NdrConvert2(PMIDL_STUB_MESSAGE,PFORMAT_STRING,long);
RPCRTAPI void RPC_ENTRY NdrConvert(PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI unsigned char* RPC_ENTRY NdrUserMarshalSimpleTypeConvert(unsigned long*,unsigned char*,unsigned char);
RPCRTAPI void RPC_ENTRY NdrClientInitializeNew(PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,unsigned int);
RPCRTAPI unsigned char* RPC_ENTRY NdrServerInitializeNew(PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC);
RPCRTAPI void RPC_ENTRY NdrServerInitializePartial(PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,unsigned long);
RPCRTAPI void RPC_ENTRY NdrClientInitialize(PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,unsigned int);
RPCRTAPI unsigned char* RPC_ENTRY NdrServerInitialize(PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC);
RPCRTAPI unsigned char* RPC_ENTRY NdrServerInitializeUnmarshall(PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,PRPC_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrServerInitializeMarshall(PRPC_MESSAGE,PMIDL_STUB_MESSAGE);
RPCRTAPI unsigned char* RPC_ENTRY NdrGetBuffer(PMIDL_STUB_MESSAGE,unsigned long,RPC_BINDING_HANDLE);
RPCRTAPI unsigned char* RPC_ENTRY NdrNsGetBuffer(PMIDL_STUB_MESSAGE,unsigned long,RPC_BINDING_HANDLE);
RPCRTAPI unsigned char* RPC_ENTRY NdrGetPipeBuffer(PMIDL_STUB_MESSAGE,unsigned long,RPC_BINDING_HANDLE);
RPCRTAPI void RPC_ENTRY NdrGetPartialBuffer(PMIDL_STUB_MESSAGE);
RPCRTAPI unsigned char* RPC_ENTRY NdrSendReceive(PMIDL_STUB_MESSAGE,unsigned char*);
RPCRTAPI unsigned char* RPC_ENTRY NdrNsSendReceive(PMIDL_STUB_MESSAGE,unsigned char*,RPC_BINDING_HANDLE*);
RPCRTAPI void RPC_ENTRY NdrPipeSendReceive(PMIDL_STUB_MESSAGE,PNDR_PIPE_DESC);
RPCRTAPI void RPC_ENTRY NdrFreeBuffer(PMIDL_STUB_MESSAGE);
RPCRTAPI RPC_STATUS RPC_ENTRY NdrGetDcomProtocolVersion(PMIDL_STUB_MESSAGE,RPC_VERSION*);
RPCRTAPI void RPC_ENTRY NdrPipesInitialize(PMIDL_STUB_MESSAGE,PFORMAT_STRING,PNDR_PIPE_DESC,PNDR_PIPE_MESSAGE,char*,unsigned long);
RPCRTAPI void RPC_ENTRY NdrPipePull(char*,void*,unsigned long,unsigned long*);
RPCRTAPI void RPC_ENTRY NdrPipePush(char*,void*,unsigned long);
RPCRTAPI void RPC_ENTRY NdrIsAppDoneWithPipes(PNDR_PIPE_DESC);
RPCRTAPI void RPC_ENTRY NdrPipesDone(PMIDL_STUB_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrAsyncServerCall(PRPC_MESSAGE);
RPCRTAPI long RPC_ENTRY NdrAsyncStubCall(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PRPC_MESSAGE,unsigned long*);
RPCRTAPI long RPC_ENTRY NdrDcomAsyncStubCall(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PRPC_MESSAGE,unsigned long*);
RPCRTAPI long RPC_ENTRY NdrStubCall2(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PRPC_MESSAGE,unsigned long*);
RPCRTAPI void RPC_ENTRY NdrServerCall2(PRPC_MESSAGE);
RPCRTAPI long RPC_ENTRY NdrStubCall(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PRPC_MESSAGE,unsigned long*);
RPCRTAPI void RPC_ENTRY NdrServerCall(PRPC_MESSAGE);
RPCRTAPI int RPC_ENTRY NdrServerUnmarshall(struct IRpcChannelBuffer*,PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
RPCRTAPI void RPC_ENTRY NdrServerMarshall(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PMIDL_STUB_MESSAGE,PFORMAT_STRING);
RPCRTAPI RPC_STATUS RPC_ENTRY NdrMapCommAndFaultStatus(PMIDL_STUB_MESSAGE,unsigned long*,unsigned long*,RPC_STATUS);
RPCRTAPI int RPC_ENTRY NdrSH_UPDecision(PMIDL_STUB_MESSAGE,unsigned char**,RPC_BUFPTR);
RPCRTAPI int RPC_ENTRY NdrSH_TLUPDecision(PMIDL_STUB_MESSAGE,unsigned char**);
RPCRTAPI int RPC_ENTRY NdrSH_TLUPDecisionBuffer(PMIDL_STUB_MESSAGE,unsigned char**);
RPCRTAPI int RPC_ENTRY NdrSH_IfAlloc(PMIDL_STUB_MESSAGE,unsigned char**,unsigned long);
RPCRTAPI int RPC_ENTRY NdrSH_IfAllocRef(PMIDL_STUB_MESSAGE,unsigned char**,unsigned long);
RPCRTAPI int RPC_ENTRY NdrSH_IfAllocSet(PMIDL_STUB_MESSAGE,unsigned char**,unsigned long);
RPCRTAPI RPC_BUFPTR RPC_ENTRY NdrSH_IfCopy(PMIDL_STUB_MESSAGE,unsigned char**,unsigned long);
RPCRTAPI RPC_BUFPTR RPC_ENTRY NdrSH_IfAllocCopy(PMIDL_STUB_MESSAGE,unsigned char**,unsigned long);
RPCRTAPI unsigned long RPC_ENTRY NdrSH_Copy(unsigned char*,unsigned char*,unsigned long);
RPCRTAPI void RPC_ENTRY NdrSH_IfFree(PMIDL_STUB_MESSAGE,unsigned char*);
RPCRTAPI RPC_BUFPTR RPC_ENTRY NdrSH_StringMarshall(PMIDL_STUB_MESSAGE,unsigned char*,unsigned long,int);
RPCRTAPI RPC_BUFPTR RPC_ENTRY NdrSH_StringUnMarshall(PMIDL_STUB_MESSAGE,unsigned char**,int);

RPCRTAPI void* RPC_ENTRY RpcSsAllocate(size_t);
RPCRTAPI void RPC_ENTRY RpcSsDisableAllocate(void);
RPCRTAPI void RPC_ENTRY RpcSsEnableAllocate(void);
RPCRTAPI void RPC_ENTRY RpcSsFree(void*);
RPCRTAPI RPC_SS_THREAD_HANDLE RPC_ENTRY RpcSsGetThreadHandle(void);
RPCRTAPI void RPC_ENTRY RpcSsSetClientAllocFree(RPC_CLIENT_ALLOC*,RPC_CLIENT_FREE*);
RPCRTAPI void RPC_ENTRY RpcSsSetThreadHandle(RPC_SS_THREAD_HANDLE);
RPCRTAPI void RPC_ENTRY RpcSsSwapClientAllocFree(RPC_CLIENT_ALLOC*,RPC_CLIENT_FREE*,RPC_CLIENT_ALLOC**,RPC_CLIENT_FREE**);
RPCRTAPI void* RPC_ENTRY RpcSmAllocate(size_t,RPC_STATUS*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmClientFree(void*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmDestroyClientContext(void**);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmDisableAllocate(void);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmEnableAllocate(void);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmFree(void*);
RPCRTAPI RPC_SS_THREAD_HANDLE RPC_ENTRY RpcSmGetThreadHandle(RPC_STATUS*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmSetClientAllocFree(RPC_CLIENT_ALLOC*,RPC_CLIENT_FREE*);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmSetThreadHandle(RPC_SS_THREAD_HANDLE);
RPCRTAPI RPC_STATUS RPC_ENTRY RpcSmSwapClientAllocFree(RPC_CLIENT_ALLOC*,RPC_CLIENT_FREE*,RPC_CLIENT_ALLOC**,RPC_CLIENT_FREE**);
RPCRTAPI void RPC_ENTRY NdrRpcSsEnableAllocate(PMIDL_STUB_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrRpcSsDisableAllocate(PMIDL_STUB_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrRpcSmSetClientToOsf(PMIDL_STUB_MESSAGE);
RPCRTAPI void* RPC_ENTRY NdrRpcSmClientAllocate(size_t);
RPCRTAPI void RPC_ENTRY NdrRpcSmClientFree(void*);
RPCRTAPI void* RPC_ENTRY NdrRpcSsDefaultAllocate(size_t);
RPCRTAPI void RPC_ENTRY NdrRpcSsDefaultFree(void*);
RPCRTAPI PFULL_PTR_XLAT_TABLES RPC_ENTRY NdrFullPointerXlatInit(unsigned long,XLAT_SIDE);
RPCRTAPI void RPC_ENTRY NdrFullPointerXlatFree(PFULL_PTR_XLAT_TABLES);
RPCRTAPI int RPC_ENTRY NdrFullPointerQueryPointer(PFULL_PTR_XLAT_TABLES,void*,unsigned char,unsigned long*);
RPCRTAPI int RPC_ENTRY NdrFullPointerQueryRefId(PFULL_PTR_XLAT_TABLES,unsigned long,unsigned char,void**);
RPCRTAPI void RPC_ENTRY NdrFullPointerInsertRefId(PFULL_PTR_XLAT_TABLES,unsigned long,void*);
RPCRTAPI int RPC_ENTRY NdrFullPointerFree(PFULL_PTR_XLAT_TABLES,void*);
RPCRTAPI void* RPC_ENTRY NdrAllocate(PMIDL_STUB_MESSAGE,size_t);
RPCRTAPI void RPC_ENTRY NdrClearOutParameters(PMIDL_STUB_MESSAGE,PFORMAT_STRING,void*);
RPCRTAPI void* RPC_ENTRY NdrOleAllocate(size_t);
RPCRTAPI void RPC_ENTRY NdrOleFree(void*);

RPC_STATUS RPC_ENTRY NdrGetUserMarshalInfo(unsigned long*,unsigned long,NDR_USER_MARSHAL_INFO*);

/* 64-bit */
RPC_STATUS RPC_ENTRY NdrCreateServerInterfaceFromStub(struct IRpcStubBuffer*,RPC_SERVER_INTERFACE*);
CLIENT_CALL_RETURN RPC_VAR_ENTRY NdrClientCall3(MIDL_STUBLESS_PROXY_INFO*,unsigned long,void*,...);
CLIENT_CALL_RETURN RPC_VAR_ENTRY Ndr64AsyncClientCall(MIDL_STUBLESS_PROXY_INFO*,unsigned long,void*,...);
CLIENT_CALL_RETURN RPC_VAR_ENTRY Ndr64DcomAsyncClientCall(MIDL_STUBLESS_PROXY_INFO*,unsigned long,void*,...);

struct IRpcStubBuffer;  /* fwd */

RPCRTAPI void RPC_ENTRY Ndr64AsyncServerCall(PRPC_MESSAGE);
RPCRTAPI void RPC_ENTRY Ndr64AsyncServerCall64(PRPC_MESSAGE);
RPCRTAPI void RPC_ENTRY Ndr64AsyncServerCallAll(PRPC_MESSAGE);
RPCRTAPI long RPC_ENTRY Ndr64AsyncStubCall(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PRPC_MESSAGE,unsigned long*);
RPCRTAPI long RPC_ENTRY Ndr64DcomAsyncStubCall(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PRPC_MESSAGE,unsigned long*);
RPCRTAPI long RPC_ENTRY NdrStubCall3(struct IRpcStubBuffer*,struct IRpcChannelBuffer*,PRPC_MESSAGE,unsigned long*);
RPCRTAPI void RPC_ENTRY NdrServerCallAll(PRPC_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrServerCallNdr64(PRPC_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrServerCall3(PRPC_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrPartialIgnoreClientMarshall(PMIDL_STUB_MESSAGE,void*);
RPCRTAPI void RPC_ENTRY NdrPartialIgnoreServerUnmarshall(PMIDL_STUB_MESSAGE,void**);
RPCRTAPI void RPC_ENTRY NdrPartialIgnoreClientBufferSize(PMIDL_STUB_MESSAGE,void*);
RPCRTAPI void RPC_ENTRY NdrPartialIgnoreServerInitialize(PMIDL_STUB_MESSAGE,void**,PFORMAT_STRING);


#ifdef __cplusplus
}
#endif

#if defined(__RPC_WIN64__)
#include <poppack.h>
#endif

#endif /* _RPCNDR_H */
