#ifndef _SQLTYPES_H
#define _SQLTYPES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ODBC type definitions */

#ifndef ODBCVER
#define ODBCVER 0x0351
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SQL_API __stdcall

typedef unsigned char SQLCHAR;
typedef long SQLINTEGER;
typedef unsigned long SQLUINTEGER;

#define SQLROWSETSIZE SQLUINTEGER

#ifdef _WIN64
typedef INT64 SQLLEN;
typedef INT64 SQLROWOFFSET;
typedef UINT64 SQLROWCOUNT;
typedef UINT64 SQLULEN;
typedef UINT64 SQLTRANSID;
typedef unsigned long SQLSETPOSIROW;
#else
#define SQLLEN SQLINTEGER
#define SQLROWOFFSET SQLINTEGER
#define SQLROWCOUNT SQLUINTEGER
#define SQLULEN SQLUINTEGER
#define SQLTRANSID DWORD
#define SQLSETPOSIROW SQLUSMALLINT
#endif /* _WIN64 */

typedef void * SQLPOINTER;
typedef short SQLSMALLINT;
typedef unsigned short SQLUSMALLINT;
typedef SQLSMALLINT SQLRETURN;

typedef unsigned char UCHAR;
typedef signed char SCHAR;
typedef SCHAR SQLSCHAR;
typedef long int SDWORD;
typedef short int SWORD;
typedef unsigned long int UDWORD;
typedef unsigned short int UWORD;
#ifndef _WIN64
typedef UDWORD SQLUINTEGER;
#endif

typedef signed long SLONG;
typedef signed short SSHORT;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef double SDOUBLE;
typedef double LDOUBLE;
typedef float SFLOAT;

typedef void *PTR;
typedef void *HENV;
typedef void *HDBC;
typedef void *HSTMT;

typedef signed short RETCODE;

typedef HWND SQLHWND;

typedef signed char SQLSCHAR;
typedef unsigned char SQLDATE;
typedef unsigned char SQLTIME;
typedef unsigned char SQLTIMESTAMP;
typedef unsigned char SQLVARCHAR;
typedef unsigned char SQLDECIMAL;
typedef unsigned char SQLNUMERIC;
typedef double SQLDOUBLE;
typedef double SQLFLOAT;
typedef float SQLREAL;

typedef void *SQLHANDLE;
typedef SQLHANDLE SQLHENV;
typedef SQLHANDLE SQLHDBC;
typedef SQLHANDLE SQLHSTMT;
typedef SQLHANDLE SQLHDESC;

#ifndef __SQLDATE
#define __SQLDATE
typedef struct tagDATE_STRUCT {
    SQLSMALLINT year;
    SQLUSMALLINT month;
    SQLUSMALLINT day;
} DATE_STRUCT;

typedef struct tagTIME_STRUCT {
    SQLUSMALLINT hour;
    SQLUSMALLINT minute;
    SQLUSMALLINT second;
} TIME_STRUCT;

typedef struct tagTIMESTAMP_STRUCT {
    SQLSMALLINT year;
    SQLUSMALLINT month;
    SQLUSMALLINT day;
    SQLUSMALLINT hour;
    SQLUSMALLINT minute;
    SQLUSMALLINT second;
    SQLUINTEGER fraction;
} TIMESTAMP_STRUCT;

typedef DATE_STRUCT SQL_DATE_STRUCT;
typedef TIME_STRUCT SQL_TIME_STRUCT;
typedef TIMESTAMP_STRUCT SQL_TIMESTAMP_STRUCT;

typedef enum {
    SQL_IS_YEAR = 1,
    SQL_IS_MONTH = 2,
    SQL_IS_DAY = 3,
    SQL_IS_HOUR = 4,
    SQL_IS_MINUTE = 5,
    SQL_IS_SECOND = 6,
    SQL_IS_YEAR_TO_MONTH = 7,
    SQL_IS_DAY_TO_HOUR = 8,
    SQL_IS_DAY_TO_MINUTE = 9,
    SQL_IS_DAY_TO_SECOND = 10,
    SQL_IS_HOUR_TO_MINUTE = 11,
    SQL_IS_HOUR_TO_SECOND = 12,
    SQL_IS_MINUTE_TO_SECOND = 13
} SQLINTERVAL;

typedef struct tagSQL_YEAR_MONTH {
    SQLUINTEGER year;
    SQLUINTEGER month;
} SQL_YEAR_MONTH_STRUCT;

typedef struct tagSQL_DAY_SECOND {
    SQLUINTEGER day;
    SQLUINTEGER hour;
    SQLUINTEGER minute;
    SQLUINTEGER second;
    SQLUINTEGER fraction;
} SQL_DAY_SECOND_STRUCT;

typedef struct tagSQL_INTERVAL_STRUCT {
    SQLINTERVAL interval_type;
    SQLSMALLINT interval_sign;
    union {
        SQL_YEAR_MONTH_STRUCT year_month;
        SQL_DAY_SECOND_STRUCT day_second;
    } intval;
} SQL_INTERVAL_STRUCT;
#endif /* __SQLDATE */

#ifdef __ORANGEC__
#define ODBCINT64 __int64
#endif
#ifdef ODBCINT64
typedef ODBCINT64 SQLBIGINT;
typedef unsigned ODBCINT64 SQLUBIGINT;
#endif

#define SQL_MAX_NUMERIC_LEN  16
typedef struct tagSQL_NUMERIC_STRUCT {
    SQLCHAR precision;
    SQLSCHAR scale;
    SQLCHAR sign;
    SQLCHAR val[SQL_MAX_NUMERIC_LEN];
} SQL_NUMERIC_STRUCT;

#if (ODBCVER >= 0x0350)
#ifdef GUID_DEFINED
typedef GUID SQLGUID;
#else
typedef struct tagSQLGUID {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[8];
} SQLGUID;
#endif /* GUID_DEFINED */
#endif /* ODBCVER >= 0x0350 */

typedef unsigned long int BOOKMARK;

#ifdef _WCHAR_T_DEFINED
typedef wchar_t SQLWCHAR;
#else
typedef unsigned short SQLWCHAR;
#endif

#ifdef UNICODE
typedef SQLWCHAR SQLTCHAR;
#else
typedef SQLCHAR SQLTCHAR;
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SQLTYPES_H */
