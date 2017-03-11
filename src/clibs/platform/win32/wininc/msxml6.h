#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
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
#ifndef __msxml6_h__
#define __msxml6_h__
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#endif
typedef interface IXMLDOMNode IXMLDOMNode;

typedef interface IXMLDOMDocument IXMLDOMDocument;

typedef interface IXMLDOMDocument2 IXMLDOMDocument2;

typedef interface IXMLDOMDocument3 IXMLDOMDocument3;

typedef interface IXMLDOMSchemaCollection IXMLDOMSchemaCollection;

typedef interface IXMLDOMNodeList IXMLDOMNodeList;

typedef interface IXMLDOMSelection IXMLDOMSelection;

typedef interface IXMLDOMNamedNodeMap IXMLDOMNamedNodeMap;

typedef interface IXMLDOMDocumentFragment IXMLDOMDocumentFragment;

typedef interface IXMLDOMCharacterData IXMLDOMCharacterData;

typedef interface IXMLDOMAttribute IXMLDOMAttribute;

typedef interface IXMLDOMElement IXMLDOMElement;

typedef interface IXMLDOMText IXMLDOMText;

typedef interface IXMLDOMComment IXMLDOMComment;

typedef interface IXMLDOMProcessingInstruction IXMLDOMProcessingInstruction;

typedef interface IXMLDOMCDATASection IXMLDOMCDATASection;

typedef interface IXMLDOMDocumentType IXMLDOMDocumentType;

typedef interface IXMLDOMNotation IXMLDOMNotation;

typedef interface IXMLDOMEntity IXMLDOMEntity;

typedef interface IXMLDOMEntityReference IXMLDOMEntityReference;

typedef interface IXMLDOMImplementation IXMLDOMImplementation;

typedef interface IXTLRuntime IXTLRuntime;

typedef interface IXMLDOMParseError IXMLDOMParseError;

typedef interface IXMLDOMParseError2 IXMLDOMParseError2;

typedef interface IXMLDOMParseErrorCollection IXMLDOMParseErrorCollection;

typedef interface XMLDOMDocumentEvents XMLDOMDocumentEvents;

typedef interface IXSLProcessor IXSLProcessor;

typedef interface IXSLTemplate IXSLTemplate;

typedef interface IXMLHTTPRequest IXMLHTTPRequest;

typedef interface IServerXMLHTTPRequest IServerXMLHTTPRequest;

typedef interface IServerXMLHTTPRequest2 IServerXMLHTTPRequest2;

typedef interface ISAXXMLReader ISAXXMLReader;

typedef interface ISAXXMLFilter ISAXXMLFilter;

typedef interface ISAXLocator ISAXLocator;

typedef interface ISAXEntityResolver ISAXEntityResolver;

typedef interface ISAXContentHandler ISAXContentHandler;

typedef interface ISAXDTDHandler ISAXDTDHandler;

typedef interface ISAXErrorHandler ISAXErrorHandler;

typedef interface ISAXLexicalHandler ISAXLexicalHandler;

typedef interface ISAXDeclHandler ISAXDeclHandler;

typedef interface ISAXAttributes ISAXAttributes;

typedef interface IVBSAXXMLReader IVBSAXXMLReader;

typedef interface IVBSAXXMLFilter IVBSAXXMLFilter;

typedef interface IVBSAXLocator IVBSAXLocator;

typedef interface IVBSAXEntityResolver IVBSAXEntityResolver;

typedef interface IVBSAXContentHandler IVBSAXContentHandler;

typedef interface IVBSAXDTDHandler IVBSAXDTDHandler;

typedef interface IVBSAXErrorHandler IVBSAXErrorHandler;

typedef interface IVBSAXLexicalHandler IVBSAXLexicalHandler;

typedef interface IVBSAXDeclHandler IVBSAXDeclHandler;

typedef interface IVBSAXAttributes IVBSAXAttributes;

typedef interface IMXWriter IMXWriter;

typedef interface IMXAttributes IMXAttributes;

typedef interface IMXReaderControl IMXReaderControl;

typedef interface IMXSchemaDeclHandler IMXSchemaDeclHandler;

typedef interface IMXNamespacePrefixes IMXNamespacePrefixes;

typedef interface IVBMXNamespaceManager IVBMXNamespaceManager;

typedef interface IMXNamespaceManager IMXNamespaceManager;

typedef interface IMXXMLFilter IMXXMLFilter;

typedef interface IXMLDOMSchemaCollection2 IXMLDOMSchemaCollection2;

typedef interface ISchemaStringCollection ISchemaStringCollection;

typedef interface ISchemaItemCollection ISchemaItemCollection;

typedef interface ISchemaItem ISchemaItem;

typedef interface ISchema ISchema;

typedef interface ISchemaParticle ISchemaParticle;

typedef interface ISchemaAttribute ISchemaAttribute;

typedef interface ISchemaElement ISchemaElement;

typedef interface ISchemaType ISchemaType;

typedef interface ISchemaComplexType ISchemaComplexType;

typedef interface ISchemaAttributeGroup ISchemaAttributeGroup;

typedef interface ISchemaModelGroup ISchemaModelGroup;

typedef interface ISchemaAny ISchemaAny;

typedef interface ISchemaIdentityConstraint ISchemaIdentityConstraint;

typedef interface ISchemaNotation ISchemaNotation;

typedef interface IXMLDOMNode IXMLDOMNode;

typedef interface IXMLDOMNotation IXMLDOMNotation;

typedef interface IXMLDOMEntity IXMLDOMEntity;

typedef interface IXMLDOMParseError IXMLDOMParseError;

typedef interface IXMLDOMParseError2 IXMLDOMParseError2;

typedef interface IXMLDOMParseErrorCollection IXMLDOMParseErrorCollection;

typedef interface IXTLRuntime IXTLRuntime;

typedef interface ISAXXMLReader ISAXXMLReader;

typedef interface ISAXXMLFilter ISAXXMLFilter;

typedef interface IVBSAXXMLFilter IVBSAXXMLFilter;

typedef interface IMXReaderControl IMXReaderControl;

typedef interface IMXSchemaDeclHandler IMXSchemaDeclHandler;

typedef interface ISchemaItem ISchemaItem;

typedef interface ISchemaParticle ISchemaParticle;

typedef interface ISchemaElement ISchemaElement;

typedef interface ISchemaType ISchemaType;

typedef interface ISchemaComplexType ISchemaComplexType;

typedef interface ISchemaAny ISchemaAny;

typedef interface ISchemaModelGroup ISchemaModelGroup;

typedef interface IMXXMLFilter IMXXMLFilter;

typedef interface ISchemaAttribute ISchemaAttribute;

typedef interface ISchemaAttributeGroup ISchemaAttributeGroup;

typedef interface ISchemaIdentityConstraint ISchemaIdentityConstraint;

typedef interface ISchemaNotation ISchemaNotation;

typedef interface IXMLDOMNodeList IXMLDOMNodeList;

typedef interface IXMLDOMSelection IXMLDOMSelection;

typedef interface XMLDOMDocumentEvents XMLDOMDocumentEvents;


typedef struct DOMDocument60 DOMDocument60;


typedef struct FreeThreadedDOMDocument60 FreeThreadedDOMDocument60;


typedef struct XMLSchemaCache60 XMLSchemaCache60;


typedef struct XSLTemplate60 XSLTemplate60;


typedef struct XMLHTTP60 XMLHTTP60;


typedef struct FreeThreadedXMLHTTP60 FreeThreadedXMLHTTP60;


typedef struct ServerXMLHTTP60 ServerXMLHTTP60;


typedef struct SAXXMLReader60 SAXXMLReader60;


typedef struct MXXMLWriter60 MXXMLWriter60;


typedef struct MXHTMLWriter60 MXHTMLWriter60;


typedef struct SAXAttributes60 SAXAttributes60;


typedef struct MXNamespaceManager60 MXNamespaceManager60;

typedef interface IXMLHTTPRequest2Callback IXMLHTTPRequest2Callback;

typedef interface IXMLHTTPRequest2 IXMLHTTPRequest2;

typedef interface IXMLHTTPRequest3Callback IXMLHTTPRequest3Callback;

typedef interface IXMLHTTPRequest3 IXMLHTTPRequest3;

