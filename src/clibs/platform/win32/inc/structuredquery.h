#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif

#ifndef _STRUCTUREDQUERY_H
#define _STRUCTUREDQUERY_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IQueryParser_FWD_DEFINED__
#define __IQueryParser_FWD_DEFINED__
typedef interface IQueryParser IQueryParser;
#endif

#ifndef __IConditionFactory_FWD_DEFINED__
#define __IConditionFactory_FWD_DEFINED__
typedef interface IConditionFactory IConditionFactory;
#endif

#ifndef __IQuerySolution_FWD_DEFINED__
#define __IQuerySolution_FWD_DEFINED__
typedef interface IQuerySolution IQuerySolution;
#endif

#ifndef __ICondition_FWD_DEFINED__
#define __ICondition_FWD_DEFINED__
typedef interface ICondition ICondition;
#endif

#ifndef __IConditionGenerator_FWD_DEFINED__
#define __IConditionGenerator_FWD_DEFINED__
typedef interface IConditionGenerator IConditionGenerator;
#endif

#ifndef __IRichChunk_FWD_DEFINED__
#define __IRichChunk_FWD_DEFINED__
typedef interface IRichChunk IRichChunk;
#endif

#ifndef __IInterval_FWD_DEFINED__
#define __IInterval_FWD_DEFINED__
typedef interface IInterval IInterval;
#endif

#ifndef __IMetaData_FWD_DEFINED__
#define __IMetaData_FWD_DEFINED__
typedef interface IMetaData IMetaData;
#endif

#ifndef __IEntity_FWD_DEFINED__
#define __IEntity_FWD_DEFINED__
typedef interface IEntity IEntity;
#endif

#ifndef __IRelationship_FWD_DEFINED__
#define __IRelationship_FWD_DEFINED__
typedef interface IRelationship IRelationship;
#endif

#ifndef __INamedEntity_FWD_DEFINED__
#define __INamedEntity_FWD_DEFINED__
typedef interface INamedEntity INamedEntity;
#endif

#ifndef __ISchemaProvider_FWD_DEFINED__
#define __ISchemaProvider_FWD_DEFINED__
typedef interface ISchemaProvider ISchemaProvider;
#endif

#ifndef __ITokenCollection_FWD_DEFINED__
#define __ITokenCollection_FWD_DEFINED__
typedef interface ITokenCollection ITokenCollection;
#endif

#ifndef __INamedEntityCollector_FWD_DEFINED__
#define __INamedEntityCollector_FWD_DEFINED__
typedef interface INamedEntityCollector INamedEntityCollector;
#endif

#ifndef __ISchemaLocalizerSupport_FWD_DEFINED__
#define __ISchemaLocalizerSupport_FWD_DEFINED__
typedef interface ISchemaLocalizerSupport ISchemaLocalizerSupport;
#endif

#ifndef __IQueryParserManager_FWD_DEFINED__
#define __IQueryParserManager_FWD_DEFINED__
typedef interface IQueryParserManager IQueryParserManager;
#endif

#ifndef __QueryParser_FWD_DEFINED__
#define __QueryParser_FWD_DEFINED__

#ifdef __cplusplus
typedef class QueryParser QueryParser;
#else
typedef struct QueryParser QueryParser;
#endif

#endif

#ifndef __NegationCondition_FWD_DEFINED__
#define __NegationCondition_FWD_DEFINED__

#ifdef __cplusplus
typedef class NegationCondition NegationCondition;
#else
typedef struct NegationCondition NegationCondition;
#endif

#endif

#ifndef __CompoundCondition_FWD_DEFINED__
#define __CompoundCondition_FWD_DEFINED__

#ifdef __cplusplus
typedef class CompoundCondition CompoundCondition;
#else
typedef struct CompoundCondition CompoundCondition;
#endif

#endif

#ifndef __LeafCondition_FWD_DEFINED__
#define __LeafCondition_FWD_DEFINED__

#ifdef __cplusplus
typedef class LeafCondition LeafCondition;
#else
typedef struct LeafCondition LeafCondition;
#endif

#endif

#ifndef __ConditionFactory_FWD_DEFINED__
#define __ConditionFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class ConditionFactory ConditionFactory;
#else
typedef struct ConditionFactory ConditionFactory;
#endif

#endif

#ifndef __Interval_FWD_DEFINED__
#define __Interval_FWD_DEFINED__

#ifdef __cplusplus
typedef class Interval Interval;
#else
typedef struct Interval Interval;
#endif

#endif

#ifndef __QueryParserManager_FWD_DEFINED__
#define __QueryParserManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class QueryParserManager QueryParserManager;
#else
typedef struct QueryParserManager QueryParserManager;
#endif

#endif

#include "oaidl.h"
#include "ocidl.h"
#include "propidl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum tagCONDITION_TYPE {
    CT_AND_CONDITION = 0,
    CT_OR_CONDITION = (CT_AND_CONDITION + 1),
    CT_NOT_CONDITION = (CT_OR_CONDITION + 1),
    CT_LEAF_CONDITION = (CT_NOT_CONDITION + 1)
} CONDITION_TYPE;

typedef enum tagCONDITION_OPERATION { 
    COP_IMPLICIT = 0,
    COP_EQUAL = (COP_IMPLICIT + 1),
    COP_NOTEQUAL = (COP_EQUAL + 1),
    COP_LESSTHAN = (COP_NOTEQUAL + 1),
    COP_GREATERTHAN = (COP_LESSTHAN + 1),
    COP_LESSTHANOREQUAL = (COP_GREATERTHAN + 1),
    COP_GREATERTHANOREQUAL = (COP_LESSTHANOREQUAL + 1),
    COP_VALUE_STARTSWITH = (COP_GREATERTHANOREQUAL + 1),
    COP_VALUE_ENDSWITH = (COP_VALUE_STARTSWITH + 1),
    COP_VALUE_CONTAINS = (COP_VALUE_ENDSWITH + 1),
    COP_VALUE_NOTCONTAINS = (COP_VALUE_CONTAINS + 1),
    COP_DOSWILDCARDS = (COP_VALUE_NOTCONTAINS + 1),
    COP_WORD_EQUAL = (COP_DOSWILDCARDS + 1),
    COP_WORD_STARTSWITH = (COP_WORD_EQUAL + 1),
    COP_APPLICATION_SPECIFIC = (COP_WORD_STARTSWITH + 1)
} CONDITION_OPERATION;

typedef enum tagSTRUCTURED_QUERY_SINGLE_OPTION {
    SQSO_SCHEMA = 0,
    SQSO_LOCALE_WORD_BREAKING = (SQSO_SCHEMA + 1),
    SQSO_WORD_BREAKER = (SQSO_LOCALE_WORD_BREAKING + 1),
    SQSO_NATURAL_SYNTAX = (SQSO_WORD_BREAKER + 1),
    SQSO_AUTOMATIC_WILDCARD = (SQSO_NATURAL_SYNTAX + 1),
    SQSO_TRACE_LEVEL = (SQSO_AUTOMATIC_WILDCARD + 1),
    SQSO_LANGUAGE_KEYWORDS = (SQSO_TRACE_LEVEL + 1)
} STRUCTURED_QUERY_SINGLE_OPTION;

typedef enum tagSTRUCTURED_QUERY_MULTIOPTION {
    SQMO_VIRTUAL_PROPERTY = 0,
    SQMO_DEFAULT_PROPERTY = (SQMO_VIRTUAL_PROPERTY + 1),
    SQMO_GENERATOR_FOR_TYPE = (SQMO_DEFAULT_PROPERTY + 1)
} STRUCTURED_QUERY_MULTIOPTION;

