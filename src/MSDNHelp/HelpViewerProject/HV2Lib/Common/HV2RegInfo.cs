namespace HVProject.HV2Lib.Common
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using Microsoft.Win32;
    using System.Windows.Forms;

    /// <summary>
    /// Class reads the registry and gets all the relavent HV2 info
    /// Note that this is VS 11+ help paths only. Windows 8 info is elsewhere.
    /// </summary>

    public class HV2RegInfo
    {
        public readonly String AppRoot = String.Empty;                         // location of HV2 runtime files 
        public readonly String DefaultHelpViewerCLSID = String.Empty;          // GUID specifies the default viewer
        public readonly String ContentStore = String.Empty;                    // location of VS 11 main catalog store
        public readonly RegistryView RegistryView = RegistryView.Registry32;   // 32bit or 64 bit area of the registry?

        public readonly String KEY_Help_v20 = @"SOFTWARE\Microsoft\Help\v2.0";
        public readonly String KEY_Help_v20_Catalogs = @"SOFTWARE\Microsoft\Help\v2.0\Catalogs";

        /// <summary>
        /// Get HelpViewer 2.0 system information.
        /// The registry keys point to the runtime installation location.
        /// Also in the registry is a list of registered help catalogs. Required by VS 11 HlpViewer.exe to open any catalog (ie. must be registered).
        /// </summary>

        public HV2RegInfo() // Constructor
        {
            //Sniff out if 64-bit or 32-bit area of the registry. App Root should always be there.
            //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0] AppRoot=C:\Program Files\Microsoft Help Viewer\v2.0\
            if (Native.Is64BitOperatingSystem)
            {
                this.RegistryView = RegistryView.Registry64;
                AppRoot = HV2Reg.ReadRegistryValue(RegistryHive.LocalMachine, KEY_Help_v20, "AppRoot", this.RegistryView);
            }
            if (String.IsNullOrEmpty(AppRoot))
            {
                this.RegistryView = RegistryView.Registry32;
                AppRoot = HV2Reg.ReadRegistryValue(RegistryHive.LocalMachine, KEY_Help_v20, "AppRoot", this.RegistryView);
            }

            RegistryKey baseKeyName = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, this.RegistryView);
            if (baseKeyName != null)
            {
                RegistryKey subkey = baseKeyName.OpenSubKey(KEY_Help_v20);
                if (subkey != null)
                {
                    //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs] DefaultHelpViewerCLSID={FE220E43-FE17-47BE-A57E-84D2B9A5635D}
                    DefaultHelpViewerCLSID = (string)subkey.GetValue("DefaultHelpViewerCLSID");
                    subkey.Close();
                }

                subkey = baseKeyName.OpenSubKey(KEY_Help_v20_Catalogs);
                if (subkey != null)
                {
                    //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs] ContentStore=C:\ProgramData\Microsoft\HelpLibrary2\Catalogs\
                    ContentStore = (string)subkey.GetValue("ContentStore");
                    subkey.Close();
                }

                baseKeyName.Close();
            }
        }



    }
}
