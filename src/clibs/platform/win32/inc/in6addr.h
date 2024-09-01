#ifndef _IN6ADDR_H
#define _IN6ADDR_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef s6_addr

typedef struct in6_addr {
    union {
        UCHAR Byte[16];
        USHORT Word[8];
    } u;
} IN6_ADDR, *PIN6_ADDR, *LPIN6_ADDR;
#define in_addr6 in6_addr

#define _S6_un  u
#define _S6_u8  Byte
#define s6_addr  _S6_un._S6_u8

#define s6_bytes  u.Byte
#define s6_words  u.Word

#endif /* s6_addr */

#endif /* _IN6ADDR_H */
