using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using HV2ApiExplore.Forms;
using HVProject.HV2Lib.Common;   //Make sure you add a reference - browse to C:\Program Files\Microsoft Help Viewer\v2.0\Windows.Help.Runtime.dll
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
using HVProject.HV2Lib.View;
using System.Windows.Forms;
using System.IO;

namespace HV2ApiExplore
{
    public static class Globals
    {
        //Registry key for saving UI Data
        static public readonly string DataRegKey = @"Software\Helpware\HV2TestApp\UIData";
        static public Form mainForm = null;
        static public String mainFormTitle = "";

        // Help Viewer 2.x objects
        public static Catalog catalog = null;          //Open/Close catalogs
        public static CatalogRead catalogRead = null;  //Read catalog data
        public static FlatCatalogListItem selectedCatalog = null;

        internal static void Initialize(Form aMainForm)
        {
            mainForm = aMainForm;
            mainFormTitle = aMainForm.Text;
            catalog = new Catalog();
            catalogRead = new CatalogRead();
        }

        public const int ID_PAGE_CATALOGS = 0;
        public static int ID_PAGE_NOTES = 1;
        public static int ID_PAGE_SEARCH = 2;
        public static int ID_PAGE_TOC = 3;
        public static int ID_PAGE_KEYWORDS = 4;
        public static int ID_PAGE_TOPICS = 5;
        public static int ID_PAGE_STORE = 101;
        public static int ID_PAGE_EVENTLOG = 102;
        
        internal static void ShowAppHelp(int pageId)
        {
            String helpPage = "";
            if (pageId == ID_PAGE_CATALOGS)
                helpPage = "catalogs";
            else if (pageId == ID_PAGE_NOTES)
                helpPage = "notes";
            else if (pageId == ID_PAGE_SEARCH)
                helpPage = "search";
            else if (pageId == ID_PAGE_TOC)
                helpPage = "toc";
            else if (pageId == ID_PAGE_KEYWORDS)
                helpPage = "keywords";
            else if (pageId == ID_PAGE_TOPICS)
                helpPage = "topics";
            else if (pageId == ID_PAGE_STORE)
                helpPage = "";
            else if (pageId == ID_PAGE_EVENTLOG)
                helpPage = "";
            Process.Start("http://hv2.helpmvp.com/code/api-test-app/" + helpPage);
        } 

        internal static void StartHelpViewer(String pageID)  // run HlpViewer.exe and show the topic
        {
            StartHelpViewer(pageID, false);
        }

        internal static void StartHelpViewer(String pageID, Boolean runAsAdmin)  // run HlpViewer.exe and show the topic
        {
            String catalog = "";
            String locale = "";
            if (Globals.selectedCatalog != null)
            {
                //We can open with just a CatalogID if it came from the registry
                if (selectedCatalog.FromRegistry)
                {
                    catalog = Globals.selectedCatalog.CatalogID;  // The viewer will look up the registry to find the path
                }
                else
                {
                    MessageBox.Show("Currently only registered collections can be opened in VS 11 HlpViewer.exe");

                    //if (File.Exists(selectedCatalog.LocationPath))  //an .mshx file?
                        //catalog = selectedCatalog.LocationPath;
                    //else if (Directory.Exists(selectedCatalog.LocationPath))  // a location dir
                        //catalog = selectedCatalog.LocationPath;
                }

                if (String.IsNullOrEmpty(catalog))            
                {
                    MessageBox.Show("Catalog name not registered or invalid location:\n" + "Name: " + selectedCatalog.CatalogID + ", Location: " + selectedCatalog.LocationPath);
                    return;
                }

                catalog = "\"" + catalog + "\""; //wrap in quotes in case of spaces

                locale = selectedCatalog.Locale;
            }

            if (String.IsNullOrEmpty(catalog))            //default to something
            {
                catalog = HlpViewer.VS11CatalogName;
            }

            if (String.IsNullOrEmpty(locale))
                locale = "en-US";

            if (String.IsNullOrEmpty(pageID))
                pageID = "-1";  //root node

            HlpViewer.StartReturn sRet = HlpViewer.Start(catalog, locale, pageID, runAsAdmin);
            if (sRet != HlpViewer.StartReturn.ok)
                MessageBox.Show("Error running HlpViewer.exe: " + sRet.ToString() + "\n" + HlpViewer.path);
        }

        internal static void SetWindowTitle(String text)
        {
            if (String.IsNullOrEmpty(text))
                mainForm.Text = mainFormTitle;
            else 
                mainForm.Text = mainFormTitle + " - " + text;
        }






    }
}
