using System;
using System.Runtime.InteropServices;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif

namespace HVProject.HV2Lib.PluggableProtocol
{
    // Class factory implementation for instantiating HelpProtocol
    public class HelpClassFactory : IClassFactory
    {
        private const uint E_NOINTERFACE = 0x80004002;
        private static readonly Guid IID_IUnknown = new Guid("00000000-0000-0000-C000-000000000046");
        private static readonly Guid IID_IInternetProtocol = new Guid("79EAC9E4-BAF9-11CE-8C82-00AA004BA90B");
        private static readonly Guid IID_IInternetProtocolRoot = new Guid("79EAC9E3-BAF9-11CE-8C82-00AA004BA90B");
        private const uint S_OK = 0;

        private HelpAsyncPluggableProtocolHandler helpAPPHandler = null;

        public HelpClassFactory()
        {
        }

        public uint CreateInstance(object pUnkOuter, ref Guid riid, out IntPtr ppvObject)
        {
            ppvObject = IntPtr.Zero;
            Type itfType;
            if (riid == IID_IInternetProtocol
             || riid == IID_IInternetProtocolRoot
             || riid == IID_IUnknown)
            {
                itfType = typeof(IInternetProtocol);
            }
            else
                return E_NOINTERFACE;

            helpAPPHandler = new HelpAsyncPluggableProtocolHandler();
            ppvObject = Marshal.GetComInterfaceForObject(helpAPPHandler, itfType);
            return S_OK;
        }

        public void LockServer(bool fLock)
        {
        }


    }

}