typedef enum tagSTRUCTURED_QUERY_PARSE_ERROR {
    SQPE_NONE = 0,
    SQPE_EXTRA_OPENING_PARENTHESIS = (SQPE_NONE + 1),
    SQPE_EXTRA_CLOSING_PARENTHESIS = (SQPE_EXTRA_OPENING_PARENTHESIS + 1),
    SQPE_IGNORED_MODIFIER = (SQPE_EXTRA_CLOSING_PARENTHESIS + 1),
    SQPE_IGNORED_CONNECTOR = (SQPE_IGNORED_MODIFIER + 1),
    SQPE_IGNORED_KEYWORD = (SQPE_IGNORED_CONNECTOR + 1),
    SQPE_UNHANDLED = (SQPE_IGNORED_KEYWORD + 1)
} STRUCTURED_QUERY_PARSE_ERROR;


enum tagSTRUCTURED_QUERY_RESOLVE_OPTION {
    SQRO_DONT_RESOLVE_DATETIME = 0x1,
    SQRO_ALWAYS_ONE_INTERVAL = 0x2,
    SQRO_DONT_SIMPLIFY_CONDITION_TREES = 0x4,
    SQRO_DONT_MAP_RELATIONS = 0x8,
    SQRO_DONT_RESOLVE_RANGES = 0x10,
    SQRO_DONT_REMOVE_UNRESTRICTED_KEYWORDS = 0x20,
    SQRO_DONT_SPLIT_WORDS = 0x40,
    SQRO_IGNORE_PHRASE_ORDER = 0x80
};
typedef int STRUCTURED_QUERY_RESOLVE_OPTION;

typedef enum tagINTERVAL_LIMIT_KIND {
    ILK_EXPLICIT_INCLUDED = 0,
    ILK_EXPLICIT_EXCLUDED = (ILK_EXPLICIT_INCLUDED + 1),
    ILK_NEGATIVE_INFINITY = (ILK_EXPLICIT_EXCLUDED + 1),
    ILK_POSITIVE_INFINITY = (ILK_NEGATIVE_INFINITY + 1)
} INTERVAL_LIMIT_KIND;

typedef enum tagQUERY_PARSER_MANAGER_OPTION {
    QPMO_SCHEMA_BINARY_NAME = 0,
    QPMO_PRELOCALIZED_SCHEMA_BINARY_PATH = (QPMO_SCHEMA_BINARY_NAME + 1),
    QPMO_UNLOCALIZED_SCHEMA_BINARY_PATH = (QPMO_PRELOCALIZED_SCHEMA_BINARY_PATH + 1),
    QPMO_LOCALIZED_SCHEMA_BINARY_PATH = (QPMO_UNLOCALIZED_SCHEMA_BINARY_PATH + 1),
    QPMO_APPEND_LCID_TO_LOCALIZED_PATH = (QPMO_LOCALIZED_SCHEMA_BINARY_PATH + 1),
    QPMO_LOCALIZER_SUPPORT = (QPMO_APPEND_LCID_TO_LOCALIZED_PATH + 1)
} QUERY_PARSER_MANAGER_OPTION;

extern RPC_IF_HANDLE __MIDL_itf_structuredquery_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_structuredquery_0000_0000_v0_0_s_ifspec;

#ifndef __IQueryParser_INTERFACE_DEFINED__
#define __IQueryParser_INTERFACE_DEFINED__

EXTERN_C const IID IID_IQueryParser;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2EBDEE67-3505-43f8-9946-EA44ABC8E5B0") IQueryParser:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Parse(LPCWSTR pszInputString,  IEnumUnknown * pCustomProperties, IQuerySolution ** ppSolution) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOption(STRUCTURED_QUERY_SINGLE_OPTION option, const PROPVARIANT * pOptionValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOption(STRUCTURED_QUERY_SINGLE_OPTION option, PROPVARIANT * pOptionValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMultiOption(STRUCTURED_QUERY_MULTIOPTION option, LPCWSTR pszOptionKey, const PROPVARIANT * pOptionValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSchemaProvider(ISchemaProvider ** ppSchemaProvider) = 0;
    virtual HRESULT STDMETHODCALLTYPE RestateToString( ICondition * pCondition, BOOL fUseEnglish, LPWSTR * ppszQueryString) = 0;
    virtual HRESULT STDMETHODCALLTYPE ParsePropertyValue(LPCWSTR pszPropertyName, LPCWSTR pszInputString, IQuerySolution ** ppSolution) = 0;
    virtual HRESULT STDMETHODCALLTYPE RestatePropertyValueToString( ICondition * pCondition, BOOL fUseEnglish, LPWSTR * ppszPropertyName, LPWSTR * ppszQueryString) = 0;
};
#else
typedef struct IQueryParserVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IQueryParser * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IQueryParser * This);
    ULONG(STDMETHODCALLTYPE *Release)(IQueryParser * This);
    HRESULT(STDMETHODCALLTYPE *Parse)(IQueryParser * This, LPCWSTR pszInputString,  IEnumUnknown * pCustomProperties, IQuerySolution ** ppSolution);
    HRESULT(STDMETHODCALLTYPE *SetOption)(IQueryParser * This, STRUCTURED_QUERY_SINGLE_OPTION option, const PROPVARIANT * pOptionValue);
    HRESULT(STDMETHODCALLTYPE *GetOption)(IQueryParser * This, STRUCTURED_QUERY_SINGLE_OPTION option, PROPVARIANT * pOptionValue);
    HRESULT(STDMETHODCALLTYPE *SetMultiOption)(IQueryParser * This, STRUCTURED_QUERY_MULTIOPTION option, LPCWSTR pszOptionKey, const PROPVARIANT * pOptionValue);
    HRESULT(STDMETHODCALLTYPE *GetSchemaProvider)(IQueryParser * This, ISchemaProvider ** ppSchemaProvider);
    HRESULT(STDMETHODCALLTYPE *RestateToString)(IQueryParser * This,  ICondition * pCondition, BOOL fUseEnglish, LPWSTR * ppszQueryString);
    HRESULT(STDMETHODCALLTYPE *ParsePropertyValue)(IQueryParser * This, LPCWSTR pszPropertyName, LPCWSTR pszInputString, IQuerySolution ** ppSolution);
    HRESULT(STDMETHODCALLTYPE *RestatePropertyValueToString)(IQueryParser * This,  ICondition * pCondition, BOOL fUseEnglish, LPWSTR * ppszPropertyName, LPWSTR * ppszQueryString);
    END_INTERFACE
} IQueryParserVtbl;

