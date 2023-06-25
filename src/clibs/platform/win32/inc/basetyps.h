#ifndef _BASETYPS_H
#define _BASETYPS_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* Windows type definitions */

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif

#define STDMETHODCALLTYPE  __stdcall
#define STDMETHODVCALLTYPE  __cdecl
#define STDAPICALLTYPE  __stdcall
#define STDAPIVCALLTYPE  __cdecl

#define STDAPI  EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)  EXTERN_C type STDAPICALLTYPE
#define STDMETHODIMP  HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)  type STDMETHODCALLTYPE
#define STDAPIV  EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type)  EXTERN_C type STDAPIVCALLTYPE
#define STDMETHODIMPV  HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type)  type STDMETHODVCALLTYPE

#if defined(__cplusplus) && !defined(CINTERFACE)
#define interface struct
#define STDMETHOD(method)  virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method)  virtual type STDMETHODCALLTYPE method
#define STDMETHODV(method)  virtual HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type,method)  virtual type STDMETHODVCALLTYPE method
#define PURE  = 0
#define THIS_
#define THIS  void
#define DECLARE_INTERFACE(iface)  interface iface
#define DECLARE_INTERFACE_(iface,baseiface)  interface iface : public baseiface
#else /* defined(__cplusplus) && !defined(CINTERFACE) */
#define interface struct
#define STDMETHOD(method)  HRESULT (STDMETHODCALLTYPE *method)
#define STDMETHOD_(type,method)  type (STDMETHODCALLTYPE *method)
#define STDMETHODV(method)  HRESULT (STDMETHODVCALLTYPE *method)
#define STDMETHODV_(type,method)  type (STDMETHODVCALLTYPE *method)
#define PURE
#define THIS_ INTERFACE *This,
#define THIS INTERFACE *This
#ifdef CONST_VTABLE
#define DECLARE_INTERFACE(iface)  typedef interface iface { const struct iface##Vtbl *lpVtbl; } iface; typedef const struct iface##Vtbl iface##Vtbl; const struct iface##Vtbl
#else
#define DECLARE_INTERFACE(iface)  typedef interface iface { struct iface##Vtbl *lpVtbl; } iface; typedef struct iface##Vtbl iface##Vtbl; struct iface##Vtbl
#endif /* CONST_VTABLE */
#define DECLARE_INTERFACE_(iface,baseiface)  DECLARE_INTERFACE(iface)
#endif /* defined(__cplusplus) && !defined(CINTERFACE) */

#include <guiddef.h>

#ifndef _ERROR_STATUS_T_DEFINED
typedef unsigned long error_status_t;
#define _ERROR_STATUS_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#endif /* _BASETYPS_H */