#include "unknwn.h"
#include "objidl.h"
#include "oaidl.h"
#ifdef __cplusplus
#endif 
#ifdef __USE_MSXML6_NAMESPACE__
namespace MSXML6 {
#endif
#if !defined(__msxml_h__)
typedef  
enum tagDOMNodeType
    {
        NODE_INVALID	= 0,
        NODE_ELEMENT	= ( NODE_INVALID + 1 ) ,
        NODE_ATTRIBUTE	= ( NODE_ELEMENT + 1 ) ,
        NODE_TEXT	= ( NODE_ATTRIBUTE + 1 ) ,
        NODE_CDATA_SECTION	= ( NODE_TEXT + 1 ) ,
        NODE_ENTITY_REFERENCE	= ( NODE_CDATA_SECTION + 1 ) ,
        NODE_ENTITY	= ( NODE_ENTITY_REFERENCE + 1 ) ,
        NODE_PROCESSING_INSTRUCTION	= ( NODE_ENTITY + 1 ) ,
        NODE_COMMENT	= ( NODE_PROCESSING_INSTRUCTION + 1 ) ,
        NODE_DOCUMENT	= ( NODE_COMMENT + 1 ) ,
        NODE_DOCUMENT_TYPE	= ( NODE_DOCUMENT + 1 ) ,
        NODE_DOCUMENT_FRAGMENT	= ( NODE_DOCUMENT_TYPE + 1 ) ,
        NODE_NOTATION	= ( NODE_DOCUMENT_FRAGMENT + 1 ) 
    } 	DOMNodeType;
#endif 
EXTERN_C const IID IID_IXMLDOMNode;
    typedef struct IXMLDOMNodeVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNode * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNode * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNode * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNode * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNode * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNode * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNode * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMNode * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMNode * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMNode * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMNode * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMNode * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMNode * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMNode * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMNode * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMNode * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMNode * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMNode * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMNode * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMNode * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMNode * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMNode * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMNode * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMNode * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMNode * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMNode * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMNode * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMNode * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMNode * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMNode * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMNode * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMNode * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMNode * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMNode * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMNode * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMNode * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMNode * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMNode * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMNode * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMNode * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMNode * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMNode * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMNode * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
        END_INTERFACE
    } IXMLDOMNodeVtbl;
    interface __uuid("2933BF80-7B36-11d2-B20E-00C04F983E60") IXMLDOMNode

    {
        CONST_VTBL struct IXMLDOMNodeVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMNode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMNode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMNode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMNode_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMNode_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMNode_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMNode_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMNode_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMNode_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMNode_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMNode_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMNode_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMNode_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMNode_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMNode_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMNode_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMNode_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMNode_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMNode_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMNode_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMNode_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMNode_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMNode_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMNode_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMNode_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMNode_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMNode_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMNode_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMNode_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMNode_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMNode_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMNode_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMNode_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMNode_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMNode_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMNode_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMNode_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMNode_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMNode_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMNode_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMNode_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMNode_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMNode_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMDocument;
    typedef struct IXMLDOMDocumentVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocument * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocument * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocument * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocument * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocument * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocument * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocument * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocument * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocument * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocument * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocument * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocument * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocument * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocument * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocument * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocument * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocument * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocument * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocument * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocument * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocument * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocument * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocument * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocument * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocument * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocument * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocument * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocument * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocument * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocument * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocument * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocument * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocument * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocument * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocument * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocument * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocument * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocument * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocument * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_doctype )( 
            IXMLDOMDocument * This,
             IXMLDOMDocumentType **documentType);
         HRESULT ( STDMETHODCALLTYPE *get_implementation )( 
            IXMLDOMDocument * This,
             IXMLDOMImplementation **impl);
         HRESULT ( STDMETHODCALLTYPE *get_documentElement )( 
            IXMLDOMDocument * This,
             IXMLDOMElement **DOMElement);
         HRESULT ( STDMETHODCALLTYPE *putref_documentElement )( 
            IXMLDOMDocument * This,
             IXMLDOMElement *DOMElement);
         HRESULT ( STDMETHODCALLTYPE *createElement )( 
            IXMLDOMDocument * This,
             BSTR tagName,
             IXMLDOMElement **element);
         HRESULT ( STDMETHODCALLTYPE *createDocumentFragment )( 
            IXMLDOMDocument * This,
             IXMLDOMDocumentFragment **docFrag);
         HRESULT ( STDMETHODCALLTYPE *createTextNode )( 
            IXMLDOMDocument * This,
             BSTR data,
             IXMLDOMText **text);
         HRESULT ( STDMETHODCALLTYPE *createComment )( 
            IXMLDOMDocument * This,
             BSTR data,
             IXMLDOMComment **comment);
         HRESULT ( STDMETHODCALLTYPE *createCDATASection )( 
            IXMLDOMDocument * This,
             BSTR data,
             IXMLDOMCDATASection **cdata);
         HRESULT ( STDMETHODCALLTYPE *createProcessingInstruction )( 
            IXMLDOMDocument * This,
             BSTR target,
             BSTR data,
             IXMLDOMProcessingInstruction **pi);
         HRESULT ( STDMETHODCALLTYPE *createAttribute )( 
            IXMLDOMDocument * This,
             BSTR name,
             IXMLDOMAttribute **attribute);
         HRESULT ( STDMETHODCALLTYPE *createEntityReference )( 
            IXMLDOMDocument * This,
             BSTR name,
             IXMLDOMEntityReference **entityRef);
         HRESULT ( STDMETHODCALLTYPE *getElementsByTagName )( 
            IXMLDOMDocument * This,
             BSTR tagName,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *createNode )( 
            IXMLDOMDocument * This,
             VARIANT Type,
             BSTR name,
             BSTR namespaceURI,
             IXMLDOMNode **node);
         HRESULT ( STDMETHODCALLTYPE *nodeFromID )( 
            IXMLDOMDocument * This,
             BSTR idString,
             IXMLDOMNode **node);
         HRESULT ( STDMETHODCALLTYPE *load )( 
            IXMLDOMDocument * This,
             VARIANT xmlSource,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLDOMDocument * This,
             long *value);
         HRESULT ( STDMETHODCALLTYPE *get_parseError )( 
            IXMLDOMDocument * This,
             IXMLDOMParseError **errorObj);
         HRESULT ( STDMETHODCALLTYPE *get_url )( 
            IXMLDOMDocument * This,
             BSTR *urlString);
         HRESULT ( STDMETHODCALLTYPE *get_async )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL *isAsync);
         HRESULT ( STDMETHODCALLTYPE *put_async )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL isAsync);
         HRESULT ( STDMETHODCALLTYPE *abort )( 
            IXMLDOMDocument * This);
         HRESULT ( STDMETHODCALLTYPE *loadXML )( 
            IXMLDOMDocument * This,
             BSTR bstrXML,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *save )( 
            IXMLDOMDocument * This,
             VARIANT destination);
         HRESULT ( STDMETHODCALLTYPE *get_validateOnParse )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL *isValidating);
         HRESULT ( STDMETHODCALLTYPE *put_validateOnParse )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL isValidating);
         HRESULT ( STDMETHODCALLTYPE *get_resolveExternals )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL *isResolving);
         HRESULT ( STDMETHODCALLTYPE *put_resolveExternals )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL isResolving);
         HRESULT ( STDMETHODCALLTYPE *get_preserveWhiteSpace )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL *isPreserving);
         HRESULT ( STDMETHODCALLTYPE *put_preserveWhiteSpace )( 
            IXMLDOMDocument * This,
             VARIANT_BOOL isPreserving);
         HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IXMLDOMDocument * This,
             VARIANT readystatechangeSink);
         HRESULT ( STDMETHODCALLTYPE *put_ondataavailable )( 
            IXMLDOMDocument * This,
             VARIANT ondataavailableSink);
         HRESULT ( STDMETHODCALLTYPE *put_ontransformnode )( 
            IXMLDOMDocument * This,
             VARIANT ontransformnodeSink);
        END_INTERFACE
    } IXMLDOMDocumentVtbl;
    interface __uuid("2933BF81-7B36-11d2-B20E-00C04F983E60") IXMLDOMDocument

    {
        CONST_VTBL struct IXMLDOMDocumentVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMDocument_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMDocument_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMDocument_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMDocument_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMDocument_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMDocument_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMDocument_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMDocument_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMDocument_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMDocument_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMDocument_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMDocument_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMDocument_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMDocument_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMDocument_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMDocument_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMDocument_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMDocument_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMDocument_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMDocument_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMDocument_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMDocument_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMDocument_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMDocument_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMDocument_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMDocument_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMDocument_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMDocument_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocument_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocument_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMDocument_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMDocument_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMDocument_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMDocument_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMDocument_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMDocument_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMDocument_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMDocument_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMDocument_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMDocument_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMDocument_get_doctype(This,documentType)	\
    ( (This)->lpVtbl -> get_doctype(This,documentType) ) 
#define IXMLDOMDocument_get_implementation(This,impl)	\
    ( (This)->lpVtbl -> get_implementation(This,impl) ) 
#define IXMLDOMDocument_get_documentElement(This,DOMElement)	\
    ( (This)->lpVtbl -> get_documentElement(This,DOMElement) ) 
#define IXMLDOMDocument_putref_documentElement(This,DOMElement)	\
    ( (This)->lpVtbl -> putref_documentElement(This,DOMElement) ) 
#define IXMLDOMDocument_createElement(This,tagName,element)	\
    ( (This)->lpVtbl -> createElement(This,tagName,element) ) 
#define IXMLDOMDocument_createDocumentFragment(This,docFrag)	\
    ( (This)->lpVtbl -> createDocumentFragment(This,docFrag) ) 
#define IXMLDOMDocument_createTextNode(This,data,text)	\
    ( (This)->lpVtbl -> createTextNode(This,data,text) ) 
#define IXMLDOMDocument_createComment(This,data,comment)	\
    ( (This)->lpVtbl -> createComment(This,data,comment) ) 
#define IXMLDOMDocument_createCDATASection(This,data,cdata)	\
    ( (This)->lpVtbl -> createCDATASection(This,data,cdata) ) 
#define IXMLDOMDocument_createProcessingInstruction(This,target,data,pi)	\
    ( (This)->lpVtbl -> createProcessingInstruction(This,target,data,pi) ) 
#define IXMLDOMDocument_createAttribute(This,name,attribute)	\
    ( (This)->lpVtbl -> createAttribute(This,name,attribute) ) 
#define IXMLDOMDocument_createEntityReference(This,name,entityRef)	\
    ( (This)->lpVtbl -> createEntityReference(This,name,entityRef) ) 
#define IXMLDOMDocument_getElementsByTagName(This,tagName,resultList)	\
    ( (This)->lpVtbl -> getElementsByTagName(This,tagName,resultList) ) 
#define IXMLDOMDocument_createNode(This,Type,name,namespaceURI,node)	\
    ( (This)->lpVtbl -> createNode(This,Type,name,namespaceURI,node) ) 
#define IXMLDOMDocument_nodeFromID(This,idString,node)	\
    ( (This)->lpVtbl -> nodeFromID(This,idString,node) ) 
#define IXMLDOMDocument_load(This,xmlSource,isSuccessful)	\
    ( (This)->lpVtbl -> load(This,xmlSource,isSuccessful) ) 
#define IXMLDOMDocument_get_readyState(This,value)	\
    ( (This)->lpVtbl -> get_readyState(This,value) ) 
#define IXMLDOMDocument_get_parseError(This,errorObj)	\
    ( (This)->lpVtbl -> get_parseError(This,errorObj) ) 
#define IXMLDOMDocument_get_url(This,urlString)	\
    ( (This)->lpVtbl -> get_url(This,urlString) ) 
#define IXMLDOMDocument_get_async(This,isAsync)	\
    ( (This)->lpVtbl -> get_async(This,isAsync) ) 
#define IXMLDOMDocument_put_async(This,isAsync)	\
    ( (This)->lpVtbl -> put_async(This,isAsync) ) 
#define IXMLDOMDocument_abort(This)	\
    ( (This)->lpVtbl -> abort(This) ) 
#define IXMLDOMDocument_loadXML(This,bstrXML,isSuccessful)	\
    ( (This)->lpVtbl -> loadXML(This,bstrXML,isSuccessful) ) 
#define IXMLDOMDocument_save(This,destination)	\
    ( (This)->lpVtbl -> save(This,destination) ) 
#define IXMLDOMDocument_get_validateOnParse(This,isValidating)	\
    ( (This)->lpVtbl -> get_validateOnParse(This,isValidating) ) 
#define IXMLDOMDocument_put_validateOnParse(This,isValidating)	\
    ( (This)->lpVtbl -> put_validateOnParse(This,isValidating) ) 
#define IXMLDOMDocument_get_resolveExternals(This,isResolving)	\
    ( (This)->lpVtbl -> get_resolveExternals(This,isResolving) ) 
#define IXMLDOMDocument_put_resolveExternals(This,isResolving)	\
    ( (This)->lpVtbl -> put_resolveExternals(This,isResolving) ) 
#define IXMLDOMDocument_get_preserveWhiteSpace(This,isPreserving)	\
    ( (This)->lpVtbl -> get_preserveWhiteSpace(This,isPreserving) ) 
#define IXMLDOMDocument_put_preserveWhiteSpace(This,isPreserving)	\
    ( (This)->lpVtbl -> put_preserveWhiteSpace(This,isPreserving) ) 
#define IXMLDOMDocument_put_onreadystatechange(This,readystatechangeSink)	\
    ( (This)->lpVtbl -> put_onreadystatechange(This,readystatechangeSink) ) 
#define IXMLDOMDocument_put_ondataavailable(This,ondataavailableSink)	\
    ( (This)->lpVtbl -> put_ondataavailable(This,ondataavailableSink) ) 
#define IXMLDOMDocument_put_ontransformnode(This,ontransformnodeSink)	\
    ( (This)->lpVtbl -> put_ontransformnode(This,ontransformnodeSink) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMDocument2;
    typedef struct IXMLDOMDocument2Vtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocument2 * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocument2 * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocument2 * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocument2 * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocument2 * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocument2 * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocument2 * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocument2 * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocument2 * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocument2 * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocument2 * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocument2 * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocument2 * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocument2 * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocument2 * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocument2 * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocument2 * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocument2 * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocument2 * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocument2 * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocument2 * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocument2 * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocument2 * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocument2 * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocument2 * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocument2 * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_doctype )( 
            IXMLDOMDocument2 * This,
             IXMLDOMDocumentType **documentType);
         HRESULT ( STDMETHODCALLTYPE *get_implementation )( 
            IXMLDOMDocument2 * This,
             IXMLDOMImplementation **impl);
         HRESULT ( STDMETHODCALLTYPE *get_documentElement )( 
            IXMLDOMDocument2 * This,
             IXMLDOMElement **DOMElement);
         HRESULT ( STDMETHODCALLTYPE *putref_documentElement )( 
            IXMLDOMDocument2 * This,
             IXMLDOMElement *DOMElement);
         HRESULT ( STDMETHODCALLTYPE *createElement )( 
            IXMLDOMDocument2 * This,
             BSTR tagName,
             IXMLDOMElement **element);
         HRESULT ( STDMETHODCALLTYPE *createDocumentFragment )( 
            IXMLDOMDocument2 * This,
             IXMLDOMDocumentFragment **docFrag);
         HRESULT ( STDMETHODCALLTYPE *createTextNode )( 
            IXMLDOMDocument2 * This,
             BSTR data,
             IXMLDOMText **text);
         HRESULT ( STDMETHODCALLTYPE *createComment )( 
            IXMLDOMDocument2 * This,
             BSTR data,
             IXMLDOMComment **comment);
         HRESULT ( STDMETHODCALLTYPE *createCDATASection )( 
            IXMLDOMDocument2 * This,
             BSTR data,
             IXMLDOMCDATASection **cdata);
         HRESULT ( STDMETHODCALLTYPE *createProcessingInstruction )( 
            IXMLDOMDocument2 * This,
             BSTR target,
             BSTR data,
             IXMLDOMProcessingInstruction **pi);
         HRESULT ( STDMETHODCALLTYPE *createAttribute )( 
            IXMLDOMDocument2 * This,
             BSTR name,
             IXMLDOMAttribute **attribute);
         HRESULT ( STDMETHODCALLTYPE *createEntityReference )( 
            IXMLDOMDocument2 * This,
             BSTR name,
             IXMLDOMEntityReference **entityRef);
         HRESULT ( STDMETHODCALLTYPE *getElementsByTagName )( 
            IXMLDOMDocument2 * This,
             BSTR tagName,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *createNode )( 
            IXMLDOMDocument2 * This,
             VARIANT Type,
             BSTR name,
             BSTR namespaceURI,
             IXMLDOMNode **node);
         HRESULT ( STDMETHODCALLTYPE *nodeFromID )( 
            IXMLDOMDocument2 * This,
             BSTR idString,
             IXMLDOMNode **node);
         HRESULT ( STDMETHODCALLTYPE *load )( 
            IXMLDOMDocument2 * This,
             VARIANT xmlSource,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLDOMDocument2 * This,
             long *value);
         HRESULT ( STDMETHODCALLTYPE *get_parseError )( 
            IXMLDOMDocument2 * This,
             IXMLDOMParseError **errorObj);
         HRESULT ( STDMETHODCALLTYPE *get_url )( 
            IXMLDOMDocument2 * This,
             BSTR *urlString);
         HRESULT ( STDMETHODCALLTYPE *get_async )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL *isAsync);
         HRESULT ( STDMETHODCALLTYPE *put_async )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL isAsync);
         HRESULT ( STDMETHODCALLTYPE *abort )( 
            IXMLDOMDocument2 * This);
         HRESULT ( STDMETHODCALLTYPE *loadXML )( 
            IXMLDOMDocument2 * This,
             BSTR bstrXML,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *save )( 
            IXMLDOMDocument2 * This,
             VARIANT destination);
         HRESULT ( STDMETHODCALLTYPE *get_validateOnParse )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL *isValidating);
         HRESULT ( STDMETHODCALLTYPE *put_validateOnParse )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL isValidating);
         HRESULT ( STDMETHODCALLTYPE *get_resolveExternals )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL *isResolving);
         HRESULT ( STDMETHODCALLTYPE *put_resolveExternals )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL isResolving);
         HRESULT ( STDMETHODCALLTYPE *get_preserveWhiteSpace )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL *isPreserving);
         HRESULT ( STDMETHODCALLTYPE *put_preserveWhiteSpace )( 
            IXMLDOMDocument2 * This,
             VARIANT_BOOL isPreserving);
         HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IXMLDOMDocument2 * This,
             VARIANT readystatechangeSink);
         HRESULT ( STDMETHODCALLTYPE *put_ondataavailable )( 
            IXMLDOMDocument2 * This,
             VARIANT ondataavailableSink);
         HRESULT ( STDMETHODCALLTYPE *put_ontransformnode )( 
            IXMLDOMDocument2 * This,
             VARIANT ontransformnodeSink);
         HRESULT ( STDMETHODCALLTYPE *get_namespaces )( 
            IXMLDOMDocument2 * This,
             IXMLDOMSchemaCollection **namespaceCollection);
         HRESULT ( STDMETHODCALLTYPE *get_schemas )( 
            IXMLDOMDocument2 * This,
             VARIANT *otherCollection);
         HRESULT ( STDMETHODCALLTYPE *putref_schemas )( 
            IXMLDOMDocument2 * This,
             VARIANT otherCollection);
         HRESULT ( STDMETHODCALLTYPE *validate )( 
            IXMLDOMDocument2 * This,
             IXMLDOMParseError **errorObj);
         HRESULT ( STDMETHODCALLTYPE *setProperty )( 
            IXMLDOMDocument2 * This,
             BSTR name,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *getProperty )( 
            IXMLDOMDocument2 * This,
             BSTR name,
             VARIANT *value);
        END_INTERFACE
    } IXMLDOMDocument2Vtbl;
    interface __uuid("2933BF95-7B36-11d2-B20E-00C04F983E60") IXMLDOMDocument2

    {
        CONST_VTBL struct IXMLDOMDocument2Vtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMDocument2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMDocument2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMDocument2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMDocument2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMDocument2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMDocument2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMDocument2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMDocument2_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMDocument2_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMDocument2_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMDocument2_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMDocument2_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMDocument2_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMDocument2_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMDocument2_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMDocument2_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMDocument2_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMDocument2_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMDocument2_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMDocument2_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMDocument2_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMDocument2_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMDocument2_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMDocument2_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMDocument2_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMDocument2_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMDocument2_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMDocument2_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMDocument2_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMDocument2_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMDocument2_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocument2_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocument2_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMDocument2_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMDocument2_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMDocument2_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMDocument2_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMDocument2_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMDocument2_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMDocument2_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMDocument2_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMDocument2_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMDocument2_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMDocument2_get_doctype(This,documentType)	\
    ( (This)->lpVtbl -> get_doctype(This,documentType) ) 
#define IXMLDOMDocument2_get_implementation(This,impl)	\
    ( (This)->lpVtbl -> get_implementation(This,impl) ) 
#define IXMLDOMDocument2_get_documentElement(This,DOMElement)	\
    ( (This)->lpVtbl -> get_documentElement(This,DOMElement) ) 
#define IXMLDOMDocument2_putref_documentElement(This,DOMElement)	\
    ( (This)->lpVtbl -> putref_documentElement(This,DOMElement) ) 
#define IXMLDOMDocument2_createElement(This,tagName,element)	\
    ( (This)->lpVtbl -> createElement(This,tagName,element) ) 
#define IXMLDOMDocument2_createDocumentFragment(This,docFrag)	\
    ( (This)->lpVtbl -> createDocumentFragment(This,docFrag) ) 
#define IXMLDOMDocument2_createTextNode(This,data,text)	\
    ( (This)->lpVtbl -> createTextNode(This,data,text) ) 
#define IXMLDOMDocument2_createComment(This,data,comment)	\
    ( (This)->lpVtbl -> createComment(This,data,comment) ) 
#define IXMLDOMDocument2_createCDATASection(This,data,cdata)	\
    ( (This)->lpVtbl -> createCDATASection(This,data,cdata) ) 
#define IXMLDOMDocument2_createProcessingInstruction(This,target,data,pi)	\
    ( (This)->lpVtbl -> createProcessingInstruction(This,target,data,pi) ) 
#define IXMLDOMDocument2_createAttribute(This,name,attribute)	\
    ( (This)->lpVtbl -> createAttribute(This,name,attribute) ) 
#define IXMLDOMDocument2_createEntityReference(This,name,entityRef)	\
    ( (This)->lpVtbl -> createEntityReference(This,name,entityRef) ) 
#define IXMLDOMDocument2_getElementsByTagName(This,tagName,resultList)	\
    ( (This)->lpVtbl -> getElementsByTagName(This,tagName,resultList) ) 
#define IXMLDOMDocument2_createNode(This,Type,name,namespaceURI,node)	\
    ( (This)->lpVtbl -> createNode(This,Type,name,namespaceURI,node) ) 
#define IXMLDOMDocument2_nodeFromID(This,idString,node)	\
    ( (This)->lpVtbl -> nodeFromID(This,idString,node) ) 
#define IXMLDOMDocument2_load(This,xmlSource,isSuccessful)	\
    ( (This)->lpVtbl -> load(This,xmlSource,isSuccessful) ) 
#define IXMLDOMDocument2_get_readyState(This,value)	\
    ( (This)->lpVtbl -> get_readyState(This,value) ) 
#define IXMLDOMDocument2_get_parseError(This,errorObj)	\
    ( (This)->lpVtbl -> get_parseError(This,errorObj) ) 
#define IXMLDOMDocument2_get_url(This,urlString)	\
    ( (This)->lpVtbl -> get_url(This,urlString) ) 
#define IXMLDOMDocument2_get_async(This,isAsync)	\
    ( (This)->lpVtbl -> get_async(This,isAsync) ) 
#define IXMLDOMDocument2_put_async(This,isAsync)	\
    ( (This)->lpVtbl -> put_async(This,isAsync) ) 
#define IXMLDOMDocument2_abort(This)	\
    ( (This)->lpVtbl -> abort(This) ) 
#define IXMLDOMDocument2_loadXML(This,bstrXML,isSuccessful)	\
    ( (This)->lpVtbl -> loadXML(This,bstrXML,isSuccessful) ) 
#define IXMLDOMDocument2_save(This,destination)	\
    ( (This)->lpVtbl -> save(This,destination) ) 
#define IXMLDOMDocument2_get_validateOnParse(This,isValidating)	\
    ( (This)->lpVtbl -> get_validateOnParse(This,isValidating) ) 
#define IXMLDOMDocument2_put_validateOnParse(This,isValidating)	\
    ( (This)->lpVtbl -> put_validateOnParse(This,isValidating) ) 
#define IXMLDOMDocument2_get_resolveExternals(This,isResolving)	\
    ( (This)->lpVtbl -> get_resolveExternals(This,isResolving) ) 
#define IXMLDOMDocument2_put_resolveExternals(This,isResolving)	\
    ( (This)->lpVtbl -> put_resolveExternals(This,isResolving) ) 
#define IXMLDOMDocument2_get_preserveWhiteSpace(This,isPreserving)	\
    ( (This)->lpVtbl -> get_preserveWhiteSpace(This,isPreserving) ) 
#define IXMLDOMDocument2_put_preserveWhiteSpace(This,isPreserving)	\
    ( (This)->lpVtbl -> put_preserveWhiteSpace(This,isPreserving) ) 
#define IXMLDOMDocument2_put_onreadystatechange(This,readystatechangeSink)	\
    ( (This)->lpVtbl -> put_onreadystatechange(This,readystatechangeSink) ) 
#define IXMLDOMDocument2_put_ondataavailable(This,ondataavailableSink)	\
    ( (This)->lpVtbl -> put_ondataavailable(This,ondataavailableSink) ) 
#define IXMLDOMDocument2_put_ontransformnode(This,ontransformnodeSink)	\
    ( (This)->lpVtbl -> put_ontransformnode(This,ontransformnodeSink) ) 
#define IXMLDOMDocument2_get_namespaces(This,namespaceCollection)	\
    ( (This)->lpVtbl -> get_namespaces(This,namespaceCollection) ) 
#define IXMLDOMDocument2_get_schemas(This,otherCollection)	\
    ( (This)->lpVtbl -> get_schemas(This,otherCollection) ) 
#define IXMLDOMDocument2_putref_schemas(This,otherCollection)	\
    ( (This)->lpVtbl -> putref_schemas(This,otherCollection) ) 
#define IXMLDOMDocument2_validate(This,errorObj)	\
    ( (This)->lpVtbl -> validate(This,errorObj) ) 
#define IXMLDOMDocument2_setProperty(This,name,value)	\
    ( (This)->lpVtbl -> setProperty(This,name,value) ) 
#define IXMLDOMDocument2_getProperty(This,name,value)	\
    ( (This)->lpVtbl -> getProperty(This,name,value) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMDocument3;
    typedef struct IXMLDOMDocument3Vtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocument3 * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocument3 * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocument3 * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocument3 * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocument3 * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocument3 * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocument3 * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocument3 * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocument3 * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocument3 * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocument3 * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocument3 * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocument3 * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocument3 * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocument3 * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocument3 * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocument3 * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocument3 * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocument3 * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocument3 * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocument3 * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocument3 * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocument3 * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocument3 * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocument3 * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_doctype )( 
            IXMLDOMDocument3 * This,
             IXMLDOMDocumentType **documentType);
         HRESULT ( STDMETHODCALLTYPE *get_implementation )( 
            IXMLDOMDocument3 * This,
             IXMLDOMImplementation **impl);
         HRESULT ( STDMETHODCALLTYPE *get_documentElement )( 
            IXMLDOMDocument3 * This,
             IXMLDOMElement **DOMElement);
         HRESULT ( STDMETHODCALLTYPE *putref_documentElement )( 
            IXMLDOMDocument3 * This,
             IXMLDOMElement *DOMElement);
         HRESULT ( STDMETHODCALLTYPE *createElement )( 
            IXMLDOMDocument3 * This,
             BSTR tagName,
             IXMLDOMElement **element);
         HRESULT ( STDMETHODCALLTYPE *createDocumentFragment )( 
            IXMLDOMDocument3 * This,
             IXMLDOMDocumentFragment **docFrag);
         HRESULT ( STDMETHODCALLTYPE *createTextNode )( 
            IXMLDOMDocument3 * This,
             BSTR data,
             IXMLDOMText **text);
         HRESULT ( STDMETHODCALLTYPE *createComment )( 
            IXMLDOMDocument3 * This,
             BSTR data,
             IXMLDOMComment **comment);
         HRESULT ( STDMETHODCALLTYPE *createCDATASection )( 
            IXMLDOMDocument3 * This,
             BSTR data,
             IXMLDOMCDATASection **cdata);
         HRESULT ( STDMETHODCALLTYPE *createProcessingInstruction )( 
            IXMLDOMDocument3 * This,
             BSTR target,
             BSTR data,
             IXMLDOMProcessingInstruction **pi);
         HRESULT ( STDMETHODCALLTYPE *createAttribute )( 
            IXMLDOMDocument3 * This,
             BSTR name,
             IXMLDOMAttribute **attribute);
         HRESULT ( STDMETHODCALLTYPE *createEntityReference )( 
            IXMLDOMDocument3 * This,
             BSTR name,
             IXMLDOMEntityReference **entityRef);
         HRESULT ( STDMETHODCALLTYPE *getElementsByTagName )( 
            IXMLDOMDocument3 * This,
             BSTR tagName,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *createNode )( 
            IXMLDOMDocument3 * This,
             VARIANT Type,
             BSTR name,
             BSTR namespaceURI,
             IXMLDOMNode **node);
         HRESULT ( STDMETHODCALLTYPE *nodeFromID )( 
            IXMLDOMDocument3 * This,
             BSTR idString,
             IXMLDOMNode **node);
         HRESULT ( STDMETHODCALLTYPE *load )( 
            IXMLDOMDocument3 * This,
             VARIANT xmlSource,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLDOMDocument3 * This,
             long *value);
         HRESULT ( STDMETHODCALLTYPE *get_parseError )( 
            IXMLDOMDocument3 * This,
             IXMLDOMParseError **errorObj);
         HRESULT ( STDMETHODCALLTYPE *get_url )( 
            IXMLDOMDocument3 * This,
             BSTR *urlString);
         HRESULT ( STDMETHODCALLTYPE *get_async )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL *isAsync);
         HRESULT ( STDMETHODCALLTYPE *put_async )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL isAsync);
         HRESULT ( STDMETHODCALLTYPE *abort )( 
            IXMLDOMDocument3 * This);
         HRESULT ( STDMETHODCALLTYPE *loadXML )( 
            IXMLDOMDocument3 * This,
             BSTR bstrXML,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *save )( 
            IXMLDOMDocument3 * This,
             VARIANT destination);
         HRESULT ( STDMETHODCALLTYPE *get_validateOnParse )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL *isValidating);
         HRESULT ( STDMETHODCALLTYPE *put_validateOnParse )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL isValidating);
         HRESULT ( STDMETHODCALLTYPE *get_resolveExternals )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL *isResolving);
         HRESULT ( STDMETHODCALLTYPE *put_resolveExternals )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL isResolving);
         HRESULT ( STDMETHODCALLTYPE *get_preserveWhiteSpace )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL *isPreserving);
         HRESULT ( STDMETHODCALLTYPE *put_preserveWhiteSpace )( 
            IXMLDOMDocument3 * This,
             VARIANT_BOOL isPreserving);
         HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IXMLDOMDocument3 * This,
             VARIANT readystatechangeSink);
         HRESULT ( STDMETHODCALLTYPE *put_ondataavailable )( 
            IXMLDOMDocument3 * This,
             VARIANT ondataavailableSink);
         HRESULT ( STDMETHODCALLTYPE *put_ontransformnode )( 
            IXMLDOMDocument3 * This,
             VARIANT ontransformnodeSink);
         HRESULT ( STDMETHODCALLTYPE *get_namespaces )( 
            IXMLDOMDocument3 * This,
             IXMLDOMSchemaCollection **namespaceCollection);
         HRESULT ( STDMETHODCALLTYPE *get_schemas )( 
            IXMLDOMDocument3 * This,
             VARIANT *otherCollection);
         HRESULT ( STDMETHODCALLTYPE *putref_schemas )( 
            IXMLDOMDocument3 * This,
             VARIANT otherCollection);
         HRESULT ( STDMETHODCALLTYPE *validate )( 
            IXMLDOMDocument3 * This,
             IXMLDOMParseError **errorObj);
         HRESULT ( STDMETHODCALLTYPE *setProperty )( 
            IXMLDOMDocument3 * This,
             BSTR name,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *getProperty )( 
            IXMLDOMDocument3 * This,
             BSTR name,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *validateNode )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *node,
             IXMLDOMParseError **errorObj);
         HRESULT ( STDMETHODCALLTYPE *importNode )( 
            IXMLDOMDocument3 * This,
             IXMLDOMNode *node,
             VARIANT_BOOL deep,
             IXMLDOMNode **clone);
        END_INTERFACE
    } IXMLDOMDocument3Vtbl;
    interface __uuid("2933BF96-7B36-11d2-B20E-00C04F983E60") IXMLDOMDocument3

    {
        CONST_VTBL struct IXMLDOMDocument3Vtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMDocument3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMDocument3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMDocument3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMDocument3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMDocument3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMDocument3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMDocument3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMDocument3_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMDocument3_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMDocument3_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMDocument3_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMDocument3_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMDocument3_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMDocument3_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMDocument3_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMDocument3_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMDocument3_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMDocument3_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMDocument3_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMDocument3_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMDocument3_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMDocument3_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMDocument3_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMDocument3_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMDocument3_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMDocument3_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMDocument3_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMDocument3_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMDocument3_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMDocument3_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMDocument3_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocument3_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocument3_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMDocument3_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMDocument3_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMDocument3_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMDocument3_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMDocument3_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMDocument3_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMDocument3_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMDocument3_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMDocument3_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMDocument3_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMDocument3_get_doctype(This,documentType)	\
    ( (This)->lpVtbl -> get_doctype(This,documentType) ) 
#define IXMLDOMDocument3_get_implementation(This,impl)	\
    ( (This)->lpVtbl -> get_implementation(This,impl) ) 
#define IXMLDOMDocument3_get_documentElement(This,DOMElement)	\
    ( (This)->lpVtbl -> get_documentElement(This,DOMElement) ) 
#define IXMLDOMDocument3_putref_documentElement(This,DOMElement)	\
    ( (This)->lpVtbl -> putref_documentElement(This,DOMElement) ) 
#define IXMLDOMDocument3_createElement(This,tagName,element)	\
    ( (This)->lpVtbl -> createElement(This,tagName,element) ) 
#define IXMLDOMDocument3_createDocumentFragment(This,docFrag)	\
    ( (This)->lpVtbl -> createDocumentFragment(This,docFrag) ) 
#define IXMLDOMDocument3_createTextNode(This,data,text)	\
    ( (This)->lpVtbl -> createTextNode(This,data,text) ) 
#define IXMLDOMDocument3_createComment(This,data,comment)	\
    ( (This)->lpVtbl -> createComment(This,data,comment) ) 
#define IXMLDOMDocument3_createCDATASection(This,data,cdata)	\
    ( (This)->lpVtbl -> createCDATASection(This,data,cdata) ) 
#define IXMLDOMDocument3_createProcessingInstruction(This,target,data,pi)	\
    ( (This)->lpVtbl -> createProcessingInstruction(This,target,data,pi) ) 
#define IXMLDOMDocument3_createAttribute(This,name,attribute)	\
    ( (This)->lpVtbl -> createAttribute(This,name,attribute) ) 
#define IXMLDOMDocument3_createEntityReference(This,name,entityRef)	\
    ( (This)->lpVtbl -> createEntityReference(This,name,entityRef) ) 
#define IXMLDOMDocument3_getElementsByTagName(This,tagName,resultList)	\
    ( (This)->lpVtbl -> getElementsByTagName(This,tagName,resultList) ) 
#define IXMLDOMDocument3_createNode(This,Type,name,namespaceURI,node)	\
    ( (This)->lpVtbl -> createNode(This,Type,name,namespaceURI,node) ) 
#define IXMLDOMDocument3_nodeFromID(This,idString,node)	\
    ( (This)->lpVtbl -> nodeFromID(This,idString,node) ) 
#define IXMLDOMDocument3_load(This,xmlSource,isSuccessful)	\
    ( (This)->lpVtbl -> load(This,xmlSource,isSuccessful) ) 
#define IXMLDOMDocument3_get_readyState(This,value)	\
    ( (This)->lpVtbl -> get_readyState(This,value) ) 
#define IXMLDOMDocument3_get_parseError(This,errorObj)	\
    ( (This)->lpVtbl -> get_parseError(This,errorObj) ) 
#define IXMLDOMDocument3_get_url(This,urlString)	\
    ( (This)->lpVtbl -> get_url(This,urlString) ) 
#define IXMLDOMDocument3_get_async(This,isAsync)	\
    ( (This)->lpVtbl -> get_async(This,isAsync) ) 
#define IXMLDOMDocument3_put_async(This,isAsync)	\
    ( (This)->lpVtbl -> put_async(This,isAsync) ) 
#define IXMLDOMDocument3_abort(This)	\
    ( (This)->lpVtbl -> abort(This) ) 
#define IXMLDOMDocument3_loadXML(This,bstrXML,isSuccessful)	\
    ( (This)->lpVtbl -> loadXML(This,bstrXML,isSuccessful) ) 
#define IXMLDOMDocument3_save(This,destination)	\
    ( (This)->lpVtbl -> save(This,destination) ) 
#define IXMLDOMDocument3_get_validateOnParse(This,isValidating)	\
    ( (This)->lpVtbl -> get_validateOnParse(This,isValidating) ) 
#define IXMLDOMDocument3_put_validateOnParse(This,isValidating)	\
    ( (This)->lpVtbl -> put_validateOnParse(This,isValidating) ) 
#define IXMLDOMDocument3_get_resolveExternals(This,isResolving)	\
    ( (This)->lpVtbl -> get_resolveExternals(This,isResolving) ) 
#define IXMLDOMDocument3_put_resolveExternals(This,isResolving)	\
    ( (This)->lpVtbl -> put_resolveExternals(This,isResolving) ) 
#define IXMLDOMDocument3_get_preserveWhiteSpace(This,isPreserving)	\
    ( (This)->lpVtbl -> get_preserveWhiteSpace(This,isPreserving) ) 
#define IXMLDOMDocument3_put_preserveWhiteSpace(This,isPreserving)	\
    ( (This)->lpVtbl -> put_preserveWhiteSpace(This,isPreserving) ) 
#define IXMLDOMDocument3_put_onreadystatechange(This,readystatechangeSink)	\
    ( (This)->lpVtbl -> put_onreadystatechange(This,readystatechangeSink) ) 
#define IXMLDOMDocument3_put_ondataavailable(This,ondataavailableSink)	\
    ( (This)->lpVtbl -> put_ondataavailable(This,ondataavailableSink) ) 
#define IXMLDOMDocument3_put_ontransformnode(This,ontransformnodeSink)	\
    ( (This)->lpVtbl -> put_ontransformnode(This,ontransformnodeSink) ) 
#define IXMLDOMDocument3_get_namespaces(This,namespaceCollection)	\
    ( (This)->lpVtbl -> get_namespaces(This,namespaceCollection) ) 
#define IXMLDOMDocument3_get_schemas(This,otherCollection)	\
    ( (This)->lpVtbl -> get_schemas(This,otherCollection) ) 
#define IXMLDOMDocument3_putref_schemas(This,otherCollection)	\
    ( (This)->lpVtbl -> putref_schemas(This,otherCollection) ) 
#define IXMLDOMDocument3_validate(This,errorObj)	\
    ( (This)->lpVtbl -> validate(This,errorObj) ) 
#define IXMLDOMDocument3_setProperty(This,name,value)	\
    ( (This)->lpVtbl -> setProperty(This,name,value) ) 
#define IXMLDOMDocument3_getProperty(This,name,value)	\
    ( (This)->lpVtbl -> getProperty(This,name,value) ) 
#define IXMLDOMDocument3_validateNode(This,node,errorObj)	\
    ( (This)->lpVtbl -> validateNode(This,node,errorObj) ) 
#define IXMLDOMDocument3_importNode(This,node,deep,clone)	\
    ( (This)->lpVtbl -> importNode(This,node,deep,clone) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMSchemaCollection;
    typedef struct IXMLDOMSchemaCollectionVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMSchemaCollection * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMSchemaCollection * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMSchemaCollection * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMSchemaCollection * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMSchemaCollection * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMSchemaCollection * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMSchemaCollection * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *add )( 
            IXMLDOMSchemaCollection * This,
             BSTR namespaceURI,
             VARIANT var);
         HRESULT ( STDMETHODCALLTYPE *get )( 
            IXMLDOMSchemaCollection * This,
             BSTR namespaceURI,
             IXMLDOMNode **schemaNode);
         HRESULT ( STDMETHODCALLTYPE *remove )( 
            IXMLDOMSchemaCollection * This,
             BSTR namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMSchemaCollection * This,
             long *length);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMSchemaCollection * This,
             long index,
             BSTR *length);
         HRESULT ( STDMETHODCALLTYPE *addCollection )( 
            IXMLDOMSchemaCollection * This,
             IXMLDOMSchemaCollection *otherCollection);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMSchemaCollection * This,
             IUnknown **ppUnk);
        END_INTERFACE
    } IXMLDOMSchemaCollectionVtbl;
    interface __uuid("373984c8-b845-449b-91e7-45ac83036ade") IXMLDOMSchemaCollection

    {
        CONST_VTBL struct IXMLDOMSchemaCollectionVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMSchemaCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMSchemaCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMSchemaCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMSchemaCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMSchemaCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMSchemaCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMSchemaCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMSchemaCollection_add(This,namespaceURI,var)	\
    ( (This)->lpVtbl -> add(This,namespaceURI,var) ) 
#define IXMLDOMSchemaCollection_get(This,namespaceURI,schemaNode)	\
    ( (This)->lpVtbl -> get(This,namespaceURI,schemaNode) ) 
#define IXMLDOMSchemaCollection_remove(This,namespaceURI)	\
    ( (This)->lpVtbl -> remove(This,namespaceURI) ) 
#define IXMLDOMSchemaCollection_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define IXMLDOMSchemaCollection_get_namespaceURI(This,index,length)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,index,length) ) 
#define IXMLDOMSchemaCollection_addCollection(This,otherCollection)	\
    ( (This)->lpVtbl -> addCollection(This,otherCollection) ) 
#define IXMLDOMSchemaCollection_get__newEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppUnk) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMNodeList;
    typedef struct IXMLDOMNodeListVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNodeList * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNodeList * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNodeList * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNodeList * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNodeList * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNodeList * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNodeList * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_item )( 
            IXMLDOMNodeList * This,
             long index,
             IXMLDOMNode **listItem);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMNodeList * This,
             long *listLength);
         HRESULT ( STDMETHODCALLTYPE *nextNode )( 
            IXMLDOMNodeList * This,
             IXMLDOMNode **nextItem);
         HRESULT ( STDMETHODCALLTYPE *reset )( 
            IXMLDOMNodeList * This);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMNodeList * This,
             IUnknown **ppUnk);
        END_INTERFACE
    } IXMLDOMNodeListVtbl;
    interface __uuid("2933BF82-7B36-11d2-B20E-00C04F983E60") IXMLDOMNodeList

    {
        CONST_VTBL struct IXMLDOMNodeListVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMNodeList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMNodeList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMNodeList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMNodeList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMNodeList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMNodeList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMNodeList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMNodeList_get_item(This,index,listItem)	\
    ( (This)->lpVtbl -> get_item(This,index,listItem) ) 
#define IXMLDOMNodeList_get_length(This,listLength)	\
    ( (This)->lpVtbl -> get_length(This,listLength) ) 
#define IXMLDOMNodeList_nextNode(This,nextItem)	\
    ( (This)->lpVtbl -> nextNode(This,nextItem) ) 
#define IXMLDOMNodeList_reset(This)	\
    ( (This)->lpVtbl -> reset(This) ) 
#define IXMLDOMNodeList_get__newEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppUnk) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMSelection;
    typedef struct IXMLDOMSelectionVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMSelection * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMSelection * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMSelection * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMSelection * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMSelection * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMSelection * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMSelection * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_item )( 
            IXMLDOMSelection * This,
             long index,
             IXMLDOMNode **listItem);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMSelection * This,
             long *listLength);
         HRESULT ( STDMETHODCALLTYPE *nextNode )( 
            IXMLDOMSelection * This,
             IXMLDOMNode **nextItem);
         HRESULT ( STDMETHODCALLTYPE *reset )( 
            IXMLDOMSelection * This);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMSelection * This,
             IUnknown **ppUnk);
         HRESULT ( STDMETHODCALLTYPE *get_expr )( 
            IXMLDOMSelection * This,
             BSTR *expression);
         HRESULT ( STDMETHODCALLTYPE *put_expr )( 
            IXMLDOMSelection * This,
             BSTR expression);
         HRESULT ( STDMETHODCALLTYPE *get_context )( 
            IXMLDOMSelection * This,
             IXMLDOMNode **ppNode);
         HRESULT ( STDMETHODCALLTYPE *putref_context )( 
            IXMLDOMSelection * This,
             IXMLDOMNode *pNode);
         HRESULT ( STDMETHODCALLTYPE *peekNode )( 
            IXMLDOMSelection * This,
             IXMLDOMNode **ppNode);
         HRESULT ( STDMETHODCALLTYPE *matches )( 
            IXMLDOMSelection * This,
             IXMLDOMNode *pNode,
             IXMLDOMNode **ppNode);
         HRESULT ( STDMETHODCALLTYPE *removeNext )( 
            IXMLDOMSelection * This,
             IXMLDOMNode **ppNode);
         HRESULT ( STDMETHODCALLTYPE *removeAll )( 
            IXMLDOMSelection * This);
         HRESULT ( STDMETHODCALLTYPE *clone )( 
            IXMLDOMSelection * This,
             IXMLDOMSelection **ppNode);
         HRESULT ( STDMETHODCALLTYPE *getProperty )( 
            IXMLDOMSelection * This,
             BSTR name,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *setProperty )( 
            IXMLDOMSelection * This,
             BSTR name,
             VARIANT value);
        END_INTERFACE
    } IXMLDOMSelectionVtbl;
    interface __uuid("AA634FC7-5888-44a7-A257-3A47150D3A0E") IXMLDOMSelection

    {
        CONST_VTBL struct IXMLDOMSelectionVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMSelection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMSelection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMSelection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMSelection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMSelection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMSelection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMSelection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMSelection_get_item(This,index,listItem)	\
    ( (This)->lpVtbl -> get_item(This,index,listItem) ) 
#define IXMLDOMSelection_get_length(This,listLength)	\
    ( (This)->lpVtbl -> get_length(This,listLength) ) 
#define IXMLDOMSelection_nextNode(This,nextItem)	\
    ( (This)->lpVtbl -> nextNode(This,nextItem) ) 
#define IXMLDOMSelection_reset(This)	\
    ( (This)->lpVtbl -> reset(This) ) 
#define IXMLDOMSelection_get__newEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppUnk) ) 
#define IXMLDOMSelection_get_expr(This,expression)	\
    ( (This)->lpVtbl -> get_expr(This,expression) ) 
#define IXMLDOMSelection_put_expr(This,expression)	\
    ( (This)->lpVtbl -> put_expr(This,expression) ) 
#define IXMLDOMSelection_get_context(This,ppNode)	\
    ( (This)->lpVtbl -> get_context(This,ppNode) ) 
#define IXMLDOMSelection_putref_context(This,pNode)	\
    ( (This)->lpVtbl -> putref_context(This,pNode) ) 
#define IXMLDOMSelection_peekNode(This,ppNode)	\
    ( (This)->lpVtbl -> peekNode(This,ppNode) ) 
#define IXMLDOMSelection_matches(This,pNode,ppNode)	\
    ( (This)->lpVtbl -> matches(This,pNode,ppNode) ) 
#define IXMLDOMSelection_removeNext(This,ppNode)	\
    ( (This)->lpVtbl -> removeNext(This,ppNode) ) 
#define IXMLDOMSelection_removeAll(This)	\
    ( (This)->lpVtbl -> removeAll(This) ) 
#define IXMLDOMSelection_clone(This,ppNode)	\
    ( (This)->lpVtbl -> clone(This,ppNode) ) 
#define IXMLDOMSelection_getProperty(This,name,value)	\
    ( (This)->lpVtbl -> getProperty(This,name,value) ) 
#define IXMLDOMSelection_setProperty(This,name,value)	\
    ( (This)->lpVtbl -> setProperty(This,name,value) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMNamedNodeMap;
    typedef struct IXMLDOMNamedNodeMapVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNamedNodeMap * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNamedNodeMap * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNamedNodeMap * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNamedNodeMap * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNamedNodeMap * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNamedNodeMap * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNamedNodeMap * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *getNamedItem )( 
            IXMLDOMNamedNodeMap * This,
             BSTR name,
             IXMLDOMNode **namedItem);
         HRESULT ( STDMETHODCALLTYPE *setNamedItem )( 
            IXMLDOMNamedNodeMap * This,
             IXMLDOMNode *newItem,
             IXMLDOMNode **nameItem);
         HRESULT ( STDMETHODCALLTYPE *removeNamedItem )( 
            IXMLDOMNamedNodeMap * This,
             BSTR name,
             IXMLDOMNode **namedItem);
         HRESULT ( STDMETHODCALLTYPE *get_item )( 
            IXMLDOMNamedNodeMap * This,
             long index,
             IXMLDOMNode **listItem);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMNamedNodeMap * This,
             long *listLength);
         HRESULT ( STDMETHODCALLTYPE *getQualifiedItem )( 
            IXMLDOMNamedNodeMap * This,
             BSTR baseName,
             BSTR namespaceURI,
             IXMLDOMNode **qualifiedItem);
         HRESULT ( STDMETHODCALLTYPE *removeQualifiedItem )( 
            IXMLDOMNamedNodeMap * This,
             BSTR baseName,
             BSTR namespaceURI,
             IXMLDOMNode **qualifiedItem);
         HRESULT ( STDMETHODCALLTYPE *nextNode )( 
            IXMLDOMNamedNodeMap * This,
             IXMLDOMNode **nextItem);
         HRESULT ( STDMETHODCALLTYPE *reset )( 
            IXMLDOMNamedNodeMap * This);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMNamedNodeMap * This,
             IUnknown **ppUnk);
        END_INTERFACE
    } IXMLDOMNamedNodeMapVtbl;
    interface __uuid("2933BF83-7B36-11d2-B20E-00C04F983E60") IXMLDOMNamedNodeMap

    {
        CONST_VTBL struct IXMLDOMNamedNodeMapVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMNamedNodeMap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMNamedNodeMap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMNamedNodeMap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMNamedNodeMap_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMNamedNodeMap_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMNamedNodeMap_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMNamedNodeMap_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMNamedNodeMap_getNamedItem(This,name,namedItem)	\
    ( (This)->lpVtbl -> getNamedItem(This,name,namedItem) ) 
#define IXMLDOMNamedNodeMap_setNamedItem(This,newItem,nameItem)	\
    ( (This)->lpVtbl -> setNamedItem(This,newItem,nameItem) ) 
#define IXMLDOMNamedNodeMap_removeNamedItem(This,name,namedItem)	\
    ( (This)->lpVtbl -> removeNamedItem(This,name,namedItem) ) 
#define IXMLDOMNamedNodeMap_get_item(This,index,listItem)	\
    ( (This)->lpVtbl -> get_item(This,index,listItem) ) 
#define IXMLDOMNamedNodeMap_get_length(This,listLength)	\
    ( (This)->lpVtbl -> get_length(This,listLength) ) 
#define IXMLDOMNamedNodeMap_getQualifiedItem(This,baseName,namespaceURI,qualifiedItem)	\
    ( (This)->lpVtbl -> getQualifiedItem(This,baseName,namespaceURI,qualifiedItem) ) 
#define IXMLDOMNamedNodeMap_removeQualifiedItem(This,baseName,namespaceURI,qualifiedItem)	\
    ( (This)->lpVtbl -> removeQualifiedItem(This,baseName,namespaceURI,qualifiedItem) ) 
#define IXMLDOMNamedNodeMap_nextNode(This,nextItem)	\
    ( (This)->lpVtbl -> nextNode(This,nextItem) ) 
#define IXMLDOMNamedNodeMap_reset(This)	\
    ( (This)->lpVtbl -> reset(This) ) 
#define IXMLDOMNamedNodeMap_get__newEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppUnk) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMDocumentFragment;
    typedef struct IXMLDOMDocumentFragmentVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocumentFragment * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocumentFragment * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocumentFragment * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocumentFragment * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocumentFragment * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocumentFragment * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocumentFragment * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocumentFragment * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocumentFragment * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocumentFragment * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocumentFragment * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocumentFragment * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocumentFragment * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocumentFragment * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocumentFragment * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocumentFragment * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocumentFragment * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocumentFragment * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocumentFragment * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocumentFragment * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocumentFragment * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocumentFragment * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocumentFragment * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocumentFragment * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocumentFragment * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocumentFragment * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocumentFragment * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocumentFragment * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocumentFragment * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
        END_INTERFACE
    } IXMLDOMDocumentFragmentVtbl;
    interface __uuid("3efaa413-272f-11d2-836f-0000f87a7782") IXMLDOMDocumentFragment

    {
        CONST_VTBL struct IXMLDOMDocumentFragmentVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMDocumentFragment_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMDocumentFragment_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMDocumentFragment_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMDocumentFragment_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMDocumentFragment_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMDocumentFragment_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMDocumentFragment_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMDocumentFragment_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMDocumentFragment_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMDocumentFragment_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMDocumentFragment_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMDocumentFragment_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMDocumentFragment_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMDocumentFragment_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMDocumentFragment_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMDocumentFragment_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMDocumentFragment_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMDocumentFragment_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMDocumentFragment_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMDocumentFragment_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMDocumentFragment_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMDocumentFragment_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMDocumentFragment_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMDocumentFragment_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMDocumentFragment_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMDocumentFragment_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMDocumentFragment_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMDocumentFragment_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMDocumentFragment_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMDocumentFragment_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMDocumentFragment_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocumentFragment_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocumentFragment_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMDocumentFragment_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMDocumentFragment_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMDocumentFragment_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMDocumentFragment_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMDocumentFragment_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMDocumentFragment_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMDocumentFragment_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMDocumentFragment_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMDocumentFragment_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMDocumentFragment_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMCharacterData;
    typedef struct IXMLDOMCharacterDataVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMCharacterData * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMCharacterData * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMCharacterData * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMCharacterData * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMCharacterData * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMCharacterData * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMCharacterData * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMCharacterData * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMCharacterData * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMCharacterData * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMCharacterData * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMCharacterData * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMCharacterData * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMCharacterData * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMCharacterData * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMCharacterData * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMCharacterData * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMCharacterData * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMCharacterData * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMCharacterData * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMCharacterData * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMCharacterData * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMCharacterData * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMCharacterData * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMCharacterData * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMCharacterData * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMCharacterData * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMCharacterData * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMCharacterData * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMCharacterData * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMCharacterData * This,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMCharacterData * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMCharacterData * This,
             long *dataLength);
         HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMCharacterData * This,
             long offset,
             long count,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMCharacterData * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMCharacterData * This,
             long offset,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMCharacterData * This,
             long offset,
             long count);
         HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMCharacterData * This,
             long offset,
             long count,
             BSTR data);
        END_INTERFACE
    } IXMLDOMCharacterDataVtbl;
    interface __uuid("2933BF84-7B36-11d2-B20E-00C04F983E60") IXMLDOMCharacterData

    {
        CONST_VTBL struct IXMLDOMCharacterDataVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMCharacterData_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMCharacterData_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMCharacterData_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMCharacterData_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMCharacterData_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMCharacterData_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMCharacterData_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMCharacterData_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMCharacterData_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMCharacterData_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMCharacterData_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMCharacterData_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMCharacterData_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMCharacterData_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMCharacterData_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMCharacterData_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMCharacterData_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMCharacterData_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMCharacterData_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMCharacterData_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMCharacterData_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMCharacterData_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMCharacterData_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMCharacterData_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMCharacterData_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMCharacterData_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMCharacterData_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMCharacterData_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMCharacterData_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMCharacterData_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMCharacterData_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMCharacterData_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMCharacterData_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMCharacterData_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMCharacterData_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMCharacterData_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMCharacterData_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMCharacterData_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMCharacterData_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMCharacterData_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMCharacterData_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMCharacterData_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMCharacterData_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMCharacterData_get_data(This,data)	\
    ( (This)->lpVtbl -> get_data(This,data) ) 
#define IXMLDOMCharacterData_put_data(This,data)	\
    ( (This)->lpVtbl -> put_data(This,data) ) 
#define IXMLDOMCharacterData_get_length(This,dataLength)	\
    ( (This)->lpVtbl -> get_length(This,dataLength) ) 
#define IXMLDOMCharacterData_substringData(This,offset,count,data)	\
    ( (This)->lpVtbl -> substringData(This,offset,count,data) ) 
#define IXMLDOMCharacterData_appendData(This,data)	\
    ( (This)->lpVtbl -> appendData(This,data) ) 
#define IXMLDOMCharacterData_insertData(This,offset,data)	\
    ( (This)->lpVtbl -> insertData(This,offset,data) ) 
#define IXMLDOMCharacterData_deleteData(This,offset,count)	\
    ( (This)->lpVtbl -> deleteData(This,offset,count) ) 
#define IXMLDOMCharacterData_replaceData(This,offset,count,data)	\
    ( (This)->lpVtbl -> replaceData(This,offset,count,data) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMAttribute;
    typedef struct IXMLDOMAttributeVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMAttribute * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMAttribute * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMAttribute * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMAttribute * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMAttribute * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMAttribute * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMAttribute * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMAttribute * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMAttribute * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMAttribute * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMAttribute * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMAttribute * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMAttribute * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMAttribute * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMAttribute * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMAttribute * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMAttribute * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMAttribute * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMAttribute * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMAttribute * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMAttribute * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMAttribute * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMAttribute * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMAttribute * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMAttribute * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMAttribute * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMAttribute * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMAttribute * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMAttribute * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMAttribute * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMAttribute * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMAttribute * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            IXMLDOMAttribute * This,
             BSTR *attributeName);
         HRESULT ( STDMETHODCALLTYPE *get_value )( 
            IXMLDOMAttribute * This,
             VARIANT *attributeValue);
         HRESULT ( STDMETHODCALLTYPE *put_value )( 
            IXMLDOMAttribute * This,
             VARIANT attributeValue);
        END_INTERFACE
    } IXMLDOMAttributeVtbl;
    interface __uuid("2933BF85-7B36-11d2-B20E-00C04F983E60") IXMLDOMAttribute

    {
        CONST_VTBL struct IXMLDOMAttributeVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMAttribute_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMAttribute_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMAttribute_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMAttribute_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMAttribute_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMAttribute_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMAttribute_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMAttribute_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMAttribute_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMAttribute_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMAttribute_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMAttribute_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMAttribute_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMAttribute_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMAttribute_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMAttribute_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMAttribute_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMAttribute_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMAttribute_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMAttribute_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMAttribute_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMAttribute_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMAttribute_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMAttribute_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMAttribute_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMAttribute_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMAttribute_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMAttribute_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMAttribute_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMAttribute_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMAttribute_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMAttribute_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMAttribute_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMAttribute_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMAttribute_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMAttribute_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMAttribute_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMAttribute_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMAttribute_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMAttribute_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMAttribute_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMAttribute_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMAttribute_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMAttribute_get_name(This,attributeName)	\
    ( (This)->lpVtbl -> get_name(This,attributeName) ) 
#define IXMLDOMAttribute_get_value(This,attributeValue)	\
    ( (This)->lpVtbl -> get_value(This,attributeValue) ) 
#define IXMLDOMAttribute_put_value(This,attributeValue)	\
    ( (This)->lpVtbl -> put_value(This,attributeValue) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMElement;
    typedef struct IXMLDOMElementVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMElement * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMElement * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMElement * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMElement * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMElement * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMElement * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMElement * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMElement * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMElement * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMElement * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMElement * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMElement * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMElement * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMElement * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMElement * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMElement * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMElement * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMElement * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMElement * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMElement * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMElement * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMElement * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMElement * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMElement * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMElement * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMElement * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMElement * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMElement * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMElement * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMElement * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMElement * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMElement * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMElement * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMElement * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMElement * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMElement * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMElement * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMElement * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMElement * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMElement * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMElement * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMElement * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMElement * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_tagName )( 
            IXMLDOMElement * This,
             BSTR *tagName);
         HRESULT ( STDMETHODCALLTYPE *getAttribute )( 
            IXMLDOMElement * This,
             BSTR name,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *setAttribute )( 
            IXMLDOMElement * This,
             BSTR name,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *removeAttribute )( 
            IXMLDOMElement * This,
             BSTR name);
         HRESULT ( STDMETHODCALLTYPE *getAttributeNode )( 
            IXMLDOMElement * This,
             BSTR name,
             IXMLDOMAttribute **attributeNode);
         HRESULT ( STDMETHODCALLTYPE *setAttributeNode )( 
            IXMLDOMElement * This,
             IXMLDOMAttribute *DOMAttribute,
             IXMLDOMAttribute **attributeNode);
         HRESULT ( STDMETHODCALLTYPE *removeAttributeNode )( 
            IXMLDOMElement * This,
             IXMLDOMAttribute *DOMAttribute,
             IXMLDOMAttribute **attributeNode);
         HRESULT ( STDMETHODCALLTYPE *getElementsByTagName )( 
            IXMLDOMElement * This,
             BSTR tagName,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *normalize )( 
            IXMLDOMElement * This);
        END_INTERFACE
    } IXMLDOMElementVtbl;
    interface __uuid("2933BF86-7B36-11d2-B20E-00C04F983E60") IXMLDOMElement

    {
        CONST_VTBL struct IXMLDOMElementVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMElement_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMElement_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMElement_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMElement_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMElement_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMElement_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMElement_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMElement_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMElement_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMElement_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMElement_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMElement_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMElement_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMElement_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMElement_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMElement_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMElement_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMElement_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMElement_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMElement_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMElement_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMElement_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMElement_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMElement_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMElement_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMElement_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMElement_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMElement_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMElement_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMElement_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMElement_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMElement_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMElement_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMElement_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMElement_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMElement_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMElement_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMElement_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMElement_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMElement_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMElement_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMElement_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMElement_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMElement_get_tagName(This,tagName)	\
    ( (This)->lpVtbl -> get_tagName(This,tagName) ) 
#define IXMLDOMElement_getAttribute(This,name,value)	\
    ( (This)->lpVtbl -> getAttribute(This,name,value) ) 
#define IXMLDOMElement_setAttribute(This,name,value)	\
    ( (This)->lpVtbl -> setAttribute(This,name,value) ) 
#define IXMLDOMElement_removeAttribute(This,name)	\
    ( (This)->lpVtbl -> removeAttribute(This,name) ) 
#define IXMLDOMElement_getAttributeNode(This,name,attributeNode)	\
    ( (This)->lpVtbl -> getAttributeNode(This,name,attributeNode) ) 
#define IXMLDOMElement_setAttributeNode(This,DOMAttribute,attributeNode)	\
    ( (This)->lpVtbl -> setAttributeNode(This,DOMAttribute,attributeNode) ) 
#define IXMLDOMElement_removeAttributeNode(This,DOMAttribute,attributeNode)	\
    ( (This)->lpVtbl -> removeAttributeNode(This,DOMAttribute,attributeNode) ) 
#define IXMLDOMElement_getElementsByTagName(This,tagName,resultList)	\
    ( (This)->lpVtbl -> getElementsByTagName(This,tagName,resultList) ) 
#define IXMLDOMElement_normalize(This)	\
    ( (This)->lpVtbl -> normalize(This) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMText;
    typedef struct IXMLDOMTextVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMText * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMText * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMText * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMText * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMText * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMText * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMText * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMText * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMText * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMText * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMText * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMText * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMText * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMText * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMText * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMText * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMText * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMText * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMText * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMText * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMText * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMText * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMText * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMText * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMText * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMText * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMText * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMText * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMText * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMText * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMText * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMText * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMText * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMText * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMText * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMText * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMText * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMText * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMText * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMText * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMText * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMText * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMText * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMText * This,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMText * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMText * This,
             long *dataLength);
         HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMText * This,
             long offset,
             long count,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMText * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMText * This,
             long offset,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMText * This,
             long offset,
             long count);
         HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMText * This,
             long offset,
             long count,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *splitText )( 
            IXMLDOMText * This,
             long offset,
             IXMLDOMText **rightHandTextNode);
        END_INTERFACE
    } IXMLDOMTextVtbl;
    interface __uuid("2933BF87-7B36-11d2-B20E-00C04F983E60") IXMLDOMText

    {
        CONST_VTBL struct IXMLDOMTextVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMText_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMText_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMText_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMText_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMText_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMText_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMText_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMText_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMText_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMText_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMText_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMText_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMText_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMText_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMText_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMText_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMText_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMText_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMText_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMText_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMText_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMText_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMText_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMText_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMText_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMText_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMText_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMText_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMText_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMText_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMText_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMText_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMText_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMText_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMText_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMText_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMText_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMText_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMText_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMText_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMText_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMText_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMText_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMText_get_data(This,data)	\
    ( (This)->lpVtbl -> get_data(This,data) ) 
#define IXMLDOMText_put_data(This,data)	\
    ( (This)->lpVtbl -> put_data(This,data) ) 
#define IXMLDOMText_get_length(This,dataLength)	\
    ( (This)->lpVtbl -> get_length(This,dataLength) ) 
#define IXMLDOMText_substringData(This,offset,count,data)	\
    ( (This)->lpVtbl -> substringData(This,offset,count,data) ) 
#define IXMLDOMText_appendData(This,data)	\
    ( (This)->lpVtbl -> appendData(This,data) ) 
#define IXMLDOMText_insertData(This,offset,data)	\
    ( (This)->lpVtbl -> insertData(This,offset,data) ) 
#define IXMLDOMText_deleteData(This,offset,count)	\
    ( (This)->lpVtbl -> deleteData(This,offset,count) ) 
#define IXMLDOMText_replaceData(This,offset,count,data)	\
    ( (This)->lpVtbl -> replaceData(This,offset,count,data) ) 
#define IXMLDOMText_splitText(This,offset,rightHandTextNode)	\
    ( (This)->lpVtbl -> splitText(This,offset,rightHandTextNode) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMComment;
    typedef struct IXMLDOMCommentVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMComment * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMComment * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMComment * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMComment * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMComment * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMComment * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMComment * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMComment * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMComment * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMComment * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMComment * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMComment * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMComment * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMComment * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMComment * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMComment * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMComment * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMComment * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMComment * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMComment * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMComment * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMComment * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMComment * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMComment * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMComment * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMComment * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMComment * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMComment * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMComment * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMComment * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMComment * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMComment * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMComment * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMComment * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMComment * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMComment * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMComment * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMComment * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMComment * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMComment * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMComment * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMComment * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMComment * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMComment * This,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMComment * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMComment * This,
             long *dataLength);
         HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMComment * This,
             long offset,
             long count,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMComment * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMComment * This,
             long offset,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMComment * This,
             long offset,
             long count);
         HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMComment * This,
             long offset,
             long count,
             BSTR data);
        END_INTERFACE
    } IXMLDOMCommentVtbl;
    interface __uuid("2933BF88-7B36-11d2-B20E-00C04F983E60") IXMLDOMComment

    {
        CONST_VTBL struct IXMLDOMCommentVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMComment_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMComment_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMComment_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMComment_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMComment_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMComment_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMComment_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMComment_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMComment_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMComment_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMComment_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMComment_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMComment_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMComment_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMComment_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMComment_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMComment_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMComment_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMComment_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMComment_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMComment_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMComment_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMComment_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMComment_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMComment_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMComment_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMComment_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMComment_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMComment_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMComment_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMComment_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMComment_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMComment_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMComment_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMComment_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMComment_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMComment_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMComment_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMComment_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMComment_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMComment_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMComment_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMComment_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMComment_get_data(This,data)	\
    ( (This)->lpVtbl -> get_data(This,data) ) 
#define IXMLDOMComment_put_data(This,data)	\
    ( (This)->lpVtbl -> put_data(This,data) ) 
#define IXMLDOMComment_get_length(This,dataLength)	\
    ( (This)->lpVtbl -> get_length(This,dataLength) ) 
#define IXMLDOMComment_substringData(This,offset,count,data)	\
    ( (This)->lpVtbl -> substringData(This,offset,count,data) ) 
#define IXMLDOMComment_appendData(This,data)	\
    ( (This)->lpVtbl -> appendData(This,data) ) 
#define IXMLDOMComment_insertData(This,offset,data)	\
    ( (This)->lpVtbl -> insertData(This,offset,data) ) 
#define IXMLDOMComment_deleteData(This,offset,count)	\
    ( (This)->lpVtbl -> deleteData(This,offset,count) ) 
#define IXMLDOMComment_replaceData(This,offset,count,data)	\
    ( (This)->lpVtbl -> replaceData(This,offset,count,data) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMProcessingInstruction;
    typedef struct IXMLDOMProcessingInstructionVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMProcessingInstruction * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMProcessingInstruction * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMProcessingInstruction * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMProcessingInstruction * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMProcessingInstruction * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMProcessingInstruction * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMProcessingInstruction * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMProcessingInstruction * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMProcessingInstruction * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMProcessingInstruction * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMProcessingInstruction * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMProcessingInstruction * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMProcessingInstruction * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMProcessingInstruction * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_target )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMProcessingInstruction * This,
             BSTR *value);
         HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMProcessingInstruction * This,
             BSTR value);
        END_INTERFACE
    } IXMLDOMProcessingInstructionVtbl;
    interface __uuid("2933BF89-7B36-11d2-B20E-00C04F983E60") IXMLDOMProcessingInstruction

    {
        CONST_VTBL struct IXMLDOMProcessingInstructionVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMProcessingInstruction_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMProcessingInstruction_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMProcessingInstruction_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMProcessingInstruction_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMProcessingInstruction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMProcessingInstruction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMProcessingInstruction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMProcessingInstruction_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMProcessingInstruction_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMProcessingInstruction_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMProcessingInstruction_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMProcessingInstruction_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMProcessingInstruction_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMProcessingInstruction_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMProcessingInstruction_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMProcessingInstruction_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMProcessingInstruction_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMProcessingInstruction_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMProcessingInstruction_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMProcessingInstruction_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMProcessingInstruction_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMProcessingInstruction_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMProcessingInstruction_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMProcessingInstruction_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMProcessingInstruction_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMProcessingInstruction_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMProcessingInstruction_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMProcessingInstruction_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMProcessingInstruction_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMProcessingInstruction_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMProcessingInstruction_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMProcessingInstruction_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMProcessingInstruction_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMProcessingInstruction_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMProcessingInstruction_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMProcessingInstruction_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMProcessingInstruction_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMProcessingInstruction_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMProcessingInstruction_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMProcessingInstruction_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMProcessingInstruction_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMProcessingInstruction_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMProcessingInstruction_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMProcessingInstruction_get_target(This,name)	\
    ( (This)->lpVtbl -> get_target(This,name) ) 
#define IXMLDOMProcessingInstruction_get_data(This,value)	\
    ( (This)->lpVtbl -> get_data(This,value) ) 
#define IXMLDOMProcessingInstruction_put_data(This,value)	\
    ( (This)->lpVtbl -> put_data(This,value) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMCDATASection;
    typedef struct IXMLDOMCDATASectionVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMCDATASection * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMCDATASection * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMCDATASection * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMCDATASection * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMCDATASection * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMCDATASection * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMCDATASection * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMCDATASection * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMCDATASection * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMCDATASection * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMCDATASection * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMCDATASection * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMCDATASection * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMCDATASection * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMCDATASection * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMCDATASection * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMCDATASection * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMCDATASection * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMCDATASection * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMCDATASection * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMCDATASection * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMCDATASection * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMCDATASection * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMCDATASection * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMCDATASection * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMCDATASection * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMCDATASection * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMCDATASection * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMCDATASection * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMCDATASection * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_data )( 
            IXMLDOMCDATASection * This,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *put_data )( 
            IXMLDOMCDATASection * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMCDATASection * This,
             long *dataLength);
         HRESULT ( STDMETHODCALLTYPE *substringData )( 
            IXMLDOMCDATASection * This,
             long offset,
             long count,
             BSTR *data);
         HRESULT ( STDMETHODCALLTYPE *appendData )( 
            IXMLDOMCDATASection * This,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *insertData )( 
            IXMLDOMCDATASection * This,
             long offset,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *deleteData )( 
            IXMLDOMCDATASection * This,
             long offset,
             long count);
         HRESULT ( STDMETHODCALLTYPE *replaceData )( 
            IXMLDOMCDATASection * This,
             long offset,
             long count,
             BSTR data);
         HRESULT ( STDMETHODCALLTYPE *splitText )( 
            IXMLDOMCDATASection * This,
             long offset,
             IXMLDOMText **rightHandTextNode);
        END_INTERFACE
    } IXMLDOMCDATASectionVtbl;
    interface __uuid("2933BF8A-7B36-11d2-B20E-00C04F983E60") IXMLDOMCDATASection

    {
        CONST_VTBL struct IXMLDOMCDATASectionVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMCDATASection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMCDATASection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMCDATASection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMCDATASection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMCDATASection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMCDATASection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMCDATASection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMCDATASection_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMCDATASection_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMCDATASection_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMCDATASection_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMCDATASection_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMCDATASection_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMCDATASection_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMCDATASection_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMCDATASection_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMCDATASection_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMCDATASection_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMCDATASection_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMCDATASection_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMCDATASection_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMCDATASection_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMCDATASection_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMCDATASection_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMCDATASection_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMCDATASection_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMCDATASection_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMCDATASection_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMCDATASection_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMCDATASection_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMCDATASection_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMCDATASection_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMCDATASection_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMCDATASection_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMCDATASection_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMCDATASection_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMCDATASection_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMCDATASection_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMCDATASection_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMCDATASection_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMCDATASection_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMCDATASection_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMCDATASection_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMCDATASection_get_data(This,data)	\
    ( (This)->lpVtbl -> get_data(This,data) ) 
#define IXMLDOMCDATASection_put_data(This,data)	\
    ( (This)->lpVtbl -> put_data(This,data) ) 
#define IXMLDOMCDATASection_get_length(This,dataLength)	\
    ( (This)->lpVtbl -> get_length(This,dataLength) ) 
#define IXMLDOMCDATASection_substringData(This,offset,count,data)	\
    ( (This)->lpVtbl -> substringData(This,offset,count,data) ) 
#define IXMLDOMCDATASection_appendData(This,data)	\
    ( (This)->lpVtbl -> appendData(This,data) ) 
#define IXMLDOMCDATASection_insertData(This,offset,data)	\
    ( (This)->lpVtbl -> insertData(This,offset,data) ) 
#define IXMLDOMCDATASection_deleteData(This,offset,count)	\
    ( (This)->lpVtbl -> deleteData(This,offset,count) ) 
#define IXMLDOMCDATASection_replaceData(This,offset,count,data)	\
    ( (This)->lpVtbl -> replaceData(This,offset,count,data) ) 
#define IXMLDOMCDATASection_splitText(This,offset,rightHandTextNode)	\
    ( (This)->lpVtbl -> splitText(This,offset,rightHandTextNode) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMDocumentType;
    typedef struct IXMLDOMDocumentTypeVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMDocumentType * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMDocumentType * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMDocumentType * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMDocumentType * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMDocumentType * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMDocumentType * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMDocumentType * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMDocumentType * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMDocumentType * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMDocumentType * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMDocumentType * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMDocumentType * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMDocumentType * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMDocumentType * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMDocumentType * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMDocumentType * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMDocumentType * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMDocumentType * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMDocumentType * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMDocumentType * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMDocumentType * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMDocumentType * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMDocumentType * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMDocumentType * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMDocumentType * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMDocumentType * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMDocumentType * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMDocumentType * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMDocumentType * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            IXMLDOMDocumentType * This,
             BSTR *rootName);
         HRESULT ( STDMETHODCALLTYPE *get_entities )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNamedNodeMap **entityMap);
         HRESULT ( STDMETHODCALLTYPE *get_notations )( 
            IXMLDOMDocumentType * This,
             IXMLDOMNamedNodeMap **notationMap);
        END_INTERFACE
    } IXMLDOMDocumentTypeVtbl;
    interface __uuid("2933BF8B-7B36-11d2-B20E-00C04F983E60") IXMLDOMDocumentType

    {
        CONST_VTBL struct IXMLDOMDocumentTypeVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMDocumentType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMDocumentType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMDocumentType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMDocumentType_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMDocumentType_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMDocumentType_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMDocumentType_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMDocumentType_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMDocumentType_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMDocumentType_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMDocumentType_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMDocumentType_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMDocumentType_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMDocumentType_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMDocumentType_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMDocumentType_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMDocumentType_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMDocumentType_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMDocumentType_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMDocumentType_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMDocumentType_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMDocumentType_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMDocumentType_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMDocumentType_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMDocumentType_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMDocumentType_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMDocumentType_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMDocumentType_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMDocumentType_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMDocumentType_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMDocumentType_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocumentType_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMDocumentType_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMDocumentType_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMDocumentType_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMDocumentType_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMDocumentType_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMDocumentType_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMDocumentType_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMDocumentType_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMDocumentType_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMDocumentType_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMDocumentType_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMDocumentType_get_name(This,rootName)	\
    ( (This)->lpVtbl -> get_name(This,rootName) ) 
#define IXMLDOMDocumentType_get_entities(This,entityMap)	\
    ( (This)->lpVtbl -> get_entities(This,entityMap) ) 
#define IXMLDOMDocumentType_get_notations(This,notationMap)	\
    ( (This)->lpVtbl -> get_notations(This,notationMap) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMNotation;
    typedef struct IXMLDOMNotationVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMNotation * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMNotation * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMNotation * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMNotation * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMNotation * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMNotation * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMNotation * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMNotation * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMNotation * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMNotation * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMNotation * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMNotation * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMNotation * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMNotation * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMNotation * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMNotation * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMNotation * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMNotation * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMNotation * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMNotation * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMNotation * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMNotation * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMNotation * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMNotation * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMNotation * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMNotation * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMNotation * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMNotation * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMNotation * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMNotation * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMNotation * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMNotation * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMNotation * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMNotation * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMNotation * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMNotation * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMNotation * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMNotation * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMNotation * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMNotation * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMNotation * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMNotation * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMNotation * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_publicId )( 
            IXMLDOMNotation * This,
             VARIANT *publicID);
         HRESULT ( STDMETHODCALLTYPE *get_systemId )( 
            IXMLDOMNotation * This,
             VARIANT *systemID);
        END_INTERFACE
    } IXMLDOMNotationVtbl;
    interface __uuid("2933BF8C-7B36-11d2-B20E-00C04F983E60") IXMLDOMNotation

    {
        CONST_VTBL struct IXMLDOMNotationVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMNotation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMNotation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMNotation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMNotation_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMNotation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMNotation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMNotation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMNotation_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMNotation_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMNotation_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMNotation_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMNotation_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMNotation_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMNotation_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMNotation_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMNotation_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMNotation_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMNotation_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMNotation_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMNotation_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMNotation_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMNotation_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMNotation_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMNotation_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMNotation_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMNotation_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMNotation_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMNotation_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMNotation_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMNotation_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMNotation_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMNotation_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMNotation_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMNotation_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMNotation_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMNotation_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMNotation_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMNotation_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMNotation_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMNotation_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMNotation_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMNotation_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMNotation_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMNotation_get_publicId(This,publicID)	\
    ( (This)->lpVtbl -> get_publicId(This,publicID) ) 
#define IXMLDOMNotation_get_systemId(This,systemID)	\
    ( (This)->lpVtbl -> get_systemId(This,systemID) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMEntity;
    typedef struct IXMLDOMEntityVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMEntity * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMEntity * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMEntity * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMEntity * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMEntity * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMEntity * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMEntity * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMEntity * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMEntity * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMEntity * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMEntity * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMEntity * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMEntity * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMEntity * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMEntity * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMEntity * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMEntity * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMEntity * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMEntity * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMEntity * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMEntity * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMEntity * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMEntity * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMEntity * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMEntity * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMEntity * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMEntity * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMEntity * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMEntity * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMEntity * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMEntity * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMEntity * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMEntity * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMEntity * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMEntity * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMEntity * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMEntity * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMEntity * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMEntity * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMEntity * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMEntity * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMEntity * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMEntity * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *get_publicId )( 
            IXMLDOMEntity * This,
             VARIANT *publicID);
         HRESULT ( STDMETHODCALLTYPE *get_systemId )( 
            IXMLDOMEntity * This,
             VARIANT *systemID);
         HRESULT ( STDMETHODCALLTYPE *get_notationName )( 
            IXMLDOMEntity * This,
             BSTR *name);
        END_INTERFACE
    } IXMLDOMEntityVtbl;
    interface __uuid("2933BF8D-7B36-11d2-B20E-00C04F983E60") IXMLDOMEntity

    {
        CONST_VTBL struct IXMLDOMEntityVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMEntity_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMEntity_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMEntity_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMEntity_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMEntity_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMEntity_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMEntity_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMEntity_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMEntity_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMEntity_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMEntity_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMEntity_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMEntity_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMEntity_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMEntity_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMEntity_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMEntity_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMEntity_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMEntity_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMEntity_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMEntity_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMEntity_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMEntity_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMEntity_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMEntity_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMEntity_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMEntity_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMEntity_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMEntity_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMEntity_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMEntity_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMEntity_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMEntity_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMEntity_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMEntity_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMEntity_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMEntity_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMEntity_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMEntity_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMEntity_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMEntity_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMEntity_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMEntity_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXMLDOMEntity_get_publicId(This,publicID)	\
    ( (This)->lpVtbl -> get_publicId(This,publicID) ) 
#define IXMLDOMEntity_get_systemId(This,systemID)	\
    ( (This)->lpVtbl -> get_systemId(This,systemID) ) 
#define IXMLDOMEntity_get_notationName(This,name)	\
    ( (This)->lpVtbl -> get_notationName(This,name) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMEntityReference;
    typedef struct IXMLDOMEntityReferenceVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMEntityReference * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMEntityReference * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMEntityReference * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMEntityReference * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMEntityReference * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMEntityReference * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMEntityReference * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXMLDOMEntityReference * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXMLDOMEntityReference * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXMLDOMEntityReference * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXMLDOMEntityReference * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXMLDOMEntityReference * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXMLDOMEntityReference * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXMLDOMEntityReference * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXMLDOMEntityReference * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXMLDOMEntityReference * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXMLDOMEntityReference * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXMLDOMEntityReference * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXMLDOMEntityReference * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXMLDOMEntityReference * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXMLDOMEntityReference * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXMLDOMEntityReference * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXMLDOMEntityReference * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXMLDOMEntityReference * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXMLDOMEntityReference * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXMLDOMEntityReference * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMEntityReference * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXMLDOMEntityReference * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXMLDOMEntityReference * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXMLDOMEntityReference * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
        END_INTERFACE
    } IXMLDOMEntityReferenceVtbl;
    interface __uuid("2933BF8E-7B36-11d2-B20E-00C04F983E60") IXMLDOMEntityReference

    {
        CONST_VTBL struct IXMLDOMEntityReferenceVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMEntityReference_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMEntityReference_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMEntityReference_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMEntityReference_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMEntityReference_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMEntityReference_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMEntityReference_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMEntityReference_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXMLDOMEntityReference_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXMLDOMEntityReference_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXMLDOMEntityReference_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXMLDOMEntityReference_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXMLDOMEntityReference_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXMLDOMEntityReference_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXMLDOMEntityReference_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXMLDOMEntityReference_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXMLDOMEntityReference_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXMLDOMEntityReference_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXMLDOMEntityReference_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXMLDOMEntityReference_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXMLDOMEntityReference_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXMLDOMEntityReference_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXMLDOMEntityReference_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXMLDOMEntityReference_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXMLDOMEntityReference_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXMLDOMEntityReference_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXMLDOMEntityReference_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXMLDOMEntityReference_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXMLDOMEntityReference_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXMLDOMEntityReference_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXMLDOMEntityReference_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMEntityReference_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXMLDOMEntityReference_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXMLDOMEntityReference_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXMLDOMEntityReference_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXMLDOMEntityReference_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXMLDOMEntityReference_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXMLDOMEntityReference_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXMLDOMEntityReference_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXMLDOMEntityReference_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXMLDOMEntityReference_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXMLDOMEntityReference_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXMLDOMEntityReference_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMImplementation;
    typedef struct IXMLDOMImplementationVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMImplementation * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMImplementation * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMImplementation * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMImplementation * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMImplementation * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMImplementation * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMImplementation * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *hasFeature )( 
            IXMLDOMImplementation * This,
             BSTR feature,
             BSTR version,
             VARIANT_BOOL *hasFeature);
        END_INTERFACE
    } IXMLDOMImplementationVtbl;
    interface __uuid("2933BF8F-7B36-11d2-B20E-00C04F983E60") IXMLDOMImplementation

    {
        CONST_VTBL struct IXMLDOMImplementationVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMImplementation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMImplementation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMImplementation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMImplementation_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMImplementation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMImplementation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMImplementation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMImplementation_hasFeature(This,feature,version,hasFeature)	\
    ( (This)->lpVtbl -> hasFeature(This,feature,version,hasFeature) ) 
#endif 
EXTERN_C const IID IID_IXTLRuntime;
    typedef struct IXTLRuntimeVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXTLRuntime * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXTLRuntime * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXTLRuntime * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXTLRuntime * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXTLRuntime * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXTLRuntime * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXTLRuntime * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_nodeName )( 
            IXTLRuntime * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_nodeValue )( 
            IXTLRuntime * This,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *put_nodeValue )( 
            IXTLRuntime * This,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *get_nodeType )( 
            IXTLRuntime * This,
             DOMNodeType *type);
         HRESULT ( STDMETHODCALLTYPE *get_parentNode )( 
            IXTLRuntime * This,
             IXMLDOMNode **parent);
         HRESULT ( STDMETHODCALLTYPE *get_childNodes )( 
            IXTLRuntime * This,
             IXMLDOMNodeList **childList);
         HRESULT ( STDMETHODCALLTYPE *get_firstChild )( 
            IXTLRuntime * This,
             IXMLDOMNode **firstChild);
         HRESULT ( STDMETHODCALLTYPE *get_lastChild )( 
            IXTLRuntime * This,
             IXMLDOMNode **lastChild);
         HRESULT ( STDMETHODCALLTYPE *get_previousSibling )( 
            IXTLRuntime * This,
             IXMLDOMNode **previousSibling);
         HRESULT ( STDMETHODCALLTYPE *get_nextSibling )( 
            IXTLRuntime * This,
             IXMLDOMNode **nextSibling);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            IXTLRuntime * This,
             IXMLDOMNamedNodeMap **attributeMap);
         HRESULT ( STDMETHODCALLTYPE *insertBefore )( 
            IXTLRuntime * This,
             IXMLDOMNode *newChild,
             VARIANT refChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *replaceChild )( 
            IXTLRuntime * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode *oldChild,
             IXMLDOMNode **outOldChild);
         HRESULT ( STDMETHODCALLTYPE *removeChild )( 
            IXTLRuntime * This,
             IXMLDOMNode *childNode,
             IXMLDOMNode **oldChild);
         HRESULT ( STDMETHODCALLTYPE *appendChild )( 
            IXTLRuntime * This,
             IXMLDOMNode *newChild,
             IXMLDOMNode **outNewChild);
         HRESULT ( STDMETHODCALLTYPE *hasChildNodes )( 
            IXTLRuntime * This,
             VARIANT_BOOL *hasChild);
         HRESULT ( STDMETHODCALLTYPE *get_ownerDocument )( 
            IXTLRuntime * This,
             IXMLDOMDocument **XMLDOMDocument);
         HRESULT ( STDMETHODCALLTYPE *cloneNode )( 
            IXTLRuntime * This,
             VARIANT_BOOL deep,
             IXMLDOMNode **cloneRoot);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypeString )( 
            IXTLRuntime * This,
             BSTR *nodeType);
         HRESULT ( STDMETHODCALLTYPE *get_text )( 
            IXTLRuntime * This,
             BSTR *text);
         HRESULT ( STDMETHODCALLTYPE *put_text )( 
            IXTLRuntime * This,
             BSTR text);
         HRESULT ( STDMETHODCALLTYPE *get_specified )( 
            IXTLRuntime * This,
             VARIANT_BOOL *isSpecified);
         HRESULT ( STDMETHODCALLTYPE *get_definition )( 
            IXTLRuntime * This,
             IXMLDOMNode **definitionNode);
         HRESULT ( STDMETHODCALLTYPE *get_nodeTypedValue )( 
            IXTLRuntime * This,
             VARIANT *typedValue);
         HRESULT ( STDMETHODCALLTYPE *put_nodeTypedValue )( 
            IXTLRuntime * This,
             VARIANT typedValue);
         HRESULT ( STDMETHODCALLTYPE *get_dataType )( 
            IXTLRuntime * This,
             VARIANT *dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *put_dataType )( 
            IXTLRuntime * This,
             BSTR dataTypeName);
         HRESULT ( STDMETHODCALLTYPE *get_xml )( 
            IXTLRuntime * This,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *transformNode )( 
            IXTLRuntime * This,
             IXMLDOMNode *stylesheet,
             BSTR *xmlString);
         HRESULT ( STDMETHODCALLTYPE *selectNodes )( 
            IXTLRuntime * This,
             BSTR queryString,
             IXMLDOMNodeList **resultList);
         HRESULT ( STDMETHODCALLTYPE *selectSingleNode )( 
            IXTLRuntime * This,
             BSTR queryString,
             IXMLDOMNode **resultNode);
         HRESULT ( STDMETHODCALLTYPE *get_parsed )( 
            IXTLRuntime * This,
             VARIANT_BOOL *isParsed);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXTLRuntime * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_prefix )( 
            IXTLRuntime * This,
             BSTR *prefixString);
         HRESULT ( STDMETHODCALLTYPE *get_baseName )( 
            IXTLRuntime * This,
             BSTR *nameString);
         HRESULT ( STDMETHODCALLTYPE *transformNodeToObject )( 
            IXTLRuntime * This,
             IXMLDOMNode *stylesheet,
             VARIANT outputObject);
         HRESULT ( STDMETHODCALLTYPE *uniqueID )( 
            IXTLRuntime * This,
             IXMLDOMNode *pNode,
             long *pID);
         HRESULT ( STDMETHODCALLTYPE *depth )( 
            IXTLRuntime * This,
             IXMLDOMNode *pNode,
             long *pDepth);
         HRESULT ( STDMETHODCALLTYPE *childNumber )( 
            IXTLRuntime * This,
             IXMLDOMNode *pNode,
             long *pNumber);
         HRESULT ( STDMETHODCALLTYPE *ancestorChildNumber )( 
            IXTLRuntime * This,
             BSTR bstrNodeName,
             IXMLDOMNode *pNode,
             long *pNumber);
         HRESULT ( STDMETHODCALLTYPE *absoluteChildNumber )( 
            IXTLRuntime * This,
             IXMLDOMNode *pNode,
             long *pNumber);
         HRESULT ( STDMETHODCALLTYPE *formatIndex )( 
            IXTLRuntime * This,
             long lIndex,
             BSTR bstrFormat,
             BSTR *pbstrFormattedString);
         HRESULT ( STDMETHODCALLTYPE *formatNumber )( 
            IXTLRuntime * This,
             double dblNumber,
             BSTR bstrFormat,
             BSTR *pbstrFormattedString);
         HRESULT ( STDMETHODCALLTYPE *formatDate )( 
            IXTLRuntime * This,
             VARIANT varDate,
             BSTR bstrFormat,
             VARIANT varDestLocale,
             BSTR *pbstrFormattedString);
         HRESULT ( STDMETHODCALLTYPE *formatTime )( 
            IXTLRuntime * This,
             VARIANT varTime,
             BSTR bstrFormat,
             VARIANT varDestLocale,
             BSTR *pbstrFormattedString);
        END_INTERFACE
    } IXTLRuntimeVtbl;
    interface __uuid("3efaa425-272f-11d2-836f-0000f87a7782") IXTLRuntime

    {
        CONST_VTBL struct IXTLRuntimeVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXTLRuntime_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXTLRuntime_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXTLRuntime_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXTLRuntime_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXTLRuntime_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXTLRuntime_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXTLRuntime_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXTLRuntime_get_nodeName(This,name)	\
    ( (This)->lpVtbl -> get_nodeName(This,name) ) 
#define IXTLRuntime_get_nodeValue(This,value)	\
    ( (This)->lpVtbl -> get_nodeValue(This,value) ) 
#define IXTLRuntime_put_nodeValue(This,value)	\
    ( (This)->lpVtbl -> put_nodeValue(This,value) ) 
#define IXTLRuntime_get_nodeType(This,type)	\
    ( (This)->lpVtbl -> get_nodeType(This,type) ) 
#define IXTLRuntime_get_parentNode(This,parent)	\
    ( (This)->lpVtbl -> get_parentNode(This,parent) ) 
#define IXTLRuntime_get_childNodes(This,childList)	\
    ( (This)->lpVtbl -> get_childNodes(This,childList) ) 
#define IXTLRuntime_get_firstChild(This,firstChild)	\
    ( (This)->lpVtbl -> get_firstChild(This,firstChild) ) 
#define IXTLRuntime_get_lastChild(This,lastChild)	\
    ( (This)->lpVtbl -> get_lastChild(This,lastChild) ) 
#define IXTLRuntime_get_previousSibling(This,previousSibling)	\
    ( (This)->lpVtbl -> get_previousSibling(This,previousSibling) ) 
#define IXTLRuntime_get_nextSibling(This,nextSibling)	\
    ( (This)->lpVtbl -> get_nextSibling(This,nextSibling) ) 
#define IXTLRuntime_get_attributes(This,attributeMap)	\
    ( (This)->lpVtbl -> get_attributes(This,attributeMap) ) 
#define IXTLRuntime_insertBefore(This,newChild,refChild,outNewChild)	\
    ( (This)->lpVtbl -> insertBefore(This,newChild,refChild,outNewChild) ) 
#define IXTLRuntime_replaceChild(This,newChild,oldChild,outOldChild)	\
    ( (This)->lpVtbl -> replaceChild(This,newChild,oldChild,outOldChild) ) 
#define IXTLRuntime_removeChild(This,childNode,oldChild)	\
    ( (This)->lpVtbl -> removeChild(This,childNode,oldChild) ) 
#define IXTLRuntime_appendChild(This,newChild,outNewChild)	\
    ( (This)->lpVtbl -> appendChild(This,newChild,outNewChild) ) 
#define IXTLRuntime_hasChildNodes(This,hasChild)	\
    ( (This)->lpVtbl -> hasChildNodes(This,hasChild) ) 
#define IXTLRuntime_get_ownerDocument(This,XMLDOMDocument)	\
    ( (This)->lpVtbl -> get_ownerDocument(This,XMLDOMDocument) ) 
#define IXTLRuntime_cloneNode(This,deep,cloneRoot)	\
    ( (This)->lpVtbl -> cloneNode(This,deep,cloneRoot) ) 
#define IXTLRuntime_get_nodeTypeString(This,nodeType)	\
    ( (This)->lpVtbl -> get_nodeTypeString(This,nodeType) ) 
#define IXTLRuntime_get_text(This,text)	\
    ( (This)->lpVtbl -> get_text(This,text) ) 
#define IXTLRuntime_put_text(This,text)	\
    ( (This)->lpVtbl -> put_text(This,text) ) 
#define IXTLRuntime_get_specified(This,isSpecified)	\
    ( (This)->lpVtbl -> get_specified(This,isSpecified) ) 
#define IXTLRuntime_get_definition(This,definitionNode)	\
    ( (This)->lpVtbl -> get_definition(This,definitionNode) ) 
#define IXTLRuntime_get_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> get_nodeTypedValue(This,typedValue) ) 
#define IXTLRuntime_put_nodeTypedValue(This,typedValue)	\
    ( (This)->lpVtbl -> put_nodeTypedValue(This,typedValue) ) 
#define IXTLRuntime_get_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> get_dataType(This,dataTypeName) ) 
#define IXTLRuntime_put_dataType(This,dataTypeName)	\
    ( (This)->lpVtbl -> put_dataType(This,dataTypeName) ) 
#define IXTLRuntime_get_xml(This,xmlString)	\
    ( (This)->lpVtbl -> get_xml(This,xmlString) ) 
#define IXTLRuntime_transformNode(This,stylesheet,xmlString)	\
    ( (This)->lpVtbl -> transformNode(This,stylesheet,xmlString) ) 
#define IXTLRuntime_selectNodes(This,queryString,resultList)	\
    ( (This)->lpVtbl -> selectNodes(This,queryString,resultList) ) 
#define IXTLRuntime_selectSingleNode(This,queryString,resultNode)	\
    ( (This)->lpVtbl -> selectSingleNode(This,queryString,resultNode) ) 
#define IXTLRuntime_get_parsed(This,isParsed)	\
    ( (This)->lpVtbl -> get_parsed(This,isParsed) ) 
#define IXTLRuntime_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define IXTLRuntime_get_prefix(This,prefixString)	\
    ( (This)->lpVtbl -> get_prefix(This,prefixString) ) 
#define IXTLRuntime_get_baseName(This,nameString)	\
    ( (This)->lpVtbl -> get_baseName(This,nameString) ) 
#define IXTLRuntime_transformNodeToObject(This,stylesheet,outputObject)	\
    ( (This)->lpVtbl -> transformNodeToObject(This,stylesheet,outputObject) ) 
#define IXTLRuntime_uniqueID(This,pNode,pID)	\
    ( (This)->lpVtbl -> uniqueID(This,pNode,pID) ) 
#define IXTLRuntime_depth(This,pNode,pDepth)	\
    ( (This)->lpVtbl -> depth(This,pNode,pDepth) ) 
#define IXTLRuntime_childNumber(This,pNode,pNumber)	\
    ( (This)->lpVtbl -> childNumber(This,pNode,pNumber) ) 
#define IXTLRuntime_ancestorChildNumber(This,bstrNodeName,pNode,pNumber)	\
    ( (This)->lpVtbl -> ancestorChildNumber(This,bstrNodeName,pNode,pNumber) ) 
#define IXTLRuntime_absoluteChildNumber(This,pNode,pNumber)	\
    ( (This)->lpVtbl -> absoluteChildNumber(This,pNode,pNumber) ) 
#define IXTLRuntime_formatIndex(This,lIndex,bstrFormat,pbstrFormattedString)	\
    ( (This)->lpVtbl -> formatIndex(This,lIndex,bstrFormat,pbstrFormattedString) ) 
#define IXTLRuntime_formatNumber(This,dblNumber,bstrFormat,pbstrFormattedString)	\
    ( (This)->lpVtbl -> formatNumber(This,dblNumber,bstrFormat,pbstrFormattedString) ) 
#define IXTLRuntime_formatDate(This,varDate,bstrFormat,varDestLocale,pbstrFormattedString)	\
    ( (This)->lpVtbl -> formatDate(This,varDate,bstrFormat,varDestLocale,pbstrFormattedString) ) 
#define IXTLRuntime_formatTime(This,varTime,bstrFormat,varDestLocale,pbstrFormattedString)	\
    ( (This)->lpVtbl -> formatTime(This,varTime,bstrFormat,varDestLocale,pbstrFormattedString) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMParseError;
    typedef struct IXMLDOMParseErrorVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMParseError * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMParseError * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMParseError * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMParseError * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMParseError * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMParseError * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMParseError * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_errorCode )( 
            IXMLDOMParseError * This,
             long *errorCode);
         HRESULT ( STDMETHODCALLTYPE *get_url )( 
            IXMLDOMParseError * This,
             BSTR *urlString);
         HRESULT ( STDMETHODCALLTYPE *get_reason )( 
            IXMLDOMParseError * This,
             BSTR *reasonString);
         HRESULT ( STDMETHODCALLTYPE *get_srcText )( 
            IXMLDOMParseError * This,
             BSTR *sourceString);
         HRESULT ( STDMETHODCALLTYPE *get_line )( 
            IXMLDOMParseError * This,
             long *lineNumber);
         HRESULT ( STDMETHODCALLTYPE *get_linepos )( 
            IXMLDOMParseError * This,
             long *linePosition);
         HRESULT ( STDMETHODCALLTYPE *get_filepos )( 
            IXMLDOMParseError * This,
             long *filePosition);
        END_INTERFACE
    } IXMLDOMParseErrorVtbl;
    interface __uuid("3efaa426-272f-11d2-836f-0000f87a7782") IXMLDOMParseError

    {
        CONST_VTBL struct IXMLDOMParseErrorVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMParseError_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMParseError_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMParseError_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMParseError_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMParseError_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMParseError_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMParseError_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMParseError_get_errorCode(This,errorCode)	\
    ( (This)->lpVtbl -> get_errorCode(This,errorCode) ) 
#define IXMLDOMParseError_get_url(This,urlString)	\
    ( (This)->lpVtbl -> get_url(This,urlString) ) 
#define IXMLDOMParseError_get_reason(This,reasonString)	\
    ( (This)->lpVtbl -> get_reason(This,reasonString) ) 
#define IXMLDOMParseError_get_srcText(This,sourceString)	\
    ( (This)->lpVtbl -> get_srcText(This,sourceString) ) 
#define IXMLDOMParseError_get_line(This,lineNumber)	\
    ( (This)->lpVtbl -> get_line(This,lineNumber) ) 
#define IXMLDOMParseError_get_linepos(This,linePosition)	\
    ( (This)->lpVtbl -> get_linepos(This,linePosition) ) 
#define IXMLDOMParseError_get_filepos(This,filePosition)	\
    ( (This)->lpVtbl -> get_filepos(This,filePosition) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMParseError2;
    typedef struct IXMLDOMParseError2Vtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMParseError2 * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMParseError2 * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMParseError2 * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMParseError2 * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMParseError2 * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMParseError2 * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMParseError2 * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_errorCode )( 
            IXMLDOMParseError2 * This,
             long *errorCode);
         HRESULT ( STDMETHODCALLTYPE *get_url )( 
            IXMLDOMParseError2 * This,
             BSTR *urlString);
         HRESULT ( STDMETHODCALLTYPE *get_reason )( 
            IXMLDOMParseError2 * This,
             BSTR *reasonString);
         HRESULT ( STDMETHODCALLTYPE *get_srcText )( 
            IXMLDOMParseError2 * This,
             BSTR *sourceString);
         HRESULT ( STDMETHODCALLTYPE *get_line )( 
            IXMLDOMParseError2 * This,
             long *lineNumber);
         HRESULT ( STDMETHODCALLTYPE *get_linepos )( 
            IXMLDOMParseError2 * This,
             long *linePosition);
         HRESULT ( STDMETHODCALLTYPE *get_filepos )( 
            IXMLDOMParseError2 * This,
             long *filePosition);
         HRESULT ( STDMETHODCALLTYPE *get_errorXPath )( 
            IXMLDOMParseError2 * This,
             BSTR *xpathexpr);
         HRESULT ( STDMETHODCALLTYPE *get_allErrors )( 
            IXMLDOMParseError2 * This,
             IXMLDOMParseErrorCollection **allErrors);
         HRESULT ( STDMETHODCALLTYPE *errorParameters )( 
            IXMLDOMParseError2 * This,
             long index,
             BSTR *param);
         HRESULT ( STDMETHODCALLTYPE *get_errorParametersCount )( 
            IXMLDOMParseError2 * This,
             long *count);
        END_INTERFACE
    } IXMLDOMParseError2Vtbl;
    interface __uuid("3efaa428-272f-11d2-836f-0000f87a7782") IXMLDOMParseError2

    {
        CONST_VTBL struct IXMLDOMParseError2Vtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMParseError2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMParseError2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMParseError2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMParseError2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMParseError2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMParseError2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMParseError2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMParseError2_get_errorCode(This,errorCode)	\
    ( (This)->lpVtbl -> get_errorCode(This,errorCode) ) 
#define IXMLDOMParseError2_get_url(This,urlString)	\
    ( (This)->lpVtbl -> get_url(This,urlString) ) 
#define IXMLDOMParseError2_get_reason(This,reasonString)	\
    ( (This)->lpVtbl -> get_reason(This,reasonString) ) 
#define IXMLDOMParseError2_get_srcText(This,sourceString)	\
    ( (This)->lpVtbl -> get_srcText(This,sourceString) ) 
#define IXMLDOMParseError2_get_line(This,lineNumber)	\
    ( (This)->lpVtbl -> get_line(This,lineNumber) ) 
#define IXMLDOMParseError2_get_linepos(This,linePosition)	\
    ( (This)->lpVtbl -> get_linepos(This,linePosition) ) 
#define IXMLDOMParseError2_get_filepos(This,filePosition)	\
    ( (This)->lpVtbl -> get_filepos(This,filePosition) ) 
#define IXMLDOMParseError2_get_errorXPath(This,xpathexpr)	\
    ( (This)->lpVtbl -> get_errorXPath(This,xpathexpr) ) 
#define IXMLDOMParseError2_get_allErrors(This,allErrors)	\
    ( (This)->lpVtbl -> get_allErrors(This,allErrors) ) 
#define IXMLDOMParseError2_errorParameters(This,index,param)	\
    ( (This)->lpVtbl -> errorParameters(This,index,param) ) 
#define IXMLDOMParseError2_get_errorParametersCount(This,count)	\
    ( (This)->lpVtbl -> get_errorParametersCount(This,count) ) 
#endif 
EXTERN_C const IID IID_IXMLDOMParseErrorCollection;
    typedef struct IXMLDOMParseErrorCollectionVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMParseErrorCollection * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMParseErrorCollection * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMParseErrorCollection * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMParseErrorCollection * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMParseErrorCollection * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMParseErrorCollection * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMParseErrorCollection * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_item )( 
            IXMLDOMParseErrorCollection * This,
             long index,
             IXMLDOMParseError2 **error);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMParseErrorCollection * This,
             long *length);
         HRESULT ( STDMETHODCALLTYPE *get_next )( 
            IXMLDOMParseErrorCollection * This,
             IXMLDOMParseError2 **error);
         HRESULT ( STDMETHODCALLTYPE *reset )( 
            IXMLDOMParseErrorCollection * This);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMParseErrorCollection * This,
             IUnknown **ppunk);
        END_INTERFACE
    } IXMLDOMParseErrorCollectionVtbl;
    interface __uuid("3efaa429-272f-11d2-836f-0000f87a7782") IXMLDOMParseErrorCollection

    {
        CONST_VTBL struct IXMLDOMParseErrorCollectionVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMParseErrorCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMParseErrorCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMParseErrorCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMParseErrorCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMParseErrorCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMParseErrorCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMParseErrorCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMParseErrorCollection_get_item(This,index,error)	\
    ( (This)->lpVtbl -> get_item(This,index,error) ) 
#define IXMLDOMParseErrorCollection_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define IXMLDOMParseErrorCollection_get_next(This,error)	\
    ( (This)->lpVtbl -> get_next(This,error) ) 
#define IXMLDOMParseErrorCollection_reset(This)	\
    ( (This)->lpVtbl -> reset(This) ) 
#define IXMLDOMParseErrorCollection_get__newEnum(This,ppunk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppunk) ) 
#endif 
EXTERN_C const IID IID_IXSLProcessor;
    typedef struct IXSLProcessorVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXSLProcessor * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXSLProcessor * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXSLProcessor * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXSLProcessor * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXSLProcessor * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXSLProcessor * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXSLProcessor * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *put_input )( 
            IXSLProcessor * This,
             VARIANT var);
         HRESULT ( STDMETHODCALLTYPE *get_input )( 
            IXSLProcessor * This,
             VARIANT *pVar);
         HRESULT ( STDMETHODCALLTYPE *get_ownerTemplate )( 
            IXSLProcessor * This,
             IXSLTemplate **ppTemplate);
         HRESULT ( STDMETHODCALLTYPE *setStartMode )( 
            IXSLProcessor * This,
             BSTR mode,
             BSTR namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_startMode )( 
            IXSLProcessor * This,
             BSTR *mode);
         HRESULT ( STDMETHODCALLTYPE *get_startModeURI )( 
            IXSLProcessor * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *put_output )( 
            IXSLProcessor * This,
             VARIANT output);
         HRESULT ( STDMETHODCALLTYPE *get_output )( 
            IXSLProcessor * This,
             VARIANT *pOutput);
         HRESULT ( STDMETHODCALLTYPE *transform )( 
            IXSLProcessor * This,
             VARIANT_BOOL *pDone);
         HRESULT ( STDMETHODCALLTYPE *reset )( 
            IXSLProcessor * This);
         HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXSLProcessor * This,
             long *pReadyState);
         HRESULT ( STDMETHODCALLTYPE *addParameter )( 
            IXSLProcessor * This,
             BSTR baseName,
             VARIANT parameter,
             BSTR namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *addObject )( 
            IXSLProcessor * This,
             IDispatch *obj,
             BSTR namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_stylesheet )( 
            IXSLProcessor * This,
             IXMLDOMNode **stylesheet);
        END_INTERFACE
    } IXSLProcessorVtbl;
    interface __uuid("2933BF92-7B36-11d2-B20E-00C04F983E60") IXSLProcessor

    {
        CONST_VTBL struct IXSLProcessorVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXSLProcessor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXSLProcessor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXSLProcessor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXSLProcessor_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXSLProcessor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXSLProcessor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXSLProcessor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXSLProcessor_put_input(This,var)	\
    ( (This)->lpVtbl -> put_input(This,var) ) 
#define IXSLProcessor_get_input(This,pVar)	\
    ( (This)->lpVtbl -> get_input(This,pVar) ) 
#define IXSLProcessor_get_ownerTemplate(This,ppTemplate)	\
    ( (This)->lpVtbl -> get_ownerTemplate(This,ppTemplate) ) 
#define IXSLProcessor_setStartMode(This,mode,namespaceURI)	\
    ( (This)->lpVtbl -> setStartMode(This,mode,namespaceURI) ) 
#define IXSLProcessor_get_startMode(This,mode)	\
    ( (This)->lpVtbl -> get_startMode(This,mode) ) 
#define IXSLProcessor_get_startModeURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_startModeURI(This,namespaceURI) ) 
#define IXSLProcessor_put_output(This,output)	\
    ( (This)->lpVtbl -> put_output(This,output) ) 
#define IXSLProcessor_get_output(This,pOutput)	\
    ( (This)->lpVtbl -> get_output(This,pOutput) ) 
#define IXSLProcessor_transform(This,pDone)	\
    ( (This)->lpVtbl -> transform(This,pDone) ) 
#define IXSLProcessor_reset(This)	\
    ( (This)->lpVtbl -> reset(This) ) 
#define IXSLProcessor_get_readyState(This,pReadyState)	\
    ( (This)->lpVtbl -> get_readyState(This,pReadyState) ) 
#define IXSLProcessor_addParameter(This,baseName,parameter,namespaceURI)	\
    ( (This)->lpVtbl -> addParameter(This,baseName,parameter,namespaceURI) ) 
#define IXSLProcessor_addObject(This,obj,namespaceURI)	\
    ( (This)->lpVtbl -> addObject(This,obj,namespaceURI) ) 
#define IXSLProcessor_get_stylesheet(This,stylesheet)	\
    ( (This)->lpVtbl -> get_stylesheet(This,stylesheet) ) 
#endif 
EXTERN_C const IID IID_IXSLTemplate;
    typedef struct IXSLTemplateVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXSLTemplate * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXSLTemplate * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXSLTemplate * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXSLTemplate * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXSLTemplate * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXSLTemplate * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXSLTemplate * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *putref_stylesheet )( 
            IXSLTemplate * This,
             IXMLDOMNode *stylesheet);
         HRESULT ( STDMETHODCALLTYPE *get_stylesheet )( 
            IXSLTemplate * This,
             IXMLDOMNode **stylesheet);
         HRESULT ( STDMETHODCALLTYPE *createProcessor )( 
            IXSLTemplate * This,
             IXSLProcessor **ppProcessor);
        END_INTERFACE
    } IXSLTemplateVtbl;
    interface __uuid("2933BF93-7B36-11d2-B20E-00C04F983E60") IXSLTemplate

    {
        CONST_VTBL struct IXSLTemplateVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXSLTemplate_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXSLTemplate_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXSLTemplate_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXSLTemplate_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXSLTemplate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXSLTemplate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXSLTemplate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXSLTemplate_putref_stylesheet(This,stylesheet)	\
    ( (This)->lpVtbl -> putref_stylesheet(This,stylesheet) ) 
#define IXSLTemplate_get_stylesheet(This,stylesheet)	\
    ( (This)->lpVtbl -> get_stylesheet(This,stylesheet) ) 
#define IXSLTemplate_createProcessor(This,ppProcessor)	\
    ( (This)->lpVtbl -> createProcessor(This,ppProcessor) ) 
#endif 
EXTERN_C const IID IID_IXMLHTTPRequest;
    typedef struct IXMLHTTPRequestVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLHTTPRequest * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLHTTPRequest * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLHTTPRequest * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLHTTPRequest * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLHTTPRequest * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLHTTPRequest * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLHTTPRequest * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *open )( 
            IXMLHTTPRequest * This,
             BSTR bstrMethod,
             BSTR bstrUrl,
             VARIANT varAsync,
             VARIANT bstrUser,
             VARIANT bstrPassword);
         HRESULT ( STDMETHODCALLTYPE *setRequestHeader )( 
            IXMLHTTPRequest * This,
             BSTR bstrHeader,
             BSTR bstrValue);
         HRESULT ( STDMETHODCALLTYPE *getResponseHeader )( 
            IXMLHTTPRequest * This,
             BSTR bstrHeader,
             BSTR *pbstrValue);
         HRESULT ( STDMETHODCALLTYPE *getAllResponseHeaders )( 
            IXMLHTTPRequest * This,
             BSTR *pbstrHeaders);
         HRESULT ( STDMETHODCALLTYPE *send )( 
            IXMLHTTPRequest * This,
             VARIANT varBody);
         HRESULT ( STDMETHODCALLTYPE *abort )( 
            IXMLHTTPRequest * This);
         HRESULT ( STDMETHODCALLTYPE *get_status )( 
            IXMLHTTPRequest * This,
             long *plStatus);
         HRESULT ( STDMETHODCALLTYPE *get_statusText )( 
            IXMLHTTPRequest * This,
             BSTR *pbstrStatus);
         HRESULT ( STDMETHODCALLTYPE *get_responseXML )( 
            IXMLHTTPRequest * This,
             IDispatch **ppBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseText )( 
            IXMLHTTPRequest * This,
             BSTR *pbstrBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseBody )( 
            IXMLHTTPRequest * This,
             VARIANT *pvarBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseStream )( 
            IXMLHTTPRequest * This,
             VARIANT *pvarBody);
         HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IXMLHTTPRequest * This,
             long *plState);
         HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IXMLHTTPRequest * This,
             IDispatch *pReadyStateSink);
        END_INTERFACE
    } IXMLHTTPRequestVtbl;
    interface __uuid("ED8C108D-4349-11D2-91A4-00C04F7969E8") IXMLHTTPRequest

    {
        CONST_VTBL struct IXMLHTTPRequestVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLHTTPRequest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLHTTPRequest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLHTTPRequest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLHTTPRequest_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLHTTPRequest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLHTTPRequest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLHTTPRequest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLHTTPRequest_open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword)	\
    ( (This)->lpVtbl -> open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword) ) 
