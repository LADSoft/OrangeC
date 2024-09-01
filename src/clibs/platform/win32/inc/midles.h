#ifndef _MIDLES_H
#define _MIDLES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Encoding/decoding support definitions (serializing/deserializing a.k.a. pickling) */

#include <rpcndr.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    MES_ENCODE,
    MES_DECODE,
} MIDL_ES_CODE;

typedef enum {
    MES_INCREMENTAL_HANDLE,
    MES_FIXED_BUFFER_HANDLE,
    MES_DYNAMIC_BUFFER_HANDLE
} MIDL_ES_HANDLE_STYLE;

typedef void (__RPC_USER *MIDL_ES_ALLOC)(void*,char**,unsigned int*);
typedef void (__RPC_USER *MIDL_ES_WRITE)(void*,char*,unsigned int);
typedef void (__RPC_USER *MIDL_ES_READ)(void*,char**,unsigned int*);

typedef struct _MIDL_ES_MESSAGE {
    MIDL_STUB_MESSAGE StubMsg;
    MIDL_ES_CODE Operation;
    void *UserState;
    unsigned long MesVersion:8;
    unsigned long HandleStyle:8;
    unsigned long HandleFlags:8;
    unsigned long Reserve:8;
    MIDL_ES_ALLOC Alloc;
    MIDL_ES_WRITE Write;
    MIDL_ES_READ Read;
    unsigned char *Buffer;
    unsigned long BufferSize;
    unsigned char **pDynBuffer;
    unsigned long *pEncodedSize;
    RPC_SYNTAX_IDENTIFIER InterfaceId;
    unsigned long ProcNumber;
    unsigned long AlienDataRep;
    unsigned long IncrDataSize;
    unsigned long ByteCount;
} MIDL_ES_MESSAGE, *PMIDL_ES_MESSAGE;

typedef PMIDL_ES_MESSAGE MIDL_ES_HANDLE;

typedef struct _MIDL_TYPE_PICKLING_INFO {
    unsigned long Version;
    unsigned long Flags;
    UINT_PTR Reserved[3];
} MIDL_TYPE_PICKLING_INFO, *PMIDL_TYPE_PICKLING_INFO;

RPC_STATUS RPC_ENTRY MesEncodeIncrementalHandleCreate(void*,MIDL_ES_ALLOC,MIDL_ES_WRITE,handle_t*);
RPC_STATUS RPC_ENTRY MesDecodeIncrementalHandleCreate(void*,MIDL_ES_READ,handle_t*);
RPC_STATUS RPC_ENTRY MesIncrementalHandleReset(handle_t,void*,MIDL_ES_ALLOC,MIDL_ES_WRITE,MIDL_ES_READ,MIDL_ES_CODE);
RPC_STATUS RPC_ENTRY MesEncodeFixedBufferHandleCreate(char*,unsigned long,unsigned long*,handle_t*);
RPC_STATUS RPC_ENTRY MesEncodeDynBufferHandleCreate(char**,unsigned long*,handle_t*);
RPC_STATUS RPC_ENTRY MesDecodeBufferHandleCreate(char*,unsigned long,handle_t*);
RPC_STATUS RPC_ENTRY MesBufferHandleReset(handle_t,unsigned long,MIDL_ES_CODE,char**,unsigned long,unsigned long*);
RPC_STATUS RPC_ENTRY MesHandleFree(handle_t);
RPC_STATUS RPC_ENTRY MesInqProcEncodingId(handle_t,PRPC_SYNTAX_IDENTIFIER,unsigned long*);

#if defined(_MIPS_) || defined(_ALPHA_) || defined(_PPC_) || defined(_IA64_)
#define __RPC_UNALIGNED  __unaligned
#else
#define __RPC_UNALIGNED
#endif

void RPC_ENTRY I_NdrMesMessageInit(PMIDL_STUB_MESSAGE);
size_t RPC_ENTRY NdrMesSimpleTypeAlignSize(handle_t);
void RPC_ENTRY NdrMesSimpleTypeDecode(handle_t,void*,short);
void RPC_ENTRY NdrMesSimpleTypeEncode(handle_t,PMIDL_STUB_DESC,void*,short);
size_t RPC_ENTRY NdrMesTypeAlignSize(handle_t,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
void RPC_ENTRY NdrMesTypeEncode(handle_t,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
void RPC_ENTRY NdrMesTypeDecode(handle_t,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
size_t RPC_ENTRY NdrMesTypeAlignSize2(handle_t,PMIDL_TYPE_PICKLING_INFO,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
void RPC_ENTRY NdrMesTypeEncode2(handle_t,PMIDL_TYPE_PICKLING_INFO,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
void RPC_ENTRY NdrMesTypeDecode2(handle_t,PMIDL_TYPE_PICKLING_INFO,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
void RPC_ENTRY NdrMesTypeFree2(handle_t,PMIDL_TYPE_PICKLING_INFO,PMIDL_STUB_DESC,PFORMAT_STRING,void*);
void RPC_VAR_ENTRY NdrMesProcEncodeDecode(handle_t,PMIDL_STUB_DESC,PFORMAT_STRING,...);
CLIENT_CALL_RETURN RPC_VAR_ENTRY NdrMesProcEncodeDecode2(handle_t,PMIDL_STUB_DESC,PFORMAT_STRING,...);


#ifdef __cplusplus
}
#endif

#endif /* __MIDLES_H__ */
