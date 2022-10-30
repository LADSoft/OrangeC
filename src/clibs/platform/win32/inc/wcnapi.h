#ifndef _WCNAPI_H
#define _WCNAPI_H

/* Windows Connect Now API definitions (Windows 7) */

#if (NTDDI_VERSION >= NTDDI_WIN7)

#include <wcntypes.h>
#include <wcndevice.h>

#ifndef NO_WCN_PKEYS
#include <wcnfunctiondiscoverykeys.h>
#endif /* NO_WCN_KEYS */

#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

#endif /* _WCNAPI_H */
