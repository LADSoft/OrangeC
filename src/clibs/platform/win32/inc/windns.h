#ifndef _WINDNS_H
#define _WINDNS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Domain Name System (DNS) API definitions */

#ifdef __cplusplus
extern "C" {
#endif


#define SIZEOF_IP4_ADDRESS  (4)
#define IP4_ADDRESS_STRING_LENGTH  (16)
#define IP4_ADDRESS_STRING_BUFFER_LENGTH  (16)

#define DNS_ADDR_MAX_SOCKADDR_LENGTH  (32)

#undef  IP6_ADDRESS_STRING_LENGTH
#define IP6_ADDRESS_STRING_LENGTH  (65)
#define IP6_ADDRESS_STRING_BUFFER_LENGTH  (65)

#define DNS_ADDRESS_STRING_LENGTH  (IP6_ADDRESS_STRING_LENGTH)

#define INLINE_WORD_FLIP(out,in)  { WORD _in = (in); (out) = (_in<<8)|(_in>>8); }
#define INLINE_HTONS(out,in)  INLINE_WORD_FLIP(out,in)
#define INLINE_NTOHS(out,in)  INLINE_WORD_FLIP(out,in)

#define INLINE_DWORD_FLIP(out,in)  { DWORD _in = (in); (out) = ((_in<<8)&0x00ff0000)|(_in<<24)|((_in>>8)&0x0000ff00)|(_in>>24); }
#define INLINE_NTOHL(out,in)  INLINE_DWORD_FLIP(out,in)
#define INLINE_HTONL(out,in)  INLINE_DWORD_FLIP(out,in)

#define INLINE_WRITE_FLIPPED_WORD(pout,in)  INLINE_WORD_FLIP(*((UNALIGNED WORD *)(pout)),in)
#define INLINE_WRITE_FLIPPED_DWORD(pout,in)  INLINE_DWORD_FLIP(*((UNALIGNED DWORD *)(pout)),in)

#define DNS_PORT_HOST_ORDER  (0x0035)
#define DNS_PORT_NET_ORDER  (0x3500)

#define DNS_RFC_MAX_UDP_PACKET_LENGTH  (512)

#define DNS_MAX_NAME_LENGTH  (255)
#define DNS_MAX_LABEL_LENGTH  (63)

#define DNS_MAX_NAME_BUFFER_LENGTH  (256)
#define DNS_MAX_LABEL_BUFFER_LENGTH  (64)

#define DNS_IP4_REVERSE_DOMAIN_STRING_A  ("in-addr.arpa.")
#define DNS_IP4_REVERSE_DOMAIN_STRING_W  (L"in-addr.arpa.")

#define DNS_MAX_IP4_REVERSE_NAME_LENGTH  (IP4_ADDRESS_STRING_LENGTH+15)

#define DNS_IP6_REVERSE_DOMAIN_STRING_A  ("ip6.arpa.")
#define DNS_IP6_REVERSE_DOMAIN_STRING_W  (L"ip6.arpa.")

#define DNS_MAX_IP6_REVERSE_NAME_LENGTH  (75)

#define DNS_MAX_REVERSE_NAME_LENGTH  DNS_MAX_IP6_REVERSE_NAME_LENGTH

#define DNS_MAX_IP4_REVERSE_NAME_BUFFER_LENGTH  (DNS_MAX_IP4_REVERSE_NAME_LENGTH+1)
#define DNS_MAX_IP6_REVERSE_NAME_BUFFER_LENGTH  DNS_MAX_IP6_REVERSE_NAME_LENGTH
#define DNS_MAX_REVERSE_NAME_BUFFER_LENGTH  DNS_MAX_REVERSE_NAME_LENGTH

#define DNS_MAX_TEXT_STRING_LENGTH  (255)

#define DNS_HEADER_FLAGS(pHead)  (*((PWORD)(pHead)+1))

#define DNS_BYTE_FLIP_HEADER_COUNTS(pHeader) \
    { \
        PDNS_HEADER _head = (pHeader); \
        INLINE_HTONS(_head->Xid,            _head->Xid); \
        INLINE_HTONS(_head->QuestionCount,  _head->QuestionCount); \
        INLINE_HTONS(_head->AnswerCount,    _head->AnswerCount); \
        INLINE_HTONS(_head->NameServerCount,_head->NameServerCount); \
        INLINE_HTONS(_head->AdditionalCount,_head->AdditionalCount); \
    }

#define DNS_OFFSET_TO_QUESTION_NAME  sizeof(DNS_HEADER)
#define DNS_COMPRESSED_QUESTION_NAME  (0xC00C)
#define DNS_QUESTION_NAME_FROM_HEADER(_pHeader_)  ((PCHAR)((PDNS_HEADER)(_pHeader_)+1))
#define DNS_ANSWER_FROM_QUESTION(_pQuestion_)  ((PCHAR)((PDNS_QUESTION)(_pQuestion_)+1))

#define DNS_OPCODE_QUERY  0
#define DNS_OPCODE_IQUERY  1
#define DNS_OPCODE_SERVER_STATUS  2
#define DNS_OPCODE_UNKNOWN  3
#define DNS_OPCODE_NOTIFY  4
#define DNS_OPCODE_UPDATE  5

#define DNS_RCODE_NOERROR  0
#define DNS_RCODE_FORMERR  1
#define DNS_RCODE_SERVFAIL  2
#define DNS_RCODE_NXDOMAIN  3
#define DNS_RCODE_NOTIMPL  4
#define DNS_RCODE_REFUSED  5
#define DNS_RCODE_YXDOMAIN  6
#define DNS_RCODE_YXRRSET  7
#define DNS_RCODE_NXRRSET  8
#define DNS_RCODE_NOTAUTH  9
#define DNS_RCODE_NOTZONE  10
#define DNS_RCODE_MAX  15

#define DNS_RCODE_BADVERS  16
#define DNS_RCODE_BADSIG  16
#define DNS_RCODE_BADKEY  17
#define DNS_RCODE_BADTIME  18

#define DNS_RCODE_NO_ERROR  DNS_RCODE_NOERROR
#define DNS_RCODE_FORMAT_ERROR  DNS_RCODE_FORMERR
#define DNS_RCODE_SERVER_FAILURE  DNS_RCODE_SERVFAIL
#define DNS_RCODE_NAME_ERROR  DNS_RCODE_NXDOMAIN
#define DNS_RCODE_NOT_IMPLEMENTED  DNS_RCODE_NOTIMPL

#define DNS_CLASS_INTERNET  0x0001
#define DNS_CLASS_CSNET  0x0002
#define DNS_CLASS_CHAOS  0x0003
#define DNS_CLASS_HESIOD  0x0004
#define DNS_CLASS_NONE  0x00fe
#define DNS_CLASS_ALL  0x00ff
#define DNS_CLASS_ANY  0x00ff
#define DNS_CLASS_UNICAST_RESPONSE  0x8000

#define DNS_RCLASS_INTERNET  0x0100
#define DNS_RCLASS_CSNET  0x0200
#define DNS_RCLASS_CHAOS  0x0300
#define DNS_RCLASS_HESIOD  0x0400
#define DNS_RCLASS_NONE  0xfe00
#define DNS_RCLASS_ALL  0xff00
#define DNS_RCLASS_ANY  0xff00
#define DNS_RCLASS_UNICAST_RESPONSE  0x0080

#define DNS_TYPE_ZERO  0x0000
#define DNS_TYPE_A  0x0001
#define DNS_TYPE_NS  0x0002
#define DNS_TYPE_MD  0x0003
#define DNS_TYPE_MF  0x0004
#define DNS_TYPE_CNAME  0x0005
#define DNS_TYPE_SOA  0x0006
#define DNS_TYPE_MB  0x0007
#define DNS_TYPE_MG  0x0008
#define DNS_TYPE_MR  0x0009
#define DNS_TYPE_NULL  0x000a
#define DNS_TYPE_WKS  0x000b
#define DNS_TYPE_PTR  0x000c
#define DNS_TYPE_HINFO  0x000d
#define DNS_TYPE_MINFO  0x000e
#define DNS_TYPE_MX  0x000f
#define DNS_TYPE_TEXT  0x0010
#define DNS_TYPE_RP  0x0011
#define DNS_TYPE_AFSDB  0x0012
#define DNS_TYPE_X25  0x0013
#define DNS_TYPE_ISDN  0x0014
#define DNS_TYPE_RT  0x0015
#define DNS_TYPE_NSAP  0x0016
#define DNS_TYPE_NSAPPTR  0x0017
#define DNS_TYPE_SIG  0x0018
#define DNS_TYPE_KEY  0x0019
#define DNS_TYPE_PX  0x001a
#define DNS_TYPE_GPOS  0x001b
#define DNS_TYPE_AAAA  0x001c
#define DNS_TYPE_LOC  0x001d
#define DNS_TYPE_NXT  0x001e
#define DNS_TYPE_EID  0x001f
#define DNS_TYPE_NIMLOC  0x0020
#define DNS_TYPE_SRV  0x0021
#define DNS_TYPE_ATMA  0x0022
#define DNS_TYPE_NAPTR  0x0023
#define DNS_TYPE_KX  0x0024
#define DNS_TYPE_CERT  0x0025
#define DNS_TYPE_A6  0x0026
#define DNS_TYPE_DNAME  0x0027
#define DNS_TYPE_SINK  0x0028
#define DNS_TYPE_OPT  0x0029
#define DNS_TYPE_DS  0x002b
#define DNS_TYPE_RRSIG  0x002e
#define DNS_TYPE_NSEC  0x002f
#define DNS_TYPE_DNSKEY  0x0030
#define DNS_TYPE_DHCID  0x0031
#define DNS_TYPE_NSEC3  0x0032
#define DNS_TYPE_NSEC3PARAM  0x0033
#define DNS_TYPE_TLSA  0x0034
#define DNS_TYPE_UINFO  0x0064
#define DNS_TYPE_UID  0x0065
#define DNS_TYPE_GID  0x0066
#define DNS_TYPE_UNSPEC  0x0067
#define DNS_TYPE_ADDRS  0x00f8
#define DNS_TYPE_TKEY  0x00f9
#define DNS_TYPE_TSIG  0x00fa
#define DNS_TYPE_IXFR  0x00fb
#define DNS_TYPE_AXFR  0x00fc
#define DNS_TYPE_MAILB  0x00fd
#define DNS_TYPE_MAILA  0x00fe
#define DNS_TYPE_ALL  0x00ff
#define DNS_TYPE_ANY  0x00ff
#define DNS_TYPE_WINS  0xff01
#define DNS_TYPE_WINSR  0xff02
#define DNS_TYPE_NBSTAT  (DNS_TYPE_WINSR)

#define DNS_RTYPE_A  0x0100
#define DNS_RTYPE_NS  0x0200
#define DNS_RTYPE_MD  0x0300
#define DNS_RTYPE_MF  0x0400
#define DNS_RTYPE_CNAME  0x0500
#define DNS_RTYPE_SOA  0x0600
#define DNS_RTYPE_MB  0x0700
#define DNS_RTYPE_MG  0x0800
#define DNS_RTYPE_MR  0x0900
#define DNS_RTYPE_NULL  0x0a00
#define DNS_RTYPE_WKS  0x0b00
#define DNS_RTYPE_PTR  0x0c00
#define DNS_RTYPE_HINFO  0x0d00
#define DNS_RTYPE_MINFO  0x0e00
#define DNS_RTYPE_MX  0x0f00
#define DNS_RTYPE_TEXT  0x1000
#define DNS_RTYPE_RP  0x1100
#define DNS_RTYPE_AFSDB  0x1200
#define DNS_RTYPE_X25  0x1300
#define DNS_RTYPE_ISDN  0x1400
#define DNS_RTYPE_RT  0x1500
#define DNS_RTYPE_NSAP  0x1600
#define DNS_RTYPE_NSAPPTR  0x1700
#define DNS_RTYPE_SIG  0x1800
#define DNS_RTYPE_KEY  0x1900
#define DNS_RTYPE_PX  0x1a00
#define DNS_RTYPE_GPOS  0x1b00
#define DNS_RTYPE_AAAA  0x1c00
#define DNS_RTYPE_LOC  0x1d00
#define DNS_RTYPE_NXT  0x1e00
#define DNS_RTYPE_EID  0x1f00
#define DNS_RTYPE_NIMLOC  0x2000
#define DNS_RTYPE_SRV  0x2100
#define DNS_RTYPE_ATMA  0x2200
#define DNS_RTYPE_NAPTR  0x2300
#define DNS_RTYPE_KX  0x2400
#define DNS_RTYPE_CERT  0x2500
#define DNS_RTYPE_A6  0x2600
#define DNS_RTYPE_DNAME  0x2700
#define DNS_RTYPE_SINK  0x2800
#define DNS_RTYPE_OPT  0x2900
#define DNS_RTYPE_DS  0x2b00
#define DNS_RTYPE_RRSIG  0x2e00
#define DNS_RTYPE_NSEC  0x2f00
#define DNS_RTYPE_DNSKEY  0x3000
#define DNS_RTYPE_DHCID  0x3100
#define DNS_RTYPE_NSEC3  0x3200
#define DNS_RTYPE_NSEC3PARAM  0x3300
#define DNS_RTYPE_TLSA  0x3400
#define DNS_RTYPE_UINFO  0x6400
#define DNS_RTYPE_UID  0x6500
#define DNS_RTYPE_GID  0x6600
#define DNS_RTYPE_UNSPEC  0x6700
#define DNS_RTYPE_TKEY  0xf900
#define DNS_RTYPE_TSIG  0xfa00
#define DNS_RTYPE_IXFR  0xfb00
#define DNS_RTYPE_AXFR  0xfc00
#define DNS_RTYPE_MAILB  0xfd00
#define DNS_RTYPE_MAILA  0xfe00
#define DNS_RTYPE_ALL  0xff00
#define DNS_RTYPE_ANY  0xff00
#define DNS_RTYPE_WINS  0x01ff
#define DNS_RTYPE_WINSR  0x02ff

#ifndef ATMA_E164
#define DNS_ATMA_FORMAT_E164  1
#define DNS_ATMA_FORMAT_AESA  2
#define DNS_ATMA_MAX_ADDR_LENGTH  (20)
#else /* !ATMA_E164 */
#define DNS_ATMA_FORMAT_E164  ATM_E164
#define DNS_ATMA_FORMAT_AESA  ATM_AESA
#define DNS_ATMA_MAX_ADDR_LENGTH  ATM_ADDR_SIZE
#endif /* !ATMA_E164 */

#define DNS_ATMA_AESA_ADDR_LENGTH  (20)
#define DNS_ATMA_MAX_RECORD_LENGTH  (DNS_ATMA_MAX_ADDR_LENGTH+1)

#define DNSSEC_ALGORITHM_RSAMD5  1
#define DNSSEC_ALGORITHM_RSASHA1  5
#define DNSSEC_ALGORITHM_RSASHA1_NSEC3  7
#define DNSSEC_ALGORITHM_RSASHA256  8
#define DNSSEC_ALGORITHM_RSASHA512  10
#define DNSSEC_ALGORITHM_ECDSAP256_SHA256  13
#define DNSSEC_ALGORITHM_ECDSAP384_SHA384  14
#define DNSSEC_ALGORITHM_NULL  253
#define DNSSEC_ALGORITHM_PRIVATE  254

#define DNSSEC_DIGEST_ALGORITHM_SHA1  1
#define DNSSEC_DIGEST_ALGORITHM_SHA256  2
#define DNSSEC_DIGEST_ALGORITHM_SHA384  4

#define DNSSEC_PROTOCOL_NONE  0
#define DNSSEC_PROTOCOL_TLS  1
#define DNSSEC_PROTOCOL_EMAIL  2
#define DNSSEC_PROTOCOL_DNSSEC  3
#define DNSSEC_PROTOCOL_IPSEC  4

#define DNSSEC_KEY_FLAG_NOAUTH  0x0001
#define DNSSEC_KEY_FLAG_NOCONF  0x0002
#define DNSSEC_KEY_FLAG_FLAG2  0x0004
#define DNSSEC_KEY_FLAG_EXTEND  0x0008
#define DNSSEC_KEY_FLAG_
#define DNSSEC_KEY_FLAG_FLAG4  0x0010
#define DNSSEC_KEY_FLAG_FLAG5  0x0020

#define DNSSEC_KEY_FLAG_USER  0x0000
#define DNSSEC_KEY_FLAG_ZONE  0x0040
#define DNSSEC_KEY_FLAG_HOST  0x0080
#define DNSSEC_KEY_FLAG_NTPE3  0x00c0

#define DNSSEC_KEY_FLAG_FLAG8  0x0100
#define DNSSEC_KEY_FLAG_FLAG9  0x0200
#define DNSSEC_KEY_FLAG_FLAG10  0x0400
#define DNSSEC_KEY_FLAG_FLAG11  0x0800

#define DNSSEC_KEY_FLAG_SIG0  0x0000
#define DNSSEC_KEY_FLAG_SIG1  0x1000
#define DNSSEC_KEY_FLAG_SIG2  0x2000
#define DNSSEC_KEY_FLAG_SIG3  0x3000
#define DNSSEC_KEY_FLAG_SIG4  0x4000
#define DNSSEC_KEY_FLAG_SIG5  0x5000
#define DNSSEC_KEY_FLAG_SIG6  0x6000
#define DNSSEC_KEY_FLAG_SIG7  0x7000
#define DNSSEC_KEY_FLAG_SIG8  0x8000
#define DNSSEC_KEY_FLAG_SIG9  0x9000
#define DNSSEC_KEY_FLAG_SIG10  0xa000
#define DNSSEC_KEY_FLAG_SIG11  0xb000
#define DNSSEC_KEY_FLAG_SIG12  0xc000
#define DNSSEC_KEY_FLAG_SIG13  0xd000
#define DNSSEC_KEY_FLAG_SIG14  0xe000
#define DNSSEC_KEY_FLAG_SIG15  0xf000

#define DNS_TKEY_MODE_SERVER_ASSIGN  1
#define DNS_TKEY_MODE_DIFFIE_HELLMAN  2
#define DNS_TKEY_MODE_GSS  3
#define DNS_TKEY_MODE_RESOLVER_ASSIGN  4

#define DNS_WINS_FLAG_SCOPE  (0x80000000)
#define DNS_WINS_FLAG_LOCAL  (0x00010000)

#define IS_WORD_ALIGNED(p)  (!((UINT_PTR)(p)&(UINT_PTR)1))
#define IS_DWORD_ALIGNED(p)  (!((UINT_PTR)(p)&(UINT_PTR)3))
#define IS_QWORD_ALIGNED(p)  (!((UINT_PTR)(p)&(UINT_PTR)7))

#define DNS_CONFIG_FLAG_ALLOC  (0x00000001)

#define DNS_TEXT_RECORD_LENGTH(StringCount)  (FIELD_OFFSET(DNS_TXT_DATA, pStringArray) + ((StringCount) * sizeof(PCHAR)))
#define DNS_NULL_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_NULL_DATA, Data) + (ByteCount))
#define DNS_WKS_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_WKS_DATA, BitMask) + (ByteCount))
#define DNS_WINS_RECORD_LENGTH(IpCount)  (FIELD_OFFSET(DNS_WINS_DATA, WinsServers) + ((IpCount) * sizeof(IP4_ADDRESS)))
#define DNS_KEY_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_KEY_DATA, Key) + (ByteCount))
#define DNS_SIG_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_SIG_DATA, Signature) + (ByteCount))
#define DNS_NSEC_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_NSEC_DATA, TypeBitMaps) + (ByteCount))
#define DNS_DS_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_DS_DATA, Digest) + (ByteCount))
#define DNS_OPT_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_OPT_DATA, Data) + (ByteCount))
#define DNS_DHCID_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_DHCID_DATA, DHCID) + (ByteCount))
#define DNS_NSEC3_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_NSEC3_DATA, chData) + (ByteCount))
#define DNS_NSEC3PARAM_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_NSEC3PARAM_DATA, pbSalt) + (ByteCount))
#define DNS_TLSA_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_TLSA_DATA, bCertificateAssociationData) + (ByteCount))
#define DNS_UNKNOWN_RECORD_LENGTH(ByteCount)  (FIELD_OFFSET(DNS_UNKNOWN_DATA, bData) + (ByteCount))

#define DnsSectionZone  DnsSectionQuestion
#define DnsSectionPrereq  DnsSectionAnswer
#define DnsSectionUpdate  DnsSectionAuthority

#define DNSREC_SECTION  (0x00000003)
#define DNSREC_QUESTION  (0x00000000)
#define DNSREC_ANSWER  (0x00000001)
#define DNSREC_AUTHORITY  (0x00000002)
#define DNSREC_ADDITIONAL  (0x00000003)
#define DNSREC_ZONE  (0x00000000)
#define DNSREC_PREREQ  (0x00000001)
#define DNSREC_UPDATE  (0x00000002)
#define DNSREC_DELETE  (0x00000004)
#define DNSREC_NOEXIST  (0x00000004)

#define DNS_RECORD_FIXED_SIZE  FIELD_OFFSET(DNS_RECORD,Data)
#define SIZEOF_DNS_RECORD_HEADER  DNS_RECORD_FIXED_SIZE

#define DNS_RRSET_INIT(rrset)  { PDNS_RRSET _prrset = &(rrset); _prrset->pFirstRR = NULL; _prrset->pLastRR = (PDNS_RECORD)&_prrset->pFirstRR; }
#define DNS_RRSET_ADD(rrset,pnewRR)  { PDNS_RRSET _prrset = &(rrset); PDNS_RECORD _prrnew = (pnewRR); _prrset->pLastRR->pNext = _prrnew; _prrset->pLastRR = _prrnew; }
#define DNS_RRSET_TERMINATE(rrset)  { PDNS_RRSET _prrset = &(rrset); _prrset->pLastRR->pNext = NULL; }

#define DNS_QUERY_STANDARD  0x00000000
#define DNS_QUERY_ACCEPT_TRUNCATED_RESPONSE  0x00000001
#define DNS_QUERY_USE_TCP_ONLY  0x00000002
#define DNS_QUERY_NO_RECURSION  0x00000004
#define DNS_QUERY_BYPASS_CACHE  0x00000008
#define DNS_QUERY_NO_WIRE_QUERY  0x00000010
#define DNS_QUERY_NO_LOCAL_NAME  0x00000020
#define DNS_QUERY_NO_HOSTS_FILE  0x00000040
#define DNS_QUERY_NO_NETBT  0x00000080
#define DNS_QUERY_WIRE_ONLY  0x00000100
#define DNS_QUERY_RETURN_MESSAGE  0x00000200
#define DNS_QUERY_MULTICAST_ONLY  0x00000400
#define DNS_QUERY_NO_MULTICAST  0x00000800
#define DNS_QUERY_TREAT_AS_FQDN  0x00001000
#define DNS_QUERY_ADDRCONFIG  0x00002000
#define DNS_QUERY_DUAL_ADDR  0x00004000
#define DNS_QUERY_DONT_RESET_TTL_VALUES  0x00100000
#define DNS_QUERY_DISABLE_IDN_ENCODING  0x00200000
#define DNS_QUERY_APPEND_MULTILABEL  0x00800000
#define DNS_QUERY_DNSSEC_OK  0x01000000
#define DNS_QUERY_DNSSEC_CHECKING_DISABLED  0x02000000
#define DNS_QUERY_RESERVED  0xff000000

#define DNS_QUERY_CACHE_ONLY  DNS_QUERY_NO_WIRE_QUERY

#define DNS_UPDATE_SECURITY_USE_DEFAULT  0x00000000
#define DNS_UPDATE_SECURITY_OFF  0x00000010
#define DNS_UPDATE_SECURITY_ON  0x00000020
#define DNS_UPDATE_SECURITY_ONLY  0x00000100
#define DNS_UPDATE_CACHE_SECURITY_CONTEXT  0x00000200
#define DNS_UPDATE_TEST_USE_LOCAL_SYS_ACCT  0x00000400
#define DNS_UPDATE_FORCE_SECURITY_NEGO  0x00000800
#define DNS_UPDATE_TRY_ALL_MASTER_SERVERS  0x00001000
#define DNS_UPDATE_SKIP_NO_UPDATE_ADAPTERS  0x00002000
#define DNS_UPDATE_REMOTE_SERVER  0x00004000
#define DNS_UPDATE_RESERVED  0xffff0000

#define DNS_VALSVR_ERROR_INVALID_ADDR  0x01
#define DNS_VALSVR_ERROR_INVALID_NAME  0x02
#define DNS_VALSVR_ERROR_UNREACHABLE  0x03
#define DNS_VALSVR_ERROR_NO_RESPONSE  0x04
#define DNS_VALSVR_ERROR_NO_AUTH  0x05
#define DNS_VALSVR_ERROR_REFUSED  0x06

#define DNS_VALSVR_ERROR_NO_TCP  0x10
#define DNS_VALSVR_ERROR_UNKNOWN  0xFF

#define DNS_CONNECTION_NAME_MAX_LENGTH  64
#define DNS_CONNECTION_PROXY_INFO_CURRENT_VERSION  1
#define DNS_CONNECTION_PROXY_INFO_SERVER_MAX_LENGTH  256
#define DNS_CONNECTION_PROXY_INFO_FRIENDLY_NAME_MAX_LENGTH  64
#define DNS_CONNECTION_PROXY_INFO_USERNAME_MAX_LENGTH  128
#define DNS_CONNECTION_PROXY_INFO_PASSWORD_MAX_LENGTH  128
#define DNS_CONNECTION_PROXY_INFO_EXCEPTION_MAX_LENGTH  1024
#define DNS_CONNECTION_PROXY_INFO_EXTRA_INFO_MAX_LENGTH  1024

#define DNS_CONNECTION_PROXY_INFO_FLAG_DISABLED  0x1
#define DNS_CONNECTION_PROXY_INFO_FLAG_BYPASSLOCAL  0x2

#define DNS_CONNECTION_POLICY_ENTRY_ONDEMAND  0x00000001

typedef unsigned __int64 QWORD, *PQWORD;
typedef LONG DNS_STATUS, *PDNS_STATUS;
typedef DWORD IP4_ADDRESS, *PIP4_ADDRESS;

typedef struct _IP4_ARRAY {
    DWORD AddrCount;
    IP4_ADDRESS AddrArray[1];
} IP4_ARRAY, *PIP4_ARRAY;

typedef union {
#ifdef _WIN64
    QWORD IP6Qword[2];
#endif /* _WIN64 */
    DWORD IP6Dword[4];
    WORD IP6Word[8];
    BYTE IP6Byte[16];
#ifdef IN6_ADDR
    IN6_ADDR In6;
#endif /* IN6_ADDR */
} IP6_ADDRESS, *PIP6_ADDRESS;

#include <pshpack1.h>
typedef struct _DnsAddr {
    CHAR MaxSa[DNS_ADDR_MAX_SOCKADDR_LENGTH];
    union {
        DWORD DnsAddrUserDword[8];
    } Data;
} DNS_ADDR, *PDNS_ADDR;

typedef struct _DnsAddrArray {
    DWORD MaxCount;
    DWORD AddrCount;
    DWORD Tag;
    WORD Family;
    WORD WordReserved;
    DWORD Flags;
    DWORD MatchFlag;
    DWORD Reserved1;
    DWORD Reserved2;
    DNS_ADDR AddrArray[1];
} DNS_ADDR_ARRAY, *PDNS_ADDR_ARRAY;
#include <poppack.h>

#include <pshpack1.h>
typedef struct _DNS_HEADER {
    WORD Xid;
    BYTE RecursionDesired:1;
    BYTE Truncation:1;
    BYTE Authoritative:1;
    BYTE Opcode:4;
    BYTE IsResponse:1;
    BYTE ResponseCode:4;
    BYTE Reserved:3;
    BYTE RecursionAvailable:1;
    WORD QuestionCount;
    WORD AnswerCount;
    WORD NameServerCount;
    WORD AdditionalCount;
} DNS_HEADER, *PDNS_HEADER;

typedef struct _DNS_HEADER_EXT {
    WORD Reserved:15;
    WORD DnssecOk:1;
    BYTE chRcode;
    BYTE chVersion;
} DNS_HEADER_EXT, *PDNS_HEADER_EXT;

typedef struct _DNS_WIRE_QUESTION {
    WORD QuestionType;
    WORD QuestionClass;
} DNS_WIRE_QUESTION, *PDNS_WIRE_QUESTION;

typedef struct _DNS_WIRE_RECORD {
    WORD RecordType;
    WORD RecordClass;
    DWORD TimeToLive;
    WORD DataLength;
} DNS_WIRE_RECORD, *PDNS_WIRE_RECORD;
#include <poppack.h>

typedef enum {
    DnsConfigPrimaryDomainName_W,
    DnsConfigPrimaryDomainName_A,
    DnsConfigPrimaryDomainName_UTF8,
    DnsConfigAdapterDomainName_W,
    DnsConfigAdapterDomainName_A,
    DnsConfigAdapterDomainName_UTF8,
    DnsConfigDnsServerList,
    DnsConfigSearchList,
    DnsConfigAdapterInfo,
    DnsConfigPrimaryHostNameRegistrationEnabled,
    DnsConfigAdapterHostNameRegistrationEnabled,
    DnsConfigAddressRegistrationMaxCount,
    DnsConfigHostName_W,
    DnsConfigHostName_A,
    DnsConfigHostName_UTF8,
    DnsConfigFullHostName_W,
    DnsConfigFullHostName_A,
    DnsConfigFullHostName_UTF8
} DNS_CONFIG_TYPE;

typedef struct {
    IP4_ADDRESS IpAddress;
} DNS_A_DATA, *PDNS_A_DATA;

typedef struct {
    PWSTR pNameHost;
} DNS_PTR_DATAW, *PDNS_PTR_DATAW;

typedef struct {
    PSTR pNameHost;
} DNS_PTR_DATAA, *PDNS_PTR_DATAA;

typedef struct {
    PWSTR pNamePrimaryServer;
    PWSTR pNameAdministrator;
    DWORD dwSerialNo;
    DWORD dwRefresh;
    DWORD dwRetry;
    DWORD dwExpire;
    DWORD dwDefaultTtl;
} DNS_SOA_DATAW, *PDNS_SOA_DATAW;

typedef struct {
    PSTR pNamePrimaryServer;
    PSTR pNameAdministrator;
    DWORD dwSerialNo;
    DWORD dwRefresh;
    DWORD dwRetry;
    DWORD dwExpire;
    DWORD dwDefaultTtl;
} DNS_SOA_DATAA, *PDNS_SOA_DATAA;

typedef struct {
    PWSTR pNameMailbox;
    PWSTR pNameErrorsMailbox;
} DNS_MINFO_DATAW, *PDNS_MINFO_DATAW;

typedef struct {
    PSTR pNameMailbox;
    PSTR pNameErrorsMailbox;
} DNS_MINFO_DATAA, *PDNS_MINFO_DATAA;

typedef struct {
    PWSTR pNameExchange;
    WORD wPreference;
    WORD Pad;
} DNS_MX_DATAW, *PDNS_MX_DATAW;

typedef struct {
    PSTR pNameExchange;
    WORD wPreference;
    WORD Pad;
} DNS_MX_DATAA, *PDNS_MX_DATAA;

typedef struct {
    DWORD dwStringCount;
    PWSTR pStringArray[1];
} DNS_TXT_DATAW, *PDNS_TXT_DATAW;

typedef struct {
    DWORD dwStringCount;
    PSTR pStringArray[1];
} DNS_TXT_DATAA, *PDNS_TXT_DATAA;

typedef struct {
    DWORD dwByteCount;
    BYTE Data[1];
} DNS_NULL_DATA, *PDNS_NULL_DATA;

typedef struct {
    IP4_ADDRESS IpAddress;
    UCHAR chProtocol;
    BYTE BitMask[1];
} DNS_WKS_DATA, *PDNS_WKS_DATA;

typedef struct {
    IP6_ADDRESS Ip6Address;
} DNS_AAAA_DATA, *PDNS_AAAA_DATA;

typedef struct {
    PWSTR pNameSigner;
    WORD wTypeCovered;
    BYTE chAlgorithm;
    BYTE chLabelCount;
    DWORD dwOriginalTtl;
    DWORD dwExpiration;
    DWORD dwTimeSigned;
    WORD wKeyTag;
    WORD Pad;
    BYTE Signature[1];
} DNS_SIG_DATAW, *PDNS_SIG_DATAW, DNS_RRSIG_DATAW, *PDNS_RRSIG_DATAW;

typedef struct {
    PSTR pNameSigner;
    WORD wTypeCovered;
    BYTE chAlgorithm;
    BYTE chLabelCount;
    DWORD dwOriginalTtl;
    DWORD dwExpiration;
    DWORD dwTimeSigned;
    WORD wKeyTag;
    WORD Pad;
    BYTE Signature[1];
} DNS_SIG_DATAA, *PDNS_SIG_DATAA, DNS_RRSIG_DATAA, *PDNS_RRSIG_DATAA;

typedef struct {
    WORD wFlags;
    BYTE chProtocol;
    BYTE chAlgorithm;
    BYTE Key[1];
} DNS_KEY_DATA, *PDNS_KEY_DATA, DNS_DNSKEY_DATA, *PDNS_DNSKEY_DATA;

typedef struct {
    DWORD dwByteCount;
    BYTE DHCID[1];
} DNS_DHCID_DATA, *PDNS_DHCID_DATA;

typedef struct {
    PWSTR pNextDomainName;
    WORD wTypeBitMapsLength;
    WORD wPad;
    BYTE TypeBitMaps[1];
} DNS_NSEC_DATAW, *PDNS_NSEC_DATAW;

typedef struct {
    PSTR pNextDomainName;
    WORD wTypeBitMapsLength;
    WORD wPad;
    BYTE TypeBitMaps[1];
} DNS_NSEC_DATAA, *PDNS_NSEC_DATAA;

typedef struct {
    BYTE chAlgorithm;
    BYTE bFlags;
    WORD wIterations;
    BYTE bSaltLength;
    BYTE bHashLength;
    WORD wTypeBitMapsLength;
    BYTE chData[1];
} DNS_NSEC3_DATA, *PDNS_NSEC3_DATA;

typedef struct {
    BYTE chAlgorithm;
    BYTE bFlags;
    WORD wIterations;
    BYTE bSaltLength;
    BYTE bPad[3];
    BYTE pbSalt[1];
} DNS_NSEC3PARAM_DATA, *PDNS_NSEC3PARAM_DATA;

typedef struct {
    BYTE bCertUsage;
    BYTE bSelector;
    BYTE bMatchingType;
    WORD bCertificateAssociationDataLength;
    BYTE bPad[3];
    BYTE bCertificateAssociationData[1];
} DNS_TLSA_DATA, *PDNS_TLSA_DATA;

typedef struct {
    WORD wKeyTag;
    BYTE chAlgorithm;
    BYTE chDigestType;
    WORD wDigestLength;
    WORD wPad;
    BYTE Digest[1];
} DNS_DS_DATA, *PDNS_DS_DATA;

typedef struct {
    WORD wDataLength;
    WORD wPad;
    BYTE Data[1];
} DNS_OPT_DATA, *PDNS_OPT_DATA;

typedef struct {
    WORD wVersion;
    WORD wSize;
    WORD wHorPrec;
    WORD wVerPrec;
    DWORD dwLatitude;
    DWORD dwLongitude;
    DWORD dwAltitude;
} DNS_LOC_DATA, *PDNS_LOC_DATA;

typedef struct {
    PWSTR pNameNext;
    WORD wNumTypes;
    WORD wTypes[1];
} DNS_NXT_DATAW, *PDNS_NXT_DATAW;

typedef struct {
    PSTR pNameNext;
    WORD wNumTypes;
    WORD wTypes[1];
} DNS_NXT_DATAA, *PDNS_NXT_DATAA;

typedef struct {
    PWSTR pNameTarget;
    WORD wPriority;
    WORD wWeight;
    WORD wPort;
    WORD Pad;
} DNS_SRV_DATAW, *PDNS_SRV_DATAW;

typedef struct {
    PSTR pNameTarget;
    WORD wPriority;
    WORD wWeight;
    WORD wPort;
    WORD Pad;
} DNS_SRV_DATAA, *PDNS_SRV_DATAA;

typedef struct {
    WORD wOrder;
    WORD wPreference;
    PWSTR pFlags;
    PWSTR pService;
    PWSTR pRegularExpression;
    PWSTR pReplacement;
} DNS_NAPTR_DATAW, *PDNS_NAPTR_DATAW;

typedef struct {
    WORD wOrder;
    WORD wPreference;
    PSTR pFlags;
    PSTR pService;
    PSTR pRegularExpression;
    PSTR pReplacement;
} DNS_NAPTR_DATAA, *PDNS_NAPTR_DATAA;

typedef struct {
    BYTE AddressType;
    BYTE Address[DNS_ATMA_MAX_ADDR_LENGTH];
} DNS_ATMA_DATA, *PDNS_ATMA_DATA;

typedef struct {
    PWSTR pNameAlgorithm;
    PBYTE pAlgorithmPacket;
    PBYTE pKey;
    PBYTE pOtherData;
    DWORD dwCreateTime;
    DWORD dwExpireTime;
    WORD wMode;
    WORD wError;
    WORD wKeyLength;
    WORD wOtherLength;
    UCHAR cAlgNameLength;
    BOOL bPacketPointers;
} DNS_TKEY_DATAW, *PDNS_TKEY_DATAW;

typedef struct {
    PSTR pNameAlgorithm;
    PBYTE pAlgorithmPacket;
    PBYTE pKey;
    PBYTE pOtherData;
    DWORD dwCreateTime;
    DWORD dwExpireTime;
    WORD wMode;
    WORD wError;
    WORD wKeyLength;
    WORD wOtherLength;
    UCHAR cAlgNameLength;
    BOOL bPacketPointers;
} DNS_TKEY_DATAA, *PDNS_TKEY_DATAA;

typedef struct {
    PWSTR pNameAlgorithm;
    PBYTE pAlgorithmPacket;
    PBYTE pSignature;
    PBYTE pOtherData;
    LONGLONG i64CreateTime;
    WORD wFudgeTime;
    WORD wOriginalXid;
    WORD wError;
    WORD wSigLength;
    WORD wOtherLength;
    UCHAR cAlgNameLength;
    BOOL bPacketPointers;
} DNS_TSIG_DATAW, *PDNS_TSIG_DATAW;

typedef struct {
    PSTR pNameAlgorithm;
    PBYTE pAlgorithmPacket;
    PBYTE pSignature;
    PBYTE pOtherData;
    LONGLONG i64CreateTime;
    WORD wFudgeTime;
    WORD wOriginalXid;
    WORD wError;
    WORD wSigLength;
    WORD wOtherLength;
    UCHAR cAlgNameLength;
    BOOL bPacketPointers;
} DNS_TSIG_DATAA, *PDNS_TSIG_DATAA;

typedef struct {
    DWORD dwByteCount;
    BYTE bData[1];
} DNS_UNKNOWN_DATA, *PDNS_UNKNOWN_DATA;

typedef struct {
    DWORD dwMappingFlag;
    DWORD dwLookupTimeout;
    DWORD dwCacheTimeout;
    DWORD cWinsServerCount;
    IP4_ADDRESS WinsServers[1];
} DNS_WINS_DATA, *PDNS_WINS_DATA;

typedef struct {
    DWORD dwMappingFlag;
    DWORD dwLookupTimeout;
    DWORD dwCacheTimeout;
    PWSTR pNameResultDomain;
} DNS_WINSR_DATAW, *PDNS_WINSR_DATAW;

typedef struct {
    DWORD dwMappingFlag;
    DWORD dwLookupTimeout;
    DWORD dwCacheTimeout;
    PSTR pNameResultDomain;
} DNS_WINSR_DATAA, *PDNS_WINSR_DATAA;

typedef struct _DnsRecordFlags {
    DWORD Section:2;
    DWORD Delete:1;
    DWORD CharSet:2;
    DWORD Unused:3;
    DWORD Reserved:24;
} DNS_RECORD_FLAGS;

typedef enum _DnsSection {
    DnsSectionQuestion,
    DnsSectionAnswer,
    DnsSectionAuthority,
    DnsSectionAddtional,
} DNS_SECTION;

#define DnsSectionZone  DnsSectionQuestion
#define DnsSectionPrereq  DnsSectionAnswer
#define DnsSectionUpdate  DnsSectionAuthority

typedef struct _DnsRecordW {
    struct _DnsRecordW *pNext;
    PWSTR pName;
    WORD wType;
    WORD wDataLength;
    union {
        DWORD DW;
        DNS_RECORD_FLAGS S;
    } Flags;
    DWORD dwTtl;
    DWORD dwReserved;
    union {
        DNS_A_DATA A;
        DNS_SOA_DATAW SOA, Soa;
        DNS_PTR_DATAW PTR, Ptr, NS, Ns, CNAME, Cname, DNAME, Dname, MB, Mb, MD, Md, MF, Mf, MG, Mg, MR, Mr;
        DNS_MINFO_DATAW MINFO, Minfo, RP, Rp;
        DNS_MX_DATAW MX, Mx, AFSDB, Afsdb, RT, Rt;
        DNS_TXT_DATAW HINFO, Hinfo, ISDN, Isdn, TXT, Txt, X25;
        DNS_NULL_DATA Null;
        DNS_WKS_DATA WKS, Wks;
        DNS_AAAA_DATA AAAA;
        DNS_KEY_DATA KEY, Key;
        DNS_SIG_DATAW SIG, Sig;
        DNS_ATMA_DATA ATMA, Atma;
        DNS_NXT_DATAW NXT, Nxt;
        DNS_SRV_DATAW SRV, Srv;
        DNS_NAPTR_DATAW NAPTR, Naptr;
        DNS_OPT_DATA OPT, Opt;
        DNS_DS_DATA DS, Ds;
        DNS_RRSIG_DATAW RRSIG, Rrsig;
        DNS_NSEC_DATAW NSEC, Nsec;
        DNS_DNSKEY_DATA DNSKEY, Dnskey;
        DNS_TKEY_DATAW TKEY, Tkey;
        DNS_TSIG_DATAW TSIG, Tsig;
        DNS_WINS_DATA WINS, Wins;
        DNS_WINSR_DATAW WINSR, WinsR, NBSTAT, Nbstat;
        DNS_DHCID_DATA DHCID;
        DNS_NSEC3_DATA NSEC3, Nsec3;
        DNS_NSEC3PARAM_DATA NSEC3PARAM, Nsec3Param;
        DNS_TLSA_DATA TLSA, Tlsa;
        DNS_UNKNOWN_DATA UNKNOWN, Unknown;
        PBYTE pDataPtr;
    } Data;
} DNS_RECORDW, *PDNS_RECORDW;

typedef struct _DnsRecordOptW {
    struct _DnsRecordW *pNext;
    PWSTR pName;
    WORD wType;
    WORD wDataLength;
    union {
        DWORD DW;
        DNS_RECORD_FLAGS S;
    } Flags;
    DNS_HEADER_EXT ExtHeader;
    WORD wPayloadSize;
    WORD wReserved;
    union {
        DNS_OPT_DATA OPT, Opt;
    } Data;
} DNS_RECORD_OPTW, *PDNS_RECORD_OPTW;

typedef struct _DnsRecordA {
    struct _DnsRecordA *pNext;
    PSTR pName;
    WORD wType;
    WORD wDataLength;
    union {
        DWORD DW;
        DNS_RECORD_FLAGS S;
    } Flags;
    DWORD dwTtl;
    DWORD dwReserved;
    union {
        DNS_A_DATA A;
        DNS_SOA_DATAA SOA, Soa;
        DNS_PTR_DATAA PTR, Ptr, NS, Ns, CNAME, Cname, DNAME, Dname, MB, Mb, MD, Md, MF, Mf, MG, Mg, MR, Mr;
        DNS_MINFO_DATAA MINFO, Minfo, RP, Rp;
        DNS_MX_DATAA MX, Mx, AFSDB, Afsdb, RT, Rt;
        DNS_TXT_DATAA HINFO, Hinfo, ISDN, Isdn, TXT, Txt, X25;
        DNS_NULL_DATA Null;
        DNS_WKS_DATA WKS, Wks;
        DNS_AAAA_DATA AAAA;
        DNS_KEY_DATA KEY, Key;
        DNS_SIG_DATAA SIG, Sig;
        DNS_ATMA_DATA ATMA, Atma;
        DNS_NXT_DATAA NXT, Nxt;
        DNS_SRV_DATAA SRV, Srv;
        DNS_NAPTR_DATAA NAPTR, Naptr;
        DNS_OPT_DATA OPT, Opt;
        DNS_DS_DATA DS, Ds;
        DNS_RRSIG_DATAA RRSIG, Rrsig;
        DNS_NSEC_DATAA NSEC, Nsec;
        DNS_DNSKEY_DATA DNSKEY, Dnskey;
        DNS_TKEY_DATAA TKEY, Tkey;
        DNS_TSIG_DATAA TSIG, Tsig;
        DNS_WINS_DATA WINS, Wins;
        DNS_WINSR_DATAA WINSR, WinsR, NBSTAT, Nbstat;
        DNS_DHCID_DATA DHCID;
        DNS_NSEC3_DATA NSEC3, Nsec3;
        DNS_NSEC3PARAM_DATA NSEC3PARAM, Nsec3Param;
        DNS_TLSA_DATA TLSA, Tlsa;
        DNS_UNKNOWN_DATA UNKNOWN, Unknown;
        PBYTE pDataPtr;
    } Data;
} DNS_RECORDA, *PDNS_RECORDA;

typedef struct _DnsRecordOptA {
    struct _DnsRecordA *pNext;
    PSTR pName;
    WORD wType;
    WORD wDataLength;
    union {
        DWORD DW;
        DNS_RECORD_FLAGS S;
    } Flags;
    DNS_HEADER_EXT ExtHeader;
    WORD wPayloadSize;
    WORD wReserved;
    union {
        DNS_OPT_DATA OPT, Opt;
    } Data;
} DNS_RECORD_OPTA, *PDNS_RECORD_OPTA;

#ifdef UNICODE
typedef DNS_RECORDW  DNS_RECORD, *PDNS_RECORD;
#else /* !UNICODE */
typedef DNS_RECORDA  DNS_RECORD, *PDNS_RECORD;
#endif /* !UNICODE */

typedef struct _DnsRRSet {
    PDNS_RECORD pFirstRR;
    PDNS_RECORD pLastRR;
} DNS_RRSET, *PDNS_RRSET;

typedef VOID (WINAPI *DNS_PROXY_COMPLETION_ROUTINE)(void *completionContext, DNS_STATUS status);

typedef enum DNS_PROXY_INFORMATION_TYPE {
    DNS_PROXY_INFORMATION_DIRECT,
    DNS_PROXY_INFORMATION_DEFAULT_SETTINGS,
    DNS_PROXY_INFORMATION_PROXY_NAME,
    DNS_PROXY_INFORMATION_DOES_NOT_EXIST
} DNS_PROXY_INFORMATION_TYPE;

typedef struct DNS_PROXY_INFORMATION {
    ULONG version;
    DNS_PROXY_INFORMATION_TYPE proxyInformationType;
    PWSTR proxyName;
} DNS_PROXY_INFORMATION;

typedef enum _DNS_CHARSET {
    DnsCharSetUnknown,
    DnsCharSetUnicode,
    DnsCharSetUtf8,
    DnsCharSetAnsi,
} DNS_CHARSET;

typedef enum {
    DnsFreeFlat = 0,
    DnsFreeRecordList,
    DnsFreeParsedMessageFields
} DNS_FREE_TYPE;

#define DNS_QUERY_RESULTS_VERSION1  0x1

typedef struct _DNS_QUERY_RESULT {
    ULONG Version;
    DNS_STATUS QueryStatus;
    ULONG64 QueryOptions;
    PDNS_RECORD pQueryRecords;
    PVOID Reserved;
} DNS_QUERY_RESULT, *PDNS_QUERY_RESULT;

typedef VOID WINAPI DNS_QUERY_COMPLETION_ROUTINE(PVOID, PDNS_QUERY_RESULT);

typedef DNS_QUERY_COMPLETION_ROUTINE *PDNS_QUERY_COMPLETION_ROUTINE;

#define DNS_QUERY_REQUEST_VERSION1  0x1

typedef struct _DNS_QUERY_REQUEST {
    ULONG Version;
    PCWSTR QueryName;
    WORD QueryType;
    ULONG64 QueryOptions;
    PDNS_ADDR_ARRAY pDnsServerList;
    ULONG InterfaceIndex;
    PDNS_QUERY_COMPLETION_ROUTINE pQueryCompletionCallback;
    PVOID pQueryContext;
} DNS_QUERY_REQUEST, *PDNS_QUERY_REQUEST;

typedef struct DECLSPEC_ALIGN(8) _DNS_QUERY_CANCEL {
    CHAR Reserved[32];
} DNS_QUERY_CANCEL, *PDNS_QUERY_CANCEL;

typedef enum _DNS_NAME_FORMAT {
    DnsNameDomain,
    DnsNameDomainLabel,
    DnsNameHostnameFull,
    DnsNameHostnameLabel,
    DnsNameWildcard,
    DnsNameSrvRecord,
    DnsNameValidateTld
} DNS_NAME_FORMAT;

typedef struct _DNS_MESSAGE_BUFFER {
    DNS_HEADER MessageHead;
    CHAR MessageBody[1];
} DNS_MESSAGE_BUFFER, *PDNS_MESSAGE_BUFFER;

typedef enum _DNS_CONNECTION_PROXY_TYPE {
    DNS_CONNECTION_PROXY_TYPE_NULL = 0,
    DNS_CONNECTION_PROXY_TYPE_HTTP = 1,
    DNS_CONNECTION_PROXY_TYPE_WAP = 2,
    DNS_CONNECTION_PROXY_TYPE_SOCKS4 = 4,
    DNS_CONNECTION_PROXY_TYPE_SOCKS5 = 5
} DNS_CONNECTION_PROXY_TYPE;

typedef enum _DNS_CONNECTION_PROXY_INFO_SWITCH {
    DNS_CONNECTION_PROXY_INFO_SWITCH_CONFIG = 0,
    DNS_CONNECTION_PROXY_INFO_SWITCH_SCRIPT,
    DNS_CONNECTION_PROXY_INFO_SWITCH_WPAD
} DNS_CONNECTION_PROXY_INFO_SWITCH;

typedef struct _DNS_CONNECTION_PROXY_INFO {  // ? disable warn?
    DWORD Version;
    WCHAR *pwszFriendlyName;
    DWORD Flags;
    DNS_CONNECTION_PROXY_INFO_SWITCH Switch;
    union {
        struct _DNS_CONNECTION_PROXY_INFO_CONFIG {
            WCHAR *pwszServer;
            WCHAR *pwszUsername;
            WCHAR *pwszPassword;
            WCHAR *pwszException;
            WCHAR *pwszExtraInfo;
            WORD Port;
        } Config;
        struct _DNS_CONNECTION_PROXY_INFO_SCRIPT {
            WCHAR *pwszScript;
            WCHAR *pwszUsername;
            WCHAR *pwszPassword;
        } Script;
    };
} DNS_CONNECTION_PROXY_INFO, *PDNS_CONNECTION_PROXY_INFO;

typedef struct _DNS_CONNECTION_PROXY_INFO_EX {
    DNS_CONNECTION_PROXY_INFO ProxyInfo;
    DWORD dwInterfaceIndex;
    WCHAR *pwszConnectionName;
    BOOL fDirectConfiguration;
    HANDLE hConnection;
} DNS_CONNECTION_PROXY_INFO_EX, *PDNS_CONNECTION_PROXY_INFO_EX;

typedef struct _DNS_CONNECTION_PROXY_ELEMENT {
    DNS_CONNECTION_PROXY_TYPE Type;
    DNS_CONNECTION_PROXY_INFO Info;
} DNS_CONNECTION_PROXY_ELEMENT;

typedef struct _DNS_CONNECTION_PROXY_LIST {
    DWORD cProxies;
    DNS_CONNECTION_PROXY_ELEMENT *pProxies;
} DNS_CONNECTION_PROXY_LIST;

typedef struct _DNS_CONNECTION_NAME {
    WCHAR wszName[DNS_CONNECTION_NAME_MAX_LENGTH + 1];
} DNS_CONNECTION_NAME;

typedef struct _DNS_CONNECTION_NAME_LIST {
    DWORD cNames;
    DNS_CONNECTION_NAME *pNames;
} DNS_CONNECTION_NAME_LIST;

typedef struct _DNS_CONNECTION_IFINDEX_ENTRY {
    PCWSTR pwszConnectionName;
    DWORD dwIfIndex;
} DNS_CONNECTION_IFINDEX_ENTRY;

typedef struct _DNS_CONNECTION_IFINDEX_LIST {
    DNS_CONNECTION_IFINDEX_ENTRY *pConnectionIfIndexEntries;
    DWORD nEntries;
} DNS_CONNECTION_IFINDEX_LIST;

typedef struct _DNS_CONNECTION_POLICY_ENTRY {
    PCWSTR pwszHost;
    PCWSTR pwszAppId;
    DWORD cbAppSid;
    PBYTE pbAppSid;
    DWORD nConnections;
    PCWSTR *ppwszConnections;
    DWORD dwPolicyEntryFlags;
}
DNS_CONNECTION_POLICY_ENTRY, *PDNS_CONNECTION_POLICY_ENTRY;

typedef struct _DNS_CONNECTION_POLICY_ENTRY_LIST {
    DNS_CONNECTION_POLICY_ENTRY *pPolicyEntries;
    DWORD nEntries;
} DNS_CONNECTION_POLICY_ENTRY_LIST;

typedef enum {
    TAG_DNS_CONNECTION_POLICY_TAG_DEFAULT = 0,
    TAG_DNS_CONNECTION_POLICY_TAG_CONNECTION_MANAGER,
    TAG_DNS_CONNECTION_POLICY_TAG_WWWPT
} DNS_CONNECTION_POLICY_TAG;

DNS_STATUS WINAPI DnsQueryConfig(DNS_CONFIG_TYPE,DWORD,PWSTR,PVOID,PVOID,PDWORD);

PDNS_RECORD WINAPI DnsRecordCopyEx(PDNS_RECORD,DNS_CHARSET,DNS_CHARSET);
PDNS_RECORD WINAPI DnsRecordSetCopyEx(PDNS_RECORD,DNS_CHARSET,DNS_CHARSET);
BOOL WINAPI DnsRecordCompare(PDNS_RECORD,PDNS_RECORD);
BOOL WINAPI DnsRecordSetCompare(PDNS_RECORD,PDNS_RECORD,PDNS_RECORD*,PDNS_RECORD*);
PDNS_RECORD DnsRecordSetDetach(PDNS_RECORD);
VOID WINAPI DnsFree(PVOID,DNS_FREE_TYPE);

#define DnsFreeRecordListDeep  DnsFreeRecordList

#if (_WIN32_WINNT >= 0x0501)
#define DnsRecordListFree(p,t)  DnsFree(p,DnsFreeRecordList)
#else /* _WIN32_WINNT < 0x0501 */
VOID WINAPI DnsRecordListFree(PDNS_RECORD,DNS_FREE_TYPE);
#endif /* _WIN32_WINNT < 0x0501 */

DNS_STATUS WINAPI DnsQuery_A(PCSTR,WORD,DWORD,PIP4_ARRAY,PDNS_RECORD*,PVOID*);
DNS_STATUS WINAPI DnsQuery_UTF8(PCSTR,WORD,DWORD,PIP4_ARRAY,PDNS_RECORD*,PVOID*);
DNS_STATUS WINAPI DnsQuery_W(PCWSTR,WORD,DWORD,PIP4_ARRAY,PDNS_RECORD*,PVOID*);

DNS_STATUS WINAPI DnsQueryEx(PDNS_QUERY_REQUEST, PDNS_QUERY_RESULT, PDNS_QUERY_CANCEL);
DNS_STATUS WINAPI DnsCancelQuery(PDNS_QUERY_CANCEL);

DNS_STATUS WINAPI DnsAcquireContextHandle_W(DWORD,PVOID,PHANDLE);
DNS_STATUS WINAPI DnsAcquireContextHandle_A(DWORD,PVOID,PHANDLE);
VOID WINAPI DnsReleaseContextHandle(HANDLE);

DNS_STATUS WINAPI DnsModifyRecordsInSet_W(PDNS_RECORD,PDNS_RECORD,DWORD,HANDLE,PIP4_ARRAY,PVOID);
DNS_STATUS WINAPI DnsModifyRecordsInSet_A(PDNS_RECORD,PDNS_RECORD,DWORD,HANDLE,PIP4_ARRAY,PVOID);
DNS_STATUS WINAPI DnsModifyRecordsInSet_UTF8(PDNS_RECORD,PDNS_RECORD,DWORD,HANDLE,PIP4_ARRAY,PVOID);
DNS_STATUS WINAPI DnsReplaceRecordSetW(PDNS_RECORD,DWORD,HANDLE,PIP4_ARRAY,PVOID);
DNS_STATUS WINAPI DnsReplaceRecordSetA(PDNS_RECORD,DWORD,HANDLE,PIP4_ARRAY,PVOID);
DNS_STATUS WINAPI DnsReplaceRecordSetUTF8(PDNS_RECORD,DWORD,HANDLE,PIP4_ARRAY,PVOID);
DNS_STATUS DnsValidateName_W(LPCWSTR,DNS_NAME_FORMAT);
DNS_STATUS DnsValidateName_A(LPCSTR,DNS_NAME_FORMAT);
DNS_STATUS DnsValidateName_UTF8(LPCSTR,DNS_NAME_FORMAT);
#ifdef _WINSOCK2_H
DNS_STATUS DnsValidateServerStatus(PSOCKADDR,PCWSTR,PDWORD);
#endif /* _WINSOCK2_H */
BOOL WINAPI DnsNameCompare_A(LPSTR,LPSTR);
BOOL WINAPI DnsNameCompare_W(LPWSTR,LPWSTR);
BOOL WINAPI DnsWriteQuestionToBuffer_W(PDNS_MESSAGE_BUFFER,PDWORD,LPWSTR,WORD,WORD,BOOL);
BOOL WINAPI DnsWriteQuestionToBuffer_UTF8(PDNS_MESSAGE_BUFFER,LPDWORD,LPSTR,WORD,WORD,BOOL);
DNS_STATUS WINAPI DnsExtractRecordsFromMessage_W(PDNS_MESSAGE_BUFFER,WORD,PDNS_RECORD*);
DNS_STATUS WINAPI DnsExtractRecordsFromMessage_UTF8(PDNS_MESSAGE_BUFFER,WORD,PDNS_RECORD*);

DWORD WINAPI DnsGetProxyInformation(PCWSTR, DNS_PROXY_INFORMATION *, DNS_PROXY_INFORMATION *, DNS_PROXY_COMPLETION_ROUTINE, void *);
VOID WINAPI DnsFreeProxyName(PWSTR);

DWORD DnsConnectionGetProxyInfoForHostUrl(PCWSTR, BYTE *, DWORD, DWORD, DNS_CONNECTION_PROXY_INFO_EX *);
VOID DnsConnectionFreeProxyInfoEx(DNS_CONNECTION_PROXY_INFO_EX *);
DWORD DnsConnectionGetProxyInfo(PCWSTR, DNS_CONNECTION_PROXY_TYPE, DNS_CONNECTION_PROXY_INFO *);
VOID DnsConnectionFreeProxyInfo(DNS_CONNECTION_PROXY_INFO *);
DWORD DnsConnectionSetProxyInfo(PCWSTR, DNS_CONNECTION_PROXY_TYPE, const DNS_CONNECTION_PROXY_INFO *);
DWORD DnsConnectionDeleteProxyInfo(PCWSTR, DNS_CONNECTION_PROXY_TYPE);
DWORD DnsConnectionGetProxyList(PCWSTR, DNS_CONNECTION_PROXY_LIST *);
VOID DnsConnectionFreeProxyList(DNS_CONNECTION_PROXY_LIST *);
DWORD DnsConnectionGetNameList(DNS_CONNECTION_NAME_LIST *);
VOID DnsConnectionFreeNameList(DNS_CONNECTION_NAME_LIST *);

DWORD DnsConnectionUpdateIfIndexTable(DNS_CONNECTION_IFINDEX_LIST *);

DWORD DnsConnectionSetPolicyEntries(DNS_CONNECTION_POLICY_TAG, DNS_CONNECTION_POLICY_ENTRY_LIST *);
DWORD DnsConnectionDeletePolicyEntries(DNS_CONNECTION_POLICY_TAG);

#ifdef UNICODE
#define DNS_IP4_REVERSE_DOMAIN_STRING  DNS_IP4_REVERSE_DOMAIN_STRING_W
#define DNS_IP6_REVERSE_DOMAIN_STRING  DNS_IP6_REVERSE_DOMAIN_STRING_W
typedef DNS_PTR_DATAW DNS_PTR_DATA, *PDNS_PTR_DATA;
typedef DNS_SOA_DATAW DNS_SOA_DATA, *PDNS_SOA_DATA;
typedef DNS_MINFO_DATAW DNS_MINFO_DATA, *PDNS_MINFO_DATA;
typedef DNS_MX_DATAW DNS_MX_DATA, *PDNS_MX_DATA;
typedef DNS_TXT_DATAW DNS_TXT_DATA, *PDNS_TXT_DATA;
typedef DNS_SIG_DATAW DNS_SIG_DATA, *PDNS_SIG_DATA;
typedef DNS_NXT_DATAW DNS_NXT_DATA, *PDNS_NXT_DATA;
typedef DNS_SRV_DATAW DNS_SRV_DATA, *PDNS_SRV_DATA;
typedef DNS_NAPTR_DATAW DNS_NAPTR_DATA, *PDNS_NAPTR_DATA;
typedef DNS_RRSIG_DATAW DNS_RRSIG_DATA, *PDNS_RRSIG_DATA;
typedef DNS_NSEC_DATAW  DNS_NSEC_DATA,  *PDNS_NSEC_DATA;
typedef DNS_TKEY_DATAW DNS_TKEY_DATA, *PDNS_TKEY_DATA;
typedef DNS_TSIG_DATAW DNS_TSIG_DATA, *PDNS_TSIG_DATA;
typedef DNS_WINSR_DATAW DNS_WINSR_DATA, *PDNS_WINSR_DATA;
typedef DNS_RECORDW DNS_RECORD, *PDNS_RECORD;
typedef DNS_RECORD_OPTW DNS_RECORD_OPT, *PDNS_RECORD_OPT;
#define DnsRecordCopy(pRR)  DnsRecordCopyEx((pRR),DnsCharSetUnicode,DnsCharSetUnicode)
#define DnsRecordSetCopy(pRR)  DnsRecordSetCopyEx((pRR),DnsCharSetUnicode,DnsCharSetUnicode)
#define DnsQuery  DnsQuery_W
#define DnsAcquireContextHandle  DnsAcquireContextHandle_W
#define DnsModifyRecordsInSet  DnsModifyRecordsInSet_W
#define DnsReplaceRecordSet  DnsReplaceRecordSetW
#define DnsValidateName(p,f)  DnsValidateName_W((p),(f))
#define DnsNameCompare(n1,n2)  DnsNameCompare_W((n1),(n2))
#else /* !UNICODE */
#define DNS_IP4_REVERSE_DOMAIN_STRING  DNS_IP4_REVERSE_DOMAIN_STRING_A
#define DNS_IP6_REVERSE_DOMAIN_STRING  DNS_IP6_REVERSE_DOMAIN_STRING_A
typedef DNS_PTR_DATAA DNS_PTR_DATA, *PDNS_PTR_DATA;
typedef DNS_SOA_DATAA DNS_SOA_DATA, *PDNS_SOA_DATA;
typedef DNS_MINFO_DATAA DNS_MINFO_DATA, *PDNS_MINFO_DATA;
typedef DNS_MX_DATAA DNS_MX_DATA, *PDNS_MX_DATA;
typedef DNS_TXT_DATAA DNS_TXT_DATA, *PDNS_TXT_DATA;
typedef DNS_SIG_DATAA DNS_SIG_DATA, *PDNS_SIG_DATA;
typedef DNS_NXT_DATAA DNS_NXT_DATA, *PDNS_NXT_DATA;
typedef DNS_SRV_DATAA DNS_SRV_DATA, *PDNS_SRV_DATA;
typedef DNS_NAPTR_DATAA DNS_NAPTR_DATA, *PDNS_NAPTR_DATA;
typedef DNS_RRSIG_DATAA DNS_RRSIG_DATA, *PDNS_RRSIG_DATA;
typedef DNS_NSEC_DATAA  DNS_NSEC_DATA,  *PDNS_NSEC_DATA;
typedef DNS_TKEY_DATAA DNS_TKEY_DATA, *PDNS_TKEY_DATA;
typedef DNS_TSIG_DATAA DNS_TSIG_DATA, *PDNS_TSIG_DATA;
typedef DNS_WINSR_DATAA DNS_WINSR_DATA, *PDNS_WINSR_DATA;
typedef DNS_RECORDA DNS_RECORD, *PDNS_RECORD;
typedef DNS_RECORD_OPTA DNS_RECORD_OPT, *PDNS_RECORD_OPT;
#define DnsRecordCopy(pRR)  DnsRecordCopyEx((pRR),DnsCharSetAnsi,DnsCharSetAnsi)
#define DnsRecordSetCopy(pRR)  DnsRecordSetCopyEx((pRR),DnsCharSetAnsi,DnsCharSetAnsi)
#define DnsQuery  DnsQuery_A
#define DnsAcquireContextHandle  DnsAcquireContextHandle_A
#define DnsModifyRecordsInSet  DnsModifyRecordsInSet_A
#define DnsReplaceRecordSet  DnsReplaceRecordSetA
#define DnsValidateName(p,f)  DnsValidateName_A((p),(f))
#define DnsNameCompare(n1,n2)  DnsNameCompare_A((n1),(n2))
#endif /* !UNICODE */


#ifdef __cplusplus
}
#endif

#endif  /* _WINDNS_H */
