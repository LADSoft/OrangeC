#ifndef _WINDOWS_H
#define _WINDOWS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows API master include file */

#include <sdkddkver.h>

#if defined(RC_INVOKED) && !defined(NOWINRES)
#include <winresrc.h>
#else

#if defined(RC_INVOKED)
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#endif /* RC_INVOKED */

#if !defined(_X86_) && !defined(_ARM_) && !defined(_AMD64_) && defined(_M_IX86)
#define _X86_
#endif

#if !defined(_X86_) && !defined(_ARM_) && !defined(_AMD64_) && defined(_M_AMD64)
#define _AMD64_
#endif

#ifdef __ORANGEC__
/* Pelles C support nameless unions and structs */
#ifndef NONAMELESSUNION
#define _ANONYMOUS_UNION
#endif
#ifndef NONAMELESSSTRUCT
#define _ANONYMOUS_STRUCT
#endif
#elif defined(_MSC_VER)
/* MSVC support nameless unions and structs */
#ifndef NONAMELESSUNION
#define _ANONYMOUS_UNION
#endif
#ifndef NONAMELESSSTRUCT
#define _ANONYMOUS_STRUCT
#endif
#endif /* _MSC_VER */

#ifndef _ANONYMOUS_UNION
#define _UNION_NAME(x)  x
#define DUMMYUNIONNAME  u
#define DUMMYUNIONNAME2  u2
#define DUMMYUNIONNAME3  u3
#define DUMMYUNIONNAME4  u4
#define DUMMYUNIONNAME5  u5
#else
#define _UNION_NAME(x)
#define DUMMYUNIONNAME
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#define DUMMYUNIONNAME4
#define DUMMYUNIONNAME5
#endif

#ifndef _ANONYMOUS_STRUCT
#define _STRUCT_NAME(x)  x
#define DUMMYSTRUCTNAME  s
#define DUMMYSTRUCTNAME2  s2
#define DUMMYSTRUCTNAME3  s3
#else
#define _STRUCT_NAME(x)
#define DUMMYSTRUCTNAME
#define DUMMYSTRUCTNAME2
#define DUMMYSTRUCTNAME3
#endif

#ifndef RC_INVOKED
#include <excpt.h>
#include <stdarg.h>
#endif /* RC_INVOKED */

/* Shut up permanently */

#include <windef.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <winnls.h>
#include <wincon.h>
#include <winver.h>
#include <winreg.h>
#include <winnetwk.h>

#ifndef WIN32_LEAN_AND_MEAN
#include <cderr.h>
#include <dde.h>
#include <ddeml.h>
#include <dlgs.h>
#include <lzexpand.h>
#include <mmsystem.h>
#include <nb30.h>
#include <rpc.h>
#include <shellapi.h>
#include <winperf.h>
#include <winsock.h>
#ifndef NOCRYPT
#include <wincrypt.h>
#include <winefs.h>
#include <winscard.h>
#endif /* NOCRYPT */
#ifndef NOGDI
#include <winspool.h>
#include <ole2.h>
#include <commdlg.h>
#endif /* NOGDI */
#endif /* WIN32_LEAN_AND_MEAN */

/* #include <stralign.h> */

#ifndef NOSERVICE
#include <winsvc.h>
#endif /* NOSERVICE */
#ifndef NOMCX
#include <mcx.h>
#endif /* NOMCX */
#ifndef NOIME
#include <imm.h>
#endif /* NOIME */

#endif /* RC_INVOKED */

#endif /* _WINDOWS_H */