interface IQueryParser {
    CONST_VTBL struct IQueryParserVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IQueryParser_QueryInterface(This,riid,ppvObject)      ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IQueryParser_AddRef(This)     ( (This)->lpVtbl -> AddRef(This) )
#define IQueryParser_Release(This)    ( (This)->lpVtbl -> Release(This) )
#define IQueryParser_Parse(This,pszInputString,pCustomProperties,ppSolution)      ( (This)->lpVtbl -> Parse(This,pszInputString,pCustomProperties,ppSolution) )
#define IQueryParser_SetOption(This,option,pOptionValue)      ( (This)->lpVtbl -> SetOption(This,option,pOptionValue) )
#define IQueryParser_GetOption(This,option,pOptionValue)      ( (This)->lpVtbl -> GetOption(This,option,pOptionValue) )
#define IQueryParser_SetMultiOption(This,option,pszOptionKey,pOptionValue)    ( (This)->lpVtbl -> SetMultiOption(This,option,pszOptionKey,pOptionValue) )
#define IQueryParser_GetSchemaProvider(This,ppSchemaProvider)     ( (This)->lpVtbl -> GetSchemaProvider(This,ppSchemaProvider) )
#define IQueryParser_RestateToString(This,pCondition,fUseEnglish,ppszQueryString)     ( (This)->lpVtbl -> RestateToString(This,pCondition,fUseEnglish,ppszQueryString) )
#define IQueryParser_ParsePropertyValue(This,pszPropertyName,pszInputString,ppSolution)   ( (This)->lpVtbl -> ParsePropertyValue(This,pszPropertyName,pszInputString,ppSolution) )
#define IQueryParser_RestatePropertyValueToString(This,pCondition,fUseEnglish,ppszPropertyName,ppszQueryString)   ( (This)->lpVtbl -> RestatePropertyValueToString(This,pCondition,fUseEnglish,ppszPropertyName,ppszQueryString) )
#endif

#endif

#endif /* __IQueryParser_INTERFACE_DEFINED__ */

#ifndef __IConditionFactory_INTERFACE_DEFINED__
#define __IConditionFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IConditionFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A5EFE073-B16F-474f-9F3E-9F8B497A3E08") IConditionFactory:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE MakeNot( ICondition * pSubCondition, BOOL simplify, ICondition ** ppResultQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE MakeAndOr(CONDITION_TYPE nodeType,  IEnumUnknown * pSubConditions, BOOL simplify, ICondition ** ppResultQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE MakeLeaf( LPCWSTR pszPropertyName, CONDITION_OPERATION op,  LPCWSTR pszValueType, const PROPVARIANT * pValue,  IRichChunk * pPropertyNameTerm,  IRichChunk * pOperationTerm,  IRichChunk * pValueTerm, BOOL expand, ICondition ** ppResultQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resolve(ICondition * pConditionTree, STRUCTURED_QUERY_RESOLVE_OPTION sqro, const SYSTEMTIME * pstReferenceTime, ICondition ** ppResolvedConditionTree) = 0;
};
#else
typedef struct IConditionFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IConditionFactory * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IConditionFactory * This);
    ULONG(STDMETHODCALLTYPE *Release)(IConditionFactory * This);
    HRESULT(STDMETHODCALLTYPE *MakeNot)(IConditionFactory * This,  ICondition * pSubCondition, BOOL simplify, ICondition ** ppResultQuery);
    HRESULT(STDMETHODCALLTYPE *MakeAndOr)(IConditionFactory * This, CONDITION_TYPE nodeType,  IEnumUnknown * pSubConditions, BOOL simplify, ICondition ** ppResultQuery);
    HRESULT(STDMETHODCALLTYPE *MakeLeaf)(IConditionFactory * This,  LPCWSTR pszPropertyName, CONDITION_OPERATION op,  LPCWSTR pszValueType, const PROPVARIANT * pValue,  IRichChunk * pPropertyNameTerm,  IRichChunk * pOperationTerm,  IRichChunk * pValueTerm, BOOL expand, ICondition ** ppResultQuery);
    HRESULT(STDMETHODCALLTYPE *Resolve)(IConditionFactory * This, ICondition * pConditionTree, STRUCTURED_QUERY_RESOLVE_OPTION sqro, const SYSTEMTIME * pstReferenceTime, ICondition ** ppResolvedConditionTree);
    END_INTERFACE
} IConditionFactoryVtbl;

interface IConditionFactory {
    CONST_VTBL struct IConditionFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IConditionFactory_QueryInterface(This,riid,ppvObject)     ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IConditionFactory_AddRef(This)    ( (This)->lpVtbl -> AddRef(This) )
#define IConditionFactory_Release(This)   ( (This)->lpVtbl -> Release(This) )
#define IConditionFactory_MakeNot(This,pSubCondition,simplify,ppResultQuery)      ( (This)->lpVtbl -> MakeNot(This,pSubCondition,simplify,ppResultQuery) )
#define IConditionFactory_MakeAndOr(This,nodeType,pSubConditions,simplify,ppResultQuery)      ( (This)->lpVtbl -> MakeAndOr(This,nodeType,pSubConditions,simplify,ppResultQuery) )
#define IConditionFactory_MakeLeaf(This,pszPropertyName,op,pszValueType,pValue,pPropertyNameTerm,pOperationTerm,pValueTerm,expand,ppResultQuery)      ( (This)->lpVtbl -> MakeLeaf(This,pszPropertyName,op,pszValueType,pValue,pPropertyNameTerm,pOperationTerm,pValueTerm,expand,ppResultQuery) )
#define IConditionFactory_Resolve(This,pConditionTree,sqro,pstReferenceTime,ppResolvedConditionTree)      ( (This)->lpVtbl -> Resolve(This,pConditionTree,sqro,pstReferenceTime,ppResolvedConditionTree) )
#endif

#endif

#endif /* __IConditionFactory_INTERFACE_DEFINED__ */

#ifndef __IQuerySolution_INTERFACE_DEFINED__
#define __IQuerySolution_INTERFACE_DEFINED__

EXTERN_C const IID IID_IQuerySolution;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D6EBC66B-8921-4193-AFDD-A1789FB7FF57") IQuerySolution:public IConditionFactory {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetQuery(ICondition ** ppQueryNode, IEntity ** ppMainType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetErrors(REFIID riid, void **ppParseErrors) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLexicalData(LPWSTR * ppszInputString, ITokenCollection ** ppTokens, LCID * pLocale, IUnknown ** ppWordBreaker) = 0;
};
#else
typedef struct IQuerySolutionVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IQuerySolution * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IQuerySolution * This);
    ULONG(STDMETHODCALLTYPE *Release)(IQuerySolution * This);
    HRESULT(STDMETHODCALLTYPE *MakeNot)(IQuerySolution * This,  ICondition * pSubCondition, BOOL simplify, ICondition ** ppResultQuery);
    HRESULT(STDMETHODCALLTYPE *MakeAndOr)(IQuerySolution * This, CONDITION_TYPE nodeType,  IEnumUnknown * pSubConditions, BOOL simplify, ICondition ** ppResultQuery);
    HRESULT(STDMETHODCALLTYPE *MakeLeaf)(IQuerySolution * This,  LPCWSTR pszPropertyName, CONDITION_OPERATION op,  LPCWSTR pszValueType, const PROPVARIANT * pValue,  IRichChunk * pPropertyNameTerm,  IRichChunk * pOperationTerm,  IRichChunk * pValueTerm, BOOL expand, ICondition ** ppResultQuery);
    HRESULT(STDMETHODCALLTYPE *Resolve)(IQuerySolution * This, ICondition * pConditionTree, STRUCTURED_QUERY_RESOLVE_OPTION sqro, const SYSTEMTIME * pstReferenceTime, ICondition ** ppResolvedConditionTree);
    HRESULT(STDMETHODCALLTYPE *GetQuery)(IQuerySolution * This, ICondition ** ppQueryNode, IEntity ** ppMainType);
    HRESULT(STDMETHODCALLTYPE *GetErrors)(IQuerySolution * This, REFIID riid, void **ppParseErrors);
    HRESULT(STDMETHODCALLTYPE *GetLexicalData)(IQuerySolution * This, LPWSTR * ppszInputString, ITokenCollection ** ppTokens, LCID * pLocale, IUnknown ** ppWordBreaker);
    END_INTERFACE
} IQuerySolutionVtbl;

