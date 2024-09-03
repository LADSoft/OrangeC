#ifndef _MSWSOCKDEF_H
#define _MSWSOCKDEF_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#if (_WIN32_WINNT >= 0x0600)
#define MSWSOCKDEF_INLINE  inline
#endif /* _WIN32_WINNT>=0x0600 */

#ifndef ASSERT
#define MSWSOCKDEF_ASSERT_UNDEFINED
#define ASSERT(exp)  ((VOID)0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if(_WIN32_WINNT >= 0x0600)

#ifdef _WS2DEF_H

extern CONST UCHAR sockaddr_size[AF_MAX];

MSWSOCKDEF_INLINE UCHAR SOCKADDR_SIZE(ADDRESS_FAMILY af) {
    return (UCHAR)((af < AF_MAX) ? sockaddr_size[af] : sockaddr_size[AF_UNSPEC]);
}

MSWSOCKDEF_INLINE SCOPE_LEVEL ScopeLevel(SCOPE_ID ScopeId) {
    return (SCOPE_LEVEL)ScopeId.Level;
}

#endif /* _WS2DEF_H */

#define SIO_SET_COMPATIBILITY_MODE  _WSAIOW(IOC_VENDOR,300)

typedef enum _WSA_COMPATIBILITY_BEHAVIOR_ID {
    WsaBehaviorAll = 0,
    WsaBehaviorReceiveBuffering,
    WsaBehaviorAutoTuning
} WSA_COMPATIBILITY_BEHAVIOR_ID, *PWSA_COMPATIBILITY_BEHAVIOR_ID;

typedef struct _WSA_COMPATIBILITY_MODE {
    WSA_COMPATIBILITY_BEHAVIOR_ID BehaviorId;
    ULONG TargetOsVersion;
} WSA_COMPATIBILITY_MODE, *PWSA_COMPATIBILITY_MODE;

#endif /* _WIN32_WINNT>=0x0600 */

#ifdef __cplusplus
}
#endif

#ifdef MSWSOCKDEF_ASSERT_UNDEFINED
#undef ASSERT
#endif

#endif /* _MSWSOCKDEF_H */
