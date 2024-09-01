#ifndef _WINLDAP_H
#define _WINLDAP_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* LDAP Client API definitions */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASETYPES
#include <windef.h>
#endif

#ifndef _SCHNLSP_H
#include <schnlsp.h>
#endif

#define WINLDAPAPI DECLSPEC_IMPORT

#ifndef LDAPAPI
#define LDAPAPI __cdecl
#endif

#ifndef LDAP_UNICODE
#ifdef UNICODE
#define LDAP_UNICODE  1
#else
#define LDAP_UNICODE  0
#endif
#endif /* LDAP_UNICODE */

#define LDAP_PORT  389
#define LDAP_SSL_PORT  636
#define LDAP_GC_PORT  3268
#define LDAP_SSL_GC_PORT  3269

#define LDAP_VERSION1  1
#define LDAP_VERSION2  2
#define LDAP_VERSION3  3
#define LDAP_VERSION  LDAP_VERSION2

#define LDAP_BIND_CMD  0x60L
#define LDAP_UNBIND_CMD  0x42L
#define LDAP_SEARCH_CMD  0x63L
#define LDAP_MODIFY_CMD  0x66L
#define LDAP_ADD_CMD  0x68L
#define LDAP_DELETE_CMD  0x4aL
#define LDAP_MODRDN_CMD  0x6cL
#define LDAP_COMPARE_CMD  0x6eL
#define LDAP_ABANDON_CMD  0x50L
#define LDAP_SESSION_CMD  0x71L
#define LDAP_EXTENDED_CMD  0x77L

#define LDAP_RES_BIND  0x61L
#define LDAP_RES_SEARCH_ENTRY  0x64L
#define LDAP_RES_SEARCH_RESULT  0x65L
#define LDAP_RES_MODIFY  0x67L
#define LDAP_RES_ADD  0x69L
#define LDAP_RES_DELETE  0x6bL
#define LDAP_RES_MODRDN  0x6dL
#define LDAP_RES_COMPARE  0x6fL
#define LDAP_RES_SESSION  0x72L
#define LDAP_RES_REFERRAL  0x73L
#define LDAP_RES_EXTENDED  0x78L
#define LDAP_RES_ANY  (-1L)

#define LDAP_INVALID_CMD  0xff
#define LDAP_INVALID_RES  0xff

#define LDAP_AUTH_SIMPLE  0x80L
#define LDAP_AUTH_SASL  0x83L
#define LDAP_AUTH_OTHERKIND  0x86L
#define LDAP_AUTH_SICILY  (LDAP_AUTH_OTHERKIND|0x0200)
#define LDAP_AUTH_MSN  (LDAP_AUTH_OTHERKIND|0x0800)
#define LDAP_AUTH_NTLM  (LDAP_AUTH_OTHERKIND|0x1000)
#define LDAP_AUTH_DPA  (LDAP_AUTH_OTHERKIND|0x2000)

#define LDAP_AUTH_NEGOTIATE  (LDAP_AUTH_OTHERKIND|0x0400)

#define LDAP_AUTH_SSPI  LDAP_AUTH_NEGOTIATE

#define LDAP_FILTER_AND  0xa0
#define LDAP_FILTER_OR  0xa1
#define LDAP_FILTER_NOT  0xa2
#define LDAP_FILTER_EQUALITY  0xa3
#define LDAP_FILTER_SUBSTRINGS  0xa4
#define LDAP_FILTER_GE  0xa5
#define LDAP_FILTER_LE  0xa6
#define LDAP_FILTER_PRESENT  0x87
#define LDAP_FILTER_APPROX  0xa8
#define LDAP_FILTER_EXTENSIBLE  0xa9

#define LDAP_SUBSTRING_INITIAL  0x80L
#define LDAP_SUBSTRING_ANY  0x81L
#define LDAP_SUBSTRING_FINAL  0x82L

#define LDAP_DEREF_NEVER  0
#define LDAP_DEREF_SEARCHING  1
#define LDAP_DEREF_FINDING  2
#define LDAP_DEREF_ALWAYS  3

#define LDAP_NO_LIMIT  0

#define LDAP_OPT_DNS  0x00000001
#define LDAP_OPT_CHASE_REFERRALS  0x00000002
#define LDAP_OPT_RETURN_REFS  0x00000004

#define LDAP_CONTROL_REFERRALS_W  L"1.2.840.113556.1.4.616"
#define LDAP_CONTROL_REFERRALS  "1.2.840.113556.1.4.616"

#define LDAP_MOD_ADD  0x00
#define LDAP_MOD_DELETE  0x01
#define LDAP_MOD_REPLACE  0x02
#define LDAP_MOD_BVALUES  0x80

#define LDAP_IS_CLDAP(ld)  ((ld)->ld_sb.sb_naddr > 0)
#define mod_values  mod_vals.modv_strvals
#define mod_bvalues  mod_vals.modv_bvals
#define NAME_ERROR(n)  ((n & 0xf0) == 0x20)

