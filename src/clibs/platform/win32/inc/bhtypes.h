#ifndef _BHTYPES_H
#define _BHTYPES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Microsoft Network Monitor definitions */

#ifdef __cplusplus
extern "C" {
#endif

#include <pshpack1.h>


#define GetTableSize(TableBaseSize,nElements,ElementSize) ((TableBaseSize) + ((nElements) * (ElementSize)))

#ifndef MAKE_IDENTIFIER
#define MAKE_IDENTIFIER(a,b,c,d)  ((DWORD) MAKELONG(MAKEWORD(a,b),MAKEWORD(c,d)))
#endif

#define HANDLE_TYPE_INVALID  MAKE_IDENTIFIER(-1,-1,-1,-1)
#define HANDLE_TYPE_CAPTURE  MAKE_IDENTIFIER('C','A','P','$')
#define HANDLE_TYPE_PARSER  MAKE_IDENTIFIER('P','S','R','$')
#define HANDLE_TYPE_ADDRESSDB  MAKE_IDENTIFIER('A','D','R','$')
#define HANDLE_TYPE_PROTOCOL  MAKE_IDENTIFIER('P','R','T','$')
#define HANDLE_TYPE_BUFFER  MAKE_IDENTIFIER('B','U','F','$')

#define INLINE  __inline
#define BHAPI  WINAPI

#define MAX_NAME_LENGTH  16
#define MAX_ADDR_LENGTH  6

#define ETYPE_LOOP  0x9000
#define ETYPE_3COM_NETMAP1  0x9001
#define ETYPE_3COM_NETMAP2  0x9002
#define ETYPE_IBM_RT  0x80D5
#define ETYPE_NETWARE  0x8137
#define ETYPE_XNS1  0x0600
#define ETYPE_XNS2  0x0807
#define ETYPE_3COM_NBP0  0x3C00
#define ETYPE_3COM_NBP1  0x3C01
#define ETYPE_3COM_NBP2  0x3C02
#define ETYPE_3COM_NBP3  0x3C03
#define ETYPE_3COM_NBP4  0x3C04
#define ETYPE_3COM_NBP5  0x3C05
#define ETYPE_3COM_NBP6  0x3C06
#define ETYPE_3COM_NBP7  0x3C07
#define ETYPE_3COM_NBP8  0x3C08
#define ETYPE_3COM_NBP9  0x3C09
#define ETYPE_3COM_NBP10  0x3C0A
#define ETYPE_IP  0x0800
#define ETYPE_ARP1  0x0806
#define ETYPE_ARP2  0x0807
#define ETYPE_RARP  0x8035
#define ETYPE_TRLR0  0x1000
#define ETYPE_TRLR1  0x1001
#define ETYPE_TRLR2  0x1002
#define ETYPE_TRLR3  0x1003
#define ETYPE_TRLR4  0x1004
#define ETYPE_TRLR5  0x1005
#define ETYPE_PUP  0x0200
#define ETYPE_PUP_ARP  0x0201
#define ETYPE_APPLETALK_ARP  0x80F3
#define ETYPE_APPLETALK_LAP  0x809B
#define ETYPE_SNMP  0x814C

#define SAP_SNAP  0xAA
#define SAP_BPDU  0x42
#define SAP_IBM_NM  0xF4
#define SAP_IBM_NETBIOS  0xF0
#define SAP_SNA1  0x04
#define SAP_SNA2  0x05
#define SAP_SNA3  0x08
#define SAP_SNA4  0x0C
#define SAP_NETWARE1  0x10
#define SAP_NETWARE2  0xE0
#define SAP_NETWARE3  0xFE
#define SAP_IP  0x06
#define SAP_X25  0x7E
#define SAP_RPL1  0xF8
#define SAP_RPL2  0xFC
#define SAP_UB  0xFA
#define SAP_XNS  0x80

#define PROP_TYPE_VOID  0x00
#define PROP_TYPE_SUMMARY  0x01
#define PROP_TYPE_BYTE  0x02
#define PROP_TYPE_WORD  0x03
#define PROP_TYPE_DWORD  0x04
#define PROP_TYPE_LARGEINT  0x05
#define PROP_TYPE_ADDR  0x06
#define PROP_TYPE_TIME  0x07
#define PROP_TYPE_STRING  0x08
#define PROP_TYPE_IP_ADDRESS  0x09
#define PROP_TYPE_IPX_ADDRESS  0x0A
#define PROP_TYPE_BYTESWAPPED_WORD  0x0B
#define PROP_TYPE_BYTESWAPPED_DWORD 0x0C
#define PROP_TYPE_TYPED_STRING  0x0D
#define PROP_TYPE_RAW_DATA  0x0E
#define PROP_TYPE_COMMENT  0x0F
#define PROP_TYPE_SRCFRIENDLYNAME  0x10
#define PROP_TYPE_DSTFRIENDLYNAME  0x11
#define PROP_TYPE_TOKENRING_ADDRESS 0x12
#define PROP_TYPE_FDDI_ADDRESS  0x13
#define PROP_TYPE_ETHERNET_ADDRESS  0x14
#define PROP_TYPE_OBJECT_IDENTIFIER  0x15
#define PROP_TYPE_VINES_IP_ADDRESS  0x16
#define PROP_TYPE_VAR_LEN_SMALL_INT 0x17

#define PROP_QUAL_NONE  0x00
#define PROP_QUAL_RANGE  0x01
#define PROP_QUAL_SET  0x02
#define PROP_QUAL_BITFIELD  0x03
#define PROP_QUAL_LABELED_SET  0x04
#define PROP_QUAL_LABELED_BITFIELD  0x08
#define PROP_QUAL_CONST  0x09
#define PROP_QUAL_FLAGS  0x0A
#define PROP_QUAL_ARRAY  0x0B

#define TYPED_STRING_NORMAL  1
#define TYPED_STRING_UNICODE  2
#define TYPED_STRING_EXFLAG  1

#define STRINGTABLE_SIZE  sizeof(STRINGTABLE)
#define PROPERTYINFO_SIZE  sizeof(PROPERTYINFO)
#define PROPERTYINSTEX_SIZE  sizeof(PROPERTYINSTEX)
#define PROPERTYINST_SIZE  sizeof(PROPERTYINST)
#define PROPERTYINSTTABLE_SIZE  sizeof(PROPERTYINSTTABLE)
#define PROPERTYTABLE_SIZE  sizeof(PROPERTYTABLE)
#define ENTRYPOINTS_SIZE  sizeof(ENTRYPOINTS)
#define PROPERTYDATABASE_SIZE  sizeof(PROPERTYDATABASE)
#define PROTOCOLINFO_SIZE  sizeof(PROTOCOLINFO)
#define PROTOCOLTABLE_SIZE  (sizeof(PROTOCOLTABLE) - sizeof(HPROTOCOL))
#define PROTOCOLTABLE_ACTUAL_SIZE(p)  GetTableSize(PROTOCOLTABLE_SIZE,(p)->nProtocols,sizeof(HPROTOCOL))
#define ADDRESSINFO_SIZE  sizeof(ADDRESSINFO)
#define ADDRESSINFOTABLE_SIZE  sizeof(ADDRESSINFOTABLE)

#define IFLAG_ERROR  0x00000001
#define IFLAG_SWAPPED  0x00000002
#define IFLAG_UNICODE  0x00000004

#define SORT_BYADDRESS  0
#define SORT_BYNAME  1

#define PERMANENT_NAME  0x00000100

typedef VOID UNALIGNED *ULPVOID;
typedef BYTE UNALIGNED *ULPBYTE;
typedef WORD UNALIGNED *ULPWORD;
typedef DWORD UNALIGNED *ULPDWORD;
typedef CHAR UNALIGNED *ULPSTR;
typedef SYSTEMTIME UNALIGNED *ULPSYSTEMTIME;

typedef struct _PARSER *HPARSER;

typedef struct _CAPFRAMEDESC *HFRAME;
typedef struct _CAPTURE *HCAPTURE;
typedef struct _FILTER *HFILTER;
typedef struct _ADDRESSDB *HADDRESSDB;
typedef struct _PROTOCOL *HPROTOCOL;
typedef DWORD HPROPERTY;

typedef HPROTOCOL *LPHPROTOCOL;

typedef DWORD OBJECTTYPE;

typedef LARGE_INTEGER *LPLARGEINT;
typedef LARGE_INTEGER UNALIGNED *ULPLARGEINT;

typedef struct _RANGE {
    DWORD MinValue;
    DWORD MaxValue;
} RANGE, *LPRANGE;

typedef struct _LABELED_BYTE {
    BYTE Value;
    LPSTR Label;
} LABELED_BYTE, *LPLABELED_BYTE;

typedef struct _LABELED_WORD {
    WORD Value;
    LPSTR Label;
} LABELED_WORD, *LPLABELED_WORD;

typedef struct _LABELED_DWORD {
    DWORD Value;
    LPSTR Label;
} LABELED_DWORD, *LPLABELED_DWORD;

typedef struct _LABELED_LARGEINT {
    LARGE_INTEGER Value;
    LPSTR Label;
} LABELED_LARGEINT, *LPLABELED_LARGEINT;

typedef struct _LABELED_SYSTEMTIME {
    SYSTEMTIME Value;
    LPSTR Label;
} LABELED_SYSTEMTIME, *LPLABELED_SYSTEMTIME;

typedef struct _LABELED_BIT {
    BYTE BitNumber;
    LPSTR LabelOff;
    LPSTR LabelOn;
} LABELED_BIT, *LPLABELED_BIT;

typedef struct _TYPED_STRING {
    BYTE StringType:7;
    BYTE fStringEx:1;
    LPSTR lpString;
    BYTE Byte[];
} TYPED_STRING, *LPTYPED_STRING;

typedef struct _OBJECT_IDENTIFIER {
    DWORD Length;
    LPDWORD lpIdentifier;
} OBJECT_IDENTIFIER, *LPOBJECT_IDENTIFIER;

typedef struct _SET {
    DWORD nEntries;
    union {
        LPVOID lpVoidTable;
        LPBYTE lpByteTable;
        LPWORD lpWordTable;
        LPDWORD lpDwordTable;
        LPLARGEINT lpLargeIntTable;
        LPSYSTEMTIME lpSystemTimeTable;
        LPLABELED_BYTE lpLabeledByteTable;
        LPLABELED_WORD lpLabeledWordTable;
        LPLABELED_DWORD lpLabeledDwordTable;
        LPLABELED_LARGEINT lpLabeledLargeIntTable;
        LPLABELED_SYSTEMTIME lpLabeledSystemTimeTable;
        LPLABELED_BIT lpLabeledBit;
    };
} SET, *LPSET;

typedef struct _STRINGTABLE {
    DWORD nStrings;
    LPSTR String[];
} STRINGTABLE, *LPSTRINGTABLE;

typedef struct _RECOGNIZEDATA {
    WORD ProtocolID;
    WORD nProtocolOffset;
    LPVOID InstData;
} RECOGNIZEDATA, *LPRECOGNIZEDATA;

typedef struct _RECOGNIZEDATATABLE {
    WORD nRecognizeDatas;
    RECOGNIZEDATA RecognizeData[];
} RECOGNIZEDATATABLE, *LPRECOGNIZEDATATABLE;

typedef struct _PROPERTYINFO {
    HPROPERTY hProperty;
    DWORD Version;
    LPSTR Label;
    LPSTR Comment;
    BYTE DataType;
    BYTE DataQualifier;
    union {
        LPVOID lpExtendedInfo;
        LPRANGE lpRange;
        LPSET lpSet;
        DWORD Bitmask;
        DWORD Value;
    };
    WORD FormatStringSize;
    LPVOID InstanceData;
} PROPERTYINFO, *LPPROPERTYINFO;

typedef struct _PROPERTYINSTEX {
    WORD Length;
    WORD LengthEx;
    ULPVOID lpData;
    union {
        BYTE Byte[];
        WORD Word[];
        DWORD Dword[];
        LARGE_INTEGER LargeInt[];
        SYSTEMTIME SysTime[];
        TYPED_STRING TypedString;
    };
} PROPERTYINSTEX, *LPPROPERTYINSTEX, UNALIGNED *ULPPROPERTYINSTEX;

typedef struct _PROPERTYINST {
    LPPROPERTYINFO lpPropertyInfo;
    LPSTR szPropertyText;
    union {
        LPVOID lpData;
        LPBYTE lpByte;
        ULPWORD lpWord;
        ULPDWORD lpDword;
        ULPLARGEINT lpLargeInt;
        ULPSYSTEMTIME lpSysTime;
        LPPROPERTYINSTEX lpPropertyInstEx;
    };
    WORD DataLength;
    WORD Level:4;
    WORD HelpID:12;
    DWORD IFlags;
} PROPERTYINST, *LPPROPERTYINST;

typedef struct _PROPERTYINSTTABLE {
    WORD nPropertyInsts;
    WORD nPropertyInstIndex;
} PROPERTYINSTTABLE, *LPPROPERTYINSTTABLE;

typedef struct _PROPERTYTABLE {
    LPVOID lpFormatBuffer;
    DWORD FormatBufferLength;
    DWORD nTotalPropertyInsts;
    LPPROPERTYINST lpFirstPropertyInst;
    BYTE nPropertyInstTables;
    PROPERTYINSTTABLE PropertyInstTable[];
} PROPERTYTABLE, *LPPROPERTYTABLE;

typedef VOID (WINAPI *REGISTER)(HPROTOCOL);
typedef VOID (WINAPI *DEREGISTER)(HPROTOCOL);
typedef LPBYTE (WINAPI *RECOGNIZEFRAME)(HFRAME,LPBYTE,LPBYTE,DWORD,DWORD,HPROTOCOL,DWORD,LPDWORD,LPHPROTOCOL,PDWORD_PTR);
typedef LPBYTE (WINAPI *ATTACHPROPERTIES)(HFRAME,LPBYTE,LPBYTE,DWORD,DWORD,HPROTOCOL,DWORD,DWORD_PTR);
typedef DWORD (WINAPI *FORMATPROPERTIES)(HFRAME,LPBYTE,LPBYTE,DWORD,LPPROPERTYINST);

typedef struct _ENTRYPOINTS {
    REGISTER Register;
    DEREGISTER Deregister;
    RECOGNIZEFRAME RecognizeFrame;
    ATTACHPROPERTIES AttachProperties;
    FORMATPROPERTIES FormatProperties;
} ENTRYPOINTS, *LPENTRYPOINTS;

typedef struct _PROPERTYDATABASE {
    DWORD nProperties;
    LPPROPERTYINFO PropertyInfo[];
} PROPERTYDATABASE, *LPPROPERTYDATABASE;

typedef struct _PROTOCOLINFO {
    DWORD ProtocolID;
    LPPROPERTYDATABASE PropertyDatabase;
    BYTE ProtocolName[16];
    BYTE HelpFile[16];
    BYTE Comment[128];
} PROTOCOLINFO, *LPPROTOCOLINFO;

typedef struct _PROTOCOLTABLE {
    DWORD nProtocols;
    HPROTOCOL hProtocol[1];
} PROTOCOLTABLE, *LPPROTOCOLTABLE;

typedef struct _ADDRESSINFO *LPADDRESSINFO;
typedef struct _ADDRESSINFO {
    ADDRESS Address;
    WCHAR Name[MAX_NAME_SIZE];
    DWORD Flags;
    LPVOID lpAddressInstData;
} ADDRESSINFO;

typedef struct _ADDRESSINFOTABLE {
    DWORD nAddressInfos;
    LPADDRESSINFO lpAddressInfo[];
} ADDRESSINFOTABLE, *LPADDRESSINFOTABLE;

typedef DWORD (WINAPI *FILTERPROC)(HCAPTURE,HFRAME,LPVOID);


#include <poppack.h>

#ifdef __cplusplus
}
#endif

#endif /* _BHTYPES_H */