interface IQuerySolution {
    CONST_VTBL struct IQuerySolutionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IQuerySolution_QueryInterface(This,riid,ppvObject)    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IQuerySolution_AddRef(This)   ( (This)->lpVtbl -> AddRef(This) )
#define IQuerySolution_Release(This)      ( (This)->lpVtbl -> Release(This) )
#define IQuerySolution_MakeNot(This,pSubCondition,simplify,ppResultQuery)     ( (This)->lpVtbl -> MakeNot(This,pSubCondition,simplify,ppResultQuery) )
#define IQuerySolution_MakeAndOr(This,nodeType,pSubConditions,simplify,ppResultQuery)     ( (This)->lpVtbl -> MakeAndOr(This,nodeType,pSubConditions,simplify,ppResultQuery) )
#define IQuerySolution_MakeLeaf(This,pszPropertyName,op,pszValueType,pValue,pPropertyNameTerm,pOperationTerm,pValueTerm,expand,ppResultQuery)     ( (This)->lpVtbl -> MakeLeaf(This,pszPropertyName,op,pszValueType,pValue,pPropertyNameTerm,pOperationTerm,pValueTerm,expand,ppResultQuery) )
#define IQuerySolution_Resolve(This,pConditionTree,sqro,pstReferenceTime,ppResolvedConditionTree)     ( (This)->lpVtbl -> Resolve(This,pConditionTree,sqro,pstReferenceTime,ppResolvedConditionTree) )
#define IQuerySolution_GetQuery(This,ppQueryNode,ppMainType)      ( (This)->lpVtbl -> GetQuery(This,ppQueryNode,ppMainType) )
#define IQuerySolution_GetErrors(This,riid,ppParseErrors)     ( (This)->lpVtbl -> GetErrors(This,riid,ppParseErrors) )
#define IQuerySolution_GetLexicalData(This,ppszInputString,ppTokens,pLocale,ppWordBreaker)    ( (This)->lpVtbl -> GetLexicalData(This,ppszInputString,ppTokens,pLocale,ppWordBreaker) )
#endif

#endif

#endif /* __IQuerySolution_INTERFACE_DEFINED__ */

#ifndef __ICondition_INTERFACE_DEFINED__
#define __ICondition_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0FC988D4-C935-4b97-A973-46282EA175C8") ICondition:public IPersistStream {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetConditionType(CONDITION_TYPE * pNodeType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSubConditions(REFIID riid, void **ppv) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetComparisonInfo(LPWSTR * ppszPropertyName, CONDITION_OPERATION * pOperation, PROPVARIANT * pValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValueType(LPWSTR * ppszValueTypeName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetValueNormalization(LPWSTR * ppszNormalization) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInputTerms(IRichChunk ** ppPropertyTerm, IRichChunk ** ppOperationTerm, IRichChunk ** ppValueTerm) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(ICondition ** ppc) = 0;
};
#else
typedef struct IConditionVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ICondition * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICondition * This);
    ULONG(STDMETHODCALLTYPE *Release)(ICondition * This);
    HRESULT(STDMETHODCALLTYPE *GetClassID)(ICondition * This, CLSID * pClassID);
    HRESULT(STDMETHODCALLTYPE *IsDirty)(ICondition * This);
    HRESULT(STDMETHODCALLTYPE *Load)(ICondition * This,  IStream * pStm);
    HRESULT(STDMETHODCALLTYPE *Save)(ICondition * This,  IStream * pStm, BOOL fClearDirty);
    HRESULT(STDMETHODCALLTYPE *GetSizeMax)(ICondition * This, ULARGE_INTEGER * pcbSize);
    HRESULT(STDMETHODCALLTYPE *GetConditionType)(ICondition * This, CONDITION_TYPE * pNodeType);
    HRESULT(STDMETHODCALLTYPE *GetSubConditions)(ICondition * This, REFIID riid, void **ppv);
    HRESULT(STDMETHODCALLTYPE *GetComparisonInfo)(ICondition * This, LPWSTR * ppszPropertyName, CONDITION_OPERATION * pOperation, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *GetValueType)(ICondition * This, LPWSTR * ppszValueTypeName);
    HRESULT(STDMETHODCALLTYPE *GetValueNormalization)(ICondition * This, LPWSTR * ppszNormalization);
    HRESULT(STDMETHODCALLTYPE *GetInputTerms)(ICondition * This, IRichChunk ** ppPropertyTerm, IRichChunk ** ppOperationTerm, IRichChunk ** ppValueTerm);
    HRESULT(STDMETHODCALLTYPE *Clone)(ICondition * This, ICondition ** ppc);
    END_INTERFACE
} IConditionVtbl;

interface ICondition {
    CONST_VTBL struct IConditionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICondition_QueryInterface(This,riid,ppvObject)    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define ICondition_AddRef(This)   ( (This)->lpVtbl -> AddRef(This) )
#define ICondition_Release(This)      ( (This)->lpVtbl -> Release(This) )
#define ICondition_GetClassID(This,pClassID)      ( (This)->lpVtbl -> GetClassID(This,pClassID) )
#define ICondition_IsDirty(This)      ( (This)->lpVtbl -> IsDirty(This) )
#define ICondition_Load(This,pStm)    ( (This)->lpVtbl -> Load(This,pStm) )
#define ICondition_Save(This,pStm,fClearDirty)    ( (This)->lpVtbl -> Save(This,pStm,fClearDirty) )
#define ICondition_GetSizeMax(This,pcbSize)   ( (This)->lpVtbl -> GetSizeMax(This,pcbSize) )
#define ICondition_GetConditionType(This,pNodeType)   ( (This)->lpVtbl -> GetConditionType(This,pNodeType) )
#define ICondition_GetSubConditions(This,riid,ppv)    ( (This)->lpVtbl -> GetSubConditions(This,riid,ppv) )
#define ICondition_GetComparisonInfo(This,ppszPropertyName,pOperation,pValue)     ( (This)->lpVtbl -> GetComparisonInfo(This,ppszPropertyName,pOperation,pValue) )
#define ICondition_GetValueType(This,ppszValueTypeName)   ( (This)->lpVtbl -> GetValueType(This,ppszValueTypeName) )
#define ICondition_GetValueNormalization(This,ppszNormalization)      ( (This)->lpVtbl -> GetValueNormalization(This,ppszNormalization) )
#define ICondition_GetInputTerms(This,ppPropertyTerm,ppOperationTerm,ppValueTerm)     ( (This)->lpVtbl -> GetInputTerms(This,ppPropertyTerm,ppOperationTerm,ppValueTerm) )
#define ICondition_Clone(This,ppc)    ( (This)->lpVtbl -> Clone(This,ppc) )
#endif

#endif

#endif /* __ICondition_INTERFACE_DEFINED__ */

#ifndef __IConditionGenerator_INTERFACE_DEFINED__
#define __IConditionGenerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IConditionGenerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("92D2CC58-4386-45a3-B98C-7E0CE64A4117") IConditionGenerator:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Initialize( ISchemaProvider * pSchemaProvider) = 0;
    virtual HRESULT STDMETHODCALLTYPE RecognizeNamedEntities(LPCWSTR pszInputString, LCID lcid,  ITokenCollection * pTokenCollection, INamedEntityCollector * pNamedEntities) = 0;
    virtual HRESULT STDMETHODCALLTYPE GenerateForLeaf( IConditionFactory * pConditionFactory,  LPCWSTR pszPropertyName, CONDITION_OPERATION op,  LPCWSTR pszValueType, LPCWSTR pszValue,  LPCWSTR pszValue2,  IRichChunk * pPropertyNameTerm,  IRichChunk * pOperationTerm,  IRichChunk * pValueTerm, BOOL automaticWildcard, BOOL * pNoStringQuery, ICondition ** ppQueryExpression) = 0;
    virtual HRESULT STDMETHODCALLTYPE DefaultPhrase(LPCWSTR pszValueType, const PROPVARIANT * ppropvar, BOOL fUseEnglish, LPWSTR * ppszPhrase) = 0;
};
#else
typedef struct IConditionGeneratorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IConditionGenerator * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IConditionGenerator * This);
    ULONG(STDMETHODCALLTYPE *Release)(IConditionGenerator * This);
    HRESULT(STDMETHODCALLTYPE *Initialize)(IConditionGenerator * This,  ISchemaProvider * pSchemaProvider);
    HRESULT(STDMETHODCALLTYPE *RecognizeNamedEntities)(IConditionGenerator * This, LPCWSTR pszInputString, LCID lcid,  ITokenCollection * pTokenCollection, INamedEntityCollector * pNamedEntities);
    HRESULT(STDMETHODCALLTYPE *GenerateForLeaf)(IConditionGenerator * This,  IConditionFactory * pConditionFactory,  LPCWSTR pszPropertyName, CONDITION_OPERATION op,  LPCWSTR pszValueType, LPCWSTR pszValue,  LPCWSTR pszValue2,  IRichChunk * pPropertyNameTerm,  IRichChunk * pOperationTerm,  IRichChunk * pValueTerm, BOOL automaticWildcard, BOOL * pNoStringQuery, ICondition ** ppQueryExpression);
    HRESULT(STDMETHODCALLTYPE *DefaultPhrase)(IConditionGenerator * This, LPCWSTR pszValueType, const PROPVARIANT * ppropvar, BOOL fUseEnglish, LPWSTR * ppszPhrase);
    END_INTERFACE
} IConditionGeneratorVtbl;

