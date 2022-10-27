#ifndef _WINRESRC_H
#define _WINRESRC_H

/* Windows resource code definitions */

#ifndef WINVER
#define WINVER  0x0600
#endif

#ifndef _WIN32_IE
#define _WIN32_IE  0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS  0x0410
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0600
#endif

#define WIN32_LEAN_AND_MEAN
#include <winuser.h>
#include <winnt.h>
#include <winver.h>
#include <commctrl.h>
#include <dde.h>
#include <dlgs.h>

#endif /* _WINRESRC_H */