#define IXMLHTTPRequest_setRequestHeader(This,bstrHeader,bstrValue)	\
    ( (This)->lpVtbl -> setRequestHeader(This,bstrHeader,bstrValue) ) 
#define IXMLHTTPRequest_getResponseHeader(This,bstrHeader,pbstrValue)	\
    ( (This)->lpVtbl -> getResponseHeader(This,bstrHeader,pbstrValue) ) 
#define IXMLHTTPRequest_getAllResponseHeaders(This,pbstrHeaders)	\
    ( (This)->lpVtbl -> getAllResponseHeaders(This,pbstrHeaders) ) 
#define IXMLHTTPRequest_send(This,varBody)	\
    ( (This)->lpVtbl -> send(This,varBody) ) 
#define IXMLHTTPRequest_abort(This)	\
    ( (This)->lpVtbl -> abort(This) ) 
#define IXMLHTTPRequest_get_status(This,plStatus)	\
    ( (This)->lpVtbl -> get_status(This,plStatus) ) 
#define IXMLHTTPRequest_get_statusText(This,pbstrStatus)	\
    ( (This)->lpVtbl -> get_statusText(This,pbstrStatus) ) 
#define IXMLHTTPRequest_get_responseXML(This,ppBody)	\
    ( (This)->lpVtbl -> get_responseXML(This,ppBody) ) 