interface IConditionGenerator {
    CONST_VTBL struct IConditionGeneratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IConditionGenerator_QueryInterface(This,riid,ppvObject)   ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IConditionGenerator_AddRef(This)      ( (This)->lpVtbl -> AddRef(This) )
#define IConditionGenerator_Release(This)     ( (This)->lpVtbl -> Release(This) )
#define IConditionGenerator_Initialize(This,pSchemaProvider)      ( (This)->lpVtbl -> Initialize(This,pSchemaProvider) )
#define IConditionGenerator_RecognizeNamedEntities(This,pszInputString,lcid,pTokenCollection,pNamedEntities)      ( (This)->lpVtbl -> RecognizeNamedEntities(This,pszInputString,lcid,pTokenCollection,pNamedEntities) )
#define IConditionGenerator_GenerateForLeaf(This,pConditionFactory,pszPropertyName,op,pszValueType,pszValue,pszValue2,pPropertyNameTerm,pOperationTerm,pValueTerm,automaticWildcard,pNoStringQuery,ppQueryExpression)     ( (This)->lpVtbl -> GenerateForLeaf(This,pConditionFactory,pszPropertyName,op,pszValueType,pszValue,pszValue2,pPropertyNameTerm,pOperationTerm,pValueTerm,automaticWildcard,pNoStringQuery,ppQueryExpression) )
#define IConditionGenerator_DefaultPhrase(This,pszValueType,ppropvar,fUseEnglish,ppszPhrase)      ( (This)->lpVtbl -> DefaultPhrase(This,pszValueType,ppropvar,fUseEnglish,ppszPhrase) )
#endif

#endif

#endif /* __IConditionGenerator_INTERFACE_DEFINED__ */

#ifndef __IRichChunk_INTERFACE_DEFINED__
#define __IRichChunk_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRichChunk;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4FDEF69C-DBC9-454e-9910-B34F3C64B510") IRichChunk:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetData(ULONG * pFirstPos, ULONG * pLength, LPWSTR * ppsz, PROPVARIANT * pValue) = 0;
};
#else
typedef struct IRichChunkVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IRichChunk * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IRichChunk * This);
    ULONG(STDMETHODCALLTYPE *Release)(IRichChunk * This);
    HRESULT(STDMETHODCALLTYPE *GetData)(IRichChunk * This, ULONG * pFirstPos, ULONG * pLength, LPWSTR * ppsz, PROPVARIANT * pValue);
    END_INTERFACE
} IRichChunkVtbl;

interface IRichChunk {
    CONST_VTBL struct IRichChunkVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define IRichChunk_QueryInterface(This,riid,ppvObject)    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IRichChunk_AddRef(This)   ( (This)->lpVtbl -> AddRef(This) )
#define IRichChunk_Release(This)      ( (This)->lpVtbl -> Release(This) )
#define IRichChunk_GetData(This,pFirstPos,pLength,ppsz,pValue)    ( (This)->lpVtbl -> GetData(This,pFirstPos,pLength,ppsz,pValue) )
#endif

#endif

#endif /* __IRichChunk_INTERFACE_DEFINED__ */

#ifndef __IInterval_INTERFACE_DEFINED__
#define __IInterval_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInterval;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6BF0A714-3C18-430b-8B5D-83B1C234D3DB") IInterval:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetLimits(INTERVAL_LIMIT_KIND * pilkLower, PROPVARIANT * ppropvarLower, INTERVAL_LIMIT_KIND * pilkUpper, PROPVARIANT * ppropvarUpper) = 0;
};
#else
typedef struct IIntervalVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IInterval * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInterval * This);
    ULONG(STDMETHODCALLTYPE *Release)(IInterval * This);
    HRESULT(STDMETHODCALLTYPE *GetLimits)(IInterval * This, INTERVAL_LIMIT_KIND * pilkLower, PROPVARIANT * ppropvarLower, INTERVAL_LIMIT_KIND * pilkUpper, PROPVARIANT * ppropvarUpper);
    END_INTERFACE
} IIntervalVtbl;

interface IInterval {
    CONST_VTBL struct IIntervalVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define IInterval_QueryInterface(This,riid,ppvObject)     ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IInterval_AddRef(This)    ( (This)->lpVtbl -> AddRef(This) )
#define IInterval_Release(This)   ( (This)->lpVtbl -> Release(This) )
#define IInterval_GetLimits(This,pilkLower,ppropvarLower,pilkUpper,ppropvarUpper)     ( (This)->lpVtbl -> GetLimits(This,pilkLower,ppropvarLower,pilkUpper,ppropvarUpper) )
#endif

#endif

#endif /* __IInterval_INTERFACE_DEFINED__ */

#ifndef __IMetaData_INTERFACE_DEFINED__
#define __IMetaData_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMetaData;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("780102B0-C43B-4876-BC7B-5E9BA5C88794") IMetaData:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetData(LPWSTR * ppszKey, LPWSTR * ppszValue) = 0;
};
#else
typedef struct IMetaDataVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMetaData * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMetaData * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMetaData * This);
    HRESULT(STDMETHODCALLTYPE *GetData)(IMetaData * This, LPWSTR * ppszKey, LPWSTR * ppszValue);
    END_INTERFACE
} IMetaDataVtbl;

