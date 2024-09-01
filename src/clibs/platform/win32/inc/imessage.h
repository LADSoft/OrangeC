#ifndef _IMESSAGE_H
#define _IMESSAGE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Messaging Applications Programming Interface (MAPI) IMessage-on-IStorage facility definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define IMSG_NO_ISTG_COMMIT  ((ULONG)0x00000001)

#define PROPATTR_MANDATORY  ((ULONG)0x00000001)
#define PROPATTR_READABLE  ((ULONG)0x00000002)
#define PROPATTR_WRITEABLE  ((ULONG)0x00000004)
#define PROPATTR_NOT_PRESENT  ((ULONG)0x00000008)

#define CbNewSPropAttrArray(_cattr)  (offsetof(SPropAttrArray,aPropAttr)+(_cattr)*sizeof(ULONG))
#define CbSPropAttrArray(_lparray)  (offsetof(SPropAttrArray,aPropAttr)+(UINT)((_lparray)->cValues)*sizeof(ULONG))
#define SizedSPropAttrArray(_cattr,_name)  struct _SPropAttrArray_ ## _name { ULONG cValues; ULONG aPropAttr[_cattr]; } _name

typedef struct _MSGSESS *LPMSGSESS;
typedef void (STDAPICALLTYPE MSGCALLRELEASE)(ULONG,LPMESSAGE);

typedef struct _SPropAttrArray {
    ULONG cValues;
    ULONG aPropAttr[MAPI_DIM];
} SPropAttrArray, *LPSPropAttrArray;

STDAPI_(SCODE) OpenIMsgSession(LPMALLOC,ULONG,LPMSGSESS*);
STDAPI_(void) CloseIMsgSession(LPMSGSESS);
STDAPI_(SCODE) OpenIMsgOnIStg(LPMSGSESS,LPALLOCATEBUFFER,LPALLOCATEMORE,LPFREEBUFFER,LPMALLOC,LPVOID,LPSTORAGE,MSGCALLRELEASE*,ULONG,ULONG,LPMESSAGE*);
STDAPI GetAttribIMsgOnIStg(LPVOID,LPSPropTagArray,LPSPropAttrArray*);
STDAPI SetAttribIMsgOnIStg(LPVOID,LPSPropTagArray,LPSPropAttrArray,LPSPropProblemArray*);
STDAPI_(SCODE) MapStorageSCode(SCODE);

#ifdef __cplusplus
}
#endif

#endif /* _IMESSAGE_H */
