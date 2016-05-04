using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Helpware.Misc
{

    /// <summary>
    /// WinExec wraps System.Diagnostics.Process.Start() to include things like opening explorer and selecting a particular file.
    /// </summary>
    
    public static class WinExec
    {
        public static void ShowExplorer(String rootDir, String selectFile)  //Assumes file has already be checked for existance
        {
            string argument = "/root,\"" +rootDir+ "\" /select,\"" + selectFile + "\"";
            System.Diagnostics.Process.Start("explorer.exe", argument);
        }

        public static void ShowExplorer(String selectFile)  //Assumes file has already be checked for existance
        {
            string argument = "/select,\"" + selectFile + "\"";
            System.Diagnostics.Process.Start("explorer.exe", argument);
        }

        public static void ShowExplorer() 
        {
            string argument = "-p";  //documents folder
            System.Diagnostics.Process.Start("explorer.exe", argument);
        }

        /*
         * http://support.microsoft.com/kb/314853
         * The options that you can use with Explorer.exe are /n, /e, /root (plus an object), and /select (plus an object).
         *  /n       Opens a new single-pane window for the default
                     selection. This is usually the root of the drive that
                     Windows is installed on. If the window is already
                     open, a duplicate opens.
         * /e                Opens Windows Explorer in its default view.
         * /root,<object>    Opens a window view of the specified object.
         * /select,<object>  Opens a window view with the specified folder, file,
                     or program selected.
         * eg. Explorer /e,/root,C:\TestDir\TestProg.exe
         * eg. Explorer /select,C:\TestDir\TestProg.exe
         * eg. Explorer /root,\\TestSvr\TestShare
         * eg. Explorer /root,\\TestSvr\TestShare,select,TestProg.exe
         * 
         * My Computer
         * %SystemRoot%\explorer.exe /E,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}
         * 
         * My Documents
           %SystemRoot%\explorer.exe /N,::{450D8FBA-AD25-11D0-98A8-0800361B1103}
           
           Recycle Bin
         * %SystemRoot%\explorer.exe /N,::{645FF040-5081-101B-9F08-00AA002F954E}
         * 
         * Network Neighborhood
           %SystemRoot%\explorer.exe /N,::{208D2C60-3AEA-1069-A2D7-08002B30309D}
         * 
         * Default Web Browser or Navigator (IE, Firefox, Safari, Google Chrome)
         * %SystemRoot%\explorer.exe /N,::{871C5380-42A0-1069-A2EA-08002B30309D}
         * 
         * Computer Search Results Folder
         * %SystemRoot%\explorer.exe /N,::{1F4DE370-D627-11D1-BA4F-00A0C91EEDBA}
         * 
         * Network Search Results Folder
         * %SystemRoot%\explorer.exe /N,::{E17D4FC0-5564-11D1-83F2-00A0C90DC849}
         * 
           Web Folders
           %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{BDEADF00-C265-11D0-BCED-00A0C90AB50F}
           
         * Control Panel
         * %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{21EC2020-3AEA-1069-A2DD-08002B30309D}
         * 
           Printers and Faxes
         * %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\::{2227A280-3AEA-1069-A2DE-08002B30309D}
         * 
         * Scanners and Cameras
         * %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\::{E211B736-43FD-11D1-9EFB-0000F8757FCD}
         * 
         * Fonts
         * %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\::{D20EA4E1-3957-11d2-A40B-0C5020524152}
         *
         * Network Connections or My Network Place
         * %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\::{7007ACC7-3202-11D1-AAD2-00805FC1270E}
         *
         * Administrative Tools
         * %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\::{D20EA4E1-3957-11d2-A40B-0C5020524153}
         *
         * Tasks Scheduler
         * %SystemRoot%\explorer.exe /N,::{20D04FE0-3AEA-1069-A2D8-08002B30309D}\::{21EC2020-3AEA-1069-A2DD-08002B30309D}\::{D6277990-4C6A-11CF-8D87-00AA0060F5BF}
         * 
         */

    }
}
