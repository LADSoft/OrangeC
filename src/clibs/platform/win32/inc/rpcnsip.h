#ifndef _RPCNSIP_H
#define _RPCNSIP_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows RPC runtime autohandle definitions */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    RPC_NS_HANDLE LookupContext;
    RPC_BINDING_HANDLE ProposedHandle;
    RPC_BINDING_VECTOR *Bindings;
} RPC_IMPORT_CONTEXT_P, *PRPC_IMPORT_CONTEXT_P;

RPCNSAPI RPC_STATUS RPC_ENTRY I_RpcNsGetBuffer(PRPC_MESSAGE);
RPCNSAPI RPC_STATUS RPC_ENTRY I_RpcNsSendReceive(PRPC_MESSAGE,RPC_BINDING_HANDLE*);
RPCNSAPI void RPC_ENTRY I_RpcNsRaiseException(PRPC_MESSAGE,RPC_STATUS);
RPCNSAPI RPC_STATUS RPC_ENTRY I_RpcReBindBuffer(PRPC_MESSAGE);
RPCNSAPI RPC_STATUS RPC_ENTRY I_NsServerBindSearch(void);
RPCNSAPI RPC_STATUS RPC_ENTRY I_NsClientBindSearch(void);
RPCNSAPI void RPC_ENTRY I_NsClientBindDone(void);

#ifdef __cplusplus
}
#endif

#endif /* _RPCNSIP_H */
