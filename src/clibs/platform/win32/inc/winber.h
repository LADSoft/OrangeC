#ifndef _WINBER_H
#define _WINBER_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Basic Encoding Rules (BER) API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define WINBERAPI  DECLSPEC_IMPORT
#define BERAPI  __cdecl

#define LBER_ERROR  0xffffffffL
#define LBER_DEFAULT  0xffffffffL

WINBERAPI BerElement *BERAPI ber_init(BERVAL*);
WINBERAPI VOID BERAPI ber_free(BerElement*,INT);
WINBERAPI VOID BERAPI ber_bvfree(BERVAL*);
WINBERAPI VOID BERAPI ber_bvecfree(PBERVAL*);
WINBERAPI BERVAL *BERAPI ber_bvdup(BERVAL*);
WINBERAPI BerElement *BERAPI ber_alloc_t(INT);
WINBERAPI ULONG BERAPI ber_skip_tag(BerElement*,ULONG*);
WINBERAPI ULONG BERAPI ber_peek_tag(BerElement*,ULONG*);
WINBERAPI ULONG BERAPI ber_first_element(BerElement*,ULONG*,CHAR**);
WINBERAPI ULONG BERAPI ber_next_element(BerElement*,ULONG*,CHAR*);
WINBERAPI INT BERAPI ber_flatten(BerElement*,PBERVAL*);
WINBERAPI INT BERAPI ber_printf(BerElement*,PCHAR,...);
WINBERAPI ULONG BERAPI ber_scanf(BerElement*,PCHAR,...);

#ifdef __cplusplus
}
#endif

#endif /* _WINBER_H */
