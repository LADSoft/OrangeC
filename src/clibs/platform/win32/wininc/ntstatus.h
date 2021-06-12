#ifndef NTSTATUS_H
#define NTSTATUS_H
#include <windows.h>

typedef LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;

#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_ERROR            0x3

#endif