#define LDAP_OPT_DESC  0x01
#define LDAP_OPT_DEREF  0x02
#define LDAP_OPT_SIZELIMIT  0x03
#define LDAP_OPT_TIMELIMIT  0x04
#define LDAP_OPT_THREAD_FN_PTRS  0x05
#define LDAP_OPT_REBIND_FN  0x06
#define LDAP_OPT_REBIND_ARG  0x07
#define LDAP_OPT_REFERRALS  0x08
#define LDAP_OPT_RESTART  0x09
#define LDAP_OPT_SSL  0x0a
#define LDAP_OPT_IO_FN_PTRS  0x0b
#define LDAP_OPT_CACHE_FN_PTRS  0x0d
#define LDAP_OPT_CACHE_STRATEGY  0x0e
#define LDAP_OPT_CACHE_ENABLE  0x0f
#define LDAP_OPT_REFERRAL_HOP_LIMIT 0x10
#define LDAP_OPT_PROTOCOL_VERSION  0x11
#define LDAP_OPT_VERSION  0x11
#define LDAP_OPT_HOST_NAME  0x30
#define LDAP_OPT_ERROR_NUMBER  0x31
#define LDAP_OPT_ERROR_STRING  0x32
#define LDAP_OPT_SERVER_ERROR  0x33
#define LDAP_OPT_SERVER_EXT_ERROR  0x34
#define LDAP_OPT_HOST_REACHABLE  0x3E
#define LDAP_OPT_PING_KEEP_ALIVE  0x36
#define LDAP_OPT_PING_WAIT_TIME  0x37
#define LDAP_OPT_PING_LIMIT  0x38
#define LDAP_OPT_DNSDOMAIN_NAME  0x3B
#define LDAP_OPT_GETDSNAME_FLAGS  0x3D
#define LDAP_OPT_PROMPT_CREDENTIALS 0x3F
#define LDAP_OPT_AUTO_RECONNECT  0x91
#define LDAP_OPT_SSPI_FLAGS  0x92
#define LDAP_OPT_SSL_INFO  0x93
#define LDAP_OPT_SIGN  0x95
#define LDAP_OPT_ENCRYPT  0x96
#define LDAP_OPT_SASL_METHOD  0x97
#define LDAP_OPT_AREC_EXCLUSIVE  0x98
#define LDAP_OPT_SECURITY_CONTEXT  0x99

#define LDAP_OPT_ON  ((void *)1)
#define LDAP_OPT_OFF  ((void *)0)

#define LDAP_CHASE_SUBORDINATE_REFERRALS  0x00000020
#define LDAP_CHASE_EXTERNAL_REFERRALS  0x00000040

#define LDAP_SCOPE_BASE  0x00
#define LDAP_SCOPE_ONELEVEL  0x01
#define LDAP_SCOPE_SUBTREE  0x02

#define LDAP_MSG_ONE  0
#define LDAP_MSG_ALL  1
#define LDAP_MSG_RECEIVED  2

#define LBER_USE_DER  0x01
#define LBER_USE_INDEFINITE_LEN 0x02
#define LBER_TRANSLATE_STRINGS  0x04

#define LAPI_MAJOR_VER1  1
#define LAPI_MINOR_VER1  1

#define LDAP_SERVER_SORT_OID  "1.2.840.113556.1.4.473"
#define LDAP_SERVER_SORT_OID_W  L"1.2.840.113556.1.4.473"

#define LDAP_SERVER_RESP_SORT_OID  "1.2.840.113556.1.4.474"
#define LDAP_SERVER_RESP_SORT_OID_W  L"1.2.840.113556.1.4.474"

#define LDAP_PAGED_RESULT_OID_STRING  "1.2.840.113556.1.4.319"
#define LDAP_PAGED_RESULT_OID_STRING_W  L"1.2.840.113556.1.4.319"

#define LDAP_OPT_REFERRAL_CALLBACK  0x70

#define LDAP_OPT_CLIENT_CERTIFICATE  0x80

#define LDAP_OPT_SERVER_CERTIFICATE  0x81

#define LDAP_OPT_REF_DEREF_CONN_PER_MSG 0x94

typedef enum {
    LDAP_SUCCESS = 0x00,
    LDAP_OPERATIONS_ERROR = 0x01,
    LDAP_PROTOCOL_ERROR = 0x02,
    LDAP_TIMELIMIT_EXCEEDED = 0x03,
    LDAP_SIZELIMIT_EXCEEDED = 0x04,
    LDAP_COMPARE_FALSE = 0x05,
    LDAP_COMPARE_TRUE = 0x06,
    LDAP_AUTH_METHOD_NOT_SUPPORTED = 0x07,
    LDAP_STRONG_AUTH_REQUIRED = 0x08,
    LDAP_REFERRAL_V2 = 0x09,
    LDAP_PARTIAL_RESULTS = 0x09,
    LDAP_REFERRAL = 0x0a,
    LDAP_ADMIN_LIMIT_EXCEEDED = 0x0b,
    LDAP_UNAVAILABLE_CRIT_EXTENSION = 0x0c,
    LDAP_CONFIDENTIALITY_REQUIRED = 0x0d,
    LDAP_SASL_BIND_IN_PROGRESS = 0x0e,
    LDAP_NO_SUCH_ATTRIBUTE = 0x10,
    LDAP_UNDEFINED_TYPE = 0x11,
    LDAP_INAPPROPRIATE_MATCHING = 0x12,
    LDAP_CONSTRAINT_VIOLATION = 0x13,
    LDAP_ATTRIBUTE_OR_VALUE_EXISTS = 0x14,
    LDAP_INVALID_SYNTAX = 0x15,
    LDAP_NO_SUCH_OBJECT = 0x20,
    LDAP_ALIAS_PROBLEM = 0x21,
    LDAP_INVALID_DN_SYNTAX = 0x22,
    LDAP_IS_LEAF = 0x23,
    LDAP_ALIAS_DEREF_PROBLEM = 0x24,
    LDAP_INAPPROPRIATE_AUTH = 0x30,
    LDAP_INVALID_CREDENTIALS = 0x31,
    LDAP_INSUFFICIENT_RIGHTS = 0x32,
    LDAP_BUSY = 0x33,
    LDAP_UNAVAILABLE = 0x34,
    LDAP_UNWILLING_TO_PERFORM = 0x35,
    LDAP_LOOP_DETECT = 0x36,
    LDAP_NAMING_VIOLATION = 0x40,
    LDAP_OBJECT_CLASS_VIOLATION = 0x41,
    LDAP_NOT_ALLOWED_ON_NONLEAF = 0x42,
    LDAP_NOT_ALLOWED_ON_RDN = 0x43,
    LDAP_ALREADY_EXISTS = 0x44,
    LDAP_NO_OBJECT_CLASS_MODS = 0x45,
    LDAP_RESULTS_TOO_LARGE = 0x46,
    LDAP_AFFECTS_MULTIPLE_DSAS = 0x47,
    LDAP_OTHER = 0x50,
    LDAP_SERVER_DOWN = 0x51,
    LDAP_LOCAL_ERROR = 0x52,
    LDAP_ENCODING_ERROR = 0x53,
    LDAP_DECODING_ERROR = 0x54,
    LDAP_TIMEOUT = 0x55,
    LDAP_AUTH_UNKNOWN = 0x56,
    LDAP_FILTER_ERROR = 0x57,
    LDAP_USER_CANCELLED = 0x58,
    LDAP_PARAM_ERROR = 0x59,
    LDAP_NO_MEMORY = 0x5a,
    LDAP_CONNECT_ERROR = 0x5b,
    LDAP_NOT_SUPPORTED = 0x5c,
    LDAP_NO_RESULTS_RETURNED = 0x5e,
    LDAP_CONTROL_NOT_FOUND = 0x5d,
    LDAP_MORE_RESULTS_TO_RETURN = 0x5f,
    LDAP_CLIENT_LOOP = 0x60,
    LDAP_REFERRAL_LIMIT_EXCEEDED = 0x61
} LDAP_RETCODE;

