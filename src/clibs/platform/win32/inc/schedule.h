#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows NT common schedule structure definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define SCHEDULE_INTERVAL  0
#define SCHEDULE_BANDWIDTH  1
#define SCHEDULE_PRIORITY  2

#define SCHEDULE_DATA_ENTRIES  (7*24)

typedef struct _SCHEDULE_HEADER {
    ULONG Type;
    ULONG Offset;
} SCHEDULE_HEADER, *PSCHEDULE_HEADER;

typedef struct _SCHEDULE {
    ULONG Size;
    ULONG Bandwidth;
    ULONG NumberOfSchedules;
    SCHEDULE_HEADER Schedules[1];
} SCHEDULE, *PSCHEDULE;

#ifdef __cplusplus
}
#endif

#endif /* _SCHEDULE_H */