interface IMetaData {
    CONST_VTBL struct IMetaDataVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define IMetaData_QueryInterface(This,riid,ppvObject)     ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IMetaData_AddRef(This)    ( (This)->lpVtbl -> AddRef(This) )
#define IMetaData_Release(This)   ( (This)->lpVtbl -> Release(This) )
#define IMetaData_GetData(This,ppszKey,ppszValue)     ( (This)->lpVtbl -> GetData(This,ppszKey,ppszValue) )
#endif

#endif

#endif /* __IMetaData_INTERFACE_DEFINED__ */

extern RPC_IF_HANDLE __MIDL_itf_structuredquery_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_structuredquery_0000_0008_v0_0_s_ifspec;

#ifndef __IEntity_INTERFACE_DEFINED__
#define __IEntity_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEntity;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("24264891-E80B-4fd3-B7CE-4FF2FAE8931F") IEntity:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Name(LPWSTR * ppszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE Base(IEntity ** pBaseEntity) = 0;
    virtual HRESULT STDMETHODCALLTYPE Relationships(REFIID riid, void **pRelationships) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRelationship(LPCWSTR pszRelationName, IRelationship ** pRelationship) = 0;
    virtual HRESULT STDMETHODCALLTYPE MetaData(REFIID riid, void **pMetaData) = 0;
    virtual HRESULT STDMETHODCALLTYPE NamedEntities(REFIID riid, void **pNamedEntities) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedEntity(LPCWSTR pszValue, INamedEntity ** ppNamedEntity) = 0;
    virtual HRESULT STDMETHODCALLTYPE DefaultPhrase(LPWSTR * ppszPhrase) = 0;
};
#else
typedef struct IEntityVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IEntity * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEntity * This);
    ULONG(STDMETHODCALLTYPE *Release)(IEntity * This);
    HRESULT(STDMETHODCALLTYPE *Name)(IEntity * This, LPWSTR * ppszName);
    HRESULT(STDMETHODCALLTYPE *Base)(IEntity * This, IEntity ** pBaseEntity);
    HRESULT(STDMETHODCALLTYPE *Relationships)(IEntity * This, REFIID riid, void **pRelationships);
    HRESULT(STDMETHODCALLTYPE *GetRelationship)(IEntity * This, LPCWSTR pszRelationName, IRelationship ** pRelationship);
    HRESULT(STDMETHODCALLTYPE *MetaData)(IEntity * This, REFIID riid, void **pMetaData);
    HRESULT(STDMETHODCALLTYPE *NamedEntities)(IEntity * This, REFIID riid, void **pNamedEntities);
    HRESULT(STDMETHODCALLTYPE *GetNamedEntity)(IEntity * This, LPCWSTR pszValue, INamedEntity ** ppNamedEntity);
    HRESULT(STDMETHODCALLTYPE *DefaultPhrase)(IEntity * This, LPWSTR * ppszPhrase);
    END_INTERFACE
} IEntityVtbl;

interface IEntity {
    CONST_VTBL struct IEntityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEntity_QueryInterface(This,riid,ppvObject)   ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IEntity_AddRef(This)      ( (This)->lpVtbl -> AddRef(This) )
#define IEntity_Release(This)     ( (This)->lpVtbl -> Release(This) )
#define IEntity_Name(This,ppszName)   ( (This)->lpVtbl -> Name(This,ppszName) )
#define IEntity_Base(This,pBaseEntity)    ( (This)->lpVtbl -> Base(This,pBaseEntity) )
#define IEntity_Relationships(This,riid,pRelationships)   ( (This)->lpVtbl -> Relationships(This,riid,pRelationships) )
#define IEntity_GetRelationship(This,pszRelationName,pRelationship)   ( (This)->lpVtbl -> GetRelationship(This,pszRelationName,pRelationship) )
#define IEntity_MetaData(This,riid,pMetaData)     ( (This)->lpVtbl -> MetaData(This,riid,pMetaData) )
#define IEntity_NamedEntities(This,riid,pNamedEntities)   ( (This)->lpVtbl -> NamedEntities(This,riid,pNamedEntities) )
#define IEntity_GetNamedEntity(This,pszValue,ppNamedEntity)   ( (This)->lpVtbl -> GetNamedEntity(This,pszValue,ppNamedEntity) )
#define IEntity_DefaultPhrase(This,ppszPhrase)    ( (This)->lpVtbl -> DefaultPhrase(This,ppszPhrase) )
#endif

#endif

#endif /* __IEntity_INTERFACE_DEFINED__ */

#ifndef __IRelationship_INTERFACE_DEFINED__
#define __IRelationship_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRelationship;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2769280B-5108-498c-9C7F-A51239B63147") IRelationship:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Name(LPWSTR * ppszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsReal(BOOL * pIsReal) = 0;
    virtual HRESULT STDMETHODCALLTYPE Destination(IEntity ** pDestinationEntity) = 0;
    virtual HRESULT STDMETHODCALLTYPE MetaData(REFIID riid, void **pMetaData) = 0;
    virtual HRESULT STDMETHODCALLTYPE DefaultPhrase(LPWSTR * ppszPhrase) = 0;
};
#else
typedef struct IRelationshipVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IRelationship * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IRelationship * This);
    ULONG(STDMETHODCALLTYPE *Release)(IRelationship * This);
    HRESULT(STDMETHODCALLTYPE *Name)(IRelationship * This, LPWSTR * ppszName);
    HRESULT(STDMETHODCALLTYPE *IsReal)(IRelationship * This, BOOL * pIsReal);
    HRESULT(STDMETHODCALLTYPE *Destination)(IRelationship * This, IEntity ** pDestinationEntity);
    HRESULT(STDMETHODCALLTYPE *MetaData)(IRelationship * This, REFIID riid, void **pMetaData);
    HRESULT(STDMETHODCALLTYPE *DefaultPhrase)(IRelationship * This, LPWSTR * ppszPhrase);
    END_INTERFACE
} IRelationshipVtbl;

