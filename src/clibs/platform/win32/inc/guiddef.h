#define _NO_SYS_GUID_OPERATOR_EQ_
/* Windows GUID definitions */

#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;
#endif /* GUID_DEFINED */

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif
#endif

#ifndef DECLSPEC_SELECTANY
#define DECLSPEC_SELECTANY
#endif

#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif

#ifdef INITGUID
#define DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)  EXTERN_C const GUID name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
#else /* !INITGUID */
#define DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)  EXTERN_C const GUID name
#endif /* !INITGUID */

#define DEFINE_OLEGUID(name,l,w1,w2)  DEFINE_GUID(name,l,w1,w2,0xC0,0,0,0,0,0,0,0x46)

#ifndef _GUIDDEF_H
#define _GUIDDEF_H

/* #pragma GCC system_header */

#ifndef __LPGUID_DEFINED__
#define __LPGUID_DEFINED__
typedef GUID *LPGUID;
#endif

#ifndef __LPCGUID_DEFINED__
#define __LPCGUID_DEFINED__
typedef const GUID *LPCGUID;
#endif

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef GUID IID;
typedef IID *LPIID;
typedef GUID CLSID;
typedef CLSID *LPCLSID;
typedef GUID FMTID;
typedef FMTID *LPFMTID;

#define IsEqualIID(riid1,riid2)  IsEqualGUID(riid1,riid2)
#define IsEqualCLSID(rclsid1,rclsid2)  IsEqualGUID(rclsid1,rclsid2)
#define IsEqualFMTID(rfmtid1,rfmtid2)  IsEqualGUID(rfmtid1,rfmtid2)

#define IID_NULL  GUID_NULL
#define CLSID_NULL  GUID_NULL
#define FMTID_NULL GUID_NULL

#ifdef __midl_proxy
#define __MIDL_CONST
#else
#define __MIDL_CONST const
#endif

#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
#ifdef __cplusplus
#define REFGUID const GUID &
#else
#define REFGUID const GUID * __MIDL_CONST
#endif
#endif

#ifndef _REFIID_DEFINED
#define _REFIID_DEFINED
#ifdef __cplusplus
#define REFIID const IID &
#else
#define REFIID const IID * __MIDL_CONST
#endif
#endif

#ifndef _REFCLSID_DEFINED
#define _REFCLSID_DEFINED
#ifdef __cplusplus
#define REFCLSID const IID &
#else
#define REFCLSID const IID * __MIDL_CONST
#endif
#endif

#ifndef _REFFMTID_DEFINED
#define _REFFMTID_DEFINED
#ifdef __cplusplus
#define REFFMTID const IID &
#else
#define REFFMTID const IID * __MIDL_CONST
#endif
#endif

#endif /* __IID_DEFINED__ */

#ifndef _SYS_GUID_OPERATORS_
#define _SYS_GUID_OPERATORS_

#include <string.h>

#ifdef __cplusplus
__inline int InlineIsEqualGUID(REFGUID rguid1, REFGUID rguid2) {
   return (
      ((unsigned long*)&rguid1)[0] == ((unsigned long*)&rguid2)[0] &&
      ((unsigned long*)&rguid1)[1] == ((unsigned long*)&rguid2)[1] &&
      ((unsigned long*)&rguid1)[2] == ((unsigned long*)&rguid2)[2] &&
      ((unsigned long*)&rguid1)[3] == ((unsigned long*)&rguid2)[3]);
}

__inline int IsEqualGUID(REFGUID rguid1, REFGUID rguid2) {
    return memcmp(&rguid1, &rguid2, sizeof(GUID)) == 0;
}
#else /* __cplusplus */
#define InlineIsEqualGUID(rguid1, rguid2)  \
    (((unsigned long*)rguid1)[0] == ((unsigned long*)rguid2)[0] && \
    ((unsigned long*)rguid1)[1] == ((unsigned long*)rguid2)[1] && \
    ((unsigned long*)rguid1)[2] == ((unsigned long*)rguid2)[2] && \
    ((unsigned long*)rguid1)[3] == ((unsigned long*)rguid2)[3])

#define IsEqualGUID(rguid1,rguid2)  (memcmp(rguid1,rguid2,sizeof(GUID)) == 0)
#endif /* __cplusplus */

#ifdef __INLINE_ISEQUAL_GUID
#undef IsEqualGUID
#define IsEqualGUID(rguid1,rguid2)  InlineIsEqualGUID(rguid1,rguid2)
#endif
#define IsEqualIID(riid1,riid2)  IsEqualGUID(riid1,riid2)
#define IsEqualCLSID(rclsid1,rclsid2)  IsEqualGUID(rclsid1,rclsid2)

#if !defined _SYS_GUID_OPERATOR_EQ_ && !defined _NO_SYS_GUID_OPERATOR_EQ_
#define _SYS_GUID_OPERATOR_EQ_
#ifdef __cplusplus
__inline int operator==(REFGUID guidOne, REFGUID guidOther) {
    return IsEqualGUID(guidOne,guidOther);
}

__inline int operator!=(REFGUID guidOne, REFGUID guidOther) {
    return (guidOne != guidOther);
}
#endif /* __cplusplus */
#endif /* _SYS_GUID_OPERATOR_EQ_ */
#endif /* _SYS_GUID_OPERATORS_ */

#endif /* _GUIDDEF_H */

