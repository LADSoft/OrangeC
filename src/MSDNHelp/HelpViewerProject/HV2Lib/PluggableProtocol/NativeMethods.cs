using System;
using System.Runtime.InteropServices;
using System.Diagnostics.CodeAnalysis;

namespace HVProject.HV2Lib.PluggableProtocol
{
    public class HRESULT
    {
        public static readonly int S_OK = 0;
        public static readonly int S_FALSE = 1;
        public static readonly int E_NOTIMPL = 2;
        public static readonly int E_INVALIDARG = 3;
        public static readonly int E_FAIL = 4;
        public static readonly uint INET_E_CANNOT_LOAD_DATA = 0x800C000F;
        public static readonly uint INET_E_OBJECT_NOT_FOUND = 0x800C0006;
    }

    public class NativeMethods
    {
        [DllImport("urlmon.dll", PreserveSig = false)]
        public static extern void CoInternetGetSession(uint dwSessionMode, out IInternetSession ppIInternetSession, uint dwReserved);
    }
}