interface IRelationship {
    CONST_VTBL struct IRelationshipVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRelationship_QueryInterface(This,riid,ppvObject)     ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IRelationship_AddRef(This)    ( (This)->lpVtbl -> AddRef(This) )
#define IRelationship_Release(This)   ( (This)->lpVtbl -> Release(This) )
#define IRelationship_Name(This,ppszName)     ( (This)->lpVtbl -> Name(This,ppszName) )
#define IRelationship_IsReal(This,pIsReal)    ( (This)->lpVtbl -> IsReal(This,pIsReal) )
#define IRelationship_Destination(This,pDestinationEntity)    ( (This)->lpVtbl -> Destination(This,pDestinationEntity) )
#define IRelationship_MetaData(This,riid,pMetaData)   ( (This)->lpVtbl -> MetaData(This,riid,pMetaData) )
#define IRelationship_DefaultPhrase(This,ppszPhrase)      ( (This)->lpVtbl -> DefaultPhrase(This,ppszPhrase) )
#endif

#endif

#endif /* __IRelationship_INTERFACE_DEFINED__ */

#ifndef __INamedEntity_INTERFACE_DEFINED__
#define __INamedEntity_INTERFACE_DEFINED__

EXTERN_C const IID IID_INamedEntity;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("ABDBD0B1-7D54-49fb-AB5C-BFF4130004CD") INamedEntity:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetValue(LPWSTR * ppszValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE DefaultPhrase(LPWSTR * ppszPhrase) = 0;
};
#else
typedef struct INamedEntityVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (INamedEntity * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(INamedEntity * This);
    ULONG(STDMETHODCALLTYPE *Release)(INamedEntity * This);
    HRESULT(STDMETHODCALLTYPE *GetValue)(INamedEntity * This, LPWSTR * ppszValue);
    HRESULT(STDMETHODCALLTYPE *DefaultPhrase)(INamedEntity * This, LPWSTR * ppszPhrase);
    END_INTERFACE
} INamedEntityVtbl;

interface INamedEntity {
    CONST_VTBL struct INamedEntityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define INamedEntity_QueryInterface(This,riid,ppvObject)      ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define INamedEntity_AddRef(This)     ( (This)->lpVtbl -> AddRef(This) )
#define INamedEntity_Release(This)    ( (This)->lpVtbl -> Release(This) )
#define INamedEntity_GetValue(This,ppszValue)     ( (This)->lpVtbl -> GetValue(This,ppszValue) )
#define INamedEntity_DefaultPhrase(This,ppszPhrase)   ( (This)->lpVtbl -> DefaultPhrase(This,ppszPhrase) )
#endif

#endif

#endif /* __INamedEntity_INTERFACE_DEFINED__ */

#ifndef __ISchemaProvider_INTERFACE_DEFINED__
#define __ISchemaProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISchemaProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8CF89BCB-394C-49b2-AE28-A59DD4ED7F68") ISchemaProvider:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Entities(REFIID riid, void **pEntities) = 0;
    virtual HRESULT STDMETHODCALLTYPE RootEntity(IEntity ** pRootEntity) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEntity(LPCWSTR pszEntityName, IEntity ** pEntity) = 0;
    virtual HRESULT STDMETHODCALLTYPE MetaData(REFIID riid, void **pMetaData) = 0;
    virtual HRESULT STDMETHODCALLTYPE Localize(LCID lcid,  ISchemaLocalizerSupport * pSchemaLocalizerSupport) = 0;
    virtual HRESULT STDMETHODCALLTYPE SaveBinary(LPCWSTR pszSchemaBinaryPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE LookupAuthoredNamedEntity( IEntity * pEntity, LPCWSTR pszInputString,  ITokenCollection * pTokenCollection, ULONG cTokensBegin, ULONG * pcTokensLength, LPWSTR * ppszValue) = 0;
};
#else
typedef struct ISchemaProviderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ISchemaProvider * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ISchemaProvider * This);
    ULONG(STDMETHODCALLTYPE *Release)(ISchemaProvider * This);
    HRESULT(STDMETHODCALLTYPE *Entities)(ISchemaProvider * This, REFIID riid, void **pEntities);
    HRESULT(STDMETHODCALLTYPE *RootEntity)(ISchemaProvider * This, IEntity ** pRootEntity);
    HRESULT(STDMETHODCALLTYPE *GetEntity)(ISchemaProvider * This, LPCWSTR pszEntityName, IEntity ** pEntity);
    HRESULT(STDMETHODCALLTYPE *MetaData)(ISchemaProvider * This, REFIID riid, void **pMetaData);
    HRESULT(STDMETHODCALLTYPE *Localize)(ISchemaProvider * This, LCID lcid,  ISchemaLocalizerSupport * pSchemaLocalizerSupport);
    HRESULT(STDMETHODCALLTYPE *SaveBinary)(ISchemaProvider * This, LPCWSTR pszSchemaBinaryPath);
    HRESULT(STDMETHODCALLTYPE *LookupAuthoredNamedEntity)(ISchemaProvider * This,  IEntity * pEntity, LPCWSTR pszInputString,  ITokenCollection * pTokenCollection, ULONG cTokensBegin, ULONG * pcTokensLength, LPWSTR * ppszValue);
    END_INTERFACE
} ISchemaProviderVtbl;

interface ISchemaProvider {
    CONST_VTBL struct ISchemaProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISchemaProvider_QueryInterface(This,riid,ppvObject)   ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define ISchemaProvider_AddRef(This)      ( (This)->lpVtbl -> AddRef(This) )
#define ISchemaProvider_Release(This)     ( (This)->lpVtbl -> Release(This) )
#define ISchemaProvider_Entities(This,riid,pEntities)     ( (This)->lpVtbl -> Entities(This,riid,pEntities) )
#define ISchemaProvider_RootEntity(This,pRootEntity)      ( (This)->lpVtbl -> RootEntity(This,pRootEntity) )
#define ISchemaProvider_GetEntity(This,pszEntityName,pEntity)     ( (This)->lpVtbl -> GetEntity(This,pszEntityName,pEntity) )
#define ISchemaProvider_MetaData(This,riid,pMetaData)     ( (This)->lpVtbl -> MetaData(This,riid,pMetaData) )
#define ISchemaProvider_Localize(This,lcid,pSchemaLocalizerSupport)   ( (This)->lpVtbl -> Localize(This,lcid,pSchemaLocalizerSupport) )
#define ISchemaProvider_SaveBinary(This,pszSchemaBinaryPath)      ( (This)->lpVtbl -> SaveBinary(This,pszSchemaBinaryPath) )
#define ISchemaProvider_LookupAuthoredNamedEntity(This,pEntity,pszInputString,pTokenCollection,cTokensBegin,pcTokensLength,ppszValue)     ( (This)->lpVtbl -> LookupAuthoredNamedEntity(This,pEntity,pszInputString,pTokenCollection,cTokensBegin,pcTokensLength,ppszValue) )
#endif

#endif

#endif /* __ISchemaProvider_INTERFACE_DEFINED__ */

#ifndef __ITokenCollection_INTERFACE_DEFINED__
#define __ITokenCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITokenCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("22D8B4F2-F577-4adb-A335-C2AE88416FAB") ITokenCollection:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE NumberOfTokens(ULONG * pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetToken(ULONG i, ULONG * pBegin, ULONG * pLength, LPWSTR * ppsz) = 0;
};
#else
typedef struct ITokenCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ITokenCollection * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITokenCollection * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITokenCollection * This);
    HRESULT(STDMETHODCALLTYPE *NumberOfTokens)(ITokenCollection * This, ULONG * pCount);
    HRESULT(STDMETHODCALLTYPE *GetToken)(ITokenCollection * This, ULONG i, ULONG * pBegin, ULONG * pLength, LPWSTR * ppsz);
    END_INTERFACE
} ITokenCollectionVtbl;

interface ITokenCollection {
    CONST_VTBL struct ITokenCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITokenCollection_QueryInterface(This,riid,ppvObject)      ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define ITokenCollection_AddRef(This)     ( (This)->lpVtbl -> AddRef(This) )
#define ITokenCollection_Release(This)    ( (This)->lpVtbl -> Release(This) )
#define ITokenCollection_NumberOfTokens(This,pCount)      ( (This)->lpVtbl -> NumberOfTokens(This,pCount) )
#define ITokenCollection_GetToken(This,i,pBegin,pLength,ppsz)     ( (This)->lpVtbl -> GetToken(This,i,pBegin,pLength,ppsz) )
#endif

#endif

#endif /* __ITokenCollection_INTERFACE_DEFINED__ */

typedef enum __MIDL___MIDL_itf_structuredquery_0000_0013_0001 {
    NEC_LOW = 0,
    NEC_MEDIUM = (NEC_LOW + 1),
    NEC_HIGH = (NEC_MEDIUM + 1)
} NAMED_ENTITY_CERTAINTY;

extern RPC_IF_HANDLE __MIDL_itf_structuredquery_0000_0013_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_structuredquery_0000_0013_v0_0_s_ifspec;

#ifndef __INamedEntityCollector_INTERFACE_DEFINED__
#define __INamedEntityCollector_INTERFACE_DEFINED__

EXTERN_C const IID IID_INamedEntityCollector;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("AF2440F6-8AFC-47d0-9A7F-396A0ACFB43D") INamedEntityCollector:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Add(ULONG beginSpan, ULONG endSpan, ULONG beginActual, ULONG endActual,  IEntity * pType, LPCWSTR pszValue, NAMED_ENTITY_CERTAINTY certainty) = 0;
};
#else
typedef struct INamedEntityCollectorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (INamedEntityCollector * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(INamedEntityCollector * This);
    ULONG(STDMETHODCALLTYPE *Release)(INamedEntityCollector * This);
    HRESULT(STDMETHODCALLTYPE *Add)(INamedEntityCollector * This, ULONG beginSpan, ULONG endSpan, ULONG beginActual, ULONG endActual,  IEntity * pType, LPCWSTR pszValue, NAMED_ENTITY_CERTAINTY certainty);
    END_INTERFACE
} INamedEntityCollectorVtbl;

