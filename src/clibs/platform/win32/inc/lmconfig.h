#ifndef _LMCONFIG_H
#define _LMCONFIG_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* NetConfig API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define REVISED_CONFIG_APIS

typedef struct _CONFIG_INFO_0 {
    LPWSTR cfgi0_key;
    LPWSTR cfgi0_data;
} CONFIG_INFO_0, *PCONFIG_INFO_0, *LPCONFIG_INFO_0;

NET_API_STATUS WINAPI NetConfigGet(LPCWSTR,LPCWSTR,LPCWSTR,PBYTE*);
NET_API_STATUS WINAPI NetConfigGetAll(LPCWSTR,LPCWSTR,PBYTE*);
NET_API_STATUS WINAPI NetConfigSet(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,DWORD,PBYTE,DWORD);
NET_API_STATUS WINAPI NetRegisterDomainNameChangeNotification(PHANDLE);
NET_API_STATUS WINAPI NetUnregisterDomainNameChangeNotification(HANDLE);

#ifdef __cplusplus
}
#endif

#endif /* _LMCONFIG_H */
