#ifndef _LMMSG_H
#define _LMMSG_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* NetMessage API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define MSGNAME_NOT_FORWARDED  0
#define MSGNAME_FORWARDED_TO  0x04
#define MSGNAME_FORWARDED_FROM  0x10

typedef struct _MSG_INFO_0 {
    LPWSTR msgi0_name;
} MSG_INFO_0, *PMSG_INFO_0, *LPMSG_INFO_0;

typedef struct _MSG_INFO_1 {
    LPWSTR msgi1_name;
    DWORD msgi1_forward_flag;
    LPWSTR msgi1_forward;
}MSG_INFO_1, *PMSG_INFO_1, *LPMSG_INFO_1;

NET_API_STATUS WINAPI NetMessageNameAdd(LPCWSTR,LPCWSTR);
NET_API_STATUS WINAPI NetMessageNameEnum(LPCWSTR,DWORD,PBYTE*,DWORD,PDWORD,PDWORD,PDWORD);
NET_API_STATUS WINAPI NetMessageNameGetInfo(LPCWSTR,LPCWSTR,DWORD,PBYTE*);
NET_API_STATUS WINAPI NetMessageNameDel(LPCWSTR,LPCWSTR);
NET_API_STATUS WINAPI NetMessageBufferSend(LPCWSTR,LPCWSTR,LPCWSTR,PBYTE,DWORD);

#ifdef __cplusplus
}
#endif

#endif /* _LMMSG_H */
