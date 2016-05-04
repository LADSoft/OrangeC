namespace HVProject.HV2Lib.Common
{
    using Microsoft.Win32;
    using System;
    using System.Collections.Generic;
    using System.Configuration;
    using System.Globalization;
    using System.IO;
    using System.Reflection;

    /// <summary>
    /// Static class to cache HV2 path info from class "HV2RegInfo"
    /// Note that this only caches VS 11+ help paths.
    /// </summary>
    public static class HV2Reg
    {
        private static String mAppRootDir = String.Empty;               
        private static String mDefaultHelpViewerCLSID = String.Empty;
        private static String mContentStore = String.Empty;
        private static RegistryView mRegistryView = RegistryView.Registry32;   //32bit or 64 bit area of the registry?

        public static void Initialize()
        {
            if (String.IsNullOrEmpty(mAppRootDir))  //not yet initialized (or maybe HV2 is not installed)
            {
                HV2RegInfo regInfo = new HV2RegInfo();

                //Populate local members
                mAppRootDir = regInfo.AppRoot;
                mDefaultHelpViewerCLSID = regInfo.DefaultHelpViewerCLSID;
                mContentStore = regInfo.ContentStore;
                mRegistryView = regInfo.RegistryView;
            }
        }

        public static String AppRoot
        {
            get
            {
                Initialize();
                return mAppRootDir;
            }
        }

        public static String DefaultHelpViewerCLSID
        {
            get
            {
                Initialize();
                return mDefaultHelpViewerCLSID;
            }
        }

        public static String ContentStore
        {
            get
            {
                Initialize();
                return mContentStore;
            }
        }

        public static RegistryView RegistryView
        {
            get
            {
                Initialize();
                return mRegistryView;
            }
        }


        //HV2 files
        //  • HlpViewer.exe help viewer same as HV 1.x
        //  • HlpCtrntMgr.exe. Background helper app (no UI). Replaces HelpLibManager. For updates and registration etc. Command line and COM interface. Uses BITS service so can kick off downloads and close.
        // 	• Microsoft.VisualStudio.Help.dll - Rendering code. Windows 8 files will be "Microsoft.Windows.Help.dll" or some such thing.
        // 	• Windows.Help.Runtime.dll --  Main DLL to interface with via COM. Has TLB available for win32. Windows 8 will have  Windows 8 files will have something like "Microsoft.Windows.Help.runtime.dll" 
        // 	• Windows.Help.Runtime.dll -- Windows 8 file of same 

        // --- Misc ---

        public static String MakeAppRootPath(String filename)
        {
            Initialize();  // This gets mAppRootDir
            if (!String.IsNullOrEmpty(mAppRootDir))
                return Path.Combine(AppRoot, filename);
            return String.Empty;
        }

        public static string ReadRegistryValue(RegistryHive registryHive, string subKey, string KeyName)
        {
            Initialize();  //This gets mRegistryView (32 or 62 bit registry)
            return ReadRegistryValue(registryHive, subKey, KeyName, mRegistryView);
        }


        //General registry call. RegistryView specifies 32 or 64 bit registry.
        public static string ReadRegistryValue(RegistryHive registryHive, string subKey, string KeyName, RegistryView registryView)
        {
            RegistryKey baseKeyName = null;
            RegistryKey key2 = null;
            string sReturnValue = string.Empty;

            try
            {
                baseKeyName = RegistryKey.OpenBaseKey(registryHive, registryView);  // eg RegistryView.Registry64
                if (baseKeyName != null)
                {
                    key2 = baseKeyName.OpenSubKey(subKey);
                    if (key2 != null)
                    {
                        string keyValue = (string)key2.GetValue(KeyName);
                        if (keyValue != null)
                        {
                            sReturnValue = keyValue;
                        }
                    }
                }
            }
            finally
            {
                if (key2 != null)
                {
                    key2.Close();
                }
                if (baseKeyName != null)
                {
                    baseKeyName.Close();
                }
            }
            return sReturnValue;
        }




    }
}