#if !defined(_WIN64)
#include <pshpack4.h>
#endif

typedef struct ldap {
    struct {
        UINT_PTR sb_sd;
        UCHAR Reserved1[(10 * sizeof(ULONG)) + 1];
        ULONG_PTR sb_naddr;
        UCHAR Reserved2[(6 * sizeof(ULONG))];
    } ld_sb;
    PCHAR ld_host;
    ULONG ld_version;
    UCHAR ld_lberoptions;
    ULONG ld_deref;
    ULONG ld_timelimit;
    ULONG ld_sizelimit;
    ULONG ld_errno;
    PCHAR ld_matched;
    PCHAR ld_error;
    ULONG ld_msgid;
    UCHAR Reserved3[(6 * sizeof(ULONG)) + 1];
    ULONG ld_cldaptries;
    ULONG ld_cldaptimeout;
    ULONG ld_refhoplimit;
    ULONG ld_options;
} LDAP, *PLDAP;

typedef struct l_timeval {
    LONG tv_sec;
    LONG tv_usec;
} LDAP_TIMEVAL, *PLDAP_TIMEVAL;

typedef struct berval {
    ULONG bv_len;
    PCHAR bv_val;
} LDAP_BERVAL, *PLDAP_BERVAL, BERVAL, *PBERVAL;

typedef struct ldapmsg {
    ULONG lm_msgid;
    ULONG lm_msgtype;
    PVOID lm_ber;
    struct ldapmsg *lm_chain;
    struct ldapmsg *lm_next;
    ULONG lm_time;
    PLDAP Connection;
    PVOID Request;
    ULONG lm_returncode;
    USHORT lm_referral;
    BOOLEAN lm_chased;
    BOOLEAN lm_eom;
    BOOLEAN ConnectionReferenced;
} LDAPMessage, *PLDAPMessage;

typedef struct ldapcontrolA {
    PCHAR ldctl_oid;
    struct berval ldctl_value;
    BOOLEAN ldctl_iscritical;
} LDAPControlA, *PLDAPControlA;

typedef struct ldapcontrolW {
    PWCHAR ldctl_oid;
    struct berval ldctl_value;
    BOOLEAN ldctl_iscritical;
} LDAPControlW, *PLDAPControlW;

#if LDAP_UNICODE
#define LDAPControl LDAPControlW
#define PLDAPControl PLDAPControlW
#define LDAPMod LDAPModW
#define PLDAPMod PLDAPModW
#else
#define LDAPControl LDAPControlA
#define PLDAPControl PLDAPControlA
#define LDAPMod LDAPModA
#define PLDAPMod PLDAPModA
#endif

typedef struct ldapmodW {
    ULONG mod_op;
    PWCHAR mod_type;
    union {
        PWCHAR *modv_strvals;
        struct berval **modv_bvals;
    } mod_vals;
} LDAPModW, *PLDAPModW;

typedef struct ldapmodA {
    ULONG mod_op;
    PCHAR mod_type;
    union {
        PCHAR *modv_strvals;
        struct berval **modv_bvals;
    } mod_vals;
} LDAPModA, *PLDAPModA;

#if !defined(_WIN64)
#include <poppack.h>
#endif

typedef struct berelement {
    PCHAR opaque;
 } BerElement;

#define NULLBER ((BerElement *)0)

typedef struct ldap_version_info {
    ULONG lv_size;
    ULONG lv_major;
    ULONG lv_minor;
} LDAP_VERSION_INFO, *PLDAP_VERSION_INFO;

typedef ULONG (_cdecl *DBGPRINT)(PCH, ...);

typedef struct ldapsearch LDAPSearch, *PLDAPSearch;

typedef struct ldapsortkeyW {
    PWCHAR sk_attrtype;
    PWCHAR sk_matchruleoid;
    BOOLEAN sk_reverseorder;
} LDAPSortKeyW, *PLDAPSortKeyW;

typedef struct ldapsortkeyA {
    PCHAR sk_attrtype;
    PCHAR sk_matchruleoid;
    BOOLEAN sk_reverseorder;
} LDAPSortKeyA, *PLDAPSortKeyA;