#define IXMLHTTPRequest_get_responseText(This,pbstrBody)	\
    ( (This)->lpVtbl -> get_responseText(This,pbstrBody) ) 
#define IXMLHTTPRequest_get_responseBody(This,pvarBody)	\
    ( (This)->lpVtbl -> get_responseBody(This,pvarBody) ) 
#define IXMLHTTPRequest_get_responseStream(This,pvarBody)	\
    ( (This)->lpVtbl -> get_responseStream(This,pvarBody) ) 
#define IXMLHTTPRequest_get_readyState(This,plState)	\
    ( (This)->lpVtbl -> get_readyState(This,plState) ) 
#define IXMLHTTPRequest_put_onreadystatechange(This,pReadyStateSink)	\
    ( (This)->lpVtbl -> put_onreadystatechange(This,pReadyStateSink) ) 
#endif 
#if !defined(__msxml_sxh_enums__)
#define __msxml_sxh_enums__
typedef  
enum _SERVERXMLHTTP_OPTION
    {
        SXH_OPTION_URL	= -1,
        SXH_OPTION_URL_CODEPAGE	= ( SXH_OPTION_URL + 1 ) ,
        SXH_OPTION_ESCAPE_PERCENT_IN_URL	= ( SXH_OPTION_URL_CODEPAGE + 1 ) ,
        SXH_OPTION_IGNORE_SERVER_SSL_CERT_ERROR_FLAGS	= ( SXH_OPTION_ESCAPE_PERCENT_IN_URL + 1 ) ,
        SXH_OPTION_SELECT_CLIENT_SSL_CERT	= ( SXH_OPTION_IGNORE_SERVER_SSL_CERT_ERROR_FLAGS + 1 ) 
    } 	SERVERXMLHTTP_OPTION;
typedef  
enum _SXH_SERVER_CERT_OPTION
    {
        SXH_SERVER_CERT_IGNORE_UNKNOWN_CA	= 0x100,
        SXH_SERVER_CERT_IGNORE_WRONG_USAGE	= 0x200,
        SXH_SERVER_CERT_IGNORE_CERT_CN_INVALID	= 0x1000,
        SXH_SERVER_CERT_IGNORE_CERT_DATE_INVALID	= 0x2000,
        SXH_SERVER_CERT_IGNORE_ALL_SERVER_ERRORS	= ( ( ( SXH_SERVER_CERT_IGNORE_UNKNOWN_CA | SXH_SERVER_CERT_IGNORE_WRONG_USAGE )  | SXH_SERVER_CERT_IGNORE_CERT_CN_INVALID )  | SXH_SERVER_CERT_IGNORE_CERT_DATE_INVALID ) 
    } 	SXH_SERVER_CERT_OPTION;
typedef  
enum _SXH_PROXY_SETTING
    {
        SXH_PROXY_SET_DEFAULT	= 0,
        SXH_PROXY_SET_PRECONFIG	= 0,
        SXH_PROXY_SET_DIRECT	= 0x1,
        SXH_PROXY_SET_PROXY	= 0x2
    } 	SXH_PROXY_SETTING;
#endif 
EXTERN_C const IID IID_IServerXMLHTTPRequest;
    typedef struct IServerXMLHTTPRequestVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IServerXMLHTTPRequest * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IServerXMLHTTPRequest * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IServerXMLHTTPRequest * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IServerXMLHTTPRequest * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IServerXMLHTTPRequest * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IServerXMLHTTPRequest * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IServerXMLHTTPRequest * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *open )( 
            IServerXMLHTTPRequest * This,
             BSTR bstrMethod,
             BSTR bstrUrl,
             VARIANT varAsync,
             VARIANT bstrUser,
             VARIANT bstrPassword);
         HRESULT ( STDMETHODCALLTYPE *setRequestHeader )( 
            IServerXMLHTTPRequest * This,
             BSTR bstrHeader,
             BSTR bstrValue);
         HRESULT ( STDMETHODCALLTYPE *getResponseHeader )( 
            IServerXMLHTTPRequest * This,
             BSTR bstrHeader,
             BSTR *pbstrValue);
         HRESULT ( STDMETHODCALLTYPE *getAllResponseHeaders )( 
            IServerXMLHTTPRequest * This,
             BSTR *pbstrHeaders);
         HRESULT ( STDMETHODCALLTYPE *send )( 
            IServerXMLHTTPRequest * This,
             VARIANT varBody);
         HRESULT ( STDMETHODCALLTYPE *abort )( 
            IServerXMLHTTPRequest * This);
         HRESULT ( STDMETHODCALLTYPE *get_status )( 
            IServerXMLHTTPRequest * This,
             long *plStatus);
         HRESULT ( STDMETHODCALLTYPE *get_statusText )( 
            IServerXMLHTTPRequest * This,
             BSTR *pbstrStatus);
         HRESULT ( STDMETHODCALLTYPE *get_responseXML )( 
            IServerXMLHTTPRequest * This,
             IDispatch **ppBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseText )( 
            IServerXMLHTTPRequest * This,
             BSTR *pbstrBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseBody )( 
            IServerXMLHTTPRequest * This,
             VARIANT *pvarBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseStream )( 
            IServerXMLHTTPRequest * This,
             VARIANT *pvarBody);
         HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IServerXMLHTTPRequest * This,
             long *plState);
         HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IServerXMLHTTPRequest * This,
             IDispatch *pReadyStateSink);
         HRESULT ( STDMETHODCALLTYPE *setTimeouts )( 
            IServerXMLHTTPRequest * This,
             long resolveTimeout,
             long connectTimeout,
             long sendTimeout,
             long receiveTimeout);
         HRESULT ( STDMETHODCALLTYPE *waitForResponse )( 
            IServerXMLHTTPRequest * This,
             VARIANT timeoutInSeconds,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *getOption )( 
            IServerXMLHTTPRequest * This,
             SERVERXMLHTTP_OPTION option,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *setOption )( 
            IServerXMLHTTPRequest * This,
             SERVERXMLHTTP_OPTION option,
             VARIANT value);
        END_INTERFACE
    } IServerXMLHTTPRequestVtbl;
    interface __uuid("2e9196bf-13ba-4dd4-91ca-6c571f281495") IServerXMLHTTPRequest

    {
        CONST_VTBL struct IServerXMLHTTPRequestVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IServerXMLHTTPRequest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IServerXMLHTTPRequest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IServerXMLHTTPRequest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IServerXMLHTTPRequest_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IServerXMLHTTPRequest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IServerXMLHTTPRequest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IServerXMLHTTPRequest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IServerXMLHTTPRequest_open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword)	\
    ( (This)->lpVtbl -> open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword) ) 
#define IServerXMLHTTPRequest_setRequestHeader(This,bstrHeader,bstrValue)	\
    ( (This)->lpVtbl -> setRequestHeader(This,bstrHeader,bstrValue) ) 
#define IServerXMLHTTPRequest_getResponseHeader(This,bstrHeader,pbstrValue)	\
    ( (This)->lpVtbl -> getResponseHeader(This,bstrHeader,pbstrValue) ) 
#define IServerXMLHTTPRequest_getAllResponseHeaders(This,pbstrHeaders)	\
    ( (This)->lpVtbl -> getAllResponseHeaders(This,pbstrHeaders) ) 
#define IServerXMLHTTPRequest_send(This,varBody)	\
    ( (This)->lpVtbl -> send(This,varBody) ) 
#define IServerXMLHTTPRequest_abort(This)	\
    ( (This)->lpVtbl -> abort(This) ) 
#define IServerXMLHTTPRequest_get_status(This,plStatus)	\
    ( (This)->lpVtbl -> get_status(This,plStatus) ) 
#define IServerXMLHTTPRequest_get_statusText(This,pbstrStatus)	\
    ( (This)->lpVtbl -> get_statusText(This,pbstrStatus) ) 
#define IServerXMLHTTPRequest_get_responseXML(This,ppBody)	\
    ( (This)->lpVtbl -> get_responseXML(This,ppBody) ) 
#define IServerXMLHTTPRequest_get_responseText(This,pbstrBody)	\
    ( (This)->lpVtbl -> get_responseText(This,pbstrBody) ) 
#define IServerXMLHTTPRequest_get_responseBody(This,pvarBody)	\
    ( (This)->lpVtbl -> get_responseBody(This,pvarBody) ) 
#define IServerXMLHTTPRequest_get_responseStream(This,pvarBody)	\
    ( (This)->lpVtbl -> get_responseStream(This,pvarBody) ) 
#define IServerXMLHTTPRequest_get_readyState(This,plState)	\
    ( (This)->lpVtbl -> get_readyState(This,plState) ) 
#define IServerXMLHTTPRequest_put_onreadystatechange(This,pReadyStateSink)	\
    ( (This)->lpVtbl -> put_onreadystatechange(This,pReadyStateSink) ) 
#define IServerXMLHTTPRequest_setTimeouts(This,resolveTimeout,connectTimeout,sendTimeout,receiveTimeout)	\
    ( (This)->lpVtbl -> setTimeouts(This,resolveTimeout,connectTimeout,sendTimeout,receiveTimeout) ) 
#define IServerXMLHTTPRequest_waitForResponse(This,timeoutInSeconds,isSuccessful)	\
    ( (This)->lpVtbl -> waitForResponse(This,timeoutInSeconds,isSuccessful) ) 
#define IServerXMLHTTPRequest_getOption(This,option,value)	\
    ( (This)->lpVtbl -> getOption(This,option,value) ) 
#define IServerXMLHTTPRequest_setOption(This,option,value)	\
    ( (This)->lpVtbl -> setOption(This,option,value) ) 
#endif 
EXTERN_C const IID IID_IServerXMLHTTPRequest2;
    typedef struct IServerXMLHTTPRequest2Vtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IServerXMLHTTPRequest2 * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IServerXMLHTTPRequest2 * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IServerXMLHTTPRequest2 * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IServerXMLHTTPRequest2 * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IServerXMLHTTPRequest2 * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IServerXMLHTTPRequest2 * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IServerXMLHTTPRequest2 * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *open )( 
            IServerXMLHTTPRequest2 * This,
             BSTR bstrMethod,
             BSTR bstrUrl,
             VARIANT varAsync,
             VARIANT bstrUser,
             VARIANT bstrPassword);
         HRESULT ( STDMETHODCALLTYPE *setRequestHeader )( 
            IServerXMLHTTPRequest2 * This,
             BSTR bstrHeader,
             BSTR bstrValue);
         HRESULT ( STDMETHODCALLTYPE *getResponseHeader )( 
            IServerXMLHTTPRequest2 * This,
             BSTR bstrHeader,
             BSTR *pbstrValue);
         HRESULT ( STDMETHODCALLTYPE *getAllResponseHeaders )( 
            IServerXMLHTTPRequest2 * This,
             BSTR *pbstrHeaders);
         HRESULT ( STDMETHODCALLTYPE *send )( 
            IServerXMLHTTPRequest2 * This,
             VARIANT varBody);
         HRESULT ( STDMETHODCALLTYPE *abort )( 
            IServerXMLHTTPRequest2 * This);
         HRESULT ( STDMETHODCALLTYPE *get_status )( 
            IServerXMLHTTPRequest2 * This,
             long *plStatus);
         HRESULT ( STDMETHODCALLTYPE *get_statusText )( 
            IServerXMLHTTPRequest2 * This,
             BSTR *pbstrStatus);
         HRESULT ( STDMETHODCALLTYPE *get_responseXML )( 
            IServerXMLHTTPRequest2 * This,
             IDispatch **ppBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseText )( 
            IServerXMLHTTPRequest2 * This,
             BSTR *pbstrBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseBody )( 
            IServerXMLHTTPRequest2 * This,
             VARIANT *pvarBody);
         HRESULT ( STDMETHODCALLTYPE *get_responseStream )( 
            IServerXMLHTTPRequest2 * This,
             VARIANT *pvarBody);
         HRESULT ( STDMETHODCALLTYPE *get_readyState )( 
            IServerXMLHTTPRequest2 * This,
             long *plState);
         HRESULT ( STDMETHODCALLTYPE *put_onreadystatechange )( 
            IServerXMLHTTPRequest2 * This,
             IDispatch *pReadyStateSink);
         HRESULT ( STDMETHODCALLTYPE *setTimeouts )( 
            IServerXMLHTTPRequest2 * This,
             long resolveTimeout,
             long connectTimeout,
             long sendTimeout,
             long receiveTimeout);
         HRESULT ( STDMETHODCALLTYPE *waitForResponse )( 
            IServerXMLHTTPRequest2 * This,
             VARIANT timeoutInSeconds,
             VARIANT_BOOL *isSuccessful);
         HRESULT ( STDMETHODCALLTYPE *getOption )( 
            IServerXMLHTTPRequest2 * This,
             SERVERXMLHTTP_OPTION option,
             VARIANT *value);
         HRESULT ( STDMETHODCALLTYPE *setOption )( 
            IServerXMLHTTPRequest2 * This,
             SERVERXMLHTTP_OPTION option,
             VARIANT value);
         HRESULT ( STDMETHODCALLTYPE *setProxy )( 
            IServerXMLHTTPRequest2 * This,
             SXH_PROXY_SETTING proxySetting,
             VARIANT varProxyServer,
             VARIANT varBypassList);
         HRESULT ( STDMETHODCALLTYPE *setProxyCredentials )( 
            IServerXMLHTTPRequest2 * This,
             BSTR bstrUserName,
             BSTR bstrPassword);
        END_INTERFACE
    } IServerXMLHTTPRequest2Vtbl;
    interface __uuid("2e01311b-c322-4b0a-bd77-b90cfdc8dce7") IServerXMLHTTPRequest2

    {
        CONST_VTBL struct IServerXMLHTTPRequest2Vtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IServerXMLHTTPRequest2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IServerXMLHTTPRequest2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IServerXMLHTTPRequest2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IServerXMLHTTPRequest2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IServerXMLHTTPRequest2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IServerXMLHTTPRequest2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IServerXMLHTTPRequest2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IServerXMLHTTPRequest2_open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword)	\
    ( (This)->lpVtbl -> open(This,bstrMethod,bstrUrl,varAsync,bstrUser,bstrPassword) ) 
#define IServerXMLHTTPRequest2_setRequestHeader(This,bstrHeader,bstrValue)	\
    ( (This)->lpVtbl -> setRequestHeader(This,bstrHeader,bstrValue) ) 
#define IServerXMLHTTPRequest2_getResponseHeader(This,bstrHeader,pbstrValue)	\
    ( (This)->lpVtbl -> getResponseHeader(This,bstrHeader,pbstrValue) ) 
#define IServerXMLHTTPRequest2_getAllResponseHeaders(This,pbstrHeaders)	\
    ( (This)->lpVtbl -> getAllResponseHeaders(This,pbstrHeaders) ) 
#define IServerXMLHTTPRequest2_send(This,varBody)	\
    ( (This)->lpVtbl -> send(This,varBody) ) 
#define IServerXMLHTTPRequest2_abort(This)	\
    ( (This)->lpVtbl -> abort(This) ) 
#define IServerXMLHTTPRequest2_get_status(This,plStatus)	\
    ( (This)->lpVtbl -> get_status(This,plStatus) ) 
#define IServerXMLHTTPRequest2_get_statusText(This,pbstrStatus)	\
    ( (This)->lpVtbl -> get_statusText(This,pbstrStatus) ) 
#define IServerXMLHTTPRequest2_get_responseXML(This,ppBody)	\
    ( (This)->lpVtbl -> get_responseXML(This,ppBody) ) 
#define IServerXMLHTTPRequest2_get_responseText(This,pbstrBody)	\
    ( (This)->lpVtbl -> get_responseText(This,pbstrBody) ) 
#define IServerXMLHTTPRequest2_get_responseBody(This,pvarBody)	\
    ( (This)->lpVtbl -> get_responseBody(This,pvarBody) ) 
#define IServerXMLHTTPRequest2_get_responseStream(This,pvarBody)	\
    ( (This)->lpVtbl -> get_responseStream(This,pvarBody) ) 
#define IServerXMLHTTPRequest2_get_readyState(This,plState)	\
    ( (This)->lpVtbl -> get_readyState(This,plState) ) 
#define IServerXMLHTTPRequest2_put_onreadystatechange(This,pReadyStateSink)	\
    ( (This)->lpVtbl -> put_onreadystatechange(This,pReadyStateSink) ) 
#define IServerXMLHTTPRequest2_setTimeouts(This,resolveTimeout,connectTimeout,sendTimeout,receiveTimeout)	\
    ( (This)->lpVtbl -> setTimeouts(This,resolveTimeout,connectTimeout,sendTimeout,receiveTimeout) ) 
#define IServerXMLHTTPRequest2_waitForResponse(This,timeoutInSeconds,isSuccessful)	\
    ( (This)->lpVtbl -> waitForResponse(This,timeoutInSeconds,isSuccessful) ) 
#define IServerXMLHTTPRequest2_getOption(This,option,value)	\
    ( (This)->lpVtbl -> getOption(This,option,value) ) 
#define IServerXMLHTTPRequest2_setOption(This,option,value)	\
    ( (This)->lpVtbl -> setOption(This,option,value) ) 
#define IServerXMLHTTPRequest2_setProxy(This,proxySetting,varProxyServer,varBypassList)	\
    ( (This)->lpVtbl -> setProxy(This,proxySetting,varProxyServer,varBypassList) ) 
#define IServerXMLHTTPRequest2_setProxyCredentials(This,bstrUserName,bstrPassword)	\
    ( (This)->lpVtbl -> setProxyCredentials(This,bstrUserName,bstrPassword) ) 
