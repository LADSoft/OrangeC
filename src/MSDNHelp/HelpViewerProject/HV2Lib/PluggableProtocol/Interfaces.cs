using System;
using System.Runtime.InteropServices;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices.ComTypes;

namespace HVProject.HV2Lib.PluggableProtocol
{
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("79EAC9E3-BAF9-11CE-8C82-00AA004BA90B")]
    [ComVisible(true)]
    public interface IInternetProtocolRoot
    {
        void Start(
           [MarshalAs(UnmanagedType.LPWStr)]string szURL,
           IInternetProtocolSink sink,
           IInternetBindInfo pOIBindInfo,
           UInt32 grfPI,
           UInt32 dwReserved
        );

        void Continue(ref _tagPROTOCOLDATA pProtocolData);
        void Abort(Int32 hrReason, UInt32 dwOptions);
        void Terminate(UInt32 dwOptions);
        void Suspend();
        void Resume();
    }

    public struct _LARGE_INTEGER
    {
        public Int64 QuadPart;
    }

    public struct SECURITY_ATTRIBUTES
    {
        public UInt32 nLength;
        public IntPtr lpSecurityDescriptor;
        public bool bInheritHandle;
    }

    public enum BSCF : int
    {
        FIRSTDATANOTIFICATION = 0,
        INTERMEDIATEDATANOTIFICATION = 1,
        LASTDATANOTIFICATION = 2,
        DATAFULLYAVAILABLE = 3,
        AVAILABLEDATASIZEUNKNOWN = 4,
    }

    public struct BINDINFO
    {
        public UInt32 cbSize;
        [MarshalAs(UnmanagedType.LPWStr)]
        public string szExtraInfo;
        public STGMEDIUM stgmedData;
        public UInt32 grfBindInfoF;
        [MarshalAs(UnmanagedType.U4)]
        public BINDVERB dwBindVerb;
        [MarshalAs(UnmanagedType.LPWStr)]
        public string szCustomVerb;
        public UInt32 cbStgmedData;
        public UInt32 dwOptions;
        public UInt32 dwOptionsFlags;
        public UInt32 dwCodePage;
        public SECURITY_ATTRIBUTES securityAttributes;
        public Guid iid;
        [MarshalAs(UnmanagedType.IUnknown)]
        public object pUnk;
        public UInt32 dwReserved;
    }

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("79EAC9E1-BAF9-11CE-8C82-00AA004BA90B")]
    [ComVisible(true)]
    public interface IInternetBindInfo
    {
        void GetBindInfo(out UInt32 grfBind, [In, Out] ref BINDINFO pBindInfo);
        void GetBindString(UInt32 ulStringType, [MarshalAs(UnmanagedType.LPWStr)] ref string szString, UInt32 cElements, ref UInt32 pcElementsFetched);
    }

    public enum BINDVERB : uint
    {
        GET = 0,
        POST = 1,
        PUT = 2,
        CUSTOM = 3,
    }

    public struct _tagPROTOCOLDATA
    {
        public uint grfFlags;
        public uint dwState;
        public IntPtr pData;
        public uint cbData;
    }

    public struct _ULARGE_INTEGER
    {
        public UInt64 QuadPart;
    }

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("79EAC9E5-BAF9-11CE-8C82-00AA004BA90B")]
    [ComVisible(true)]
    public interface IInternetProtocolSink
    {
        void Switch(ref _tagPROTOCOLDATA pProtocolData);
        void ReportProgress(UInt32 ulStatusCode, [MarshalAs(UnmanagedType.LPWStr)] string szStatusText);
        void ReportData(BSCF grfBSCF, UInt32 ulProgress, UInt32 ulProgressMax);
        void ReportResult(UInt32 hrResult, UInt32 dwError, [MarshalAs(UnmanagedType.LPWStr)] string szResult);
    }

    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    [Guid("79EAC9E4-BAF9-11CE-8C82-00AA004BA90B")]
    [ComVisible(true)]
    public interface IInternetProtocol
    {
        // IInternetProtocolRoot methods
        void Start(
           [MarshalAs(UnmanagedType.LPWStr)] string szUrl,
           IInternetProtocolSink sink,
           IInternetBindInfo pOIBindInfo,
           UInt32 grfPI,
           UInt32 dwReserved
        );
        void Continue(ref _tagPROTOCOLDATA pProtocolData);
        void Abort(Int32 hrReason, UInt32 dwOptions);
        void Terminate(UInt32 dwOptions);
        void Suspend();
        void Resume();

        // IInternetProtocol methods
        [PreserveSig()]
        UInt32 Read(IntPtr pv, UInt32 cb, out UInt32 pcbRead);
        void Seek(_LARGE_INTEGER dlibMove, UInt32 dwOrigin, out _ULARGE_INTEGER plibNewPosition);
        void LockRequest(UInt32 dwOptions);
        void UnlockRequest();
    }

    /// <summary>
    /// IInternetSession definition
    /// </summary>
    [
        ComImport(),
        InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
        Guid("79eac9e7-baf9-11ce-8c82-00aa004ba90b")
    ]
    public interface IInternetSession
    {
        void RegisterNameSpace([MarshalAs(UnmanagedType.Interface)]IClassFactory pCF,
        ref Guid rclsid,
        [MarshalAs(UnmanagedType.LPWStr)]string pwzProtocol,
        uint cPatterns,
        [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)] string[] ppwzPatterns,
        uint dwReserved);

        void UnregisterNameSpace([MarshalAs(UnmanagedType.Interface)]IClassFactory pCF, [MarshalAs(UnmanagedType.LPWStr)]string pszProtocol);

        void RegisterMimeFilter(IntPtr pCF, ref Guid rclsid, [MarshalAs(UnmanagedType.LPWStr)]string pwzType);

        void UnregisterMimeFilter(IntPtr pCF, [MarshalAs(UnmanagedType.LPWStr)]string pwzType);

        void CreateBinding(IntPtr pBC,
                            [MarshalAs(UnmanagedType.LPWStr)] string szUrl,
                            IntPtr pUnkOuter,
                            IntPtr ppUnk,
                            IInternetProtocol ppOInetProt,
                            uint dwOption);

        void SetSessionOption(uint dwOption, IntPtr pBuffer, uint dwBufferLength, uint dwReserved);

        void GetSessionOption(uint dwOption, IntPtr pBuffer, uint pdwBufferLength, uint dwReserved);
    }

    /// <summary>
    /// IClassFactory definitions
    /// </summary>
    [ComImport,
    ComVisible(false),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
    Guid("00000001-0000-0000-C000-000000000046")]
    public interface IClassFactory
    {
        [PreserveSig]
        uint CreateInstance([MarshalAs(UnmanagedType.IUnknown)] object pUnkOuter, ref Guid riid, out IntPtr ppvObject);

        void LockServer([MarshalAs(UnmanagedType.Bool)] bool fLock);
    }
}