typedef ULONG (_cdecl QUERYFORCONNECTION)(PLDAP,PLDAP,PWCHAR,PCHAR,ULONG,PVOID,PVOID,PLDAP*);
typedef BOOLEAN (_cdecl NOTIFYOFNEWCONNECTION)(PLDAP,PLDAP,PWCHAR,PCHAR,PLDAP,ULONG,PVOID,PVOID,ULONG);
typedef ULONG (_cdecl DEREFERENCECONNECTION)(PLDAP,PLDAP);

typedef struct LdapReferralCallback {
    ULONG SizeOfCallbacks;
    QUERYFORCONNECTION *QueryForConnection;
    NOTIFYOFNEWCONNECTION *NotifyRoutine;
    DEREFERENCECONNECTION *DereferenceRoutine;
} LDAP_REFERRAL_CALLBACK, *PLDAP_REFERRAL_CALLBACK;

typedef BOOLEAN (_cdecl QUERYCLIENTCERT)(PLDAP,PSecPkgContext_IssuerListInfoEx,HCERTSTORE,DWORD*);
typedef BOOLEAN (_cdecl VERIFYSERVERCERT)(PLDAP,PCCERT_CONTEXT);

WINLDAPAPI LDAP * LDAPAPI ldap_openW(const PWCHAR,ULONG);
WINLDAPAPI LDAP * LDAPAPI ldap_openA(const PCHAR,ULONG);
WINLDAPAPI LDAP * LDAPAPI ldap_initW(const PWCHAR,ULONG);
WINLDAPAPI LDAP * LDAPAPI ldap_initA(const PCHAR,ULONG);
WINLDAPAPI LDAP * LDAPAPI ldap_sslinitW(PWCHAR,ULONG,int);
WINLDAPAPI LDAP * LDAPAPI ldap_sslinitA(PCHAR,ULONG,int);
WINLDAPAPI ULONG LDAPAPI ldap_connect(LDAP*,struct l_timeval*);
WINLDAPAPI LDAP * LDAPAPI cldap_openW(PWCHAR,ULONG);
WINLDAPAPI LDAP * LDAPAPI cldap_openA(PCHAR,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_unbind(LDAP*);
WINLDAPAPI ULONG LDAPAPI ldap_unbind_s(LDAP*);
WINLDAPAPI ULONG LDAPAPI ldap_get_option(LDAP*,int,void*);
WINLDAPAPI ULONG LDAPAPI ldap_get_optionW(LDAP*,int,void*);
WINLDAPAPI ULONG LDAPAPI ldap_set_option(LDAP*,int,const void*);
WINLDAPAPI ULONG LDAPAPI ldap_set_optionW(LDAP*,int,const void*);
WINLDAPAPI ULONG LDAPAPI ldap_simple_bindW(LDAP*,PWCHAR,PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_simple_bindA(LDAP*,PCHAR,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_simple_bind_sW(LDAP*,PWCHAR,PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_simple_bind_sA(LDAP*,PCHAR,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_bindW(LDAP*,PWCHAR,PWCHAR cred,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_bindA(LDAP*,PCHAR,PCHAR cred,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_bind_sW(LDAP*,PWCHAR,PWCHAR cred,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_bind_sA(LDAP*,PCHAR,PCHAR cred,ULONG);
WINLDAPAPI INT LDAPAPI ldap_sasl_bindA(LDAP*,const PCHAR,const PCHAR,const BERVAL*,PLDAPControlA*,PLDAPControlA*,int*);
WINLDAPAPI INT LDAPAPI ldap_sasl_bindW(LDAP*,const PWCHAR,const PWCHAR,const BERVAL*,PLDAPControlW*,PLDAPControlW*,int*);
WINLDAPAPI INT LDAPAPI ldap_sasl_bind_sA(LDAP*,const PCHAR,const PCHAR,const BERVAL*,PLDAPControlA*,PLDAPControlA*,PBERVAL*);
WINLDAPAPI INT LDAPAPI ldap_sasl_bind_sW(LDAP*,const PWCHAR,const PWCHAR,const BERVAL*,PLDAPControlW*,PLDAPControlW*,PBERVAL*);
WINLDAPAPI ULONG LDAPAPI ldap_searchW(LDAP*,const PWCHAR base,ULONG,const PWCHAR,PWCHAR [],ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_searchA(LDAP*,const PCHAR,ULONG,const PCHAR,PCHAR [],ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_search_sW(LDAP*,const PWCHAR base,ULONG,const PWCHAR,PWCHAR [],ULONG,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_search_sA(LDAP*,const PCHAR,ULONG,const PCHAR,PCHAR [],ULONG,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_search_stW(LDAP*,const PWCHAR base,ULONG,const PWCHAR,PWCHAR [],ULONG,struct l_timeval*,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_search_stA(LDAP*,const PCHAR,ULONG,const PCHAR,PCHAR [],ULONG,struct l_timeval*,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_search_extW(LDAP*,const PWCHAR base,ULONG,const PWCHAR,PWCHAR [],ULONG,PLDAPControlW*,PLDAPControlW*,ULONG,ULONG,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_search_extA(LDAP*,const PCHAR,ULONG,const PCHAR,PCHAR [],ULONG,PLDAPControlA*,PLDAPControlA*,ULONG,ULONG,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_search_ext_sW(LDAP*,const PWCHAR base,ULONG,const PWCHAR,PWCHAR [],ULONG,PLDAPControlW*,PLDAPControlW*,struct l_timeval*,ULONG,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_search_ext_sA(LDAP*,const PCHAR,ULONG,const PCHAR,PCHAR [],ULONG,PLDAPControlA*,PLDAPControlA*,struct l_timeval*,ULONG,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_check_filterW(LDAP*,PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_check_filterA(LDAP*,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_modifyW(LDAP*,PWCHAR,LDAPModW*[]);
WINLDAPAPI ULONG LDAPAPI ldap_modifyA(LDAP*,PCHAR,LDAPModA*[]);
WINLDAPAPI ULONG LDAPAPI ldap_modify_sW(LDAP*,PWCHAR,LDAPModW*[]);
WINLDAPAPI ULONG LDAPAPI ldap_modify_sA(LDAP*,PCHAR,LDAPModA*[]);
WINLDAPAPI ULONG LDAPAPI ldap_modify_extW(LDAP*,const PWCHAR,LDAPModW*[],PLDAPControlW*,PLDAPControlW*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_modify_extA(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_modify_ext_sW(LDAP*,const PWCHAR,LDAPModW*[],PLDAPControlW*,PLDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_modify_ext_sA(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn2W(LDAP*,const PWCHAR,const PWCHAR,INT);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn2A(LDAP*,const PCHAR,const PCHAR,INT);
WINLDAPAPI ULONG LDAPAPI ldap_modrdnW(LDAP*,const PWCHAR,const PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_modrdnA(LDAP*,const PCHAR,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn2_sW(LDAP*,const PWCHAR,const PWCHAR,INT);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn2_sA(LDAP*,const PCHAR,const PCHAR,INT);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn_sW(LDAP*,const PWCHAR,const PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn_sA(LDAP*,const PCHAR,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_rename_extW(LDAP*,const PWCHAR,const PWCHAR,const PWCHAR,INT,PLDAPControlW*,PLDAPControlW*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_rename_extA(LDAP*,const PCHAR,const PCHAR,const PCHAR,INT,PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_rename_ext_sW(LDAP*,const PWCHAR,const PWCHAR,const PWCHAR,INT,PLDAPControlW*,PLDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_rename_ext_sA(LDAP*,const PCHAR,const PCHAR,const PCHAR,INT,PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_addW(LDAP*,PWCHAR,LDAPModW*[]);
WINLDAPAPI ULONG LDAPAPI ldap_addA(LDAP*,PCHAR,LDAPModA*[]);
WINLDAPAPI ULONG LDAPAPI ldap_add_sW(LDAP*,PWCHAR,LDAPModW*[]);
WINLDAPAPI ULONG LDAPAPI ldap_add_sA(LDAP*,PCHAR,LDAPModA*[]);
WINLDAPAPI ULONG LDAPAPI ldap_add_extW(LDAP*,const PWCHAR,LDAPModW*[],PLDAPControlW*,PLDAPControlW*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_add_extA(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_add_ext_sW(LDAP*,const PWCHAR,LDAPModW*[],PLDAPControlW*,PLDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_add_ext_sA(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_compareW(LDAP*,const PWCHAR,const PWCHAR,PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_compareA(LDAP*,const PCHAR,const PCHAR,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_compare_sW(LDAP*,const PWCHAR,const PWCHAR,PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_compare_sA(LDAP*,const PCHAR,const PCHAR,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_compare_extW(LDAP*,const PWCHAR,const PWCHAR,const PWCHAR,struct berval*,PLDAPControlW*,PLDAPControlW*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_compare_extA(LDAP*,const PCHAR,const PCHAR,const PCHAR,struct berval*,PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_compare_ext_sW(LDAP*,const PWCHAR,const PWCHAR Attr,const PWCHAR,struct berval*,PLDAPControlW*,PLDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_compare_ext_sA(LDAP*,const PCHAR,const PCHAR,const PCHAR,struct berval*,PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_deleteW(LDAP*,const PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_deleteA(LDAP*,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_delete_sW(LDAP*,const PWCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_delete_sA(LDAP*,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_delete_extW(LDAP*,const PWCHAR,PLDAPControlW*,PLDAPControlW*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_delete_extA(LDAP*,const PCHAR,PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_delete_ext_sW(LDAP*,const PWCHAR,PLDAPControlW*,PLDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_delete_ext_sA(LDAP*,const PCHAR,PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_abandon(LDAP*,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_result(LDAP*,ULONG,ULONG,struct l_timeval*,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_msgfree(LDAPMessage*);
WINLDAPAPI ULONG LDAPAPI ldap_result2error(LDAP*,LDAPMessage*,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_parse_resultW(LDAP*,LDAPMessage*,ULONG*,PWCHAR*,PWCHAR*,PWCHAR**,PLDAPControlW**,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_parse_resultA(LDAP*,LDAPMessage*,ULONG*,PCHAR*,PCHAR*,PCHAR**,PLDAPControlA**,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_parse_extended_resultA(LDAP*,LDAPMessage*,PCHAR*,struct berval**,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_parse_extended_resultW(LDAP*,LDAPMessage*,PWCHAR*,struct berval**,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_controls_freeA(LDAPControlA**);
WINLDAPAPI ULONG LDAPAPI ldap_control_freeA(LDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_controls_freeW(LDAPControlW**);
WINLDAPAPI ULONG LDAPAPI ldap_control_freeW(LDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_free_controlsW(LDAPControlW**);
WINLDAPAPI ULONG LDAPAPI ldap_free_controlsA(LDAPControlA**);
WINLDAPAPI PWCHAR LDAPAPI ldap_err2stringW(ULONG);
WINLDAPAPI PCHAR LDAPAPI ldap_err2stringA(ULONG);
WINLDAPAPI void LDAPAPI ldap_perror(LDAP*,const PCHAR);
WINLDAPAPI LDAPMessage *LDAPAPI ldap_first_entry(LDAP*,LDAPMessage*);
WINLDAPAPI LDAPMessage *LDAPAPI ldap_next_entry(LDAP*,LDAPMessage*);
WINLDAPAPI ULONG LDAPAPI ldap_count_entries(LDAP*,LDAPMessage*);
WINLDAPAPI PWCHAR LDAPAPI ldap_first_attributeW(LDAP*,LDAPMessage*,BerElement**);
WINLDAPAPI PCHAR LDAPAPI ldap_first_attributeA(LDAP*,LDAPMessage*,BerElement**);
WINLDAPAPI PWCHAR LDAPAPI ldap_next_attributeW(LDAP*,LDAPMessage*,BerElement*);
WINLDAPAPI PCHAR LDAPAPI ldap_next_attributeA(LDAP*,LDAPMessage*,BerElement*);
WINLDAPAPI PWCHAR *LDAPAPI ldap_get_valuesW(LDAP*,LDAPMessage*,const PWCHAR);
WINLDAPAPI PCHAR *LDAPAPI ldap_get_valuesA(LDAP*,LDAPMessage*,const PCHAR);
WINLDAPAPI struct berval **LDAPAPI ldap_get_values_lenW(LDAP*,LDAPMessage*,const PWCHAR);
WINLDAPAPI struct berval **LDAPAPI ldap_get_values_lenA(LDAP*,LDAPMessage*,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_count_valuesW(PWCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_count_valuesA(PCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_count_values_len(struct berval**);
WINLDAPAPI ULONG LDAPAPI ldap_value_freeW(PWCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_value_freeA(PCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_value_free_len(struct berval**);
WINLDAPAPI PWCHAR LDAPAPI ldap_get_dnW(LDAP*,LDAPMessage*);
WINLDAPAPI PCHAR LDAPAPI ldap_get_dnA(LDAP*,LDAPMessage*);
WINLDAPAPI PWCHAR *LDAPAPI ldap_explode_dnW(const PWCHAR,ULONG);
WINLDAPAPI PCHAR *LDAPAPI ldap_explode_dnA(const PCHAR,ULONG);
WINLDAPAPI PWCHAR LDAPAPI ldap_dn2ufnW(const PWCHAR);
WINLDAPAPI PCHAR LDAPAPI ldap_dn2ufnA(const PCHAR);
WINLDAPAPI VOID LDAPAPI ldap_memfreeW(PWCHAR);
WINLDAPAPI VOID LDAPAPI ldap_memfreeA(PCHAR);
WINLDAPAPI VOID LDAPAPI ber_bvfree(struct berval*);
WINLDAPAPI ULONG LDAPAPI ldap_ufn2dnW(const PWCHAR,PWCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_ufn2dnA(const PCHAR,PCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_startup(PLDAP_VERSION_INFO,HANDLE*);
WINLDAPAPI ULONG LDAPAPI ldap_cleanup(HANDLE);
WINLDAPAPI ULONG LDAPAPI ldap_escape_filter_elementW(PCHAR,ULONG,PWCHAR,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_escape_filter_elementA(PCHAR,ULONG,PCHAR,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_set_dbg_flags(ULONG);
WINLDAPAPI VOID LDAPAPI ldap_set_dbg_routine(DBGPRINT);
WINLDAPAPI int LDAPAPI LdapUTF8ToUnicode(LPCSTR,int,LPWSTR,int);
WINLDAPAPI int LDAPAPI LdapUnicodeToUTF8(LPCWSTR,int,LPSTR,int);
WINLDAPAPI ULONG LDAPAPI ldap_create_sort_controlA(PLDAP,PLDAPSortKeyA*,UCHAR,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_create_sort_controlW(PLDAP,PLDAPSortKeyW*,UCHAR,PLDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_sort_controlA(PLDAP,PLDAPControlA*,ULONG*,PCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_sort_controlW(PLDAP,PLDAPControlW*,ULONG*,PWCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_encode_sort_controlW(PLDAP,PLDAPSortKeyW*,PLDAPControlW,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_encode_sort_controlA(PLDAP,PLDAPSortKeyA*,PLDAPControlA,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_create_page_controlW(PLDAP,ULONG,struct berval*,UCHAR,PLDAPControlW*);
WINLDAPAPI ULONG LDAPAPI ldap_create_page_controlA(PLDAP,ULONG,struct berval*,UCHAR,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_page_controlW(PLDAP,PLDAPControlW*,ULONG*,struct berval**);
WINLDAPAPI ULONG LDAPAPI ldap_parse_page_controlA(PLDAP,PLDAPControlA*,ULONG*,struct berval**);
WINLDAPAPI PLDAPSearch LDAPAPI ldap_search_init_pageW(PLDAP,const PWCHAR,ULONG,const PWCHAR,PWCHAR[],ULONG,PLDAPControlW*,PLDAPControlW*,ULONG,ULONG,PLDAPSortKeyW*);
WINLDAPAPI PLDAPSearch LDAPAPI ldap_search_init_pageA(PLDAP,const PCHAR,ULONG,const PCHAR,PCHAR[],ULONG,PLDAPControlA*,PLDAPControlA*,ULONG,ULONG,PLDAPSortKeyA*);
WINLDAPAPI ULONG LDAPAPI ldap_get_next_page(PLDAP,PLDAPSearch,ULONG,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_get_next_page_s(PLDAP,PLDAPSearch,struct l_timeval*,ULONG,ULONG*,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_get_paged_count(PLDAP,PLDAPSearch,ULONG*,PLDAPMessage);
WINLDAPAPI ULONG LDAPAPI ldap_search_abandon_page(PLDAP,PLDAPSearch);
WINLDAPAPI LDAPMessage * LDAPAPI ldap_first_reference(LDAP*,LDAPMessage*);
WINLDAPAPI LDAPMessage * LDAPAPI ldap_next_reference(LDAP*,LDAPMessage*);
WINLDAPAPI ULONG LDAPAPI ldap_count_references(LDAP*,LDAPMessage*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_referenceW(LDAP*,LDAPMessage*,PWCHAR**);
WINLDAPAPI ULONG LDAPAPI ldap_parse_referenceA(LDAP*,LDAPMessage*,PCHAR**);
WINLDAPAPI ULONG LDAPAPI ldap_extended_operationW(LDAP*,const PWCHAR,struct berval*,PLDAPControlW*,PLDAPControlW*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_extended_operationA(LDAP*,const PCHAR,struct berval*,PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_close_extended_op(LDAP*,ULONG);
WINLDAPAPI ULONG LDAPAPI LdapGetLastError(VOID);
WINLDAPAPI ULONG LDAPAPI LdapMapErrorToWin32(ULONG);
WINLDAPAPI LDAP * LDAPAPI ldap_conn_from_msg(LDAP*,LDAPMessage*);

#if LDAP_UNICODE
#define ldap_open ldap_openW
#define ldap_init ldap_initW
#define ldap_sslinit ldap_sslinitW
#define cldap_open cldap_openW
#define ldap_get_option ldap_get_optionW
#define ldap_set_option ldap_set_optionW
#define ldap_simple_bind ldap_simple_bindW
#define ldap_simple_bind_s ldap_simple_bind_sW
#define ldap_bind ldap_bindW
#define ldap_bind_s ldap_bind_sW
#define ldap_sasl_bind ldap_sasl_bindW
#define ldap_sasl_bind_s ldap_sasl_bind_sW
#define ldap_search ldap_searchW
#define ldap_search_s ldap_search_sW
#define ldap_search_st ldap_search_stW
#define ldap_search_ext ldap_search_extW
#define ldap_search_ext_s ldap_search_ext_sW
#define ldap_check_filter ldap_check_filterW
#define ldap_modify ldap_modifyW
#define ldap_modify_s ldap_modify_sW
#define ldap_modify_ext ldap_modify_extW
#define ldap_modify_ext_s ldap_modify_ext_sW
#define ldap_modrdn2 ldap_modrdn2W
#define ldap_modrdn ldap_modrdnW
#define ldap_modrdn2_s ldap_modrdn2_sW
#define ldap_modrdn_s ldap_modrdn_sW
#define ldap_rename  ldap_rename_extW
#define ldap_rename_s ldap_rename_ext_sW
#define ldap_rename_ext ldap_rename_extW
#define ldap_rename_ext_s ldap_rename_ext_sW
#define ldap_add ldap_addW
#define ldap_add_s ldap_add_sW
#define ldap_add_ext ldap_add_extW
#define ldap_add_ext_s ldap_add_ext_sW
#define ldap_compare ldap_compareW
#define ldap_compare_s ldap_compare_sW
#define ldap_compare_ext ldap_compare_extW
#define ldap_compare_ext_s ldap_compare_ext_sW
#define ldap_delete ldap_deleteW
#define ldap_delete_ext ldap_delete_extW
#define ldap_delete_s ldap_delete_sW
#define ldap_delete_ext_s ldap_delete_ext_sW
#define ldap_parse_result ldap_parse_resultW
#define ldap_controls_free ldap_controls_freeW
#define ldap_control_free ldap_control_freeW
#define ldap_free_controls ldap_free_controlsW
#define ldap_parse_extended_result ldap_parse_extended_resultW
#define ldap_err2string ldap_err2stringW
#define ldap_first_attribute ldap_first_attributeW
#define ldap_next_attribute ldap_next_attributeW
#define ldap_get_values ldap_get_valuesW
#define ldap_get_values_len ldap_get_values_lenW
#define ldap_count_values ldap_count_valuesW
#define ldap_value_free ldap_value_freeW
#define ldap_get_dn ldap_get_dnW
#define ldap_explode_dn ldap_explode_dnW
#define ldap_dn2ufn ldap_dn2ufnW
#define ldap_memfree ldap_memfreeW
#define ldap_ufn2dn ldap_ufn2dnW
#define ldap_escape_filter_element ldap_escape_filter_elementW
#define LDAPSortKey LDAPSortKeyW
#define PLDAPSortKey PLDAPSortKeyW
#define ldap_create_sort_control ldap_create_sort_controlW
#define ldap_parse_sort_control ldap_parse_sort_controlW
#define ldap_encode_sort_control ldap_encode_sort_controlW
#define ldap_create_page_control ldap_create_page_controlW
#define ldap_parse_page_control  ldap_parse_page_controlW
#define ldap_search_init_page ldap_search_init_pageW
#define ldap_parse_reference ldap_parse_referenceW
#define ldap_extended_operation ldap_extended_operationW
#else /* LDAP_UNICODE */
WINLDAPAPI LDAP *LDAPAPI ldap_open(PCHAR,ULONG);
WINLDAPAPI LDAP *LDAPAPI ldap_init(PCHAR,ULONG);
WINLDAPAPI LDAP *LDAPAPI ldap_sslinit(PCHAR,ULONG,int);
WINLDAPAPI LDAP *LDAPAPI cldap_open(PCHAR,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_simple_bind(LDAP*,const PCHAR,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_simple_bind_s(LDAP*,const PCHAR,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_bind(LDAP*,const PCHAR,const PCHAR,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_bind_s(LDAP*,const PCHAR,const PCHAR,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_search(LDAP*,PCHAR,ULONG,PCHAR,PCHAR [],ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_search_s(LDAP*,PCHAR,ULONG,PCHAR,PCHAR [],ULONG,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_search_st(LDAP*,PCHAR,ULONG,PCHAR,PCHAR [],ULONG,struct l_timeval*,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_search_ext(LDAP*,PCHAR,ULONG,PCHAR,PCHAR [],ULONG,PLDAPControlA*,PLDAPControlA*,ULONG,ULONG,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_search_ext_s(LDAP*,PCHAR,ULONG,PCHAR,PCHAR [],ULONG,PLDAPControlA*,PLDAPControlA*,struct l_timeval*,ULONG,LDAPMessage**);
WINLDAPAPI ULONG LDAPAPI ldap_modify(LDAP*,PCHAR,LDAPModA*[]);
WINLDAPAPI ULONG LDAPAPI ldap_modify_s(LDAP*,PCHAR,LDAPModA*[]);
WINLDAPAPI ULONG LDAPAPI ldap_modify_ext(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_modify_ext_s(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn2(LDAP*,const PCHAR,const PCHAR,INT);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn(LDAP*,const PCHAR,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn2_s(LDAP*,const PCHAR,const PCHAR,INT);
WINLDAPAPI ULONG LDAPAPI ldap_modrdn_s(LDAP*,const PCHAR,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_rename_ext(LDAP*,const PCHAR,const PCHAR,const PCHAR,INT,PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_rename_ext_s(LDAP*,const PCHAR,const PCHAR,const PCHAR,INT,PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_add(LDAP*,PCHAR,LDAPMod*[]);
WINLDAPAPI ULONG LDAPAPI ldap_add_s(LDAP*,PCHAR,LDAPMod*[]);
WINLDAPAPI ULONG LDAPAPI ldap_add_ext(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_add_ext_s(LDAP*,const PCHAR,LDAPModA*[],PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_compare(LDAP*,const PCHAR,const PCHAR,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_compare_s(LDAP*,const PCHAR,const PCHAR,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_compare_ext(LDAP*,const PCHAR,const PCHAR,const PCHAR,struct berval*,PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_compare_ext_s(LDAP*,const PCHAR,const PCHAR,const PCHAR,struct berval*,PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_delete(LDAP*,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_delete_s(LDAP*,PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_delete_ext(LDAP*,const PCHAR,PLDAPControlA*,PLDAPControlA*,ULONG*);
WINLDAPAPI ULONG LDAPAPI ldap_delete_ext_s(LDAP*,const PCHAR,PLDAPControlA*,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_result(LDAP*,LDAPMessage*,ULONG*,PCHAR*,PCHAR*,PCHAR**,PLDAPControlA**,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_controls_free(LDAPControlA**);
WINLDAPAPI ULONG LDAPAPI ldap_control_free(LDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_free_controls(LDAPControlA**);
WINLDAPAPI PCHAR LDAPAPI ldap_err2string(ULONG);
WINLDAPAPI PCHAR LDAPAPI ldap_first_attribute(LDAP*,LDAPMessage*,BerElement**);
WINLDAPAPI PCHAR LDAPAPI ldap_next_attribute(LDAP*,LDAPMessage*,BerElement*);
WINLDAPAPI PCHAR *LDAPAPI ldap_get_values(LDAP*,LDAPMessage*,const PCHAR);
WINLDAPAPI struct berval **LDAPAPI ldap_get_values_len(LDAP*,LDAPMessage*,const PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_count_values(PCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_value_free(PCHAR*);
WINLDAPAPI PCHAR LDAPAPI ldap_get_dn(LDAP*,LDAPMessage*);
WINLDAPAPI PCHAR *LDAPAPI ldap_explode_dn(const PCHAR,ULONG);
WINLDAPAPI PCHAR LDAPAPI ldap_dn2ufn(const PCHAR);
WINLDAPAPI VOID LDAPAPI ldap_memfree(PCHAR);
WINLDAPAPI ULONG LDAPAPI ldap_ufn2dn(const PCHAR,PCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_escape_filter_element(PCHAR,ULONG,PCHAR,ULONG);
WINLDAPAPI ULONG LDAPAPI ldap_create_sort_control(PLDAP,PLDAPSortKeyA*,UCHAR,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_sort_control(PLDAP,PLDAPControlA*,ULONG*,PCHAR*);
WINLDAPAPI ULONG LDAPAPI ldap_encode_sort_control(PLDAP,PLDAPSortKeyA*,PLDAPControlA,BOOLEAN);
WINLDAPAPI ULONG LDAPAPI ldap_create_page_control(PLDAP,ULONG PageSize,struct berval*,UCHAR,PLDAPControlA*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_page_control(PLDAP,PLDAPControlA*,ULONG*,struct berval**);
WINLDAPAPI PLDAPSearch LDAPAPI ldap_search_init_page(PLDAP,const PCHAR,ULONG,const PCHAR,PCHAR[],ULONG,PLDAPControl*,PLDAPControl*,ULONG,ULONG,PLDAPSortKeyA*);
WINLDAPAPI ULONG LDAPAPI ldap_parse_reference(LDAP*,LDAPMessage*,PCHAR**);
WINLDAPAPI ULONG LDAPAPI ldap_extended_operation(LDAP*,const PCHAR,struct berval*,PLDAPControlA*,PLDAPControlA*,ULONG*);
#define ldap_sasl_bind ldap_sasl_bindA
#define ldap_sasl_bind_s ldap_sasl_bind_sA
#define ldap_check_filter ldap_check_filterA
#define ldap_rename  ldap_rename_extA
#define ldap_rename_s ldap_rename_ext_sA
#define ldap_parse_extended_result ldap_parse_extended_resultA
#define LDAPSortKey LDAPSortKeyA
#define PLDAPSortKey PLDAPSortKeyA
#endif /* LDAP_UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _WINLDAP_H */