#endif 
EXTERN_C const IID IID_ISAXXMLReader;
    typedef struct ISAXXMLReaderVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXXMLReader * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXXMLReader * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXXMLReader * This);
        HRESULT ( STDMETHODCALLTYPE *getFeature )( 
            ISAXXMLReader * This,
             const wchar_t *pwchName,
             VARIANT_BOOL *pvfValue);
        HRESULT ( STDMETHODCALLTYPE *putFeature )( 
            ISAXXMLReader * This,
             const wchar_t *pwchName,
             VARIANT_BOOL vfValue);
        HRESULT ( STDMETHODCALLTYPE *getProperty )( 
            ISAXXMLReader * This,
             const wchar_t *pwchName,
             VARIANT *pvarValue);
        HRESULT ( STDMETHODCALLTYPE *putProperty )( 
            ISAXXMLReader * This,
             const wchar_t *pwchName,
             VARIANT varValue);
        HRESULT ( STDMETHODCALLTYPE *getEntityResolver )( 
            ISAXXMLReader * This,
             ISAXEntityResolver **ppResolver);
        HRESULT ( STDMETHODCALLTYPE *putEntityResolver )( 
            ISAXXMLReader * This,
             ISAXEntityResolver *pResolver);
        HRESULT ( STDMETHODCALLTYPE *getContentHandler )( 
            ISAXXMLReader * This,
             ISAXContentHandler **ppHandler);
        HRESULT ( STDMETHODCALLTYPE *putContentHandler )( 
            ISAXXMLReader * This,
             ISAXContentHandler *pHandler);
        HRESULT ( STDMETHODCALLTYPE *getDTDHandler )( 
            ISAXXMLReader * This,
             ISAXDTDHandler **ppHandler);
        HRESULT ( STDMETHODCALLTYPE *putDTDHandler )( 
            ISAXXMLReader * This,
             ISAXDTDHandler *pHandler);
        HRESULT ( STDMETHODCALLTYPE *getErrorHandler )( 
            ISAXXMLReader * This,
             ISAXErrorHandler **ppHandler);
        HRESULT ( STDMETHODCALLTYPE *putErrorHandler )( 
            ISAXXMLReader * This,
             ISAXErrorHandler *pHandler);
        HRESULT ( STDMETHODCALLTYPE *getBaseURL )( 
            ISAXXMLReader * This,
             const wchar_t **ppwchBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *putBaseURL )( 
            ISAXXMLReader * This,
             const wchar_t *pwchBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *getSecureBaseURL )( 
            ISAXXMLReader * This,
             const wchar_t **ppwchSecureBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *putSecureBaseURL )( 
            ISAXXMLReader * This,
             const wchar_t *pwchSecureBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *parse )( 
            ISAXXMLReader * This,
             VARIANT varInput);
        HRESULT ( STDMETHODCALLTYPE *parseURL )( 
            ISAXXMLReader * This,
             const wchar_t *pwchUrl);
        END_INTERFACE
    } ISAXXMLReaderVtbl;
    interface __uuid("a4f96ed0-f829-476e-81c0-cdc7bd2a0802") ISAXXMLReader

    {
        CONST_VTBL struct ISAXXMLReaderVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXXMLReader_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXXMLReader_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXXMLReader_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXXMLReader_getFeature(This,pwchName,pvfValue)	\
    ( (This)->lpVtbl -> getFeature(This,pwchName,pvfValue) ) 
#define ISAXXMLReader_putFeature(This,pwchName,vfValue)	\
    ( (This)->lpVtbl -> putFeature(This,pwchName,vfValue) ) 
#define ISAXXMLReader_getProperty(This,pwchName,pvarValue)	\
    ( (This)->lpVtbl -> getProperty(This,pwchName,pvarValue) ) 
#define ISAXXMLReader_putProperty(This,pwchName,varValue)	\
    ( (This)->lpVtbl -> putProperty(This,pwchName,varValue) ) 
#define ISAXXMLReader_getEntityResolver(This,ppResolver)	\
    ( (This)->lpVtbl -> getEntityResolver(This,ppResolver) ) 
#define ISAXXMLReader_putEntityResolver(This,pResolver)	\
    ( (This)->lpVtbl -> putEntityResolver(This,pResolver) ) 
#define ISAXXMLReader_getContentHandler(This,ppHandler)	\
    ( (This)->lpVtbl -> getContentHandler(This,ppHandler) ) 
#define ISAXXMLReader_putContentHandler(This,pHandler)	\
    ( (This)->lpVtbl -> putContentHandler(This,pHandler) ) 
#define ISAXXMLReader_getDTDHandler(This,ppHandler)	\
    ( (This)->lpVtbl -> getDTDHandler(This,ppHandler) ) 
#define ISAXXMLReader_putDTDHandler(This,pHandler)	\
    ( (This)->lpVtbl -> putDTDHandler(This,pHandler) ) 
#define ISAXXMLReader_getErrorHandler(This,ppHandler)	\
    ( (This)->lpVtbl -> getErrorHandler(This,ppHandler) ) 
#define ISAXXMLReader_putErrorHandler(This,pHandler)	\
    ( (This)->lpVtbl -> putErrorHandler(This,pHandler) ) 
#define ISAXXMLReader_getBaseURL(This,ppwchBaseUrl)	\
    ( (This)->lpVtbl -> getBaseURL(This,ppwchBaseUrl) ) 
#define ISAXXMLReader_putBaseURL(This,pwchBaseUrl)	\
    ( (This)->lpVtbl -> putBaseURL(This,pwchBaseUrl) ) 
#define ISAXXMLReader_getSecureBaseURL(This,ppwchSecureBaseUrl)	\
    ( (This)->lpVtbl -> getSecureBaseURL(This,ppwchSecureBaseUrl) ) 
#define ISAXXMLReader_putSecureBaseURL(This,pwchSecureBaseUrl)	\
    ( (This)->lpVtbl -> putSecureBaseURL(This,pwchSecureBaseUrl) ) 
#define ISAXXMLReader_parse(This,varInput)	\
    ( (This)->lpVtbl -> parse(This,varInput) ) 
#define ISAXXMLReader_parseURL(This,pwchUrl)	\
    ( (This)->lpVtbl -> parseURL(This,pwchUrl) ) 
#endif 
EXTERN_C const IID IID_ISAXXMLFilter;
    typedef struct ISAXXMLFilterVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXXMLFilter * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXXMLFilter * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXXMLFilter * This);
        HRESULT ( STDMETHODCALLTYPE *getFeature )( 
            ISAXXMLFilter * This,
             const wchar_t *pwchName,
             VARIANT_BOOL *pvfValue);
        HRESULT ( STDMETHODCALLTYPE *putFeature )( 
            ISAXXMLFilter * This,
             const wchar_t *pwchName,
             VARIANT_BOOL vfValue);
        HRESULT ( STDMETHODCALLTYPE *getProperty )( 
            ISAXXMLFilter * This,
             const wchar_t *pwchName,
             VARIANT *pvarValue);
        HRESULT ( STDMETHODCALLTYPE *putProperty )( 
            ISAXXMLFilter * This,
             const wchar_t *pwchName,
             VARIANT varValue);
        HRESULT ( STDMETHODCALLTYPE *getEntityResolver )( 
            ISAXXMLFilter * This,
             ISAXEntityResolver **ppResolver);
        HRESULT ( STDMETHODCALLTYPE *putEntityResolver )( 
            ISAXXMLFilter * This,
             ISAXEntityResolver *pResolver);
        HRESULT ( STDMETHODCALLTYPE *getContentHandler )( 
            ISAXXMLFilter * This,
             ISAXContentHandler **ppHandler);
        HRESULT ( STDMETHODCALLTYPE *putContentHandler )( 
            ISAXXMLFilter * This,
             ISAXContentHandler *pHandler);
        HRESULT ( STDMETHODCALLTYPE *getDTDHandler )( 
            ISAXXMLFilter * This,
             ISAXDTDHandler **ppHandler);
        HRESULT ( STDMETHODCALLTYPE *putDTDHandler )( 
            ISAXXMLFilter * This,
             ISAXDTDHandler *pHandler);
        HRESULT ( STDMETHODCALLTYPE *getErrorHandler )( 
            ISAXXMLFilter * This,
             ISAXErrorHandler **ppHandler);
        HRESULT ( STDMETHODCALLTYPE *putErrorHandler )( 
            ISAXXMLFilter * This,
             ISAXErrorHandler *pHandler);
        HRESULT ( STDMETHODCALLTYPE *getBaseURL )( 
            ISAXXMLFilter * This,
             const wchar_t **ppwchBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *putBaseURL )( 
            ISAXXMLFilter * This,
             const wchar_t *pwchBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *getSecureBaseURL )( 
            ISAXXMLFilter * This,
             const wchar_t **ppwchSecureBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *putSecureBaseURL )( 
            ISAXXMLFilter * This,
             const wchar_t *pwchSecureBaseUrl);
        HRESULT ( STDMETHODCALLTYPE *parse )( 
            ISAXXMLFilter * This,
             VARIANT varInput);
        HRESULT ( STDMETHODCALLTYPE *parseURL )( 
            ISAXXMLFilter * This,
             const wchar_t *pwchUrl);
        HRESULT ( STDMETHODCALLTYPE *getParent )( 
            ISAXXMLFilter * This,
             ISAXXMLReader **ppReader);
        HRESULT ( STDMETHODCALLTYPE *putParent )( 
            ISAXXMLFilter * This,
             ISAXXMLReader *pReader);
        END_INTERFACE
    } ISAXXMLFilterVtbl;
    interface __uuid("70409222-ca09-4475-acb8-40312fe8d145") ISAXXMLFilter

    {
        CONST_VTBL struct ISAXXMLFilterVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXXMLFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXXMLFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXXMLFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXXMLFilter_getFeature(This,pwchName,pvfValue)	\
    ( (This)->lpVtbl -> getFeature(This,pwchName,pvfValue) ) 
#define ISAXXMLFilter_putFeature(This,pwchName,vfValue)	\
    ( (This)->lpVtbl -> putFeature(This,pwchName,vfValue) ) 
#define ISAXXMLFilter_getProperty(This,pwchName,pvarValue)	\
    ( (This)->lpVtbl -> getProperty(This,pwchName,pvarValue) ) 
#define ISAXXMLFilter_putProperty(This,pwchName,varValue)	\
    ( (This)->lpVtbl -> putProperty(This,pwchName,varValue) ) 
#define ISAXXMLFilter_getEntityResolver(This,ppResolver)	\
    ( (This)->lpVtbl -> getEntityResolver(This,ppResolver) ) 
#define ISAXXMLFilter_putEntityResolver(This,pResolver)	\
    ( (This)->lpVtbl -> putEntityResolver(This,pResolver) ) 
#define ISAXXMLFilter_getContentHandler(This,ppHandler)	\
    ( (This)->lpVtbl -> getContentHandler(This,ppHandler) ) 
#define ISAXXMLFilter_putContentHandler(This,pHandler)	\
    ( (This)->lpVtbl -> putContentHandler(This,pHandler) ) 
#define ISAXXMLFilter_getDTDHandler(This,ppHandler)	\
    ( (This)->lpVtbl -> getDTDHandler(This,ppHandler) ) 
#define ISAXXMLFilter_putDTDHandler(This,pHandler)	\
    ( (This)->lpVtbl -> putDTDHandler(This,pHandler) ) 
#define ISAXXMLFilter_getErrorHandler(This,ppHandler)	\
    ( (This)->lpVtbl -> getErrorHandler(This,ppHandler) ) 
#define ISAXXMLFilter_putErrorHandler(This,pHandler)	\
    ( (This)->lpVtbl -> putErrorHandler(This,pHandler) ) 
#define ISAXXMLFilter_getBaseURL(This,ppwchBaseUrl)	\
    ( (This)->lpVtbl -> getBaseURL(This,ppwchBaseUrl) ) 
#define ISAXXMLFilter_putBaseURL(This,pwchBaseUrl)	\
    ( (This)->lpVtbl -> putBaseURL(This,pwchBaseUrl) ) 
#define ISAXXMLFilter_getSecureBaseURL(This,ppwchSecureBaseUrl)	\
    ( (This)->lpVtbl -> getSecureBaseURL(This,ppwchSecureBaseUrl) ) 
#define ISAXXMLFilter_putSecureBaseURL(This,pwchSecureBaseUrl)	\
    ( (This)->lpVtbl -> putSecureBaseURL(This,pwchSecureBaseUrl) ) 
#define ISAXXMLFilter_parse(This,varInput)	\
    ( (This)->lpVtbl -> parse(This,varInput) ) 
#define ISAXXMLFilter_parseURL(This,pwchUrl)	\
    ( (This)->lpVtbl -> parseURL(This,pwchUrl) ) 
#define ISAXXMLFilter_getParent(This,ppReader)	\
    ( (This)->lpVtbl -> getParent(This,ppReader) ) 
#define ISAXXMLFilter_putParent(This,pReader)	\
    ( (This)->lpVtbl -> putParent(This,pReader) ) 
#endif 
EXTERN_C const IID IID_ISAXLocator;
    typedef struct ISAXLocatorVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXLocator * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXLocator * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXLocator * This);
        HRESULT ( STDMETHODCALLTYPE *getColumnNumber )( 
            ISAXLocator * This,
             int *pnColumn);
        HRESULT ( STDMETHODCALLTYPE *getLineNumber )( 
            ISAXLocator * This,
             int *pnLine);
        HRESULT ( STDMETHODCALLTYPE *getPublicId )( 
            ISAXLocator * This,
             const wchar_t **ppwchPublicId);
        HRESULT ( STDMETHODCALLTYPE *getSystemId )( 
            ISAXLocator * This,
             const wchar_t **ppwchSystemId);
        END_INTERFACE
    } ISAXLocatorVtbl;
    interface __uuid("9b7e472a-0de4-4640-bff3-84d38a051c31") ISAXLocator

    {
        CONST_VTBL struct ISAXLocatorVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXLocator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXLocator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXLocator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXLocator_getColumnNumber(This,pnColumn)	\
    ( (This)->lpVtbl -> getColumnNumber(This,pnColumn) ) 
#define ISAXLocator_getLineNumber(This,pnLine)	\
    ( (This)->lpVtbl -> getLineNumber(This,pnLine) ) 
#define ISAXLocator_getPublicId(This,ppwchPublicId)	\
    ( (This)->lpVtbl -> getPublicId(This,ppwchPublicId) ) 
#define ISAXLocator_getSystemId(This,ppwchSystemId)	\
    ( (This)->lpVtbl -> getSystemId(This,ppwchSystemId) ) 
#endif 
EXTERN_C const IID IID_ISAXEntityResolver;
    typedef struct ISAXEntityResolverVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXEntityResolver * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXEntityResolver * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXEntityResolver * This);
        HRESULT ( STDMETHODCALLTYPE *resolveEntity )( 
            ISAXEntityResolver * This,
             const wchar_t *pwchPublicId,
             const wchar_t *pwchSystemId,
             VARIANT *pvarInput);
        END_INTERFACE
    } ISAXEntityResolverVtbl;
    interface __uuid("99bca7bd-e8c4-4d5f-a0cf-6d907901ff07") ISAXEntityResolver

    {
        CONST_VTBL struct ISAXEntityResolverVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXEntityResolver_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXEntityResolver_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXEntityResolver_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXEntityResolver_resolveEntity(This,pwchPublicId,pwchSystemId,pvarInput)	\
    ( (This)->lpVtbl -> resolveEntity(This,pwchPublicId,pwchSystemId,pvarInput) ) 
#endif 
EXTERN_C const IID IID_ISAXContentHandler;
    typedef struct ISAXContentHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXContentHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXContentHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXContentHandler * This);
        HRESULT ( STDMETHODCALLTYPE *putDocumentLocator )( 
            ISAXContentHandler * This,
             ISAXLocator *pLocator);
        HRESULT ( STDMETHODCALLTYPE *startDocument )( 
            ISAXContentHandler * This);
        HRESULT ( STDMETHODCALLTYPE *endDocument )( 
            ISAXContentHandler * This);
        HRESULT ( STDMETHODCALLTYPE *startPrefixMapping )( 
            ISAXContentHandler * This,
             const wchar_t *pwchPrefix,
             int cchPrefix,
             const wchar_t *pwchUri,
             int cchUri);
        HRESULT ( STDMETHODCALLTYPE *endPrefixMapping )( 
            ISAXContentHandler * This,
             const wchar_t *pwchPrefix,
             int cchPrefix);
        HRESULT ( STDMETHODCALLTYPE *startElement )( 
            ISAXContentHandler * This,
             const wchar_t *pwchNamespaceUri,
             int cchNamespaceUri,
             const wchar_t *pwchLocalName,
             int cchLocalName,
             const wchar_t *pwchQName,
             int cchQName,
             ISAXAttributes *pAttributes);
        HRESULT ( STDMETHODCALLTYPE *endElement )( 
            ISAXContentHandler * This,
             const wchar_t *pwchNamespaceUri,
             int cchNamespaceUri,
             const wchar_t *pwchLocalName,
             int cchLocalName,
             const wchar_t *pwchQName,
             int cchQName);
        HRESULT ( STDMETHODCALLTYPE *characters )( 
            ISAXContentHandler * This,
             const wchar_t *pwchChars,
             int cchChars);
        HRESULT ( STDMETHODCALLTYPE *ignorableWhitespace )( 
            ISAXContentHandler * This,
             const wchar_t *pwchChars,
             int cchChars);
        HRESULT ( STDMETHODCALLTYPE *processingInstruction )( 
            ISAXContentHandler * This,
             const wchar_t *pwchTarget,
             int cchTarget,
             const wchar_t *pwchData,
             int cchData);
        HRESULT ( STDMETHODCALLTYPE *skippedEntity )( 
            ISAXContentHandler * This,
             const wchar_t *pwchName,
             int cchName);
        END_INTERFACE
    } ISAXContentHandlerVtbl;
    interface __uuid("1545cdfa-9e4e-4497-a8a4-2bf7d0112c44") ISAXContentHandler

    {
        CONST_VTBL struct ISAXContentHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXContentHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXContentHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXContentHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXContentHandler_putDocumentLocator(This,pLocator)	\
    ( (This)->lpVtbl -> putDocumentLocator(This,pLocator) ) 
#define ISAXContentHandler_startDocument(This)	\
    ( (This)->lpVtbl -> startDocument(This) ) 
#define ISAXContentHandler_endDocument(This)	\
    ( (This)->lpVtbl -> endDocument(This) ) 
#define ISAXContentHandler_startPrefixMapping(This,pwchPrefix,cchPrefix,pwchUri,cchUri)	\
    ( (This)->lpVtbl -> startPrefixMapping(This,pwchPrefix,cchPrefix,pwchUri,cchUri) ) 
#define ISAXContentHandler_endPrefixMapping(This,pwchPrefix,cchPrefix)	\
    ( (This)->lpVtbl -> endPrefixMapping(This,pwchPrefix,cchPrefix) ) 
#define ISAXContentHandler_startElement(This,pwchNamespaceUri,cchNamespaceUri,pwchLocalName,cchLocalName,pwchQName,cchQName,pAttributes)	\
    ( (This)->lpVtbl -> startElement(This,pwchNamespaceUri,cchNamespaceUri,pwchLocalName,cchLocalName,pwchQName,cchQName,pAttributes) ) 
#define ISAXContentHandler_endElement(This,pwchNamespaceUri,cchNamespaceUri,pwchLocalName,cchLocalName,pwchQName,cchQName)	\
    ( (This)->lpVtbl -> endElement(This,pwchNamespaceUri,cchNamespaceUri,pwchLocalName,cchLocalName,pwchQName,cchQName) ) 
#define ISAXContentHandler_characters(This,pwchChars,cchChars)	\
    ( (This)->lpVtbl -> characters(This,pwchChars,cchChars) ) 
#define ISAXContentHandler_ignorableWhitespace(This,pwchChars,cchChars)	\
    ( (This)->lpVtbl -> ignorableWhitespace(This,pwchChars,cchChars) ) 
#define ISAXContentHandler_processingInstruction(This,pwchTarget,cchTarget,pwchData,cchData)	\
    ( (This)->lpVtbl -> processingInstruction(This,pwchTarget,cchTarget,pwchData,cchData) ) 
#define ISAXContentHandler_skippedEntity(This,pwchName,cchName)	\
    ( (This)->lpVtbl -> skippedEntity(This,pwchName,cchName) ) 
#endif 
EXTERN_C const IID IID_ISAXDTDHandler;
    typedef struct ISAXDTDHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXDTDHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXDTDHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXDTDHandler * This);
        HRESULT ( STDMETHODCALLTYPE *notationDecl )( 
            ISAXDTDHandler * This,
             const wchar_t *pwchName,
             int cchName,
             const wchar_t *pwchPublicId,
             int cchPublicId,
             const wchar_t *pwchSystemId,
             int cchSystemId);
        HRESULT ( STDMETHODCALLTYPE *unparsedEntityDecl )( 
            ISAXDTDHandler * This,
             const wchar_t *pwchName,
             int cchName,
             const wchar_t *pwchPublicId,
             int cchPublicId,
             const wchar_t *pwchSystemId,
             int cchSystemId,
             const wchar_t *pwchNotationName,
             int cchNotationName);
        END_INTERFACE
    } ISAXDTDHandlerVtbl;
    interface __uuid("e15c1baf-afb3-4d60-8c36-19a8c45defed") ISAXDTDHandler

    {
        CONST_VTBL struct ISAXDTDHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXDTDHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXDTDHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXDTDHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXDTDHandler_notationDecl(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId)	\
    ( (This)->lpVtbl -> notationDecl(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId) ) 
#define ISAXDTDHandler_unparsedEntityDecl(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId,pwchNotationName,cchNotationName)	\
    ( (This)->lpVtbl -> unparsedEntityDecl(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId,pwchNotationName,cchNotationName) ) 
#endif 
EXTERN_C const IID IID_ISAXErrorHandler;
    typedef struct ISAXErrorHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXErrorHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXErrorHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXErrorHandler * This);
        HRESULT ( STDMETHODCALLTYPE *error )( 
            ISAXErrorHandler * This,
             ISAXLocator *pLocator,
             const wchar_t *pwchErrorMessage,
             HRESULT hrErrorCode);
        HRESULT ( STDMETHODCALLTYPE *fatalError )( 
            ISAXErrorHandler * This,
             ISAXLocator *pLocator,
             const wchar_t *pwchErrorMessage,
             HRESULT hrErrorCode);
        HRESULT ( STDMETHODCALLTYPE *ignorableWarning )( 
            ISAXErrorHandler * This,
             ISAXLocator *pLocator,
             const wchar_t *pwchErrorMessage,
             HRESULT hrErrorCode);
        END_INTERFACE
    } ISAXErrorHandlerVtbl;
    interface __uuid("a60511c4-ccf5-479e-98a3-dc8dc545b7d0") ISAXErrorHandler

    {
        CONST_VTBL struct ISAXErrorHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXErrorHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXErrorHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXErrorHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXErrorHandler_error(This,pLocator,pwchErrorMessage,hrErrorCode)	\
    ( (This)->lpVtbl -> error(This,pLocator,pwchErrorMessage,hrErrorCode) ) 
#define ISAXErrorHandler_fatalError(This,pLocator,pwchErrorMessage,hrErrorCode)	\
    ( (This)->lpVtbl -> fatalError(This,pLocator,pwchErrorMessage,hrErrorCode) ) 
#define ISAXErrorHandler_ignorableWarning(This,pLocator,pwchErrorMessage,hrErrorCode)	\
    ( (This)->lpVtbl -> ignorableWarning(This,pLocator,pwchErrorMessage,hrErrorCode) ) 
#endif 
EXTERN_C const IID IID_ISAXLexicalHandler;
    typedef struct ISAXLexicalHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXLexicalHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXLexicalHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXLexicalHandler * This);
        HRESULT ( STDMETHODCALLTYPE *startDTD )( 
            ISAXLexicalHandler * This,
             const wchar_t *pwchName,
             int cchName,
             const wchar_t *pwchPublicId,
             int cchPublicId,
             const wchar_t *pwchSystemId,
             int cchSystemId);
        HRESULT ( STDMETHODCALLTYPE *endDTD )( 
            ISAXLexicalHandler * This);
        HRESULT ( STDMETHODCALLTYPE *startEntity )( 
            ISAXLexicalHandler * This,
             const wchar_t *pwchName,
             int cchName);
        HRESULT ( STDMETHODCALLTYPE *endEntity )( 
            ISAXLexicalHandler * This,
             const wchar_t *pwchName,
             int cchName);
        HRESULT ( STDMETHODCALLTYPE *startCDATA )( 
            ISAXLexicalHandler * This);
        HRESULT ( STDMETHODCALLTYPE *endCDATA )( 
            ISAXLexicalHandler * This);
        HRESULT ( STDMETHODCALLTYPE *comment )( 
            ISAXLexicalHandler * This,
             const wchar_t *pwchChars,
             int cchChars);
        END_INTERFACE
    } ISAXLexicalHandlerVtbl;
    interface __uuid("7f85d5f5-47a8-4497-bda5-84ba04819ea6") ISAXLexicalHandler

    {
        CONST_VTBL struct ISAXLexicalHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXLexicalHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXLexicalHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXLexicalHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXLexicalHandler_startDTD(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId)	\
    ( (This)->lpVtbl -> startDTD(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId) ) 
#define ISAXLexicalHandler_endDTD(This)	\
    ( (This)->lpVtbl -> endDTD(This) ) 
#define ISAXLexicalHandler_startEntity(This,pwchName,cchName)	\
    ( (This)->lpVtbl -> startEntity(This,pwchName,cchName) ) 
#define ISAXLexicalHandler_endEntity(This,pwchName,cchName)	\
    ( (This)->lpVtbl -> endEntity(This,pwchName,cchName) ) 
#define ISAXLexicalHandler_startCDATA(This)	\
    ( (This)->lpVtbl -> startCDATA(This) ) 
#define ISAXLexicalHandler_endCDATA(This)	\
    ( (This)->lpVtbl -> endCDATA(This) ) 
#define ISAXLexicalHandler_comment(This,pwchChars,cchChars)	\
    ( (This)->lpVtbl -> comment(This,pwchChars,cchChars) ) 
#endif 
EXTERN_C const IID IID_ISAXDeclHandler;
    typedef struct ISAXDeclHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXDeclHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXDeclHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXDeclHandler * This);
        HRESULT ( STDMETHODCALLTYPE *elementDecl )( 
            ISAXDeclHandler * This,
             const wchar_t *pwchName,
             int cchName,
             const wchar_t *pwchModel,
             int cchModel);
        HRESULT ( STDMETHODCALLTYPE *attributeDecl )( 
            ISAXDeclHandler * This,
             const wchar_t *pwchElementName,
             int cchElementName,
             const wchar_t *pwchAttributeName,
             int cchAttributeName,
             const wchar_t *pwchType,
             int cchType,
             const wchar_t *pwchValueDefault,
             int cchValueDefault,
             const wchar_t *pwchValue,
             int cchValue);
        HRESULT ( STDMETHODCALLTYPE *internalEntityDecl )( 
            ISAXDeclHandler * This,
             const wchar_t *pwchName,
             int cchName,
             const wchar_t *pwchValue,
             int cchValue);
        HRESULT ( STDMETHODCALLTYPE *externalEntityDecl )( 
            ISAXDeclHandler * This,
             const wchar_t *pwchName,
             int cchName,
             const wchar_t *pwchPublicId,
             int cchPublicId,
             const wchar_t *pwchSystemId,
             int cchSystemId);
        END_INTERFACE
    } ISAXDeclHandlerVtbl;
    interface __uuid("862629ac-771a-47b2-8337-4e6843c1be90") ISAXDeclHandler

    {
        CONST_VTBL struct ISAXDeclHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXDeclHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXDeclHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXDeclHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXDeclHandler_elementDecl(This,pwchName,cchName,pwchModel,cchModel)	\
    ( (This)->lpVtbl -> elementDecl(This,pwchName,cchName,pwchModel,cchModel) ) 
#define ISAXDeclHandler_attributeDecl(This,pwchElementName,cchElementName,pwchAttributeName,cchAttributeName,pwchType,cchType,pwchValueDefault,cchValueDefault,pwchValue,cchValue)	\
    ( (This)->lpVtbl -> attributeDecl(This,pwchElementName,cchElementName,pwchAttributeName,cchAttributeName,pwchType,cchType,pwchValueDefault,cchValueDefault,pwchValue,cchValue) ) 
#define ISAXDeclHandler_internalEntityDecl(This,pwchName,cchName,pwchValue,cchValue)	\
    ( (This)->lpVtbl -> internalEntityDecl(This,pwchName,cchName,pwchValue,cchValue) ) 
#define ISAXDeclHandler_externalEntityDecl(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId)	\
    ( (This)->lpVtbl -> externalEntityDecl(This,pwchName,cchName,pwchPublicId,cchPublicId,pwchSystemId,cchSystemId) ) 
#endif 
EXTERN_C const IID IID_ISAXAttributes;
    typedef struct ISAXAttributesVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISAXAttributes * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISAXAttributes * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISAXAttributes * This);
        HRESULT ( STDMETHODCALLTYPE *getLength )( 
            ISAXAttributes * This,
             int *pnLength);
        HRESULT ( STDMETHODCALLTYPE *getURI )( 
            ISAXAttributes * This,
             int nIndex,
             const wchar_t **ppwchUri,
             int *pcchUri);
        HRESULT ( STDMETHODCALLTYPE *getLocalName )( 
            ISAXAttributes * This,
             int nIndex,
             const wchar_t **ppwchLocalName,
             int *pcchLocalName);
        HRESULT ( STDMETHODCALLTYPE *getQName )( 
            ISAXAttributes * This,
             int nIndex,
             const wchar_t **ppwchQName,
             int *pcchQName);
        HRESULT ( STDMETHODCALLTYPE *getName )( 
            ISAXAttributes * This,
             int nIndex,
             const wchar_t **ppwchUri,
             int *pcchUri,
             const wchar_t **ppwchLocalName,
             int *pcchLocalName,
             const wchar_t **ppwchQName,
             int *pcchQName);
        HRESULT ( STDMETHODCALLTYPE *getIndexFromName )( 
            ISAXAttributes * This,
             const wchar_t *pwchUri,
             int cchUri,
             const wchar_t *pwchLocalName,
             int cchLocalName,
             int *pnIndex);
        HRESULT ( STDMETHODCALLTYPE *getIndexFromQName )( 
            ISAXAttributes * This,
             const wchar_t *pwchQName,
             int cchQName,
             int *pnIndex);
        HRESULT ( STDMETHODCALLTYPE *getType )( 
            ISAXAttributes * This,
             int nIndex,
             const wchar_t **ppwchType,
             int *pcchType);
        HRESULT ( STDMETHODCALLTYPE *getTypeFromName )( 
            ISAXAttributes * This,
             const wchar_t *pwchUri,
             int cchUri,
             const wchar_t *pwchLocalName,
             int cchLocalName,
             const wchar_t **ppwchType,
             int *pcchType);
        HRESULT ( STDMETHODCALLTYPE *getTypeFromQName )( 
            ISAXAttributes * This,
             const wchar_t *pwchQName,
             int cchQName,
             const wchar_t **ppwchType,
             int *pcchType);
        HRESULT ( STDMETHODCALLTYPE *getValue )( 
            ISAXAttributes * This,
             int nIndex,
             const wchar_t **ppwchValue,
             int *pcchValue);
        HRESULT ( STDMETHODCALLTYPE *getValueFromName )( 
            ISAXAttributes * This,
             const wchar_t *pwchUri,
             int cchUri,
             const wchar_t *pwchLocalName,
             int cchLocalName,
             const wchar_t **ppwchValue,
             int *pcchValue);
        HRESULT ( STDMETHODCALLTYPE *getValueFromQName )( 
            ISAXAttributes * This,
             const wchar_t *pwchQName,
             int cchQName,
             const wchar_t **ppwchValue,
             int *pcchValue);
        END_INTERFACE
    } ISAXAttributesVtbl;
    interface __uuid("f078abe1-45d2-4832-91ea-4466ce2f25c9") ISAXAttributes

    {
        CONST_VTBL struct ISAXAttributesVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISAXAttributes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISAXAttributes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISAXAttributes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISAXAttributes_getLength(This,pnLength)	\
    ( (This)->lpVtbl -> getLength(This,pnLength) ) 
#define ISAXAttributes_getURI(This,nIndex,ppwchUri,pcchUri)	\
    ( (This)->lpVtbl -> getURI(This,nIndex,ppwchUri,pcchUri) ) 
#define ISAXAttributes_getLocalName(This,nIndex,ppwchLocalName,pcchLocalName)	\
    ( (This)->lpVtbl -> getLocalName(This,nIndex,ppwchLocalName,pcchLocalName) ) 
#define ISAXAttributes_getQName(This,nIndex,ppwchQName,pcchQName)	\
    ( (This)->lpVtbl -> getQName(This,nIndex,ppwchQName,pcchQName) ) 
#define ISAXAttributes_getName(This,nIndex,ppwchUri,pcchUri,ppwchLocalName,pcchLocalName,ppwchQName,pcchQName)	\
    ( (This)->lpVtbl -> getName(This,nIndex,ppwchUri,pcchUri,ppwchLocalName,pcchLocalName,ppwchQName,pcchQName) ) 
#define ISAXAttributes_getIndexFromName(This,pwchUri,cchUri,pwchLocalName,cchLocalName,pnIndex)	\
    ( (This)->lpVtbl -> getIndexFromName(This,pwchUri,cchUri,pwchLocalName,cchLocalName,pnIndex) ) 
#define ISAXAttributes_getIndexFromQName(This,pwchQName,cchQName,pnIndex)	\
    ( (This)->lpVtbl -> getIndexFromQName(This,pwchQName,cchQName,pnIndex) ) 
#define ISAXAttributes_getType(This,nIndex,ppwchType,pcchType)	\
    ( (This)->lpVtbl -> getType(This,nIndex,ppwchType,pcchType) ) 
#define ISAXAttributes_getTypeFromName(This,pwchUri,cchUri,pwchLocalName,cchLocalName,ppwchType,pcchType)	\
    ( (This)->lpVtbl -> getTypeFromName(This,pwchUri,cchUri,pwchLocalName,cchLocalName,ppwchType,pcchType) ) 
#define ISAXAttributes_getTypeFromQName(This,pwchQName,cchQName,ppwchType,pcchType)	\
    ( (This)->lpVtbl -> getTypeFromQName(This,pwchQName,cchQName,ppwchType,pcchType) ) 
#define ISAXAttributes_getValue(This,nIndex,ppwchValue,pcchValue)	\
    ( (This)->lpVtbl -> getValue(This,nIndex,ppwchValue,pcchValue) ) 
#define ISAXAttributes_getValueFromName(This,pwchUri,cchUri,pwchLocalName,cchLocalName,ppwchValue,pcchValue)	\
    ( (This)->lpVtbl -> getValueFromName(This,pwchUri,cchUri,pwchLocalName,cchLocalName,ppwchValue,pcchValue) ) 
#define ISAXAttributes_getValueFromQName(This,pwchQName,cchQName,ppwchValue,pcchValue)	\
    ( (This)->lpVtbl -> getValueFromQName(This,pwchQName,cchQName,ppwchValue,pcchValue) ) 
#endif 
EXTERN_C const IID IID_IVBSAXXMLReader;
    typedef struct IVBSAXXMLReaderVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXXMLReader * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXXMLReader * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXXMLReader * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXXMLReader * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXXMLReader * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXXMLReader * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXXMLReader * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *getFeature )( 
            IVBSAXXMLReader * This,
             BSTR strName,
             VARIANT_BOOL *fValue);
         HRESULT ( STDMETHODCALLTYPE *putFeature )( 
            IVBSAXXMLReader * This,
             BSTR strName,
             VARIANT_BOOL fValue);
         HRESULT ( STDMETHODCALLTYPE *getProperty )( 
            IVBSAXXMLReader * This,
             BSTR strName,
             VARIANT *varValue);
         HRESULT ( STDMETHODCALLTYPE *putProperty )( 
            IVBSAXXMLReader * This,
             BSTR strName,
             VARIANT varValue);
         HRESULT ( STDMETHODCALLTYPE *get_entityResolver )( 
            IVBSAXXMLReader * This,
             IVBSAXEntityResolver **oResolver);
         HRESULT ( STDMETHODCALLTYPE *putref_entityResolver )( 
            IVBSAXXMLReader * This,
             IVBSAXEntityResolver *oResolver);
         HRESULT ( STDMETHODCALLTYPE *get_contentHandler )( 
            IVBSAXXMLReader * This,
             IVBSAXContentHandler **oHandler);
         HRESULT ( STDMETHODCALLTYPE *putref_contentHandler )( 
            IVBSAXXMLReader * This,
             IVBSAXContentHandler *oHandler);
         HRESULT ( STDMETHODCALLTYPE *get_dtdHandler )( 
            IVBSAXXMLReader * This,
             IVBSAXDTDHandler **oHandler);
         HRESULT ( STDMETHODCALLTYPE *putref_dtdHandler )( 
            IVBSAXXMLReader * This,
             IVBSAXDTDHandler *oHandler);
         HRESULT ( STDMETHODCALLTYPE *get_errorHandler )( 
            IVBSAXXMLReader * This,
             IVBSAXErrorHandler **oHandler);
         HRESULT ( STDMETHODCALLTYPE *putref_errorHandler )( 
            IVBSAXXMLReader * This,
             IVBSAXErrorHandler *oHandler);
         HRESULT ( STDMETHODCALLTYPE *get_baseURL )( 
            IVBSAXXMLReader * This,
             BSTR *strBaseURL);
         HRESULT ( STDMETHODCALLTYPE *put_baseURL )( 
            IVBSAXXMLReader * This,
             BSTR strBaseURL);
         HRESULT ( STDMETHODCALLTYPE *get_secureBaseURL )( 
            IVBSAXXMLReader * This,
             BSTR *strSecureBaseURL);
         HRESULT ( STDMETHODCALLTYPE *put_secureBaseURL )( 
            IVBSAXXMLReader * This,
             BSTR strSecureBaseURL);
         HRESULT ( STDMETHODCALLTYPE *parse )( 
            IVBSAXXMLReader * This,
             VARIANT varInput);
         HRESULT ( STDMETHODCALLTYPE *parseURL )( 
            IVBSAXXMLReader * This,
             BSTR strURL);
        END_INTERFACE
    } IVBSAXXMLReaderVtbl;
    interface __uuid("8c033caa-6cd6-4f73-b728-4531af74945f") IVBSAXXMLReader

    {
        CONST_VTBL struct IVBSAXXMLReaderVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXXMLReader_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXXMLReader_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXXMLReader_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXXMLReader_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXXMLReader_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXXMLReader_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXXMLReader_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXXMLReader_getFeature(This,strName,fValue)	\
    ( (This)->lpVtbl -> getFeature(This,strName,fValue) ) 
#define IVBSAXXMLReader_putFeature(This,strName,fValue)	\
    ( (This)->lpVtbl -> putFeature(This,strName,fValue) ) 
#define IVBSAXXMLReader_getProperty(This,strName,varValue)	\
    ( (This)->lpVtbl -> getProperty(This,strName,varValue) ) 
#define IVBSAXXMLReader_putProperty(This,strName,varValue)	\
    ( (This)->lpVtbl -> putProperty(This,strName,varValue) ) 
#define IVBSAXXMLReader_get_entityResolver(This,oResolver)	\
    ( (This)->lpVtbl -> get_entityResolver(This,oResolver) ) 
#define IVBSAXXMLReader_putref_entityResolver(This,oResolver)	\
    ( (This)->lpVtbl -> putref_entityResolver(This,oResolver) ) 
#define IVBSAXXMLReader_get_contentHandler(This,oHandler)	\
    ( (This)->lpVtbl -> get_contentHandler(This,oHandler) ) 
#define IVBSAXXMLReader_putref_contentHandler(This,oHandler)	\
    ( (This)->lpVtbl -> putref_contentHandler(This,oHandler) ) 
#define IVBSAXXMLReader_get_dtdHandler(This,oHandler)	\
    ( (This)->lpVtbl -> get_dtdHandler(This,oHandler) ) 
#define IVBSAXXMLReader_putref_dtdHandler(This,oHandler)	\
    ( (This)->lpVtbl -> putref_dtdHandler(This,oHandler) ) 
#define IVBSAXXMLReader_get_errorHandler(This,oHandler)	\
    ( (This)->lpVtbl -> get_errorHandler(This,oHandler) ) 
#define IVBSAXXMLReader_putref_errorHandler(This,oHandler)	\
    ( (This)->lpVtbl -> putref_errorHandler(This,oHandler) ) 
#define IVBSAXXMLReader_get_baseURL(This,strBaseURL)	\
    ( (This)->lpVtbl -> get_baseURL(This,strBaseURL) ) 
#define IVBSAXXMLReader_put_baseURL(This,strBaseURL)	\
    ( (This)->lpVtbl -> put_baseURL(This,strBaseURL) ) 
#define IVBSAXXMLReader_get_secureBaseURL(This,strSecureBaseURL)	\
    ( (This)->lpVtbl -> get_secureBaseURL(This,strSecureBaseURL) ) 
#define IVBSAXXMLReader_put_secureBaseURL(This,strSecureBaseURL)	\
    ( (This)->lpVtbl -> put_secureBaseURL(This,strSecureBaseURL) ) 
#define IVBSAXXMLReader_parse(This,varInput)	\
    ( (This)->lpVtbl -> parse(This,varInput) ) 
#define IVBSAXXMLReader_parseURL(This,strURL)	\
    ( (This)->lpVtbl -> parseURL(This,strURL) ) 
#endif 
EXTERN_C const IID IID_IVBSAXXMLFilter;
    typedef struct IVBSAXXMLFilterVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXXMLFilter * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXXMLFilter * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXXMLFilter * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXXMLFilter * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXXMLFilter * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXXMLFilter * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXXMLFilter * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_parent )( 
            IVBSAXXMLFilter * This,
             IVBSAXXMLReader **oReader);
         HRESULT ( STDMETHODCALLTYPE *putref_parent )( 
            IVBSAXXMLFilter * This,
             IVBSAXXMLReader *oReader);
        END_INTERFACE
    } IVBSAXXMLFilterVtbl;
    interface __uuid("1299eb1b-5b88-433e-82de-82ca75ad4e04") IVBSAXXMLFilter

    {
        CONST_VTBL struct IVBSAXXMLFilterVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXXMLFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXXMLFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXXMLFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXXMLFilter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXXMLFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXXMLFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXXMLFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXXMLFilter_get_parent(This,oReader)	\
    ( (This)->lpVtbl -> get_parent(This,oReader) ) 
#define IVBSAXXMLFilter_putref_parent(This,oReader)	\
    ( (This)->lpVtbl -> putref_parent(This,oReader) ) 
#endif 
EXTERN_C const IID IID_IVBSAXLocator;
    typedef struct IVBSAXLocatorVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXLocator * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXLocator * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXLocator * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXLocator * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXLocator * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXLocator * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXLocator * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_columnNumber )( 
            IVBSAXLocator * This,
             int *nColumn);
         HRESULT ( STDMETHODCALLTYPE *get_lineNumber )( 
            IVBSAXLocator * This,
             int *nLine);
         HRESULT ( STDMETHODCALLTYPE *get_publicId )( 
            IVBSAXLocator * This,
             BSTR *strPublicId);
         HRESULT ( STDMETHODCALLTYPE *get_systemId )( 
            IVBSAXLocator * This,
             BSTR *strSystemId);
        END_INTERFACE
    } IVBSAXLocatorVtbl;
    interface __uuid("796e7ac5-5aa2-4eff-acad-3faaf01a3288") IVBSAXLocator

    {
        CONST_VTBL struct IVBSAXLocatorVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXLocator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXLocator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXLocator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXLocator_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXLocator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXLocator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXLocator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXLocator_get_columnNumber(This,nColumn)	\
    ( (This)->lpVtbl -> get_columnNumber(This,nColumn) ) 
#define IVBSAXLocator_get_lineNumber(This,nLine)	\
    ( (This)->lpVtbl -> get_lineNumber(This,nLine) ) 
#define IVBSAXLocator_get_publicId(This,strPublicId)	\
    ( (This)->lpVtbl -> get_publicId(This,strPublicId) ) 
#define IVBSAXLocator_get_systemId(This,strSystemId)	\
    ( (This)->lpVtbl -> get_systemId(This,strSystemId) ) 
#endif 
EXTERN_C const IID IID_IVBSAXEntityResolver;
    typedef struct IVBSAXEntityResolverVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXEntityResolver * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXEntityResolver * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXEntityResolver * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXEntityResolver * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXEntityResolver * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXEntityResolver * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXEntityResolver * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *resolveEntity )( 
            IVBSAXEntityResolver * This,
             BSTR *strPublicId,
             BSTR *strSystemId,
             VARIANT *varInput);
        END_INTERFACE
    } IVBSAXEntityResolverVtbl;
    interface __uuid("0c05d096-f45b-4aca-ad1a-aa0bc25518dc") IVBSAXEntityResolver

    {
        CONST_VTBL struct IVBSAXEntityResolverVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXEntityResolver_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXEntityResolver_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXEntityResolver_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXEntityResolver_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXEntityResolver_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXEntityResolver_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXEntityResolver_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXEntityResolver_resolveEntity(This,strPublicId,strSystemId,varInput)	\
    ( (This)->lpVtbl -> resolveEntity(This,strPublicId,strSystemId,varInput) ) 
#endif 
EXTERN_C const IID IID_IVBSAXContentHandler;
    typedef struct IVBSAXContentHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXContentHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXContentHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXContentHandler * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXContentHandler * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXContentHandler * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXContentHandler * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXContentHandler * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *putref_documentLocator )( 
            IVBSAXContentHandler * This,
             IVBSAXLocator *oLocator);
         HRESULT ( STDMETHODCALLTYPE *startDocument )( 
            IVBSAXContentHandler * This);
         HRESULT ( STDMETHODCALLTYPE *endDocument )( 
            IVBSAXContentHandler * This);
         HRESULT ( STDMETHODCALLTYPE *startPrefixMapping )( 
            IVBSAXContentHandler * This,
             BSTR *strPrefix,
             BSTR *strURI);
         HRESULT ( STDMETHODCALLTYPE *endPrefixMapping )( 
            IVBSAXContentHandler * This,
             BSTR *strPrefix);
         HRESULT ( STDMETHODCALLTYPE *startElement )( 
            IVBSAXContentHandler * This,
             BSTR *strNamespaceURI,
             BSTR *strLocalName,
             BSTR *strQName,
             IVBSAXAttributes *oAttributes);
         HRESULT ( STDMETHODCALLTYPE *endElement )( 
            IVBSAXContentHandler * This,
             BSTR *strNamespaceURI,
             BSTR *strLocalName,
             BSTR *strQName);
         HRESULT ( STDMETHODCALLTYPE *characters )( 
            IVBSAXContentHandler * This,
             BSTR *strChars);
         HRESULT ( STDMETHODCALLTYPE *ignorableWhitespace )( 
            IVBSAXContentHandler * This,
             BSTR *strChars);
         HRESULT ( STDMETHODCALLTYPE *processingInstruction )( 
            IVBSAXContentHandler * This,
             BSTR *strTarget,
             BSTR *strData);
         HRESULT ( STDMETHODCALLTYPE *skippedEntity )( 
            IVBSAXContentHandler * This,
             BSTR *strName);
        END_INTERFACE
    } IVBSAXContentHandlerVtbl;
    interface __uuid("2ed7290a-4dd5-4b46-bb26-4e4155e77faa") IVBSAXContentHandler

    {
        CONST_VTBL struct IVBSAXContentHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXContentHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXContentHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXContentHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXContentHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXContentHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXContentHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXContentHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXContentHandler_putref_documentLocator(This,oLocator)	\
    ( (This)->lpVtbl -> putref_documentLocator(This,oLocator) ) 
#define IVBSAXContentHandler_startDocument(This)	\
    ( (This)->lpVtbl -> startDocument(This) ) 
#define IVBSAXContentHandler_endDocument(This)	\
    ( (This)->lpVtbl -> endDocument(This) ) 
#define IVBSAXContentHandler_startPrefixMapping(This,strPrefix,strURI)	\
    ( (This)->lpVtbl -> startPrefixMapping(This,strPrefix,strURI) ) 
#define IVBSAXContentHandler_endPrefixMapping(This,strPrefix)	\
    ( (This)->lpVtbl -> endPrefixMapping(This,strPrefix) ) 
#define IVBSAXContentHandler_startElement(This,strNamespaceURI,strLocalName,strQName,oAttributes)	\
    ( (This)->lpVtbl -> startElement(This,strNamespaceURI,strLocalName,strQName,oAttributes) ) 
#define IVBSAXContentHandler_endElement(This,strNamespaceURI,strLocalName,strQName)	\
    ( (This)->lpVtbl -> endElement(This,strNamespaceURI,strLocalName,strQName) ) 
#define IVBSAXContentHandler_characters(This,strChars)	\
    ( (This)->lpVtbl -> characters(This,strChars) ) 
#define IVBSAXContentHandler_ignorableWhitespace(This,strChars)	\
    ( (This)->lpVtbl -> ignorableWhitespace(This,strChars) ) 
#define IVBSAXContentHandler_processingInstruction(This,strTarget,strData)	\
    ( (This)->lpVtbl -> processingInstruction(This,strTarget,strData) ) 
#define IVBSAXContentHandler_skippedEntity(This,strName)	\
    ( (This)->lpVtbl -> skippedEntity(This,strName) ) 
#endif 
EXTERN_C const IID IID_IVBSAXDTDHandler;
    typedef struct IVBSAXDTDHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXDTDHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXDTDHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXDTDHandler * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXDTDHandler * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXDTDHandler * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXDTDHandler * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXDTDHandler * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *notationDecl )( 
            IVBSAXDTDHandler * This,
             BSTR *strName,
             BSTR *strPublicId,
             BSTR *strSystemId);
         HRESULT ( STDMETHODCALLTYPE *unparsedEntityDecl )( 
            IVBSAXDTDHandler * This,
             BSTR *strName,
             BSTR *strPublicId,
             BSTR *strSystemId,
             BSTR *strNotationName);
        END_INTERFACE
    } IVBSAXDTDHandlerVtbl;
    interface __uuid("24fb3297-302d-4620-ba39-3a732d850558") IVBSAXDTDHandler

    {
        CONST_VTBL struct IVBSAXDTDHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXDTDHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXDTDHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXDTDHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXDTDHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXDTDHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXDTDHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXDTDHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXDTDHandler_notationDecl(This,strName,strPublicId,strSystemId)	\
    ( (This)->lpVtbl -> notationDecl(This,strName,strPublicId,strSystemId) ) 
#define IVBSAXDTDHandler_unparsedEntityDecl(This,strName,strPublicId,strSystemId,strNotationName)	\
    ( (This)->lpVtbl -> unparsedEntityDecl(This,strName,strPublicId,strSystemId,strNotationName) ) 
#endif 
EXTERN_C const IID IID_IVBSAXErrorHandler;
    typedef struct IVBSAXErrorHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXErrorHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXErrorHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXErrorHandler * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXErrorHandler * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXErrorHandler * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXErrorHandler * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXErrorHandler * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *error )( 
            IVBSAXErrorHandler * This,
             IVBSAXLocator *oLocator,
             BSTR *strErrorMessage,
             long nErrorCode);
         HRESULT ( STDMETHODCALLTYPE *fatalError )( 
            IVBSAXErrorHandler * This,
             IVBSAXLocator *oLocator,
             BSTR *strErrorMessage,
             long nErrorCode);
         HRESULT ( STDMETHODCALLTYPE *ignorableWarning )( 
            IVBSAXErrorHandler * This,
             IVBSAXLocator *oLocator,
             BSTR *strErrorMessage,
             long nErrorCode);
        END_INTERFACE
    } IVBSAXErrorHandlerVtbl;
    interface __uuid("d963d3fe-173c-4862-9095-b92f66995f52") IVBSAXErrorHandler

    {
        CONST_VTBL struct IVBSAXErrorHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXErrorHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXErrorHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXErrorHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXErrorHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXErrorHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXErrorHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXErrorHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXErrorHandler_error(This,oLocator,strErrorMessage,nErrorCode)	\
    ( (This)->lpVtbl -> error(This,oLocator,strErrorMessage,nErrorCode) ) 
#define IVBSAXErrorHandler_fatalError(This,oLocator,strErrorMessage,nErrorCode)	\
    ( (This)->lpVtbl -> fatalError(This,oLocator,strErrorMessage,nErrorCode) ) 
#define IVBSAXErrorHandler_ignorableWarning(This,oLocator,strErrorMessage,nErrorCode)	\
    ( (This)->lpVtbl -> ignorableWarning(This,oLocator,strErrorMessage,nErrorCode) ) 
#endif 
EXTERN_C const IID IID_IVBSAXLexicalHandler;
    typedef struct IVBSAXLexicalHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXLexicalHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXLexicalHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXLexicalHandler * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXLexicalHandler * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXLexicalHandler * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXLexicalHandler * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXLexicalHandler * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *startDTD )( 
            IVBSAXLexicalHandler * This,
             BSTR *strName,
             BSTR *strPublicId,
             BSTR *strSystemId);
         HRESULT ( STDMETHODCALLTYPE *endDTD )( 
            IVBSAXLexicalHandler * This);
         HRESULT ( STDMETHODCALLTYPE *startEntity )( 
            IVBSAXLexicalHandler * This,
             BSTR *strName);
         HRESULT ( STDMETHODCALLTYPE *endEntity )( 
            IVBSAXLexicalHandler * This,
             BSTR *strName);
         HRESULT ( STDMETHODCALLTYPE *startCDATA )( 
            IVBSAXLexicalHandler * This);
         HRESULT ( STDMETHODCALLTYPE *endCDATA )( 
            IVBSAXLexicalHandler * This);
         HRESULT ( STDMETHODCALLTYPE *comment )( 
            IVBSAXLexicalHandler * This,
             BSTR *strChars);
        END_INTERFACE
    } IVBSAXLexicalHandlerVtbl;
    interface __uuid("032aac35-8c0e-4d9d-979f-e3b702935576") IVBSAXLexicalHandler

    {
        CONST_VTBL struct IVBSAXLexicalHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXLexicalHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXLexicalHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXLexicalHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXLexicalHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXLexicalHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXLexicalHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXLexicalHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXLexicalHandler_startDTD(This,strName,strPublicId,strSystemId)	\
    ( (This)->lpVtbl -> startDTD(This,strName,strPublicId,strSystemId) ) 
#define IVBSAXLexicalHandler_endDTD(This)	\
    ( (This)->lpVtbl -> endDTD(This) ) 
#define IVBSAXLexicalHandler_startEntity(This,strName)	\
    ( (This)->lpVtbl -> startEntity(This,strName) ) 
#define IVBSAXLexicalHandler_endEntity(This,strName)	\
    ( (This)->lpVtbl -> endEntity(This,strName) ) 
#define IVBSAXLexicalHandler_startCDATA(This)	\
    ( (This)->lpVtbl -> startCDATA(This) ) 
#define IVBSAXLexicalHandler_endCDATA(This)	\
    ( (This)->lpVtbl -> endCDATA(This) ) 
#define IVBSAXLexicalHandler_comment(This,strChars)	\
    ( (This)->lpVtbl -> comment(This,strChars) ) 
#endif 
EXTERN_C const IID IID_IVBSAXDeclHandler;
    typedef struct IVBSAXDeclHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXDeclHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXDeclHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXDeclHandler * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXDeclHandler * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXDeclHandler * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXDeclHandler * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXDeclHandler * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *elementDecl )( 
            IVBSAXDeclHandler * This,
             BSTR *strName,
             BSTR *strModel);
         HRESULT ( STDMETHODCALLTYPE *attributeDecl )( 
            IVBSAXDeclHandler * This,
             BSTR *strElementName,
             BSTR *strAttributeName,
             BSTR *strType,
             BSTR *strValueDefault,
             BSTR *strValue);
         HRESULT ( STDMETHODCALLTYPE *internalEntityDecl )( 
            IVBSAXDeclHandler * This,
             BSTR *strName,
             BSTR *strValue);
         HRESULT ( STDMETHODCALLTYPE *externalEntityDecl )( 
            IVBSAXDeclHandler * This,
             BSTR *strName,
             BSTR *strPublicId,
             BSTR *strSystemId);
        END_INTERFACE
    } IVBSAXDeclHandlerVtbl;
    interface __uuid("e8917260-7579-4be1-b5dd-7afbfa6f077b") IVBSAXDeclHandler

    {
        CONST_VTBL struct IVBSAXDeclHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXDeclHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXDeclHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXDeclHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXDeclHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXDeclHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXDeclHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXDeclHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXDeclHandler_elementDecl(This,strName,strModel)	\
    ( (This)->lpVtbl -> elementDecl(This,strName,strModel) ) 
#define IVBSAXDeclHandler_attributeDecl(This,strElementName,strAttributeName,strType,strValueDefault,strValue)	\
    ( (This)->lpVtbl -> attributeDecl(This,strElementName,strAttributeName,strType,strValueDefault,strValue) ) 
#define IVBSAXDeclHandler_internalEntityDecl(This,strName,strValue)	\
    ( (This)->lpVtbl -> internalEntityDecl(This,strName,strValue) ) 
#define IVBSAXDeclHandler_externalEntityDecl(This,strName,strPublicId,strSystemId)	\
    ( (This)->lpVtbl -> externalEntityDecl(This,strName,strPublicId,strSystemId) ) 
#endif 
EXTERN_C const IID IID_IVBSAXAttributes;
    typedef struct IVBSAXAttributesVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBSAXAttributes * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBSAXAttributes * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBSAXAttributes * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBSAXAttributes * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBSAXAttributes * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBSAXAttributes * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBSAXAttributes * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IVBSAXAttributes * This,
             int *nLength);
         HRESULT ( STDMETHODCALLTYPE *getURI )( 
            IVBSAXAttributes * This,
             int nIndex,
             BSTR *strURI);
         HRESULT ( STDMETHODCALLTYPE *getLocalName )( 
            IVBSAXAttributes * This,
             int nIndex,
             BSTR *strLocalName);
         HRESULT ( STDMETHODCALLTYPE *getQName )( 
            IVBSAXAttributes * This,
             int nIndex,
             BSTR *strQName);
         HRESULT ( STDMETHODCALLTYPE *getIndexFromName )( 
            IVBSAXAttributes * This,
             BSTR strURI,
             BSTR strLocalName,
             int *nIndex);
         HRESULT ( STDMETHODCALLTYPE *getIndexFromQName )( 
            IVBSAXAttributes * This,
             BSTR strQName,
             int *nIndex);
         HRESULT ( STDMETHODCALLTYPE *getType )( 
            IVBSAXAttributes * This,
             int nIndex,
             BSTR *strType);
         HRESULT ( STDMETHODCALLTYPE *getTypeFromName )( 
            IVBSAXAttributes * This,
             BSTR strURI,
             BSTR strLocalName,
             BSTR *strType);
         HRESULT ( STDMETHODCALLTYPE *getTypeFromQName )( 
            IVBSAXAttributes * This,
             BSTR strQName,
             BSTR *strType);
         HRESULT ( STDMETHODCALLTYPE *getValue )( 
            IVBSAXAttributes * This,
             int nIndex,
             BSTR *strValue);
         HRESULT ( STDMETHODCALLTYPE *getValueFromName )( 
            IVBSAXAttributes * This,
             BSTR strURI,
             BSTR strLocalName,
             BSTR *strValue);
         HRESULT ( STDMETHODCALLTYPE *getValueFromQName )( 
            IVBSAXAttributes * This,
             BSTR strQName,
             BSTR *strValue);
        END_INTERFACE
    } IVBSAXAttributesVtbl;
    interface __uuid("10dc0586-132b-4cac-8bb3-db00ac8b7ee0") IVBSAXAttributes

    {
        CONST_VTBL struct IVBSAXAttributesVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBSAXAttributes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBSAXAttributes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBSAXAttributes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBSAXAttributes_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBSAXAttributes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBSAXAttributes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBSAXAttributes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBSAXAttributes_get_length(This,nLength)	\
    ( (This)->lpVtbl -> get_length(This,nLength) ) 
#define IVBSAXAttributes_getURI(This,nIndex,strURI)	\
    ( (This)->lpVtbl -> getURI(This,nIndex,strURI) ) 
#define IVBSAXAttributes_getLocalName(This,nIndex,strLocalName)	\
    ( (This)->lpVtbl -> getLocalName(This,nIndex,strLocalName) ) 
#define IVBSAXAttributes_getQName(This,nIndex,strQName)	\
    ( (This)->lpVtbl -> getQName(This,nIndex,strQName) ) 
#define IVBSAXAttributes_getIndexFromName(This,strURI,strLocalName,nIndex)	\
    ( (This)->lpVtbl -> getIndexFromName(This,strURI,strLocalName,nIndex) ) 
#define IVBSAXAttributes_getIndexFromQName(This,strQName,nIndex)	\
    ( (This)->lpVtbl -> getIndexFromQName(This,strQName,nIndex) ) 
#define IVBSAXAttributes_getType(This,nIndex,strType)	\
    ( (This)->lpVtbl -> getType(This,nIndex,strType) ) 
#define IVBSAXAttributes_getTypeFromName(This,strURI,strLocalName,strType)	\
    ( (This)->lpVtbl -> getTypeFromName(This,strURI,strLocalName,strType) ) 
#define IVBSAXAttributes_getTypeFromQName(This,strQName,strType)	\
    ( (This)->lpVtbl -> getTypeFromQName(This,strQName,strType) ) 
#define IVBSAXAttributes_getValue(This,nIndex,strValue)	\
    ( (This)->lpVtbl -> getValue(This,nIndex,strValue) ) 
#define IVBSAXAttributes_getValueFromName(This,strURI,strLocalName,strValue)	\
    ( (This)->lpVtbl -> getValueFromName(This,strURI,strLocalName,strValue) ) 
#define IVBSAXAttributes_getValueFromQName(This,strQName,strValue)	\
    ( (This)->lpVtbl -> getValueFromQName(This,strQName,strValue) ) 
#endif 
EXTERN_C const IID IID_IMXWriter;
    typedef struct IMXWriterVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMXWriter * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMXWriter * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMXWriter * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMXWriter * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMXWriter * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMXWriter * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMXWriter * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *put_output )( 
            IMXWriter * This,
             VARIANT varDestination);
         HRESULT ( STDMETHODCALLTYPE *get_output )( 
            IMXWriter * This,
             VARIANT *varDestination);
         HRESULT ( STDMETHODCALLTYPE *put_encoding )( 
            IMXWriter * This,
             BSTR strEncoding);
         HRESULT ( STDMETHODCALLTYPE *get_encoding )( 
            IMXWriter * This,
             BSTR *strEncoding);
         HRESULT ( STDMETHODCALLTYPE *put_byteOrderMark )( 
            IMXWriter * This,
             VARIANT_BOOL fWriteByteOrderMark);
         HRESULT ( STDMETHODCALLTYPE *get_byteOrderMark )( 
            IMXWriter * This,
             VARIANT_BOOL *fWriteByteOrderMark);
         HRESULT ( STDMETHODCALLTYPE *put_indent )( 
            IMXWriter * This,
             VARIANT_BOOL fIndentMode);
         HRESULT ( STDMETHODCALLTYPE *get_indent )( 
            IMXWriter * This,
             VARIANT_BOOL *fIndentMode);
         HRESULT ( STDMETHODCALLTYPE *put_standalone )( 
            IMXWriter * This,
             VARIANT_BOOL fValue);
         HRESULT ( STDMETHODCALLTYPE *get_standalone )( 
            IMXWriter * This,
             VARIANT_BOOL *fValue);
         HRESULT ( STDMETHODCALLTYPE *put_omitXMLDeclaration )( 
            IMXWriter * This,
             VARIANT_BOOL fValue);
         HRESULT ( STDMETHODCALLTYPE *get_omitXMLDeclaration )( 
            IMXWriter * This,
             VARIANT_BOOL *fValue);
         HRESULT ( STDMETHODCALLTYPE *put_version )( 
            IMXWriter * This,
             BSTR strVersion);
         HRESULT ( STDMETHODCALLTYPE *get_version )( 
            IMXWriter * This,
             BSTR *strVersion);
         HRESULT ( STDMETHODCALLTYPE *put_disableOutputEscaping )( 
            IMXWriter * This,
             VARIANT_BOOL fValue);
         HRESULT ( STDMETHODCALLTYPE *get_disableOutputEscaping )( 
            IMXWriter * This,
             VARIANT_BOOL *fValue);
         HRESULT ( STDMETHODCALLTYPE *flush )( 
            IMXWriter * This);
        END_INTERFACE
    } IMXWriterVtbl;
    interface __uuid("4d7ff4ba-1565-4ea8-94e1-6e724a46f98d") IMXWriter

    {
        CONST_VTBL struct IMXWriterVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IMXWriter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMXWriter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IMXWriter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IMXWriter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IMXWriter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IMXWriter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IMXWriter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IMXWriter_put_output(This,varDestination)	\
    ( (This)->lpVtbl -> put_output(This,varDestination) ) 
#define IMXWriter_get_output(This,varDestination)	\
    ( (This)->lpVtbl -> get_output(This,varDestination) ) 
#define IMXWriter_put_encoding(This,strEncoding)	\
    ( (This)->lpVtbl -> put_encoding(This,strEncoding) ) 
#define IMXWriter_get_encoding(This,strEncoding)	\
    ( (This)->lpVtbl -> get_encoding(This,strEncoding) ) 
#define IMXWriter_put_byteOrderMark(This,fWriteByteOrderMark)	\
    ( (This)->lpVtbl -> put_byteOrderMark(This,fWriteByteOrderMark) ) 
#define IMXWriter_get_byteOrderMark(This,fWriteByteOrderMark)	\
    ( (This)->lpVtbl -> get_byteOrderMark(This,fWriteByteOrderMark) ) 
#define IMXWriter_put_indent(This,fIndentMode)	\
    ( (This)->lpVtbl -> put_indent(This,fIndentMode) ) 
#define IMXWriter_get_indent(This,fIndentMode)	\
    ( (This)->lpVtbl -> get_indent(This,fIndentMode) ) 
#define IMXWriter_put_standalone(This,fValue)	\
    ( (This)->lpVtbl -> put_standalone(This,fValue) ) 
#define IMXWriter_get_standalone(This,fValue)	\
    ( (This)->lpVtbl -> get_standalone(This,fValue) ) 
#define IMXWriter_put_omitXMLDeclaration(This,fValue)	\
    ( (This)->lpVtbl -> put_omitXMLDeclaration(This,fValue) ) 
#define IMXWriter_get_omitXMLDeclaration(This,fValue)	\
    ( (This)->lpVtbl -> get_omitXMLDeclaration(This,fValue) ) 
#define IMXWriter_put_version(This,strVersion)	\
    ( (This)->lpVtbl -> put_version(This,strVersion) ) 
#define IMXWriter_get_version(This,strVersion)	\
    ( (This)->lpVtbl -> get_version(This,strVersion) ) 
#define IMXWriter_put_disableOutputEscaping(This,fValue)	\
    ( (This)->lpVtbl -> put_disableOutputEscaping(This,fValue) ) 
#define IMXWriter_get_disableOutputEscaping(This,fValue)	\
    ( (This)->lpVtbl -> get_disableOutputEscaping(This,fValue) ) 
#define IMXWriter_flush(This)	\
    ( (This)->lpVtbl -> flush(This) ) 
#endif 
EXTERN_C const IID IID_IMXAttributes;
    typedef struct IMXAttributesVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMXAttributes * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMXAttributes * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMXAttributes * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMXAttributes * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMXAttributes * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMXAttributes * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMXAttributes * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *addAttribute )( 
            IMXAttributes * This,
             BSTR strURI,
             BSTR strLocalName,
             BSTR strQName,
             BSTR strType,
             BSTR strValue);
         HRESULT ( STDMETHODCALLTYPE *addAttributeFromIndex )( 
            IMXAttributes * This,
             VARIANT varAtts,
             int nIndex);
         HRESULT ( STDMETHODCALLTYPE *clear )( 
            IMXAttributes * This);
         HRESULT ( STDMETHODCALLTYPE *removeAttribute )( 
            IMXAttributes * This,
             int nIndex);
         HRESULT ( STDMETHODCALLTYPE *setAttribute )( 
            IMXAttributes * This,
             int nIndex,
             BSTR strURI,
             BSTR strLocalName,
             BSTR strQName,
             BSTR strType,
             BSTR strValue);
         HRESULT ( STDMETHODCALLTYPE *setAttributes )( 
            IMXAttributes * This,
             VARIANT varAtts);
         HRESULT ( STDMETHODCALLTYPE *setLocalName )( 
            IMXAttributes * This,
             int nIndex,
             BSTR strLocalName);
         HRESULT ( STDMETHODCALLTYPE *setQName )( 
            IMXAttributes * This,
             int nIndex,
             BSTR strQName);
         HRESULT ( STDMETHODCALLTYPE *setType )( 
            IMXAttributes * This,
             int nIndex,
             BSTR strType);
         HRESULT ( STDMETHODCALLTYPE *setURI )( 
            IMXAttributes * This,
             int nIndex,
             BSTR strURI);
         HRESULT ( STDMETHODCALLTYPE *setValue )( 
            IMXAttributes * This,
             int nIndex,
             BSTR strValue);
        END_INTERFACE
    } IMXAttributesVtbl;
    interface __uuid("f10d27cc-3ec0-415c-8ed8-77ab1c5e7262") IMXAttributes

    {
        CONST_VTBL struct IMXAttributesVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IMXAttributes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMXAttributes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IMXAttributes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IMXAttributes_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IMXAttributes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IMXAttributes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IMXAttributes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IMXAttributes_addAttribute(This,strURI,strLocalName,strQName,strType,strValue)	\
    ( (This)->lpVtbl -> addAttribute(This,strURI,strLocalName,strQName,strType,strValue) ) 
#define IMXAttributes_addAttributeFromIndex(This,varAtts,nIndex)	\
    ( (This)->lpVtbl -> addAttributeFromIndex(This,varAtts,nIndex) ) 
#define IMXAttributes_clear(This)	\
    ( (This)->lpVtbl -> clear(This) ) 
#define IMXAttributes_removeAttribute(This,nIndex)	\
    ( (This)->lpVtbl -> removeAttribute(This,nIndex) ) 
#define IMXAttributes_setAttribute(This,nIndex,strURI,strLocalName,strQName,strType,strValue)	\
    ( (This)->lpVtbl -> setAttribute(This,nIndex,strURI,strLocalName,strQName,strType,strValue) ) 
#define IMXAttributes_setAttributes(This,varAtts)	\
    ( (This)->lpVtbl -> setAttributes(This,varAtts) ) 
#define IMXAttributes_setLocalName(This,nIndex,strLocalName)	\
    ( (This)->lpVtbl -> setLocalName(This,nIndex,strLocalName) ) 
#define IMXAttributes_setQName(This,nIndex,strQName)	\
    ( (This)->lpVtbl -> setQName(This,nIndex,strQName) ) 
#define IMXAttributes_setType(This,nIndex,strType)	\
    ( (This)->lpVtbl -> setType(This,nIndex,strType) ) 
#define IMXAttributes_setURI(This,nIndex,strURI)	\
    ( (This)->lpVtbl -> setURI(This,nIndex,strURI) ) 
#define IMXAttributes_setValue(This,nIndex,strValue)	\
    ( (This)->lpVtbl -> setValue(This,nIndex,strValue) ) 
#endif 
EXTERN_C const IID IID_IMXReaderControl;
    typedef struct IMXReaderControlVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMXReaderControl * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMXReaderControl * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMXReaderControl * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMXReaderControl * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMXReaderControl * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMXReaderControl * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMXReaderControl * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *abort )( 
            IMXReaderControl * This);
         HRESULT ( STDMETHODCALLTYPE *resume )( 
            IMXReaderControl * This);
         HRESULT ( STDMETHODCALLTYPE *suspend )( 
            IMXReaderControl * This);
        END_INTERFACE
    } IMXReaderControlVtbl;
    interface __uuid("808f4e35-8d5a-4fbe-8466-33a41279ed30") IMXReaderControl

    {
        CONST_VTBL struct IMXReaderControlVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IMXReaderControl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMXReaderControl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IMXReaderControl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IMXReaderControl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IMXReaderControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IMXReaderControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IMXReaderControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IMXReaderControl_abort(This)	\
    ( (This)->lpVtbl -> abort(This) ) 
#define IMXReaderControl_resume(This)	\
    ( (This)->lpVtbl -> resume(This) ) 
#define IMXReaderControl_suspend(This)	\
    ( (This)->lpVtbl -> suspend(This) ) 
#endif 
EXTERN_C const IID IID_IMXSchemaDeclHandler;
    typedef struct IMXSchemaDeclHandlerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMXSchemaDeclHandler * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMXSchemaDeclHandler * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMXSchemaDeclHandler * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMXSchemaDeclHandler * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMXSchemaDeclHandler * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMXSchemaDeclHandler * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMXSchemaDeclHandler * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *schemaElementDecl )( 
            IMXSchemaDeclHandler * This,
             ISchemaElement *oSchemaElement);
        END_INTERFACE
    } IMXSchemaDeclHandlerVtbl;
    interface __uuid("fa4bb38c-faf9-4cca-9302-d1dd0fe520db") IMXSchemaDeclHandler

    {
        CONST_VTBL struct IMXSchemaDeclHandlerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IMXSchemaDeclHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMXSchemaDeclHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IMXSchemaDeclHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IMXSchemaDeclHandler_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IMXSchemaDeclHandler_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IMXSchemaDeclHandler_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IMXSchemaDeclHandler_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IMXSchemaDeclHandler_schemaElementDecl(This,oSchemaElement)	\
    ( (This)->lpVtbl -> schemaElementDecl(This,oSchemaElement) ) 
#endif 
EXTERN_C const IID IID_IMXNamespacePrefixes;
    typedef struct IMXNamespacePrefixesVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMXNamespacePrefixes * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMXNamespacePrefixes * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMXNamespacePrefixes * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMXNamespacePrefixes * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMXNamespacePrefixes * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMXNamespacePrefixes * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMXNamespacePrefixes * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_item )( 
            IMXNamespacePrefixes * This,
             long index,
             BSTR *prefix);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IMXNamespacePrefixes * This,
             long *length);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IMXNamespacePrefixes * This,
             IUnknown **ppUnk);
        END_INTERFACE
    } IMXNamespacePrefixesVtbl;
    interface __uuid("c90352f4-643c-4fbc-bb23-e996eb2d51fd") IMXNamespacePrefixes

    {
        CONST_VTBL struct IMXNamespacePrefixesVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IMXNamespacePrefixes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMXNamespacePrefixes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IMXNamespacePrefixes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IMXNamespacePrefixes_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IMXNamespacePrefixes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IMXNamespacePrefixes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IMXNamespacePrefixes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IMXNamespacePrefixes_get_item(This,index,prefix)	\
    ( (This)->lpVtbl -> get_item(This,index,prefix) ) 
#define IMXNamespacePrefixes_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define IMXNamespacePrefixes_get__newEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppUnk) ) 
#endif 
EXTERN_C const IID IID_IVBMXNamespaceManager;
    typedef struct IVBMXNamespaceManagerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVBMXNamespaceManager * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVBMXNamespaceManager * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVBMXNamespaceManager * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVBMXNamespaceManager * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVBMXNamespaceManager * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVBMXNamespaceManager * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVBMXNamespaceManager * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *put_allowOverride )( 
            IVBMXNamespaceManager * This,
             VARIANT_BOOL fOverride);
         HRESULT ( STDMETHODCALLTYPE *get_allowOverride )( 
            IVBMXNamespaceManager * This,
             VARIANT_BOOL *fOverride);
         HRESULT ( STDMETHODCALLTYPE *reset )( 
            IVBMXNamespaceManager * This);
         HRESULT ( STDMETHODCALLTYPE *pushContext )( 
            IVBMXNamespaceManager * This);
         HRESULT ( STDMETHODCALLTYPE *pushNodeContext )( 
            IVBMXNamespaceManager * This,
             IXMLDOMNode *contextNode,
             VARIANT_BOOL fDeep);
         HRESULT ( STDMETHODCALLTYPE *popContext )( 
            IVBMXNamespaceManager * This);
         HRESULT ( STDMETHODCALLTYPE *declarePrefix )( 
            IVBMXNamespaceManager * This,
             BSTR prefix,
             BSTR namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *getDeclaredPrefixes )( 
            IVBMXNamespaceManager * This,
             IMXNamespacePrefixes **prefixes);
         HRESULT ( STDMETHODCALLTYPE *getPrefixes )( 
            IVBMXNamespaceManager * This,
             BSTR namespaceURI,
             IMXNamespacePrefixes **prefixes);
         HRESULT ( STDMETHODCALLTYPE *getURI )( 
            IVBMXNamespaceManager * This,
             BSTR prefix,
             VARIANT *uri);
         HRESULT ( STDMETHODCALLTYPE *getURIFromNode )( 
            IVBMXNamespaceManager * This,
             BSTR strPrefix,
             IXMLDOMNode *contextNode,
             VARIANT *uri);
        END_INTERFACE
    } IVBMXNamespaceManagerVtbl;
    interface __uuid("c90352f5-643c-4fbc-bb23-e996eb2d51fd") IVBMXNamespaceManager

    {
        CONST_VTBL struct IVBMXNamespaceManagerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IVBMXNamespaceManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IVBMXNamespaceManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IVBMXNamespaceManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IVBMXNamespaceManager_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IVBMXNamespaceManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IVBMXNamespaceManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IVBMXNamespaceManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IVBMXNamespaceManager_put_allowOverride(This,fOverride)	\
    ( (This)->lpVtbl -> put_allowOverride(This,fOverride) ) 
#define IVBMXNamespaceManager_get_allowOverride(This,fOverride)	\
    ( (This)->lpVtbl -> get_allowOverride(This,fOverride) ) 
#define IVBMXNamespaceManager_reset(This)	\
    ( (This)->lpVtbl -> reset(This) ) 
#define IVBMXNamespaceManager_pushContext(This)	\
    ( (This)->lpVtbl -> pushContext(This) ) 
#define IVBMXNamespaceManager_pushNodeContext(This,contextNode,fDeep)	\
    ( (This)->lpVtbl -> pushNodeContext(This,contextNode,fDeep) ) 
#define IVBMXNamespaceManager_popContext(This)	\
    ( (This)->lpVtbl -> popContext(This) ) 
#define IVBMXNamespaceManager_declarePrefix(This,prefix,namespaceURI)	\
    ( (This)->lpVtbl -> declarePrefix(This,prefix,namespaceURI) ) 
#define IVBMXNamespaceManager_getDeclaredPrefixes(This,prefixes)	\
    ( (This)->lpVtbl -> getDeclaredPrefixes(This,prefixes) ) 
#define IVBMXNamespaceManager_getPrefixes(This,namespaceURI,prefixes)	\
    ( (This)->lpVtbl -> getPrefixes(This,namespaceURI,prefixes) ) 
#define IVBMXNamespaceManager_getURI(This,prefix,uri)	\
    ( (This)->lpVtbl -> getURI(This,prefix,uri) ) 
#define IVBMXNamespaceManager_getURIFromNode(This,strPrefix,contextNode,uri)	\
    ( (This)->lpVtbl -> getURIFromNode(This,strPrefix,contextNode,uri) ) 
#endif 
EXTERN_C const IID IID_IMXNamespaceManager;
    typedef struct IMXNamespaceManagerVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMXNamespaceManager * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMXNamespaceManager * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMXNamespaceManager * This);
        HRESULT ( STDMETHODCALLTYPE *putAllowOverride )( 
            IMXNamespaceManager * This,
             VARIANT_BOOL fOverride);
        HRESULT ( STDMETHODCALLTYPE *getAllowOverride )( 
            IMXNamespaceManager * This,
             VARIANT_BOOL *fOverride);
        HRESULT ( STDMETHODCALLTYPE *reset )( 
            IMXNamespaceManager * This);
        HRESULT ( STDMETHODCALLTYPE *pushContext )( 
            IMXNamespaceManager * This);
        HRESULT ( STDMETHODCALLTYPE *pushNodeContext )( 
            IMXNamespaceManager * This,
             IXMLDOMNode *contextNode,
             VARIANT_BOOL fDeep);
        HRESULT ( STDMETHODCALLTYPE *popContext )( 
            IMXNamespaceManager * This);
        HRESULT ( STDMETHODCALLTYPE *declarePrefix )( 
            IMXNamespaceManager * This,
             LPCWSTR prefix,
             LPCWSTR namespaceURI);
        HRESULT ( STDMETHODCALLTYPE *getDeclaredPrefix )( 
            IMXNamespaceManager * This,
             long nIndex,
             wchar_t *pwchPrefix,
             int *pcchPrefix);
        HRESULT ( STDMETHODCALLTYPE *getPrefix )( 
            IMXNamespaceManager * This,
             LPCWSTR pwszNamespaceURI,
             long nIndex,
             wchar_t *pwchPrefix,
             int *pcchPrefix);
        HRESULT ( STDMETHODCALLTYPE *getURI )( 
            IMXNamespaceManager * This,
             LPCWSTR pwchPrefix,
             IXMLDOMNode *pContextNode,
             wchar_t *pwchUri,
             int *pcchUri);
        END_INTERFACE
    } IMXNamespaceManagerVtbl;
    interface __uuid("c90352f6-643c-4fbc-bb23-e996eb2d51fd") IMXNamespaceManager

    {
        CONST_VTBL struct IMXNamespaceManagerVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IMXNamespaceManager_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMXNamespaceManager_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IMXNamespaceManager_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IMXNamespaceManager_putAllowOverride(This,fOverride)	\
    ( (This)->lpVtbl -> putAllowOverride(This,fOverride) ) 
#define IMXNamespaceManager_getAllowOverride(This,fOverride)	\
    ( (This)->lpVtbl -> getAllowOverride(This,fOverride) ) 
#define IMXNamespaceManager_reset(This)	\
    ( (This)->lpVtbl -> reset(This) ) 
#define IMXNamespaceManager_pushContext(This)	\
    ( (This)->lpVtbl -> pushContext(This) ) 
#define IMXNamespaceManager_pushNodeContext(This,contextNode,fDeep)	\
    ( (This)->lpVtbl -> pushNodeContext(This,contextNode,fDeep) ) 
#define IMXNamespaceManager_popContext(This)	\
    ( (This)->lpVtbl -> popContext(This) ) 
#define IMXNamespaceManager_declarePrefix(This,prefix,namespaceURI)	\
    ( (This)->lpVtbl -> declarePrefix(This,prefix,namespaceURI) ) 
#define IMXNamespaceManager_getDeclaredPrefix(This,nIndex,pwchPrefix,pcchPrefix)	\
    ( (This)->lpVtbl -> getDeclaredPrefix(This,nIndex,pwchPrefix,pcchPrefix) ) 
#define IMXNamespaceManager_getPrefix(This,pwszNamespaceURI,nIndex,pwchPrefix,pcchPrefix)	\
    ( (This)->lpVtbl -> getPrefix(This,pwszNamespaceURI,nIndex,pwchPrefix,pcchPrefix) ) 
#define IMXNamespaceManager_getURI(This,pwchPrefix,pContextNode,pwchUri,pcchUri)	\
    ( (This)->lpVtbl -> getURI(This,pwchPrefix,pContextNode,pwchUri,pcchUri) ) 
#endif 
EXTERN_C const IID IID_IMXXMLFilter;
    typedef struct IMXXMLFilterVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMXXMLFilter * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMXXMLFilter * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMXXMLFilter * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMXXMLFilter * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMXXMLFilter * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMXXMLFilter * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMXXMLFilter * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *getFeature )( 
            IMXXMLFilter * This,
             BSTR strName,
             VARIANT_BOOL *fValue);
         HRESULT ( STDMETHODCALLTYPE *putFeature )( 
            IMXXMLFilter * This,
             BSTR strName,
             VARIANT_BOOL fValue);
         HRESULT ( STDMETHODCALLTYPE *getProperty )( 
            IMXXMLFilter * This,
             BSTR strName,
             VARIANT *varValue);
         HRESULT ( STDMETHODCALLTYPE *putProperty )( 
            IMXXMLFilter * This,
             BSTR strName,
             VARIANT varValue);
         HRESULT ( STDMETHODCALLTYPE *get_entityResolver )( 
            IMXXMLFilter * This,
             IUnknown **oResolver);
         HRESULT ( STDMETHODCALLTYPE *putref_entityResolver )( 
            IMXXMLFilter * This,
             IUnknown *oResolver);
         HRESULT ( STDMETHODCALLTYPE *get_contentHandler )( 
            IMXXMLFilter * This,
             IUnknown **oHandler);
         HRESULT ( STDMETHODCALLTYPE *putref_contentHandler )( 
            IMXXMLFilter * This,
             IUnknown *oHandler);
         HRESULT ( STDMETHODCALLTYPE *get_dtdHandler )( 
            IMXXMLFilter * This,
             IUnknown **oHandler);
         HRESULT ( STDMETHODCALLTYPE *putref_dtdHandler )( 
            IMXXMLFilter * This,
             IUnknown *oHandler);
         HRESULT ( STDMETHODCALLTYPE *get_errorHandler )( 
            IMXXMLFilter * This,
             IUnknown **oHandler);
         HRESULT ( STDMETHODCALLTYPE *putref_errorHandler )( 
            IMXXMLFilter * This,
             IUnknown *oHandler);
        END_INTERFACE
    } IMXXMLFilterVtbl;
    interface __uuid("c90352f7-643c-4fbc-bb23-e996eb2d51fd") IMXXMLFilter

    {
        CONST_VTBL struct IMXXMLFilterVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IMXXMLFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IMXXMLFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IMXXMLFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IMXXMLFilter_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IMXXMLFilter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IMXXMLFilter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IMXXMLFilter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IMXXMLFilter_getFeature(This,strName,fValue)	\
    ( (This)->lpVtbl -> getFeature(This,strName,fValue) ) 
#define IMXXMLFilter_putFeature(This,strName,fValue)	\
    ( (This)->lpVtbl -> putFeature(This,strName,fValue) ) 
#define IMXXMLFilter_getProperty(This,strName,varValue)	\
    ( (This)->lpVtbl -> getProperty(This,strName,varValue) ) 
#define IMXXMLFilter_putProperty(This,strName,varValue)	\
    ( (This)->lpVtbl -> putProperty(This,strName,varValue) ) 
#define IMXXMLFilter_get_entityResolver(This,oResolver)	\
    ( (This)->lpVtbl -> get_entityResolver(This,oResolver) ) 
#define IMXXMLFilter_putref_entityResolver(This,oResolver)	\
    ( (This)->lpVtbl -> putref_entityResolver(This,oResolver) ) 
#define IMXXMLFilter_get_contentHandler(This,oHandler)	\
    ( (This)->lpVtbl -> get_contentHandler(This,oHandler) ) 
#define IMXXMLFilter_putref_contentHandler(This,oHandler)	\
    ( (This)->lpVtbl -> putref_contentHandler(This,oHandler) ) 
#define IMXXMLFilter_get_dtdHandler(This,oHandler)	\
    ( (This)->lpVtbl -> get_dtdHandler(This,oHandler) ) 
#define IMXXMLFilter_putref_dtdHandler(This,oHandler)	\
    ( (This)->lpVtbl -> putref_dtdHandler(This,oHandler) ) 
#define IMXXMLFilter_get_errorHandler(This,oHandler)	\
    ( (This)->lpVtbl -> get_errorHandler(This,oHandler) ) 
#define IMXXMLFilter_putref_errorHandler(This,oHandler)	\
    ( (This)->lpVtbl -> putref_errorHandler(This,oHandler) ) 
#endif 
#if !defined(__msxml_som_enums__)
#define __msxml_som_enums__
typedef  
enum _SOMITEMTYPE
    {
        SOMITEM_SCHEMA	= 0x1000,
        SOMITEM_ATTRIBUTE	= 0x1001,
        SOMITEM_ATTRIBUTEGROUP	= 0x1002,
        SOMITEM_NOTATION	= 0x1003,
        SOMITEM_ANNOTATION	= 0x1004,
        SOMITEM_IDENTITYCONSTRAINT	= 0x1100,
        SOMITEM_KEY	= 0x1101,
        SOMITEM_KEYREF	= 0x1102,
        SOMITEM_UNIQUE	= 0x1103,
        SOMITEM_ANYTYPE	= 0x2000,
        SOMITEM_DATATYPE	= 0x2100,
        SOMITEM_DATATYPE_ANYTYPE	= 0x2101,
        SOMITEM_DATATYPE_ANYURI	= 0x2102,
        SOMITEM_DATATYPE_BASE64BINARY	= 0x2103,
        SOMITEM_DATATYPE_BOOLEAN	= 0x2104,
        SOMITEM_DATATYPE_BYTE	= 0x2105,
        SOMITEM_DATATYPE_DATE	= 0x2106,
        SOMITEM_DATATYPE_DATETIME	= 0x2107,
        SOMITEM_DATATYPE_DAY	= 0x2108,
        SOMITEM_DATATYPE_DECIMAL	= 0x2109,
        SOMITEM_DATATYPE_DOUBLE	= 0x210a,
        SOMITEM_DATATYPE_DURATION	= 0x210b,
        SOMITEM_DATATYPE_ENTITIES	= 0x210c,
        SOMITEM_DATATYPE_ENTITY	= 0x210d,
        SOMITEM_DATATYPE_FLOAT	= 0x210e,
        SOMITEM_DATATYPE_HEXBINARY	= 0x210f,
        SOMITEM_DATATYPE_ID	= 0x2110,
        SOMITEM_DATATYPE_IDREF	= 0x2111,
        SOMITEM_DATATYPE_IDREFS	= 0x2112,
        SOMITEM_DATATYPE_INT	= 0x2113,
        SOMITEM_DATATYPE_INTEGER	= 0x2114,
        SOMITEM_DATATYPE_LANGUAGE	= 0x2115,
        SOMITEM_DATATYPE_LONG	= 0x2116,
        SOMITEM_DATATYPE_MONTH	= 0x2117,
        SOMITEM_DATATYPE_MONTHDAY	= 0x2118,
        SOMITEM_DATATYPE_NAME	= 0x2119,
        SOMITEM_DATATYPE_NCNAME	= 0x211a,
        SOMITEM_DATATYPE_NEGATIVEINTEGER	= 0x211b,
        SOMITEM_DATATYPE_NMTOKEN	= 0x211c,
        SOMITEM_DATATYPE_NMTOKENS	= 0x211d,
        SOMITEM_DATATYPE_NONNEGATIVEINTEGER	= 0x211e,
        SOMITEM_DATATYPE_NONPOSITIVEINTEGER	= 0x211f,
        SOMITEM_DATATYPE_NORMALIZEDSTRING	= 0x2120,
        SOMITEM_DATATYPE_NOTATION	= 0x2121,
        SOMITEM_DATATYPE_POSITIVEINTEGER	= 0x2122,
        SOMITEM_DATATYPE_QNAME	= 0x2123,
        SOMITEM_DATATYPE_SHORT	= 0x2124,
        SOMITEM_DATATYPE_STRING	= 0x2125,
        SOMITEM_DATATYPE_TIME	= 0x2126,
        SOMITEM_DATATYPE_TOKEN	= 0x2127,
        SOMITEM_DATATYPE_UNSIGNEDBYTE	= 0x2128,
        SOMITEM_DATATYPE_UNSIGNEDINT	= 0x2129,
        SOMITEM_DATATYPE_UNSIGNEDLONG	= 0x212a,
        SOMITEM_DATATYPE_UNSIGNEDSHORT	= 0x212b,
        SOMITEM_DATATYPE_YEAR	= 0x212c,
        SOMITEM_DATATYPE_YEARMONTH	= 0x212d,
        SOMITEM_DATATYPE_ANYSIMPLETYPE	= 0x21ff,
        SOMITEM_SIMPLETYPE	= 0x2200,
        SOMITEM_COMPLEXTYPE	= 0x2400,
        SOMITEM_PARTICLE	= 0x4000,
        SOMITEM_ANY	= 0x4001,
        SOMITEM_ANYATTRIBUTE	= 0x4002,
        SOMITEM_ELEMENT	= 0x4003,
        SOMITEM_GROUP	= 0x4100,
        SOMITEM_ALL	= 0x4101,
        SOMITEM_CHOICE	= 0x4102,
        SOMITEM_SEQUENCE	= 0x4103,
        SOMITEM_EMPTYPARTICLE	= 0x4104,
        SOMITEM_NULL	= 0x800,
        SOMITEM_NULL_TYPE	= 0x2800,
        SOMITEM_NULL_ANY	= 0x4801,
        SOMITEM_NULL_ANYATTRIBUTE	= 0x4802,
        SOMITEM_NULL_ELEMENT	= 0x4803
    } 	SOMITEMTYPE;
typedef  
enum _SCHEMAUSE
    {
        SCHEMAUSE_OPTIONAL	= 0,
        SCHEMAUSE_PROHIBITED	= ( SCHEMAUSE_OPTIONAL + 1 ) ,
        SCHEMAUSE_REQUIRED	= ( SCHEMAUSE_PROHIBITED + 1 ) 
    } 	SCHEMAUSE;
typedef  
enum _SCHEMADERIVATIONMETHOD
    {
        SCHEMADERIVATIONMETHOD_EMPTY	= 0,
        SCHEMADERIVATIONMETHOD_SUBSTITUTION	= 0x1,
        SCHEMADERIVATIONMETHOD_EXTENSION	= 0x2,
        SCHEMADERIVATIONMETHOD_RESTRICTION	= 0x4,
        SCHEMADERIVATIONMETHOD_LIST	= 0x8,
        SCHEMADERIVATIONMETHOD_UNION	= 0x10,
        SCHEMADERIVATIONMETHOD_ALL	= 0xff,
        SCHEMADERIVATIONMETHOD_NONE	= 0x100
    } 	SCHEMADERIVATIONMETHOD;
typedef  
enum _SCHEMACONTENTTYPE
    {
        SCHEMACONTENTTYPE_EMPTY	= 0,
        SCHEMACONTENTTYPE_TEXTONLY	= ( SCHEMACONTENTTYPE_EMPTY + 1 ) ,
        SCHEMACONTENTTYPE_ELEMENTONLY	= ( SCHEMACONTENTTYPE_TEXTONLY + 1 ) ,
        SCHEMACONTENTTYPE_MIXED	= ( SCHEMACONTENTTYPE_ELEMENTONLY + 1 ) 
    } 	SCHEMACONTENTTYPE;
typedef  
enum _SCHEMAPROCESSCONTENTS
    {
        SCHEMAPROCESSCONTENTS_NONE	= 0,
        SCHEMAPROCESSCONTENTS_SKIP	= ( SCHEMAPROCESSCONTENTS_NONE + 1 ) ,
        SCHEMAPROCESSCONTENTS_LAX	= ( SCHEMAPROCESSCONTENTS_SKIP + 1 ) ,
        SCHEMAPROCESSCONTENTS_STRICT	= ( SCHEMAPROCESSCONTENTS_LAX + 1 ) 
    } 	SCHEMAPROCESSCONTENTS;
typedef  
enum _SCHEMAWHITESPACE
    {
        SCHEMAWHITESPACE_NONE	= -1,
        SCHEMAWHITESPACE_PRESERVE	= 0,
        SCHEMAWHITESPACE_REPLACE	= 1,
        SCHEMAWHITESPACE_COLLAPSE	= 2
    } 	SCHEMAWHITESPACE;
typedef  
enum _SCHEMATYPEVARIETY
    {
        SCHEMATYPEVARIETY_NONE	= -1,
        SCHEMATYPEVARIETY_ATOMIC	= 0,
        SCHEMATYPEVARIETY_LIST	= 1,
        SCHEMATYPEVARIETY_UNION	= 2
    } 	SCHEMATYPEVARIETY;
#endif 
EXTERN_C const IID IID_IXMLDOMSchemaCollection2;
    typedef struct IXMLDOMSchemaCollection2Vtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLDOMSchemaCollection2 * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLDOMSchemaCollection2 * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLDOMSchemaCollection2 * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IXMLDOMSchemaCollection2 * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IXMLDOMSchemaCollection2 * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IXMLDOMSchemaCollection2 * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IXMLDOMSchemaCollection2 * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *add )( 
            IXMLDOMSchemaCollection2 * This,
             BSTR namespaceURI,
             VARIANT var);
         HRESULT ( STDMETHODCALLTYPE *get )( 
            IXMLDOMSchemaCollection2 * This,
             BSTR namespaceURI,
             IXMLDOMNode **schemaNode);
         HRESULT ( STDMETHODCALLTYPE *remove )( 
            IXMLDOMSchemaCollection2 * This,
             BSTR namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            IXMLDOMSchemaCollection2 * This,
             long *length);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            IXMLDOMSchemaCollection2 * This,
             long index,
             BSTR *length);
         HRESULT ( STDMETHODCALLTYPE *addCollection )( 
            IXMLDOMSchemaCollection2 * This,
             IXMLDOMSchemaCollection *otherCollection);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            IXMLDOMSchemaCollection2 * This,
             IUnknown **ppUnk);
         HRESULT ( STDMETHODCALLTYPE *validate )( 
            IXMLDOMSchemaCollection2 * This);
         HRESULT ( STDMETHODCALLTYPE *put_validateOnLoad )( 
            IXMLDOMSchemaCollection2 * This,
             VARIANT_BOOL validateOnLoad);
         HRESULT ( STDMETHODCALLTYPE *get_validateOnLoad )( 
            IXMLDOMSchemaCollection2 * This,
             VARIANT_BOOL *validateOnLoad);
         HRESULT ( STDMETHODCALLTYPE *getSchema )( 
            IXMLDOMSchemaCollection2 * This,
             BSTR namespaceURI,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *getDeclaration )( 
            IXMLDOMSchemaCollection2 * This,
             IXMLDOMNode *node,
             ISchemaItem **item);
        END_INTERFACE
    } IXMLDOMSchemaCollection2Vtbl;
    interface __uuid("50ea08b0-dd1b-4664-9a50-c2f40f4bd79a") IXMLDOMSchemaCollection2

    {
        CONST_VTBL struct IXMLDOMSchemaCollection2Vtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLDOMSchemaCollection2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLDOMSchemaCollection2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLDOMSchemaCollection2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLDOMSchemaCollection2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define IXMLDOMSchemaCollection2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define IXMLDOMSchemaCollection2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define IXMLDOMSchemaCollection2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define IXMLDOMSchemaCollection2_add(This,namespaceURI,var)	\
    ( (This)->lpVtbl -> add(This,namespaceURI,var) ) 
#define IXMLDOMSchemaCollection2_get(This,namespaceURI,schemaNode)	\
    ( (This)->lpVtbl -> get(This,namespaceURI,schemaNode) ) 
#define IXMLDOMSchemaCollection2_remove(This,namespaceURI)	\
    ( (This)->lpVtbl -> remove(This,namespaceURI) ) 
#define IXMLDOMSchemaCollection2_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define IXMLDOMSchemaCollection2_get_namespaceURI(This,index,length)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,index,length) ) 
#define IXMLDOMSchemaCollection2_addCollection(This,otherCollection)	\
    ( (This)->lpVtbl -> addCollection(This,otherCollection) ) 
#define IXMLDOMSchemaCollection2_get__newEnum(This,ppUnk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppUnk) ) 
#define IXMLDOMSchemaCollection2_validate(This)	\
    ( (This)->lpVtbl -> validate(This) ) 
#define IXMLDOMSchemaCollection2_put_validateOnLoad(This,validateOnLoad)	\
    ( (This)->lpVtbl -> put_validateOnLoad(This,validateOnLoad) ) 
#define IXMLDOMSchemaCollection2_get_validateOnLoad(This,validateOnLoad)	\
    ( (This)->lpVtbl -> get_validateOnLoad(This,validateOnLoad) ) 
#define IXMLDOMSchemaCollection2_getSchema(This,namespaceURI,schema)	\
    ( (This)->lpVtbl -> getSchema(This,namespaceURI,schema) ) 
#define IXMLDOMSchemaCollection2_getDeclaration(This,node,item)	\
    ( (This)->lpVtbl -> getDeclaration(This,node,item) ) 
#endif 
EXTERN_C const IID IID_ISchemaStringCollection;
    typedef struct ISchemaStringCollectionVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaStringCollection * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaStringCollection * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaStringCollection * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaStringCollection * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaStringCollection * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaStringCollection * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaStringCollection * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_item )( 
            ISchemaStringCollection * This,
             long index,
             BSTR *bstr);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            ISchemaStringCollection * This,
             long *length);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            ISchemaStringCollection * This,
             IUnknown **ppunk);
        END_INTERFACE
    } ISchemaStringCollectionVtbl;
    interface __uuid("50ea08b1-dd1b-4664-9a50-c2f40f4bd79a") ISchemaStringCollection

    {
        CONST_VTBL struct ISchemaStringCollectionVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaStringCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaStringCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaStringCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaStringCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaStringCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaStringCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaStringCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaStringCollection_get_item(This,index,bstr)	\
    ( (This)->lpVtbl -> get_item(This,index,bstr) ) 
#define ISchemaStringCollection_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define ISchemaStringCollection_get__newEnum(This,ppunk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppunk) ) 
#endif 
EXTERN_C const IID IID_ISchemaItemCollection;
    typedef struct ISchemaItemCollectionVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaItemCollection * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaItemCollection * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaItemCollection * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaItemCollection * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaItemCollection * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaItemCollection * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaItemCollection * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_item )( 
            ISchemaItemCollection * This,
             long index,
             ISchemaItem **item);
         HRESULT ( STDMETHODCALLTYPE *itemByName )( 
            ISchemaItemCollection * This,
             BSTR name,
             ISchemaItem **item);
         HRESULT ( STDMETHODCALLTYPE *itemByQName )( 
            ISchemaItemCollection * This,
             BSTR name,
             BSTR namespaceURI,
             ISchemaItem **item);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            ISchemaItemCollection * This,
             long *length);
         HRESULT ( STDMETHODCALLTYPE *get__newEnum )( 
            ISchemaItemCollection * This,
             IUnknown **ppunk);
        END_INTERFACE
    } ISchemaItemCollectionVtbl;
    interface __uuid("50ea08b2-dd1b-4664-9a50-c2f40f4bd79a") ISchemaItemCollection

    {
        CONST_VTBL struct ISchemaItemCollectionVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaItemCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaItemCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaItemCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaItemCollection_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaItemCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaItemCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaItemCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaItemCollection_get_item(This,index,item)	\
    ( (This)->lpVtbl -> get_item(This,index,item) ) 
#define ISchemaItemCollection_itemByName(This,name,item)	\
    ( (This)->lpVtbl -> itemByName(This,name,item) ) 
#define ISchemaItemCollection_itemByQName(This,name,namespaceURI,item)	\
    ( (This)->lpVtbl -> itemByQName(This,name,namespaceURI,item) ) 
#define ISchemaItemCollection_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define ISchemaItemCollection_get__newEnum(This,ppunk)	\
    ( (This)->lpVtbl -> get__newEnum(This,ppunk) ) 
#endif 
EXTERN_C const IID IID_ISchemaItem;
    typedef struct ISchemaItemVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaItem * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaItem * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaItem * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaItem * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaItem * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaItem * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaItem * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaItem * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaItem * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaItem * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaItem * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaItem * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaItem * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaItem * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
        END_INTERFACE
    } ISchemaItemVtbl;
    interface __uuid("50ea08b3-dd1b-4664-9a50-c2f40f4bd79a") ISchemaItem

    {
        CONST_VTBL struct ISchemaItemVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaItem_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaItem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaItem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaItem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaItem_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaItem_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaItem_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaItem_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaItem_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaItem_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaItem_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#endif 
EXTERN_C const IID IID_ISchema;
    typedef struct ISchemaVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchema * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchema * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchema * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchema * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchema * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchema * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchema * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchema * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchema * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchema * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchema * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchema * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchema * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchema * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_targetNamespace )( 
            ISchema * This,
             BSTR *targetNamespace);
         HRESULT ( STDMETHODCALLTYPE *get_version )( 
            ISchema * This,
             BSTR *version);
         HRESULT ( STDMETHODCALLTYPE *get_types )( 
            ISchema * This,
             ISchemaItemCollection **types);
         HRESULT ( STDMETHODCALLTYPE *get_elements )( 
            ISchema * This,
             ISchemaItemCollection **elements);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            ISchema * This,
             ISchemaItemCollection **attributes);
         HRESULT ( STDMETHODCALLTYPE *get_attributeGroups )( 
            ISchema * This,
             ISchemaItemCollection **attributeGroups);
         HRESULT ( STDMETHODCALLTYPE *get_modelGroups )( 
            ISchema * This,
             ISchemaItemCollection **modelGroups);
         HRESULT ( STDMETHODCALLTYPE *get_notations )( 
            ISchema * This,
             ISchemaItemCollection **notations);
         HRESULT ( STDMETHODCALLTYPE *get_schemaLocations )( 
            ISchema * This,
             ISchemaStringCollection **schemaLocations);
        END_INTERFACE
    } ISchemaVtbl;
    interface __uuid("50ea08b4-dd1b-4664-9a50-c2f40f4bd79a") ISchema

    {
        CONST_VTBL struct ISchemaVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchema_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchema_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchema_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchema_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchema_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchema_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchema_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchema_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchema_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchema_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchema_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchema_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchema_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchema_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchema_get_targetNamespace(This,targetNamespace)	\
    ( (This)->lpVtbl -> get_targetNamespace(This,targetNamespace) ) 
#define ISchema_get_version(This,version)	\
    ( (This)->lpVtbl -> get_version(This,version) ) 
#define ISchema_get_types(This,types)	\
    ( (This)->lpVtbl -> get_types(This,types) ) 
#define ISchema_get_elements(This,elements)	\
    ( (This)->lpVtbl -> get_elements(This,elements) ) 
#define ISchema_get_attributes(This,attributes)	\
    ( (This)->lpVtbl -> get_attributes(This,attributes) ) 
#define ISchema_get_attributeGroups(This,attributeGroups)	\
    ( (This)->lpVtbl -> get_attributeGroups(This,attributeGroups) ) 
#define ISchema_get_modelGroups(This,modelGroups)	\
    ( (This)->lpVtbl -> get_modelGroups(This,modelGroups) ) 
#define ISchema_get_notations(This,notations)	\
    ( (This)->lpVtbl -> get_notations(This,notations) ) 
#define ISchema_get_schemaLocations(This,schemaLocations)	\
    ( (This)->lpVtbl -> get_schemaLocations(This,schemaLocations) ) 
#endif 
EXTERN_C const IID IID_ISchemaParticle;
    typedef struct ISchemaParticleVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaParticle * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaParticle * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaParticle * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaParticle * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaParticle * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaParticle * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaParticle * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaParticle * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaParticle * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaParticle * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaParticle * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaParticle * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaParticle * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaParticle * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_minOccurs )( 
            ISchemaParticle * This,
             VARIANT *minOccurs);
         HRESULT ( STDMETHODCALLTYPE *get_maxOccurs )( 
            ISchemaParticle * This,
             VARIANT *maxOccurs);
        END_INTERFACE
    } ISchemaParticleVtbl;
    interface __uuid("50ea08b5-dd1b-4664-9a50-c2f40f4bd79a") ISchemaParticle

    {
        CONST_VTBL struct ISchemaParticleVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaParticle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaParticle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaParticle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaParticle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaParticle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaParticle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaParticle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaParticle_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaParticle_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaParticle_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaParticle_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaParticle_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaParticle_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaParticle_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaParticle_get_minOccurs(This,minOccurs)	\
    ( (This)->lpVtbl -> get_minOccurs(This,minOccurs) ) 
#define ISchemaParticle_get_maxOccurs(This,maxOccurs)	\
    ( (This)->lpVtbl -> get_maxOccurs(This,maxOccurs) ) 
#endif 
EXTERN_C const IID IID_ISchemaAttribute;
    typedef struct ISchemaAttributeVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaAttribute * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaAttribute * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaAttribute * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaAttribute * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaAttribute * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaAttribute * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaAttribute * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaAttribute * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaAttribute * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaAttribute * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaAttribute * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaAttribute * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaAttribute * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaAttribute * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_type )( 
            ISchemaAttribute * This,
             ISchemaType **type);
         HRESULT ( STDMETHODCALLTYPE *get_scope )( 
            ISchemaAttribute * This,
             ISchemaComplexType **scope);
         HRESULT ( STDMETHODCALLTYPE *get_defaultValue )( 
            ISchemaAttribute * This,
             BSTR *defaultValue);
         HRESULT ( STDMETHODCALLTYPE *get_fixedValue )( 
            ISchemaAttribute * This,
             BSTR *fixedValue);
         HRESULT ( STDMETHODCALLTYPE *get_use )( 
            ISchemaAttribute * This,
             SCHEMAUSE *use);
         HRESULT ( STDMETHODCALLTYPE *get_isReference )( 
            ISchemaAttribute * This,
             VARIANT_BOOL *reference);
        END_INTERFACE
    } ISchemaAttributeVtbl;
    interface __uuid("50ea08b6-dd1b-4664-9a50-c2f40f4bd79a") ISchemaAttribute

    {
        CONST_VTBL struct ISchemaAttributeVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaAttribute_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaAttribute_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaAttribute_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaAttribute_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaAttribute_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaAttribute_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaAttribute_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaAttribute_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaAttribute_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaAttribute_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaAttribute_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaAttribute_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaAttribute_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaAttribute_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaAttribute_get_type(This,type)	\
    ( (This)->lpVtbl -> get_type(This,type) ) 
#define ISchemaAttribute_get_scope(This,scope)	\
    ( (This)->lpVtbl -> get_scope(This,scope) ) 
#define ISchemaAttribute_get_defaultValue(This,defaultValue)	\
    ( (This)->lpVtbl -> get_defaultValue(This,defaultValue) ) 
#define ISchemaAttribute_get_fixedValue(This,fixedValue)	\
    ( (This)->lpVtbl -> get_fixedValue(This,fixedValue) ) 
#define ISchemaAttribute_get_use(This,use)	\
    ( (This)->lpVtbl -> get_use(This,use) ) 
#define ISchemaAttribute_get_isReference(This,reference)	\
    ( (This)->lpVtbl -> get_isReference(This,reference) ) 
#endif 
EXTERN_C const IID IID_ISchemaElement;
    typedef struct ISchemaElementVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaElement * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaElement * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaElement * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaElement * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaElement * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaElement * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaElement * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaElement * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaElement * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaElement * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaElement * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaElement * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaElement * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaElement * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_minOccurs )( 
            ISchemaElement * This,
             VARIANT *minOccurs);
         HRESULT ( STDMETHODCALLTYPE *get_maxOccurs )( 
            ISchemaElement * This,
             VARIANT *maxOccurs);
         HRESULT ( STDMETHODCALLTYPE *get_type )( 
            ISchemaElement * This,
             ISchemaType **type);
         HRESULT ( STDMETHODCALLTYPE *get_scope )( 
            ISchemaElement * This,
             ISchemaComplexType **scope);
         HRESULT ( STDMETHODCALLTYPE *get_defaultValue )( 
            ISchemaElement * This,
             BSTR *defaultValue);
         HRESULT ( STDMETHODCALLTYPE *get_fixedValue )( 
            ISchemaElement * This,
             BSTR *fixedValue);
         HRESULT ( STDMETHODCALLTYPE *get_isNillable )( 
            ISchemaElement * This,
             VARIANT_BOOL *nillable);
         HRESULT ( STDMETHODCALLTYPE *get_identityConstraints )( 
            ISchemaElement * This,
             ISchemaItemCollection **constraints);
         HRESULT ( STDMETHODCALLTYPE *get_substitutionGroup )( 
            ISchemaElement * This,
             ISchemaElement **element);
         HRESULT ( STDMETHODCALLTYPE *get_substitutionGroupExclusions )( 
            ISchemaElement * This,
             SCHEMADERIVATIONMETHOD *exclusions);
         HRESULT ( STDMETHODCALLTYPE *get_disallowedSubstitutions )( 
            ISchemaElement * This,
             SCHEMADERIVATIONMETHOD *disallowed);
         HRESULT ( STDMETHODCALLTYPE *get_isAbstract )( 
            ISchemaElement * This,
             VARIANT_BOOL *abstract);
         HRESULT ( STDMETHODCALLTYPE *get_isReference )( 
            ISchemaElement * This,
             VARIANT_BOOL *reference);
        END_INTERFACE
    } ISchemaElementVtbl;
    interface __uuid("50ea08b7-dd1b-4664-9a50-c2f40f4bd79a") ISchemaElement

    {
        CONST_VTBL struct ISchemaElementVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaElement_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaElement_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaElement_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaElement_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaElement_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaElement_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaElement_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaElement_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaElement_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaElement_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaElement_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaElement_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaElement_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaElement_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaElement_get_minOccurs(This,minOccurs)	\
    ( (This)->lpVtbl -> get_minOccurs(This,minOccurs) ) 
#define ISchemaElement_get_maxOccurs(This,maxOccurs)	\
    ( (This)->lpVtbl -> get_maxOccurs(This,maxOccurs) ) 
#define ISchemaElement_get_type(This,type)	\
    ( (This)->lpVtbl -> get_type(This,type) ) 
#define ISchemaElement_get_scope(This,scope)	\
    ( (This)->lpVtbl -> get_scope(This,scope) ) 
#define ISchemaElement_get_defaultValue(This,defaultValue)	\
    ( (This)->lpVtbl -> get_defaultValue(This,defaultValue) ) 
#define ISchemaElement_get_fixedValue(This,fixedValue)	\
    ( (This)->lpVtbl -> get_fixedValue(This,fixedValue) ) 
#define ISchemaElement_get_isNillable(This,nillable)	\
    ( (This)->lpVtbl -> get_isNillable(This,nillable) ) 
#define ISchemaElement_get_identityConstraints(This,constraints)	\
    ( (This)->lpVtbl -> get_identityConstraints(This,constraints) ) 
#define ISchemaElement_get_substitutionGroup(This,element)	\
    ( (This)->lpVtbl -> get_substitutionGroup(This,element) ) 
#define ISchemaElement_get_substitutionGroupExclusions(This,exclusions)	\
    ( (This)->lpVtbl -> get_substitutionGroupExclusions(This,exclusions) ) 
#define ISchemaElement_get_disallowedSubstitutions(This,disallowed)	\
    ( (This)->lpVtbl -> get_disallowedSubstitutions(This,disallowed) ) 
#define ISchemaElement_get_isAbstract(This,abstract)	\
    ( (This)->lpVtbl -> get_isAbstract(This,abstract) ) 
#define ISchemaElement_get_isReference(This,reference)	\
    ( (This)->lpVtbl -> get_isReference(This,reference) ) 
#endif 
EXTERN_C const IID IID_ISchemaType;
    typedef struct ISchemaTypeVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaType * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaType * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaType * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaType * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaType * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaType * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaType * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaType * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaType * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaType * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaType * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaType * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaType * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaType * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_baseTypes )( 
            ISchemaType * This,
             ISchemaItemCollection **baseTypes);
         HRESULT ( STDMETHODCALLTYPE *get_final_ )( 
            ISchemaType * This,
             SCHEMADERIVATIONMETHOD *final_);
         HRESULT ( STDMETHODCALLTYPE *get_variety )( 
            ISchemaType * This,
             SCHEMATYPEVARIETY *variety);
         HRESULT ( STDMETHODCALLTYPE *get_derivedBy )( 
            ISchemaType * This,
             SCHEMADERIVATIONMETHOD *derivedBy);
         HRESULT ( STDMETHODCALLTYPE *isValid )( 
            ISchemaType * This,
             BSTR data,
             VARIANT_BOOL *valid);
         HRESULT ( STDMETHODCALLTYPE *get_minExclusive )( 
            ISchemaType * This,
             BSTR *minExclusive);
         HRESULT ( STDMETHODCALLTYPE *get_minInclusive )( 
            ISchemaType * This,
             BSTR *minInclusive);
         HRESULT ( STDMETHODCALLTYPE *get_maxExclusive )( 
            ISchemaType * This,
             BSTR *maxExclusive);
         HRESULT ( STDMETHODCALLTYPE *get_maxInclusive )( 
            ISchemaType * This,
             BSTR *maxInclusive);
         HRESULT ( STDMETHODCALLTYPE *get_totalDigits )( 
            ISchemaType * This,
             VARIANT *totalDigits);
         HRESULT ( STDMETHODCALLTYPE *get_fractionDigits )( 
            ISchemaType * This,
             VARIANT *fractionDigits);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            ISchemaType * This,
             VARIANT *length);
         HRESULT ( STDMETHODCALLTYPE *get_minLength )( 
            ISchemaType * This,
             VARIANT *minLength);
         HRESULT ( STDMETHODCALLTYPE *get_maxLength )( 
            ISchemaType * This,
             VARIANT *maxLength);
         HRESULT ( STDMETHODCALLTYPE *get_enumeration )( 
            ISchemaType * This,
             ISchemaStringCollection **enumeration);
         HRESULT ( STDMETHODCALLTYPE *get_whitespace )( 
            ISchemaType * This,
             SCHEMAWHITESPACE *whitespace);
         HRESULT ( STDMETHODCALLTYPE *get_patterns )( 
            ISchemaType * This,
             ISchemaStringCollection **patterns);
        END_INTERFACE
    } ISchemaTypeVtbl;
    interface __uuid("50ea08b8-dd1b-4664-9a50-c2f40f4bd79a") ISchemaType

    {
        CONST_VTBL struct ISchemaTypeVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaType_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaType_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaType_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaType_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaType_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaType_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaType_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaType_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaType_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaType_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaType_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaType_get_baseTypes(This,baseTypes)	\
    ( (This)->lpVtbl -> get_baseTypes(This,baseTypes) ) 
#define ISchemaType_get_final_(This,final_)	\
    ( (This)->lpVtbl -> get_final_(This,final_) ) 
#define ISchemaType_get_variety(This,variety)	\
    ( (This)->lpVtbl -> get_variety(This,variety) ) 
#define ISchemaType_get_derivedBy(This,derivedBy)	\
    ( (This)->lpVtbl -> get_derivedBy(This,derivedBy) ) 
#define ISchemaType_isValid(This,data,valid)	\
    ( (This)->lpVtbl -> isValid(This,data,valid) ) 
#define ISchemaType_get_minExclusive(This,minExclusive)	\
    ( (This)->lpVtbl -> get_minExclusive(This,minExclusive) ) 
#define ISchemaType_get_minInclusive(This,minInclusive)	\
    ( (This)->lpVtbl -> get_minInclusive(This,minInclusive) ) 
#define ISchemaType_get_maxExclusive(This,maxExclusive)	\
    ( (This)->lpVtbl -> get_maxExclusive(This,maxExclusive) ) 
#define ISchemaType_get_maxInclusive(This,maxInclusive)	\
    ( (This)->lpVtbl -> get_maxInclusive(This,maxInclusive) ) 
#define ISchemaType_get_totalDigits(This,totalDigits)	\
    ( (This)->lpVtbl -> get_totalDigits(This,totalDigits) ) 
#define ISchemaType_get_fractionDigits(This,fractionDigits)	\
    ( (This)->lpVtbl -> get_fractionDigits(This,fractionDigits) ) 
#define ISchemaType_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define ISchemaType_get_minLength(This,minLength)	\
    ( (This)->lpVtbl -> get_minLength(This,minLength) ) 
#define ISchemaType_get_maxLength(This,maxLength)	\
    ( (This)->lpVtbl -> get_maxLength(This,maxLength) ) 
#define ISchemaType_get_enumeration(This,enumeration)	\
    ( (This)->lpVtbl -> get_enumeration(This,enumeration) ) 
#define ISchemaType_get_whitespace(This,whitespace)	\
    ( (This)->lpVtbl -> get_whitespace(This,whitespace) ) 
#define ISchemaType_get_patterns(This,patterns)	\
    ( (This)->lpVtbl -> get_patterns(This,patterns) ) 
#endif 
EXTERN_C const IID IID_ISchemaComplexType;
    typedef struct ISchemaComplexTypeVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaComplexType * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaComplexType * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaComplexType * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaComplexType * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaComplexType * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaComplexType * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaComplexType * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaComplexType * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaComplexType * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaComplexType * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaComplexType * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaComplexType * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaComplexType * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaComplexType * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_baseTypes )( 
            ISchemaComplexType * This,
             ISchemaItemCollection **baseTypes);
         HRESULT ( STDMETHODCALLTYPE *get_final_ )( 
            ISchemaComplexType * This,
             SCHEMADERIVATIONMETHOD *final_);
         HRESULT ( STDMETHODCALLTYPE *get_variety )( 
            ISchemaComplexType * This,
             SCHEMATYPEVARIETY *variety);
         HRESULT ( STDMETHODCALLTYPE *get_derivedBy )( 
            ISchemaComplexType * This,
             SCHEMADERIVATIONMETHOD *derivedBy);
         HRESULT ( STDMETHODCALLTYPE *isValid )( 
            ISchemaComplexType * This,
             BSTR data,
             VARIANT_BOOL *valid);
         HRESULT ( STDMETHODCALLTYPE *get_minExclusive )( 
            ISchemaComplexType * This,
             BSTR *minExclusive);
         HRESULT ( STDMETHODCALLTYPE *get_minInclusive )( 
            ISchemaComplexType * This,
             BSTR *minInclusive);
         HRESULT ( STDMETHODCALLTYPE *get_maxExclusive )( 
            ISchemaComplexType * This,
             BSTR *maxExclusive);
         HRESULT ( STDMETHODCALLTYPE *get_maxInclusive )( 
            ISchemaComplexType * This,
             BSTR *maxInclusive);
         HRESULT ( STDMETHODCALLTYPE *get_totalDigits )( 
            ISchemaComplexType * This,
             VARIANT *totalDigits);
         HRESULT ( STDMETHODCALLTYPE *get_fractionDigits )( 
            ISchemaComplexType * This,
             VARIANT *fractionDigits);
         HRESULT ( STDMETHODCALLTYPE *get_length )( 
            ISchemaComplexType * This,
             VARIANT *length);
         HRESULT ( STDMETHODCALLTYPE *get_minLength )( 
            ISchemaComplexType * This,
             VARIANT *minLength);
         HRESULT ( STDMETHODCALLTYPE *get_maxLength )( 
            ISchemaComplexType * This,
             VARIANT *maxLength);
         HRESULT ( STDMETHODCALLTYPE *get_enumeration )( 
            ISchemaComplexType * This,
             ISchemaStringCollection **enumeration);
         HRESULT ( STDMETHODCALLTYPE *get_whitespace )( 
            ISchemaComplexType * This,
             SCHEMAWHITESPACE *whitespace);
         HRESULT ( STDMETHODCALLTYPE *get_patterns )( 
            ISchemaComplexType * This,
             ISchemaStringCollection **patterns);
         HRESULT ( STDMETHODCALLTYPE *get_isAbstract )( 
            ISchemaComplexType * This,
             VARIANT_BOOL *abstract);
         HRESULT ( STDMETHODCALLTYPE *get_anyAttribute )( 
            ISchemaComplexType * This,
             ISchemaAny **anyAttribute);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            ISchemaComplexType * This,
             ISchemaItemCollection **attributes);
         HRESULT ( STDMETHODCALLTYPE *get_contentType )( 
            ISchemaComplexType * This,
             SCHEMACONTENTTYPE *contentType);
         HRESULT ( STDMETHODCALLTYPE *get_contentModel )( 
            ISchemaComplexType * This,
             ISchemaModelGroup **contentModel);
         HRESULT ( STDMETHODCALLTYPE *get_prohibitedSubstitutions )( 
            ISchemaComplexType * This,
             SCHEMADERIVATIONMETHOD *prohibited);
        END_INTERFACE
    } ISchemaComplexTypeVtbl;
    interface __uuid("50ea08b9-dd1b-4664-9a50-c2f40f4bd79a") ISchemaComplexType

    {
        CONST_VTBL struct ISchemaComplexTypeVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaComplexType_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaComplexType_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaComplexType_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaComplexType_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaComplexType_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaComplexType_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaComplexType_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaComplexType_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaComplexType_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaComplexType_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaComplexType_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaComplexType_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaComplexType_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaComplexType_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaComplexType_get_baseTypes(This,baseTypes)	\
    ( (This)->lpVtbl -> get_baseTypes(This,baseTypes) ) 
#define ISchemaComplexType_get_final_(This,final_)	\
    ( (This)->lpVtbl -> get_final_(This,final_) ) 
#define ISchemaComplexType_get_variety(This,variety)	\
    ( (This)->lpVtbl -> get_variety(This,variety) ) 
#define ISchemaComplexType_get_derivedBy(This,derivedBy)	\
    ( (This)->lpVtbl -> get_derivedBy(This,derivedBy) ) 
#define ISchemaComplexType_isValid(This,data,valid)	\
    ( (This)->lpVtbl -> isValid(This,data,valid) ) 
#define ISchemaComplexType_get_minExclusive(This,minExclusive)	\
    ( (This)->lpVtbl -> get_minExclusive(This,minExclusive) ) 
#define ISchemaComplexType_get_minInclusive(This,minInclusive)	\
    ( (This)->lpVtbl -> get_minInclusive(This,minInclusive) ) 
#define ISchemaComplexType_get_maxExclusive(This,maxExclusive)	\
    ( (This)->lpVtbl -> get_maxExclusive(This,maxExclusive) ) 
#define ISchemaComplexType_get_maxInclusive(This,maxInclusive)	\
    ( (This)->lpVtbl -> get_maxInclusive(This,maxInclusive) ) 
#define ISchemaComplexType_get_totalDigits(This,totalDigits)	\
    ( (This)->lpVtbl -> get_totalDigits(This,totalDigits) ) 
#define ISchemaComplexType_get_fractionDigits(This,fractionDigits)	\
    ( (This)->lpVtbl -> get_fractionDigits(This,fractionDigits) ) 
#define ISchemaComplexType_get_length(This,length)	\
    ( (This)->lpVtbl -> get_length(This,length) ) 
#define ISchemaComplexType_get_minLength(This,minLength)	\
    ( (This)->lpVtbl -> get_minLength(This,minLength) ) 
#define ISchemaComplexType_get_maxLength(This,maxLength)	\
    ( (This)->lpVtbl -> get_maxLength(This,maxLength) ) 
#define ISchemaComplexType_get_enumeration(This,enumeration)	\
    ( (This)->lpVtbl -> get_enumeration(This,enumeration) ) 
#define ISchemaComplexType_get_whitespace(This,whitespace)	\
    ( (This)->lpVtbl -> get_whitespace(This,whitespace) ) 
#define ISchemaComplexType_get_patterns(This,patterns)	\
    ( (This)->lpVtbl -> get_patterns(This,patterns) ) 
#define ISchemaComplexType_get_isAbstract(This,abstract)	\
    ( (This)->lpVtbl -> get_isAbstract(This,abstract) ) 
#define ISchemaComplexType_get_anyAttribute(This,anyAttribute)	\
    ( (This)->lpVtbl -> get_anyAttribute(This,anyAttribute) ) 
#define ISchemaComplexType_get_attributes(This,attributes)	\
    ( (This)->lpVtbl -> get_attributes(This,attributes) ) 
#define ISchemaComplexType_get_contentType(This,contentType)	\
    ( (This)->lpVtbl -> get_contentType(This,contentType) ) 
#define ISchemaComplexType_get_contentModel(This,contentModel)	\
    ( (This)->lpVtbl -> get_contentModel(This,contentModel) ) 
#define ISchemaComplexType_get_prohibitedSubstitutions(This,prohibited)	\
    ( (This)->lpVtbl -> get_prohibitedSubstitutions(This,prohibited) ) 
#endif 
EXTERN_C const IID IID_ISchemaAttributeGroup;
    typedef struct ISchemaAttributeGroupVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaAttributeGroup * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaAttributeGroup * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaAttributeGroup * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaAttributeGroup * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaAttributeGroup * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaAttributeGroup * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaAttributeGroup * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaAttributeGroup * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaAttributeGroup * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaAttributeGroup * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaAttributeGroup * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaAttributeGroup * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaAttributeGroup * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaAttributeGroup * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_anyAttribute )( 
            ISchemaAttributeGroup * This,
             ISchemaAny **anyAttribute);
         HRESULT ( STDMETHODCALLTYPE *get_attributes )( 
            ISchemaAttributeGroup * This,
             ISchemaItemCollection **attributes);
        END_INTERFACE
    } ISchemaAttributeGroupVtbl;
    interface __uuid("50ea08ba-dd1b-4664-9a50-c2f40f4bd79a") ISchemaAttributeGroup

    {
        CONST_VTBL struct ISchemaAttributeGroupVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaAttributeGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaAttributeGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaAttributeGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaAttributeGroup_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaAttributeGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaAttributeGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaAttributeGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaAttributeGroup_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaAttributeGroup_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaAttributeGroup_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaAttributeGroup_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaAttributeGroup_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaAttributeGroup_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaAttributeGroup_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaAttributeGroup_get_anyAttribute(This,anyAttribute)	\
    ( (This)->lpVtbl -> get_anyAttribute(This,anyAttribute) ) 
#define ISchemaAttributeGroup_get_attributes(This,attributes)	\
    ( (This)->lpVtbl -> get_attributes(This,attributes) ) 
#endif 
EXTERN_C const IID IID_ISchemaModelGroup;
    typedef struct ISchemaModelGroupVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaModelGroup * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaModelGroup * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaModelGroup * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaModelGroup * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaModelGroup * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaModelGroup * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaModelGroup * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaModelGroup * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaModelGroup * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaModelGroup * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaModelGroup * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaModelGroup * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaModelGroup * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaModelGroup * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_minOccurs )( 
            ISchemaModelGroup * This,
             VARIANT *minOccurs);
         HRESULT ( STDMETHODCALLTYPE *get_maxOccurs )( 
            ISchemaModelGroup * This,
             VARIANT *maxOccurs);
         HRESULT ( STDMETHODCALLTYPE *get_particles )( 
            ISchemaModelGroup * This,
             ISchemaItemCollection **particles);
        END_INTERFACE
    } ISchemaModelGroupVtbl;
    interface __uuid("50ea08bb-dd1b-4664-9a50-c2f40f4bd79a") ISchemaModelGroup

    {
        CONST_VTBL struct ISchemaModelGroupVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaModelGroup_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaModelGroup_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaModelGroup_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaModelGroup_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaModelGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaModelGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaModelGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaModelGroup_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaModelGroup_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaModelGroup_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaModelGroup_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaModelGroup_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaModelGroup_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaModelGroup_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaModelGroup_get_minOccurs(This,minOccurs)	\
    ( (This)->lpVtbl -> get_minOccurs(This,minOccurs) ) 
#define ISchemaModelGroup_get_maxOccurs(This,maxOccurs)	\
    ( (This)->lpVtbl -> get_maxOccurs(This,maxOccurs) ) 
#define ISchemaModelGroup_get_particles(This,particles)	\
    ( (This)->lpVtbl -> get_particles(This,particles) ) 
#endif 
EXTERN_C const IID IID_ISchemaAny;
    typedef struct ISchemaAnyVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaAny * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaAny * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaAny * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaAny * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaAny * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaAny * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaAny * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaAny * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaAny * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaAny * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaAny * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaAny * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaAny * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaAny * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_minOccurs )( 
            ISchemaAny * This,
             VARIANT *minOccurs);
         HRESULT ( STDMETHODCALLTYPE *get_maxOccurs )( 
            ISchemaAny * This,
             VARIANT *maxOccurs);
         HRESULT ( STDMETHODCALLTYPE *get_namespaces )( 
            ISchemaAny * This,
             ISchemaStringCollection **namespaces);
         HRESULT ( STDMETHODCALLTYPE *get_processContents )( 
            ISchemaAny * This,
             SCHEMAPROCESSCONTENTS *processContents);
        END_INTERFACE
    } ISchemaAnyVtbl;
    interface __uuid("50ea08bc-dd1b-4664-9a50-c2f40f4bd79a") ISchemaAny

    {
        CONST_VTBL struct ISchemaAnyVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaAny_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaAny_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaAny_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaAny_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaAny_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaAny_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaAny_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaAny_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaAny_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaAny_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaAny_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaAny_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaAny_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaAny_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaAny_get_minOccurs(This,minOccurs)	\
    ( (This)->lpVtbl -> get_minOccurs(This,minOccurs) ) 
#define ISchemaAny_get_maxOccurs(This,maxOccurs)	\
    ( (This)->lpVtbl -> get_maxOccurs(This,maxOccurs) ) 
#define ISchemaAny_get_namespaces(This,namespaces)	\
    ( (This)->lpVtbl -> get_namespaces(This,namespaces) ) 
#define ISchemaAny_get_processContents(This,processContents)	\
    ( (This)->lpVtbl -> get_processContents(This,processContents) ) 
#endif 
EXTERN_C const IID IID_ISchemaIdentityConstraint;
    typedef struct ISchemaIdentityConstraintVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaIdentityConstraint * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaIdentityConstraint * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaIdentityConstraint * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaIdentityConstraint * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaIdentityConstraint * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaIdentityConstraint * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaIdentityConstraint * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaIdentityConstraint * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaIdentityConstraint * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaIdentityConstraint * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaIdentityConstraint * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaIdentityConstraint * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaIdentityConstraint * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaIdentityConstraint * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_selector )( 
            ISchemaIdentityConstraint * This,
             BSTR *selector);
         HRESULT ( STDMETHODCALLTYPE *get_fields )( 
            ISchemaIdentityConstraint * This,
             ISchemaStringCollection **fields);
         HRESULT ( STDMETHODCALLTYPE *get_referencedKey )( 
            ISchemaIdentityConstraint * This,
             ISchemaIdentityConstraint **key);
        END_INTERFACE
    } ISchemaIdentityConstraintVtbl;
    interface __uuid("50ea08bd-dd1b-4664-9a50-c2f40f4bd79a") ISchemaIdentityConstraint

    {
        CONST_VTBL struct ISchemaIdentityConstraintVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaIdentityConstraint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaIdentityConstraint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaIdentityConstraint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaIdentityConstraint_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaIdentityConstraint_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaIdentityConstraint_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaIdentityConstraint_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaIdentityConstraint_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaIdentityConstraint_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaIdentityConstraint_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaIdentityConstraint_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaIdentityConstraint_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaIdentityConstraint_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaIdentityConstraint_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaIdentityConstraint_get_selector(This,selector)	\
    ( (This)->lpVtbl -> get_selector(This,selector) ) 
#define ISchemaIdentityConstraint_get_fields(This,fields)	\
    ( (This)->lpVtbl -> get_fields(This,fields) ) 
#define ISchemaIdentityConstraint_get_referencedKey(This,key)	\
    ( (This)->lpVtbl -> get_referencedKey(This,key) ) 
#endif 
EXTERN_C const IID IID_ISchemaNotation;
    typedef struct ISchemaNotationVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISchemaNotation * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISchemaNotation * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISchemaNotation * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISchemaNotation * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISchemaNotation * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISchemaNotation * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISchemaNotation * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
         HRESULT ( STDMETHODCALLTYPE *get_name )( 
            ISchemaNotation * This,
             BSTR *name);
         HRESULT ( STDMETHODCALLTYPE *get_namespaceURI )( 
            ISchemaNotation * This,
             BSTR *namespaceURI);
         HRESULT ( STDMETHODCALLTYPE *get_schema )( 
            ISchemaNotation * This,
             ISchema **schema);
         HRESULT ( STDMETHODCALLTYPE *get_id )( 
            ISchemaNotation * This,
             BSTR *id);
         HRESULT ( STDMETHODCALLTYPE *get_itemType )( 
            ISchemaNotation * This,
             SOMITEMTYPE *itemType);
         HRESULT ( STDMETHODCALLTYPE *get_unhandledAttributes )( 
            ISchemaNotation * This,
             IVBSAXAttributes **attributes);
         HRESULT ( STDMETHODCALLTYPE *writeAnnotation )( 
            ISchemaNotation * This,
             IUnknown *annotationSink,
             VARIANT_BOOL *isWritten);
         HRESULT ( STDMETHODCALLTYPE *get_systemIdentifier )( 
            ISchemaNotation * This,
             BSTR *uri);
         HRESULT ( STDMETHODCALLTYPE *get_publicIdentifier )( 
            ISchemaNotation * This,
             BSTR *uri);
        END_INTERFACE
    } ISchemaNotationVtbl;
    interface __uuid("50ea08be-dd1b-4664-9a50-c2f40f4bd79a") ISchemaNotation

    {
        CONST_VTBL struct ISchemaNotationVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define ISchemaNotation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define ISchemaNotation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define ISchemaNotation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define ISchemaNotation_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define ISchemaNotation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define ISchemaNotation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define ISchemaNotation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#define ISchemaNotation_get_name(This,name)	\
    ( (This)->lpVtbl -> get_name(This,name) ) 
#define ISchemaNotation_get_namespaceURI(This,namespaceURI)	\
    ( (This)->lpVtbl -> get_namespaceURI(This,namespaceURI) ) 
#define ISchemaNotation_get_schema(This,schema)	\
    ( (This)->lpVtbl -> get_schema(This,schema) ) 
#define ISchemaNotation_get_id(This,id)	\
    ( (This)->lpVtbl -> get_id(This,id) ) 
#define ISchemaNotation_get_itemType(This,itemType)	\
    ( (This)->lpVtbl -> get_itemType(This,itemType) ) 
#define ISchemaNotation_get_unhandledAttributes(This,attributes)	\
    ( (This)->lpVtbl -> get_unhandledAttributes(This,attributes) ) 
#define ISchemaNotation_writeAnnotation(This,annotationSink,isWritten)	\
    ( (This)->lpVtbl -> writeAnnotation(This,annotationSink,isWritten) ) 
#define ISchemaNotation_get_systemIdentifier(This,uri)	\
    ( (This)->lpVtbl -> get_systemIdentifier(This,uri) ) 
#define ISchemaNotation_get_publicIdentifier(This,uri)	\
    ( (This)->lpVtbl -> get_publicIdentifier(This,uri) ) 
#endif 
#ifndef __MSXML2_LIBRARY_DEFINED__
#define __MSXML2_LIBRARY_DEFINED__
typedef  struct __msxml6_ReferenceRemainingTypes__
    {
    enum tagDOMNodeType __tagDomNodeType__;
    DOMNodeType __domNodeType__;
    enum _SERVERXMLHTTP_OPTION __serverXmlHttpOptionEnum__;
    SERVERXMLHTTP_OPTION __serverXmlHttpOption__;
    enum _SXH_SERVER_CERT_OPTION __serverCertOptionEnum__;
    SXH_SERVER_CERT_OPTION __serverCertOption__;
    enum _SXH_PROXY_SETTING __proxySettingEnum__;
    SXH_PROXY_SETTING __proxySetting__;
    enum _SOMITEMTYPE __somItemTypeEnum__;
    SOMITEMTYPE __somItemType__;
    enum _SCHEMAUSE __schemaUseEnum__;
    SCHEMAUSE __schemaUse__;
    enum _SCHEMADERIVATIONMETHOD __schemaDerivationMethodEnum__;
    SCHEMADERIVATIONMETHOD __schemaDerivationMethod__;
    enum _SCHEMACONTENTTYPE __schemaContentTypeEnum__;
    SCHEMACONTENTTYPE __schemaContentType__;
    enum _SCHEMAPROCESSCONTENTS __schemaProcessContentsEnum__;
    SCHEMAPROCESSCONTENTS __schemaProcessContents__;
    enum _SCHEMAWHITESPACE __schemaWhitespaceEnum__;
    SCHEMAWHITESPACE __schemaWhitespace__;
    enum _SCHEMATYPEVARIETY __schemaTypeVarietyEnum__;
    SCHEMATYPEVARIETY __schemaTypeVariety__;
    } 	__msxml6_ReferenceRemainingTypes__;
#define CLSID_XmlHttpRequest CLSID_FreeThreadedXMLHTTP60
EXTERN_C const IID LIBID_MSXML2;
EXTERN_C const IID DIID_XMLDOMDocumentEvents;
    typedef struct XMLDOMDocumentEventsVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            XMLDOMDocumentEvents * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            XMLDOMDocumentEvents * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            XMLDOMDocumentEvents * This);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            XMLDOMDocumentEvents * This,
             UINT *pctinfo);
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            XMLDOMDocumentEvents * This,
             UINT iTInfo,
             LCID lcid,
             ITypeInfo **ppTInfo);
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            XMLDOMDocumentEvents * This,
             REFIID riid,
             LPOLESTR *rgszNames,
             UINT cNames,
             LCID lcid,
             DISPID *rgDispId);
         HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            XMLDOMDocumentEvents * This,
             DISPID dispIdMember,
             REFIID riid,
             LCID lcid,
             WORD wFlags,
             DISPPARAMS *pDispParams,
             VARIANT *pVarResult,
             EXCEPINFO *pExcepInfo,
             UINT *puArgErr);
        END_INTERFACE
    } XMLDOMDocumentEventsVtbl;
    interface __uuid("3efaa427-272f-11d2-836f-0000f87a7782") XMLDOMDocumentEvents

    {
        CONST_VTBL struct XMLDOMDocumentEventsVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define XMLDOMDocumentEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define XMLDOMDocumentEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define XMLDOMDocumentEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define XMLDOMDocumentEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 
#define XMLDOMDocumentEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 
#define XMLDOMDocumentEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 
#define XMLDOMDocumentEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 
#endif 
EXTERN_C const CLSID CLSID_DOMDocument60;
struct __uuid("88d96a05-f192-11d4-a65f-0040963251e5")
DOMDocument60;
EXTERN_C const CLSID CLSID_FreeThreadedDOMDocument60;
struct __uuid("88d96a06-f192-11d4-a65f-0040963251e5")
FreeThreadedDOMDocument60;
EXTERN_C const CLSID CLSID_XMLSchemaCache60;
struct __uuid("88d96a07-f192-11d4-a65f-0040963251e5")
XMLSchemaCache60;
EXTERN_C const CLSID CLSID_XSLTemplate60;
struct __uuid("88d96a08-f192-11d4-a65f-0040963251e5")
XSLTemplate60;
EXTERN_C const CLSID CLSID_XMLHTTP60;
struct __uuid("88d96a0a-f192-11d4-a65f-0040963251e5")
XMLHTTP60;
EXTERN_C const CLSID CLSID_FreeThreadedXMLHTTP60;
struct __uuid("88d96a09-f192-11d4-a65f-0040963251e5")
FreeThreadedXMLHTTP60;
EXTERN_C const CLSID CLSID_ServerXMLHTTP60;
struct __uuid("88d96a0b-f192-11d4-a65f-0040963251e5")
ServerXMLHTTP60;
EXTERN_C const CLSID CLSID_SAXXMLReader60;
struct __uuid("88d96a0c-f192-11d4-a65f-0040963251e5")
SAXXMLReader60;
EXTERN_C const CLSID CLSID_MXXMLWriter60;
struct __uuid("88d96a0f-f192-11d4-a65f-0040963251e5")
MXXMLWriter60;
EXTERN_C const CLSID CLSID_MXHTMLWriter60;
struct __uuid("88d96a10-f192-11d4-a65f-0040963251e5")
MXHTMLWriter60;
EXTERN_C const CLSID CLSID_SAXAttributes60;
struct __uuid("88d96a0e-f192-11d4-a65f-0040963251e5")
SAXAttributes60;
EXTERN_C const CLSID CLSID_MXNamespaceManager60;
struct __uuid("88d96a11-f192-11d4-a65f-0040963251e5")
MXNamespaceManager60;
#endif 
#define E_XML_NOTWF                0xC00CE223L  
#define E_XML_NODTD                0xC00CE224L  
#define E_XML_INVALID              0xC00CE225L  
#define E_XML_BUFFERTOOSMALL       0xC00CE226L  
#if !defined(__msxml_xhr_enums)
#define __msxml_xhr_enums
typedef  
enum _XHR_COOKIE_STATE
    {
        XHR_COOKIE_STATE_UNKNOWN	= 0,
        XHR_COOKIE_STATE_ACCEPT	= 0x1,
        XHR_COOKIE_STATE_PROMPT	= 0x2,
        XHR_COOKIE_STATE_LEASH	= 0x3,
        XHR_COOKIE_STATE_DOWNGRADE	= 0x4,
        XHR_COOKIE_STATE_REJECT	= 0x5
    } 	XHR_COOKIE_STATE;
typedef  
enum _XHR_COOKIE_FLAG
    {
        XHR_COOKIE_IS_SECURE	= 0x1,
        XHR_COOKIE_IS_SESSION	= 0x2,
        XHR_COOKIE_THIRD_PARTY	= 0x10,
        XHR_COOKIE_PROMPT_REQUIRED	= 0x20,
        XHR_COOKIE_EVALUATE_P3P	= 0x40,
        XHR_COOKIE_APPLY_P3P	= 0x80,
        XHR_COOKIE_P3P_ENABLED	= 0x100,
        XHR_COOKIE_IS_RESTRICTED	= 0x200,
        XHR_COOKIE_IE6	= 0x400,
        XHR_COOKIE_IS_LEGACY	= 0x800,
        XHR_COOKIE_NON_SCRIPT	= 0x1000,
        XHR_COOKIE_HTTPONLY	= 0x2000
    } 	XHR_COOKIE_FLAG;
typedef  
enum _XHR_CRED_PROMPT
    {
        XHR_CRED_PROMPT_ALL	= 0,
        XHR_CRED_PROMPT_NONE	= 0x1,
        XHR_CRED_PROMPT_PROXY	= 0x2
    } 	XHR_CRED_PROMPT;
typedef  
enum _XHR_AUTH
    {
        XHR_AUTH_ALL	= 0,
        XHR_AUTH_NONE	= 0x1,
        XHR_AUTH_PROXY	= 0x2
    } 	XHR_AUTH;
typedef  
enum _XHR_PROPERTY
    {
        XHR_PROP_NO_CRED_PROMPT	= 0,
        XHR_PROP_NO_AUTH	= 0x1,
        XHR_PROP_TIMEOUT	= 0x2,
        XHR_PROP_NO_DEFAULT_HEADERS	= 0x3,
        XHR_PROP_REPORT_REDIRECT_STATUS	= 0x4,
        XHR_PROP_NO_CACHE	= 0x5,
        XHR_PROP_EXTENDED_ERROR	= 0x6,
        XHR_PROP_QUERY_STRING_UTF8	= 0x7,
        XHR_PROP_IGNORE_CERT_ERRORS	= 0x8,
        XHR_PROP_ONDATA_THRESHOLD	= 0x9,
        XHR_PROP_SET_ENTERPRISEID	= 0xa
    } 	XHR_PROPERTY;
#define XHR_PROP_ONDATA_ALWAYS 0x0
#define XHR_PROP_ONDATA_NEVER 0xFFFFFFFFFFFFFFFF
typedef  
enum _XHR_CERT_IGNORE_FLAG
    {
        XHR_CERT_IGNORE_REVOCATION_FAILED	= 0x80UL,
        XHR_CERT_IGNORE_UNKNOWN_CA	= 0x100UL,
        XHR_CERT_IGNORE_CERT_CN_INVALID	= 0x1000UL,
        XHR_CERT_IGNORE_CERT_DATE_INVALID	= 0x2000UL,
        XHR_CERT_IGNORE_ALL_SERVER_ERRORS	= ( ( ( XHR_CERT_IGNORE_REVOCATION_FAILED | XHR_CERT_IGNORE_UNKNOWN_CA )  | XHR_CERT_IGNORE_CERT_CN_INVALID )  | XHR_CERT_IGNORE_CERT_DATE_INVALID ) 
    } 	XHR_CERT_IGNORE_FLAG;
typedef  
enum _XHR_CERT_ERROR_FLAG
    {
        XHR_CERT_ERROR_REVOCATION_FAILED	= 0x800000UL,
        XHR_CERT_ERROR_UNKNOWN_CA	= 0x1000000UL,
        XHR_CERT_ERROR_CERT_CN_INVALID	= 0x2000000UL,
        XHR_CERT_ERROR_CERT_DATE_INVALID	= 0x4000000UL,
        XHR_CERT_ERROR_ALL_SERVER_ERRORS	= ( ( ( XHR_CERT_ERROR_REVOCATION_FAILED | XHR_CERT_ERROR_UNKNOWN_CA )  | XHR_CERT_ERROR_CERT_CN_INVALID )  | XHR_CERT_ERROR_CERT_DATE_INVALID ) 
    } 	XHR_CERT_ERROR_FLAG;
#endif 
typedef struct tagXHR_COOKIE
    {
     WCHAR *pwszUrl;
     WCHAR *pwszName;
     WCHAR *pwszValue;
     WCHAR *pwszP3PPolicy;
    FILETIME ftExpires;
    DWORD dwFlags;
    } 	XHR_COOKIE;
EXTERN_C const IID IID_IXMLHTTPRequest2Callback;
    typedef struct IXMLHTTPRequest2CallbackVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLHTTPRequest2Callback * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLHTTPRequest2Callback * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLHTTPRequest2Callback * This);
        HRESULT ( STDMETHODCALLTYPE *OnRedirect )( 
            IXMLHTTPRequest2Callback * This,
             IXMLHTTPRequest2 *pXHR,
             const WCHAR *pwszRedirectUrl);
        HRESULT ( STDMETHODCALLTYPE *OnHeadersAvailable )( 
            IXMLHTTPRequest2Callback * This,
             IXMLHTTPRequest2 *pXHR,
             DWORD dwStatus,
             const WCHAR *pwszStatus);
        HRESULT ( STDMETHODCALLTYPE *OnDataAvailable )( 
            IXMLHTTPRequest2Callback * This,
             IXMLHTTPRequest2 *pXHR,
             ISequentialStream *pResponseStream);
        HRESULT ( STDMETHODCALLTYPE *OnResponseReceived )( 
            IXMLHTTPRequest2Callback * This,
             IXMLHTTPRequest2 *pXHR,
             ISequentialStream *pResponseStream);
        HRESULT ( STDMETHODCALLTYPE *OnError )( 
            IXMLHTTPRequest2Callback * This,
             IXMLHTTPRequest2 *pXHR,
             HRESULT hrError);
        END_INTERFACE
    } IXMLHTTPRequest2CallbackVtbl;
    interface __uuid("A44A9299-E321-40DE-8866-341B41669162") IXMLHTTPRequest2Callback

    {
        CONST_VTBL struct IXMLHTTPRequest2CallbackVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLHTTPRequest2Callback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLHTTPRequest2Callback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLHTTPRequest2Callback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLHTTPRequest2Callback_OnRedirect(This,pXHR,pwszRedirectUrl)	\
    ( (This)->lpVtbl -> OnRedirect(This,pXHR,pwszRedirectUrl) ) 
#define IXMLHTTPRequest2Callback_OnHeadersAvailable(This,pXHR,dwStatus,pwszStatus)	\
    ( (This)->lpVtbl -> OnHeadersAvailable(This,pXHR,dwStatus,pwszStatus) ) 
#define IXMLHTTPRequest2Callback_OnDataAvailable(This,pXHR,pResponseStream)	\
    ( (This)->lpVtbl -> OnDataAvailable(This,pXHR,pResponseStream) ) 
#define IXMLHTTPRequest2Callback_OnResponseReceived(This,pXHR,pResponseStream)	\
    ( (This)->lpVtbl -> OnResponseReceived(This,pXHR,pResponseStream) ) 
#define IXMLHTTPRequest2Callback_OnError(This,pXHR,hrError)	\
    ( (This)->lpVtbl -> OnError(This,pXHR,hrError) ) 
#endif 
EXTERN_C const IID IID_IXMLHTTPRequest2;
    typedef struct IXMLHTTPRequest2Vtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLHTTPRequest2 * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLHTTPRequest2 * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLHTTPRequest2 * This);
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IXMLHTTPRequest2 * This,
             const WCHAR *pwszMethod,
             const WCHAR *pwszUrl,
             IXMLHTTPRequest2Callback *pStatusCallback,
             const WCHAR *pwszUserName,
             const WCHAR *pwszPassword,
             const WCHAR *pwszProxyUserName,
             const WCHAR *pwszProxyPassword);
        HRESULT ( STDMETHODCALLTYPE *Send )( 
            IXMLHTTPRequest2 * This,
             ISequentialStream *pBody,
             ULONGLONG cbBody);
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IXMLHTTPRequest2 * This);
        HRESULT ( STDMETHODCALLTYPE *SetCookie )( 
            IXMLHTTPRequest2 * This,
             const XHR_COOKIE *pCookie,
             DWORD *pdwCookieState);
        HRESULT ( STDMETHODCALLTYPE *SetCustomResponseStream )( 
            IXMLHTTPRequest2 * This,
             ISequentialStream *pSequentialStream);
        HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IXMLHTTPRequest2 * This,
             XHR_PROPERTY eProperty,
             ULONGLONG ullValue);
        HRESULT ( STDMETHODCALLTYPE *SetRequestHeader )( 
            IXMLHTTPRequest2 * This,
             const WCHAR *pwszHeader,
             const WCHAR *pwszValue);
        HRESULT ( STDMETHODCALLTYPE *GetAllResponseHeaders )( 
            IXMLHTTPRequest2 * This,
             WCHAR **ppwszHeaders);
        HRESULT ( STDMETHODCALLTYPE *GetCookie )( 
            IXMLHTTPRequest2 * This,
             const WCHAR *pwszUrl,
             const WCHAR *pwszName,
             DWORD dwFlags,
             ULONG *pcCookies,
             XHR_COOKIE **ppCookies);
        HRESULT ( STDMETHODCALLTYPE *GetResponseHeader )( 
            IXMLHTTPRequest2 * This,
             const WCHAR *pwszHeader,
             WCHAR **ppwszValue);
        END_INTERFACE
    } IXMLHTTPRequest2Vtbl;
    interface __uuid("E5D37DC0-552A-4D52-9CC0-A14D546FBD04") IXMLHTTPRequest2

    {
        CONST_VTBL struct IXMLHTTPRequest2Vtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLHTTPRequest2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLHTTPRequest2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLHTTPRequest2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLHTTPRequest2_Open(This,pwszMethod,pwszUrl,pStatusCallback,pwszUserName,pwszPassword,pwszProxyUserName,pwszProxyPassword)	\
    ( (This)->lpVtbl -> Open(This,pwszMethod,pwszUrl,pStatusCallback,pwszUserName,pwszPassword,pwszProxyUserName,pwszProxyPassword) ) 
#define IXMLHTTPRequest2_Send(This,pBody,cbBody)	\
    ( (This)->lpVtbl -> Send(This,pBody,cbBody) ) 
#define IXMLHTTPRequest2_Abort(This)	\
    ( (This)->lpVtbl -> Abort(This) ) 
#define IXMLHTTPRequest2_SetCookie(This,pCookie,pdwCookieState)	\
    ( (This)->lpVtbl -> SetCookie(This,pCookie,pdwCookieState) ) 
#define IXMLHTTPRequest2_SetCustomResponseStream(This,pSequentialStream)	\
    ( (This)->lpVtbl -> SetCustomResponseStream(This,pSequentialStream) ) 
#define IXMLHTTPRequest2_SetProperty(This,eProperty,ullValue)	\
    ( (This)->lpVtbl -> SetProperty(This,eProperty,ullValue) ) 
#define IXMLHTTPRequest2_SetRequestHeader(This,pwszHeader,pwszValue)	\
    ( (This)->lpVtbl -> SetRequestHeader(This,pwszHeader,pwszValue) ) 
#define IXMLHTTPRequest2_GetAllResponseHeaders(This,ppwszHeaders)	\
    ( (This)->lpVtbl -> GetAllResponseHeaders(This,ppwszHeaders) ) 
#define IXMLHTTPRequest2_GetCookie(This,pwszUrl,pwszName,dwFlags,pcCookies,ppCookies)	\
    ( (This)->lpVtbl -> GetCookie(This,pwszUrl,pwszName,dwFlags,pcCookies,ppCookies) ) 
#define IXMLHTTPRequest2_GetResponseHeader(This,pwszHeader,ppwszValue)	\
    ( (This)->lpVtbl -> GetResponseHeader(This,pwszHeader,ppwszValue) ) 
#endif 
typedef struct tagXHR_CERT
    {
    DWORD cbCert;
     BYTE *pbCert;
    } 	XHR_CERT;
EXTERN_C const IID IID_IXMLHTTPRequest3Callback;
    typedef struct IXMLHTTPRequest3CallbackVtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLHTTPRequest3Callback * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLHTTPRequest3Callback * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLHTTPRequest3Callback * This);
        HRESULT ( STDMETHODCALLTYPE *OnRedirect )( 
            IXMLHTTPRequest3Callback * This,
             IXMLHTTPRequest2 *pXHR,
             const WCHAR *pwszRedirectUrl);
        HRESULT ( STDMETHODCALLTYPE *OnHeadersAvailable )( 
            IXMLHTTPRequest3Callback * This,
             IXMLHTTPRequest2 *pXHR,
             DWORD dwStatus,
             const WCHAR *pwszStatus);
        HRESULT ( STDMETHODCALLTYPE *OnDataAvailable )( 
            IXMLHTTPRequest3Callback * This,
             IXMLHTTPRequest2 *pXHR,
             ISequentialStream *pResponseStream);
        HRESULT ( STDMETHODCALLTYPE *OnResponseReceived )( 
            IXMLHTTPRequest3Callback * This,
             IXMLHTTPRequest2 *pXHR,
             ISequentialStream *pResponseStream);
        HRESULT ( STDMETHODCALLTYPE *OnError )( 
            IXMLHTTPRequest3Callback * This,
             IXMLHTTPRequest2 *pXHR,
             HRESULT hrError);
        HRESULT ( STDMETHODCALLTYPE *OnServerCertificateReceived )( 
            IXMLHTTPRequest3Callback * This,
             IXMLHTTPRequest3 *pXHR,
             DWORD dwCertificateErrors,
             DWORD cServerCertificateChain,
             const XHR_CERT *rgServerCertificateChain);
        HRESULT ( STDMETHODCALLTYPE *OnClientCertificateRequested )( 
            IXMLHTTPRequest3Callback * This,
             IXMLHTTPRequest3 *pXHR,
             DWORD cIssuerList,
             const WCHAR **rgpwszIssuerList);
        END_INTERFACE
    } IXMLHTTPRequest3CallbackVtbl;
    interface __uuid("b9e57830-8c6c-4a6f-9c13-47772bb047bb") IXMLHTTPRequest3Callback

    {
        CONST_VTBL struct IXMLHTTPRequest3CallbackVtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLHTTPRequest3Callback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLHTTPRequest3Callback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLHTTPRequest3Callback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLHTTPRequest3Callback_OnRedirect(This,pXHR,pwszRedirectUrl)	\
    ( (This)->lpVtbl -> OnRedirect(This,pXHR,pwszRedirectUrl) ) 
#define IXMLHTTPRequest3Callback_OnHeadersAvailable(This,pXHR,dwStatus,pwszStatus)	\
    ( (This)->lpVtbl -> OnHeadersAvailable(This,pXHR,dwStatus,pwszStatus) ) 
#define IXMLHTTPRequest3Callback_OnDataAvailable(This,pXHR,pResponseStream)	\
    ( (This)->lpVtbl -> OnDataAvailable(This,pXHR,pResponseStream) ) 
#define IXMLHTTPRequest3Callback_OnResponseReceived(This,pXHR,pResponseStream)	\
    ( (This)->lpVtbl -> OnResponseReceived(This,pXHR,pResponseStream) ) 
#define IXMLHTTPRequest3Callback_OnError(This,pXHR,hrError)	\
    ( (This)->lpVtbl -> OnError(This,pXHR,hrError) ) 
#define IXMLHTTPRequest3Callback_OnServerCertificateReceived(This,pXHR,dwCertificateErrors,cServerCertificateChain,rgServerCertificateChain)	\
    ( (This)->lpVtbl -> OnServerCertificateReceived(This,pXHR,dwCertificateErrors,cServerCertificateChain,rgServerCertificateChain) ) 
#define IXMLHTTPRequest3Callback_OnClientCertificateRequested(This,pXHR,cIssuerList,rgpwszIssuerList)	\
    ( (This)->lpVtbl -> OnClientCertificateRequested(This,pXHR,cIssuerList,rgpwszIssuerList) ) 
#endif 
EXTERN_C const IID IID_IXMLHTTPRequest3;
    typedef struct IXMLHTTPRequest3Vtbl
    {
        BEGIN_INTERFACE
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IXMLHTTPRequest3 * This,
             REFIID riid,
             void **ppvObject);
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IXMLHTTPRequest3 * This);
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IXMLHTTPRequest3 * This);
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IXMLHTTPRequest3 * This,
             const WCHAR *pwszMethod,
             const WCHAR *pwszUrl,
             IXMLHTTPRequest2Callback *pStatusCallback,
             const WCHAR *pwszUserName,
             const WCHAR *pwszPassword,
             const WCHAR *pwszProxyUserName,
             const WCHAR *pwszProxyPassword);
        HRESULT ( STDMETHODCALLTYPE *Send )( 
            IXMLHTTPRequest3 * This,
             ISequentialStream *pBody,
             ULONGLONG cbBody);
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IXMLHTTPRequest3 * This);
        HRESULT ( STDMETHODCALLTYPE *SetCookie )( 
            IXMLHTTPRequest3 * This,
             const XHR_COOKIE *pCookie,
             DWORD *pdwCookieState);
        HRESULT ( STDMETHODCALLTYPE *SetCustomResponseStream )( 
            IXMLHTTPRequest3 * This,
             ISequentialStream *pSequentialStream);
        HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IXMLHTTPRequest3 * This,
             XHR_PROPERTY eProperty,
             ULONGLONG ullValue);
        HRESULT ( STDMETHODCALLTYPE *SetRequestHeader )( 
            IXMLHTTPRequest3 * This,
             const WCHAR *pwszHeader,
             const WCHAR *pwszValue);
        HRESULT ( STDMETHODCALLTYPE *GetAllResponseHeaders )( 
            IXMLHTTPRequest3 * This,
             WCHAR **ppwszHeaders);
        HRESULT ( STDMETHODCALLTYPE *GetCookie )( 
            IXMLHTTPRequest3 * This,
             const WCHAR *pwszUrl,
             const WCHAR *pwszName,
             DWORD dwFlags,
             ULONG *pcCookies,
             XHR_COOKIE **ppCookies);
        HRESULT ( STDMETHODCALLTYPE *GetResponseHeader )( 
            IXMLHTTPRequest3 * This,
             const WCHAR *pwszHeader,
             WCHAR **ppwszValue);
        HRESULT ( STDMETHODCALLTYPE *SetClientCertificate )( 
            IXMLHTTPRequest3 * This,
             DWORD cbClientCertificateHash,
             const BYTE *pbClientCertificateHash,
             const WCHAR *pwszPin);
        END_INTERFACE
    } IXMLHTTPRequest3Vtbl;
    interface __uuid("a1c9feee-0617-4f23-9d58-8961ea43567c") IXMLHTTPRequest3

    {
        CONST_VTBL struct IXMLHTTPRequest3Vtbl *lpVtbl;
    };
