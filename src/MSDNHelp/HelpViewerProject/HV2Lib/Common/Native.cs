namespace HVProject.HV2Lib.Common
{
    using System;
    using System.Runtime.InteropServices;
    using Microsoft.Win32;
    

    public class Native
    {
        public static readonly int S_OK = 0;

        [DllImport("kernel32.dll", SetLastError=true)]
        [return:MarshalAs(UnmanagedType.Bool)]
        extern static bool IsWow64Process(IntPtr hProcess, [MarshalAs(UnmanagedType.Bool)] out bool isWow64);
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError=true)]
        extern static IntPtr GetCurrentProcess();
        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        extern static IntPtr GetModuleHandle(string moduleName);
        [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError=true)]
        extern static IntPtr GetProcAddress(IntPtr hModule, string methodName);

        public static bool Is64BitProcess
        {
            get { return IntPtr.Size == 8; }   //current process is 64-bit
        }

        // Note: You can simply this if - Windows 7, Windows Vista SP1 or later, Windows XP SP3, Windows Server 2008 (Server Core not supported), Windows Server 2008 R2 (Server Core supported with SP1 or later), Windows Server 2003 SP2
        //    RegistryView view = Environment.Is64BitOperatingSystem ? RegistryView.Registry64 : RegistryView.Registry32;

        public static bool Is64BitOperatingSystem
        {
            get
            {
                if (Is64BitProcess)  // Clearly if this is a 64-bit process we must be on a 64-bit OS.
                    return true;
                // We are a 32-bit process, but is the OS 64-bit? 32-bit OS won't contain function 'IsWow64Process()'
                bool isWow64;
                return ModuleContainsFunction("kernel32.dll", "IsWow64Process") && IsWow64Process(GetCurrentProcess(), out isWow64) && isWow64;
            }
        }

        public static bool ModuleContainsFunction(string moduleName, string methodName)
        {
        IntPtr hModule = GetModuleHandle(moduleName);
        if (hModule != IntPtr.Zero)
            return GetProcAddress(hModule, methodName) != IntPtr.Zero;
        return false;
        }

        //http://msdn.microsoft.com/en-us/library/windows/desktop/bb759827(v=vs.85).aspx

        [DllImport("uxtheme.dll", CharSet = CharSet.Unicode)]
        extern static int SetWindowTheme(IntPtr hWnd, string pszSubAppName, string pszSubIdList);

        //Theme TreeView or ListView as Windows Explorer Theme eg. SetWindowTheme(this.treeView1.Handle, "explorer"); Use null to set back to app theme
        public static Boolean SetWindowTheme(IntPtr hWnd, string pszSubAppName)  
        {
            return SetWindowTheme(hWnd, pszSubAppName, null) == S_OK;
        }


    }


}




 