interface INamedEntityCollector {
    CONST_VTBL struct INamedEntityCollectorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define INamedEntityCollector_QueryInterface(This,riid,ppvObject)     ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define INamedEntityCollector_AddRef(This)    ( (This)->lpVtbl -> AddRef(This) )
#define INamedEntityCollector_Release(This)   ( (This)->lpVtbl -> Release(This) )
#define INamedEntityCollector_Add(This,beginSpan,endSpan,beginActual,endActual,pType,pszValue,certainty)      ( (This)->lpVtbl -> Add(This,beginSpan,endSpan,beginActual,endActual,pType,pszValue,certainty) )
#endif

#endif

#endif /* __INamedEntityCollector_INTERFACE_DEFINED__ */

#ifndef __ISchemaLocalizerSupport_INTERFACE_DEFINED__
#define __ISchemaLocalizerSupport_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISchemaLocalizerSupport;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("CA3FDCA2-BFBE-4eed-90D7-0CAEF0A1BDA1") ISchemaLocalizerSupport:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Localize(LPCWSTR pszGlobalString, LPWSTR * ppszLocalString) = 0;
    };
#else
typedef struct ISchemaLocalizerSupportVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (ISchemaLocalizerSupport * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ISchemaLocalizerSupport * This);
    ULONG(STDMETHODCALLTYPE *Release)(ISchemaLocalizerSupport * This);
    HRESULT(STDMETHODCALLTYPE *Localize)(ISchemaLocalizerSupport * This, LPCWSTR pszGlobalString, LPWSTR * ppszLocalString);
    END_INTERFACE
} ISchemaLocalizerSupportVtbl;

interface ISchemaLocalizerSupport {
    CONST_VTBL struct ISchemaLocalizerSupportVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISchemaLocalizerSupport_QueryInterface(This,riid,ppvObject)   ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define ISchemaLocalizerSupport_AddRef(This)      ( (This)->lpVtbl -> AddRef(This) )
#define ISchemaLocalizerSupport_Release(This)     ( (This)->lpVtbl -> Release(This) )
#define ISchemaLocalizerSupport_Localize(This,pszGlobalString,ppszLocalString)    ( (This)->lpVtbl -> Localize(This,pszGlobalString,ppszLocalString) )
#endif

#endif

#endif /* __ISchemaLocalizerSupport_INTERFACE_DEFINED__ */

#ifndef __IQueryParserManager_INTERFACE_DEFINED__
#define __IQueryParserManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IQueryParserManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A879E3C4-AF77-44fb-8F37-EBD1487CF920") IQueryParserManager:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateLoadedParser(LPCWSTR pszCatalog, LANGID langidForKeywords, REFIID riid, void **ppQueryParser) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeOptions(BOOL fUnderstandNQS, BOOL fAutoWildCard,  IQueryParser * pQueryParser) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOption(QUERY_PARSER_MANAGER_OPTION option, const PROPVARIANT * pOptionValue) = 0;
};
#else
typedef struct IQueryParserManagerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IQueryParserManager * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IQueryParserManager * This);
    ULONG(STDMETHODCALLTYPE *Release)(IQueryParserManager * This);
    HRESULT(STDMETHODCALLTYPE *CreateLoadedParser)(IQueryParserManager * This, LPCWSTR pszCatalog, LANGID langidForKeywords, REFIID riid, void **ppQueryParser);
    HRESULT(STDMETHODCALLTYPE *InitializeOptions)(IQueryParserManager * This, BOOL fUnderstandNQS, BOOL fAutoWildCard,  IQueryParser * pQueryParser);
    HRESULT(STDMETHODCALLTYPE *SetOption)(IQueryParserManager * This, QUERY_PARSER_MANAGER_OPTION option, const PROPVARIANT * pOptionValue);
    END_INTERFACE
} IQueryParserManagerVtbl;

interface IQueryParserManager {
    CONST_VTBL struct IQueryParserManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IQueryParserManager_QueryInterface(This,riid,ppvObject)   ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IQueryParserManager_AddRef(This)      ( (This)->lpVtbl -> AddRef(This) )
#define IQueryParserManager_Release(This)     ( (This)->lpVtbl -> Release(This) )
#define IQueryParserManager_CreateLoadedParser(This,pszCatalog,langidForKeywords,riid,ppQueryParser)      ( (This)->lpVtbl -> CreateLoadedParser(This,pszCatalog,langidForKeywords,riid,ppQueryParser) )
#define IQueryParserManager_InitializeOptions(This,fUnderstandNQS,fAutoWildCard,pQueryParser)     ( (This)->lpVtbl -> InitializeOptions(This,fUnderstandNQS,fAutoWildCard,pQueryParser) )
#define IQueryParserManager_SetOption(This,option,pOptionValue)   ( (This)->lpVtbl -> SetOption(This,option,pOptionValue) )
#endif

#endif

#endif /* __IQueryParserManager_INTERFACE_DEFINED__ */

#ifndef __StructuredQuery1_LIBRARY_DEFINED__
#define __StructuredQuery1_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_StructuredQuery1;
EXTERN_C const CLSID CLSID_QueryParser;

#ifdef __cplusplus
class DECLSPEC_UUID("B72F8FD8-0FAB-4dd9-BDBF-245A6CE1485B") QueryParser;
#endif

EXTERN_C const CLSID CLSID_NegationCondition;

#ifdef __cplusplus
class DECLSPEC_UUID("8DE9C74C-605A-4acd-BEE3-2B222AA2D23D") NegationCondition;
#endif

EXTERN_C const CLSID CLSID_CompoundCondition;

#ifdef __cplusplus
class DECLSPEC_UUID("116F8D13-101E-4fa5-84D4-FF8279381935") CompoundCondition;
#endif

EXTERN_C const CLSID CLSID_LeafCondition;

#ifdef __cplusplus
class DECLSPEC_UUID("52F15C89-5A17-48e1-BBCD-46A3F89C7CC2") LeafCondition;
#endif

EXTERN_C const CLSID CLSID_ConditionFactory;

#ifdef __cplusplus
class DECLSPEC_UUID("E03E85B0-7BE3-4000-BA98-6C13DE9FA486") ConditionFactory;
#endif

EXTERN_C const CLSID CLSID_Interval;

#ifdef __cplusplus
class DECLSPEC_UUID("D957171F-4BF9-4de2-BCD5-C70A7CA55836") Interval;
#endif

EXTERN_C const CLSID CLSID_QueryParserManager;

#ifdef __cplusplus
class DECLSPEC_UUID("5088B39A-29B4-4d9d-8245-4EE289222F66") QueryParserManager;
#endif
#endif

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree(unsigned long *, LPSAFEARRAY *);

#ifdef __cplusplus
}
#endif

#endif /* _STRUCTUREDQUERY_H */
