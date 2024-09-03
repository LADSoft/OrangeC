#ifndef _MMSYSTEM_H
#define _MMSYSTEM_H

/* Windows multimedia API definitions */

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#include <mmsyscom.h>

#include <pshpack1.h>

#ifndef MMNOMCI
#include <mciapi.h>
#endif /* !MMNOMCI */

#include <mmiscapi.h>
#include <mmiscapi2.h>

#include <playsoundapi.h>

#include <mmeapi.h>

#ifndef MMNOTIMER
#include <timeapi.h>
#endif /* !MMNOTIMER */

#include <joystickapi.h>

#ifndef NEWTRANSPARENT
#define NEWTRANSPARENT  3
#define QUERYROPSUPPORT  40
#endif /* !NEWTRANSPARENT */

#define SELECTDIB    41
#define DIBINDEX(n)  MAKELONG((n),0x10FF)

#ifndef SC_SCREENSAVE
#define SC_SCREENSAVE  0xF140
#endif /* !SC_SCREENSAVE */

#include <poppack.h>

#endif /* _MMSYSTEM_H */
