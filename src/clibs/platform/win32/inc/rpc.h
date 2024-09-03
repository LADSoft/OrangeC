#ifndef RPC_NO_WINDOWS_H
#include <windows.h>
#endif

#ifndef _RPC_H
#define _RPC_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows RPC master include file */

#ifdef __cplusplus
extern "C" {
#endif

#include <basetsd.h>

#if defined(_M_IA64) || defined(_M_AMD64)
#define __RPC_WIN64__
#else
#define __RPC_WIN32__
#endif

#if defined(__RPC_WIN64__)
#include <pshpack8.h>
#endif

#ifndef __MIDL_USER_DEFINED
#define midl_user_allocate MIDL_user_allocate
#define midl_user_free MIDL_user_free
#define __MIDL_USER_DEFINED
#endif

#define RPC_UNICODE_SUPPORTED  /* in case somebody is looking */
#define __RPC_FAR  /* ditto */

#if !defined(_M_AMD64)
#define __RPC_API  __stdcall
#define __RPC_USER  __stdcall
#define __RPC_STUB  __stdcall
#define RPC_ENTRY  __stdcall
#else  /* 05-04-08 */
#define __RPC_API
#define __RPC_USER
#define __RPC_STUB
#define RPC_ENTRY
#endif

#ifndef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT  __declspec(dllimport)
#endif

#define RPCRTAPI  DECLSPEC_IMPORT
#define RPCNSAPI  DECLSPEC_IMPORT

typedef void *I_RPC_HANDLE;
typedef long RPC_STATUS;

#include <rpcdce.h>
#include <rpcnsi.h>
#include <rpcnterr.h>
#include <excpt.h>
#include <winerror.h>

#define RpcTryExcept  __try {
#define RpcExcept(x)  } __except (x) {
#define RpcEndExcept  }
#define RpcTryFinally  __try {
#define RpcFinally }  __finally {
#define RpcEndFinally  }
#define RpcExceptionCode()  GetExceptionCode()
#define RpcAbnormalTermination()  AbnormalTermination()

#ifndef RPC_NO_WINDOWS_H
#include <rpcasync.h>
#endif

#if defined(__RPC_WIN64__)
#include <poppack.h>
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RPC_H */