#ifdef COBJMACROS
#define IXMLHTTPRequest3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 
#define IXMLHTTPRequest3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 
#define IXMLHTTPRequest3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 
#define IXMLHTTPRequest3_Open(This,pwszMethod,pwszUrl,pStatusCallback,pwszUserName,pwszPassword,pwszProxyUserName,pwszProxyPassword)	\
    ( (This)->lpVtbl -> Open(This,pwszMethod,pwszUrl,pStatusCallback,pwszUserName,pwszPassword,pwszProxyUserName,pwszProxyPassword) ) 
#define IXMLHTTPRequest3_Send(This,pBody,cbBody)	\
    ( (This)->lpVtbl -> Send(This,pBody,cbBody) ) 
#define IXMLHTTPRequest3_Abort(This)	\
    ( (This)->lpVtbl -> Abort(This) ) 
#define IXMLHTTPRequest3_SetCookie(This,pCookie,pdwCookieState)	\
    ( (This)->lpVtbl -> SetCookie(This,pCookie,pdwCookieState) ) 
#define IXMLHTTPRequest3_SetCustomResponseStream(This,pSequentialStream)	\
    ( (This)->lpVtbl -> SetCustomResponseStream(This,pSequentialStream) ) 
#define IXMLHTTPRequest3_SetProperty(This,eProperty,ullValue)	\
    ( (This)->lpVtbl -> SetProperty(This,eProperty,ullValue) ) 
