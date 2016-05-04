using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using HVProject.HV2Lib.Common;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif

namespace HVProject.HV2Lib.View
{
    /// <summary>
    /// Class to help execute HlpViewer.exe and open at a catalog + topic.
    /// Note: This viewer currently ships with VS 11 only. Windows 8 has it's own viewer and runtime.
    /// VS 11 HlpViewer.exe can only open registered User Managed catalogs (see documentation for more)
    /// </summary>
    
    public static class HlpViewer
    {
        public const String VS11CatalogName = "VisualStudio11";
        public static String path;

        public enum StartReturn
        {
            ok,
            AppRootRegKeyNotFound,
            FileNotFound,
            Exception
        }

        // start HlpViewer.EXE -- the StartReturn enum returns status run.

        public static StartReturn Start(String catalogName, String locale, String pageID)
        {
            String cmdLine = "/catalogName " + catalogName + " /locale " + locale + " /helpQuery method=page&id="+pageID;
            return Start(cmdLine, false);
        }

        public static StartReturn RunAsAdmin(String catalogName, String locale, String pageID)
        {
            String cmdLine = "/catalogName " + catalogName + " /locale " + locale + " /helpQuery method=page&id=" + pageID;
            return Start(cmdLine, true);
        }

        public static StartReturn Start(String catalogName, String locale, String pageID, Boolean runElevated)
        {
            String cmdLine = "/catalogName " + catalogName + " /locale " + locale + " /helpQuery method=page&id=" + pageID;
            return Start(cmdLine, runElevated);
        }

        public static StartReturn Start()
        {
            String cmdLine = "/catalogName VisualStudio11 /locale en-us /helpQuery method=page&id=-1";
            return Start(cmdLine, false);
        }

        public static StartReturn RunAsAdmin()
        {
            String cmdLine = "/catalogName VisualStudio11 /locale en-us /helpQuery method=page&id=-1";
            return Start(cmdLine, true);
        }

        public static StartReturn Start(params String[] clArgList)
        {
            String cmdLine = String.Join(" ", clArgList);
            return Start(cmdLine, false);
        }

        public static StartReturn RunAsAdmin(params String[] clArgList)
        {
            String cmdLine = String.Join(" ", clArgList);
            return Start(cmdLine, true);
        }

        public static StartReturn Start(String cmdLine, Boolean runElevated)  
        {
            path = HV2Reg.MakeAppRootPath("HlpViewer.exe");

            if (String.IsNullOrEmpty(path))
                return StartReturn.AppRootRegKeyNotFound;

            if (!File.Exists(path))
                return StartReturn.FileNotFound;

            cmdLine = cmdLine.Trim();
            try
            {
                ProcessStartInfo psi = new ProcessStartInfo(path);
                if (!String.IsNullOrEmpty(cmdLine))
                    psi.Arguments = cmdLine;
                if (runElevated)
                    psi.Verb = "runas";
                Process.Start(psi);
            }
            catch
            {
                return StartReturn.Exception;
            }
            return StartReturn.ok;
        }


        // Command-line parameters

        /* When I press F1 in Visual Studio 11, hlpViewer.exe is called with the following parameters
         *    HelpViewer.exe /catalogName VisualStudio11 /helpQuery method=f1&query=VS.TextEditor&LCID=1033&LCID=1033&TargetFrameworkMoniker=.NETFramework,Version%3Dv4.0&DevLang=csharp /locale en-US /sku 3000 /launchingApp Microsoft,VisualStudio,11.0
         * 
         * /catalogName is the only required parameter. If you have more than one help language installed you will need to also use the /locale argument.
         * 
         * See the documentation for more details.
         */ 
          
          
         /**** Some ruff notes *****
         * 
         * HelpViewer.exe command line parameters
         *
         * Required:
         *   /catalogName VisualStudio11     There is currentky a 100 char length restriction on caralog names -- This can also be a path to the catalog (since a CatalogID will only work for registered catalogs)
         *   /locale en-us                    Defaults to thread locale if ommitted 
         *   /helpQuery method=page&id=-1  <see more below>
         *   
         * Optional:
         *   /Vendor Microsoft     tie-breaker. Set at merge time in .msha
         *   /ProductName xxx      tie-breaker. Set in document by meta tag MICROSOFT.HELP.PRODUCT   
         *   
         * Option 1 param
         *   /? or /help
         *   /silent               for silent installs
         *   /concurrent
         *   /noupdate
         *   /noonline
         *   /uninstall
         *   /manage
         *   /minimum              minimum install
         *   /update
         *   
         * Book Control    
         *   /launchingApp xx      if "Microsoft,VisualStudio,11.0" then hlpViewer starts sqmRecorder (this has a 200 char limit internally)
         *   /sku=3000             Used by different version of VS to setup book lists etc
         *   /sourceMedia ?
         *   /sourceWeb ?
         *   /content ?
         *   /mediaBookList ?
         *   /webBookList ?
         *   
         * /helpQuery method=page&id=-1               page id or -1 (TOC root)
         * /helpQuery method=f1&query=<keyword>       query can have multiple values separated by %00 (null)      
         * 
         * /helpQuery method=keywords&query=<keyword> Still supported?
         * /helpQuery method=search&query=<keyword>   Still supported?
         * 
         *    Additional
         *      &locale=en-us or &lcid=1033            defaults to thread locale if ommitted 
         *      &vendor=Microsoft                      tie-breaker for F1 calls
         *      &format=html or &format=xml
         *      &category=xx                           F1 tie-breaker - see topic meta MICROSOFT.HELP.CATEGORY 
         *      &topicversion=xx                       F1 tie-breaker - see topic meta MICROSOFT.HELP.TOPICVERSION
         *      &topiclocale=xx                        F1 tie-breaker - see topic meta MICROSOFT.HELP.LOCALE
         *      &targetFrameworkMoniker=xx             F1 tie-breaker - Note: encode =v4.0 as %3Dv4.0
         *          eg. targetframeworkmoniker=.NETFramework,Version=v2.0    
         *          eg. targetframeworkmoniker=.NETFramework,Version=v3.0
         *          eg. targetframeworkmoniker=.NETFramework,Version=v3.5
         *          eg. targetframeworkmoniker=.NETFramework,Version=v4.0
         *          eg. targetframeworkmoniker=Silverlight,Version=v2.0
         *          eg. targetframeworkmoniker=Silverlight,Version=v3.0
         *          eg. targetframeworkmoniker=Silverlight,Version=v4.0
         *      &devLang=xx                            Use as a filter for F1 calls - Currently used values are:  C++, VB, CSharp, FSharp, Jscript.
         *      &targetOS=xx
         *      &print=true
         *      &isExactTopicMatch=true
         *      
         *      eg <meta name="Microsoft.Help.Category" value="DevLang:CSharp" />
         *      eg <meta name="Microsoft.Help.Category" value="TargetFrameworkMoniker:.NETFramework,Version=v4.0" />
         * 
         * 
        */


    }
}
