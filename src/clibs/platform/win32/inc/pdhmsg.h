#ifndef _PDHMSG_H
#define _PDHMSG_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* EXTERN_C __declspec(selectany) const GUID S_PDH = {0x04d66358, 0xc4a1, 0x419b, {0x80, 0x23, 0x23, 0xb7, 0x39, 0x02, 0xde, 0x2c}}; */

#define PDH_CSTATUS_VALID_DATA  0x0L
#define PDH_CSTATUS_NEW_DATA  0x1L
#define PDH_CSTATUS_NO_MACHINE  0x800007D0L
#define PDH_CSTATUS_NO_INSTANCE  0x800007D1L
#define PDH_MORE_DATA  0x800007D2L
#define PDH_CSTATUS_ITEM_NOT_VALIDATED  0x800007D3L
#define PDH_RETRY  0x800007D4L
#define PDH_NO_DATA  0x800007D5L
#define PDH_CALC_NEGATIVE_DENOMINATOR  0x800007D6L
#define PDH_CALC_NEGATIVE_TIMEBASE  0x800007D7L
#define PDH_CALC_NEGATIVE_VALUE  0x800007D8L
#define PDH_DIALOG_CANCELLED  0x800007D9L
#define PDH_END_OF_LOG_FILE  0x800007DAL
#define PDH_ASYNC_QUERY_TIMEOUT  0x800007DBL
#define PDH_CANNOT_SET_DEFAULT_REALTIME_DATASOURCE  0x800007DCL
#define PDH_UNABLE_MAP_NAME_FILES  0x80000BD5L
#define PDH_PLA_VALIDATION_WARNING  0x80000BF3L
#define PDH_CSTATUS_NO_OBJECT  0xC0000BB8L
#define PDH_CSTATUS_NO_COUNTER  0xC0000BB9L
#define PDH_CSTATUS_INVALID_DATA  0xC0000BBAL
#define PDH_MEMORY_ALLOCATION_FAILURE  0xC0000BBBL
#define PDH_INVALID_HANDLE  0xC0000BBCL
#define PDH_INVALID_ARGUMENT  0xC0000BBDL
#define PDH_FUNCTION_NOT_FOUND  0xC0000BBEL
#define PDH_CSTATUS_NO_COUNTERNAME  0xC0000BBFL
#define PDH_CSTATUS_BAD_COUNTERNAME  0xC0000BC0L
#define PDH_INVALID_BUFFER  0xC0000BC1L
#define PDH_INSUFFICIENT_BUFFER  0xC0000BC2L
#define PDH_CANNOT_CONNECT_MACHINE  0xC0000BC3L
#define PDH_INVALID_PATH  0xC0000BC4L
#define PDH_INVALID_INSTANCE  0xC0000BC5L
#define PDH_INVALID_DATA  0xC0000BC6L
#define PDH_NO_DIALOG_DATA  0xC0000BC7L
#define PDH_CANNOT_READ_NAME_STRINGS  0xC0000BC8L
#define PDH_LOG_FILE_CREATE_ERROR  0xC0000BC9L
#define PDH_LOG_FILE_OPEN_ERROR  0xC0000BCAL
#define PDH_LOG_TYPE_NOT_FOUND  0xC0000BCBL
#define PDH_NO_MORE_DATA  0xC0000BCCL
#define PDH_ENTRY_NOT_IN_LOG_FILE  0xC0000BCDL
#define PDH_DATA_SOURCE_IS_LOG_FILE  0xC0000BCEL
#define PDH_DATA_SOURCE_IS_REAL_TIME  0xC0000BCFL
#define PDH_UNABLE_READ_LOG_HEADER  0xC0000BD0L
#define PDH_FILE_NOT_FOUND  0xC0000BD1L
#define PDH_FILE_ALREADY_EXISTS  0xC0000BD2L
#define PDH_NOT_IMPLEMENTED  0xC0000BD3L
#define PDH_STRING_NOT_FOUND  0xC0000BD4L
#define PDH_UNKNOWN_LOG_FORMAT  0xC0000BD6L
#define PDH_UNKNOWN_LOGSVC_COMMAND  0xC0000BD7L
#define PDH_LOGSVC_QUERY_NOT_FOUND  0xC0000BD8L
#define PDH_LOGSVC_NOT_OPENED  0xC0000BD9L
#define PDH_WBEM_ERROR  0xC0000BDAL
#define PDH_ACCESS_DENIED  0xC0000BDBL
#define PDH_LOG_FILE_TOO_SMALL  0xC0000BDCL
#define PDH_INVALID_DATASOURCE  0xC0000BDDL
#define PDH_INVALID_SQLDB  0xC0000BDEL
#define PDH_NO_COUNTERS  0xC0000BDFL
#define PDH_SQL_ALLOC_FAILED  0xC0000BE0L
#define PDH_SQL_ALLOCCON_FAILED  0xC0000BE1L
#define PDH_SQL_EXEC_DIRECT_FAILED  0xC0000BE2L
#define PDH_SQL_FETCH_FAILED  0xC0000BE3L
#define PDH_SQL_ROWCOUNT_FAILED  0xC0000BE4L
#define PDH_SQL_MORE_RESULTS_FAILED  0xC0000BE5L
#define PDH_SQL_CONNECT_FAILED  0xC0000BE6L
#define PDH_SQL_BIND_FAILED  0xC0000BE7L
#define PDH_CANNOT_CONNECT_WMI_SERVER  0xC0000BE8L
#define PDH_PLA_COLLECTION_ALREADY_RUNNING  0xC0000BE9L
#define PDH_PLA_ERROR_SCHEDULE_OVERLAP  0xC0000BEAL
#define PDH_PLA_COLLECTION_NOT_FOUND  0xC0000BEBL
#define PDH_PLA_ERROR_SCHEDULE_ELAPSED  0xC0000BECL
#define PDH_PLA_ERROR_NOSTART  0xC0000BEDL
#define PDH_PLA_ERROR_ALREADY_EXISTS  0xC0000BEEL
#define PDH_PLA_ERROR_TYPE_MISMATCH  0xC0000BEFL
#define PDH_PLA_ERROR_FILEPATH  0xC0000BF0L
#define PDH_PLA_SERVICE_ERROR  0xC0000BF1L
#define PDH_PLA_VALIDATION_ERROR  0xC0000BF2L
#define PDH_PLA_ERROR_NAME_TOO_LONG  0xC0000BF4L
#define PDH_INVALID_SQL_LOG_FORMAT  0xC0000BF5L
#define PDH_COUNTER_ALREADY_IN_QUERY  0xC0000BF6L
#define PDH_BINARY_LOG_CORRUPT  0xC0000BF7L
#define PDH_LOG_SAMPLE_TOO_SMALL  0xC0000BF8L
#define PDH_OS_LATER_VERSION  0xC0000BF9L
#define PDH_OS_EARLIER_VERSION  0xC0000BFAL
#define PDH_INCORRECT_APPEND_TIME  0xC0000BFBL
#define PDH_UNMATCHED_APPEND_COUNTER  0xC0000BFCL
#define PDH_SQL_ALTER_DETAIL_FAILED  0xC0000BFDL
#define PDH_QUERY_PERF_DATA_TIMEOUT  0xC0000BFEL
#define MSG_Publisher_Name  0x90000001L

#endif /* _PDHMSG_H */