#define IXMLHTTPRequest3_SetRequestHeader(This,pwszHeader,pwszValue)	\
    ( (This)->lpVtbl -> SetRequestHeader(This,pwszHeader,pwszValue) ) 
#define IXMLHTTPRequest3_GetAllResponseHeaders(This,ppwszHeaders)	\
    ( (This)->lpVtbl -> GetAllResponseHeaders(This,ppwszHeaders) ) 
#define IXMLHTTPRequest3_GetCookie(This,pwszUrl,pwszName,dwFlags,pcCookies,ppCookies)	\
    ( (This)->lpVtbl -> GetCookie(This,pwszUrl,pwszName,dwFlags,pcCookies,ppCookies) ) 
#define IXMLHTTPRequest3_GetResponseHeader(This,pwszHeader,ppwszValue)	\
    ( (This)->lpVtbl -> GetResponseHeader(This,pwszHeader,ppwszValue) ) 
#define IXMLHTTPRequest3_SetClientCertificate(This,cbClientCertificateHash,pbClientCertificateHash,pwszPin)	\
    ( (This)->lpVtbl -> SetClientCertificate(This,cbClientCertificateHash,pbClientCertificateHash,pwszPin) ) 
#endif 
#ifdef __USE_MSXML6_NAMESPACE__
}
#endif
unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 
unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 
unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 
unsigned long             __RPC_USER  VARIANT_UserSize64(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal64(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree64(     unsigned long *, VARIANT * ); 
#ifdef __cplusplus
#endif
#endif
