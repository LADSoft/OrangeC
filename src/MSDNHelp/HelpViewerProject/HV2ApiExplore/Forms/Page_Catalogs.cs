using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
using HVProject.HV2Lib.Common;
using HV2ApiExplore.Forms;
using Helpware.Misc;
using System.Diagnostics;
using HVProject.HV2Lib.PluggableProtocol;

//----------------------------------------------------------------------------------------------
// Page: Catalogs
//   - Display registered catalogs 
//   - Open & Close a catalog 
//----------------------------------------------------------------------------------------------

namespace HV2ApiExplore.Forms
{
    public partial class Page_Catalogs : UserControl
    {
        public static Page_Catalogs self = null;

        private List<CatalogItem> _catalogList = null;               //orginal hierachical - eg. cat1/locale1|locale2, cat2/locale1|locale2,
        private List<FlatCatalogListItem> _flatCalaogList = null;    //flattened list - eg. cat1/local1, cat1/local2, cat2/local1 ...

        internal static Page_Catalogs Create()
        {
            self = new Page_Catalogs();
            return self;
        }

        public Page_Catalogs()
        {
            InitializeComponent();
            self = this;

            //List of available Catalogs
            _catalogList = CatalogList.GetCatalogs();   //Get list from the HV2 registry

            //Add Windows 8 catalogs if found
            String winDir = Environment.GetFolderPath(Environment.SpecialFolder.Windows);
            CatalogList.WildAddCollection(Path.Combine(winDir, "help\\windows\\"), _catalogList, "Windws 8 Help");
            CatalogList.WildAddCollection(Path.Combine(winDir, "help\\oem\\"), _catalogList, "Windws 8 OEM Help");

            FillCatalogListBox(0);
            RegistryDumpToDebug();

            //SuperTips
            new SuperTip(OpenCatalogBtnTip, "ICatalog.Open(catalogDirectory, localeList)",
                "Open a catalog to view its contents with the various ICatalogRead. methods.",
                "A valid catalog directory will contain the folders \"ContentStore\" & \"IndexStore\".",
                "Always close your catalogs when done by calling ICatalog.Close()."
                );

            new SuperTip(GetCatalogReadWriteLockBtnTip, "ICatalog.GetReadWriteLock(catalogDirectory)",
                "Returns an ICatalogReadWriteLock object to set/get locking at the catalog level.");

            new SuperTip(AvailableCatalogsTip, "Available Catalogs",
                "Microsoft HlpViewer.exe can open a catalog by a short ID if the catalog is registered (in registry).",
                "We can also open a catalog by the directory path. And there are special standalone help files",
                "(.MSHX) that contain both (.mshc) content and (.mshi) index information.");

            new SuperTip(AddBtnTip, "Add Catalog Folder or File",
                "You can open an unregistered catalog by using its full directory path.",
                "You can also open a .mshx file. These files contain both .mshc (content)",
                " & .mshi (index) information like a .chm or .hlp file.");


        }

        private void RegistryDumpToDebug()  // HV2 registry info --> Debug window
        {
            DebugForm.Clear();
            HV2RegInfo regInfo = new HV2RegInfo();

            Color textColor = Color.DarkCyan;
            DebugForm.rtf.WriteLine("RegistryView: " + regInfo.RegistryView.ToString(), textColor);
            DebugForm.rtf.WriteLine(@"[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0]");
            DebugForm.rtf.WriteLine("AppRoot: " + regInfo.AppRoot, textColor);
            DebugForm.rtf.WriteLine("DefaultHelpViewerCLSID = " + regInfo.DefaultHelpViewerCLSID, textColor);
            DebugForm.rtf.WriteLine(@"[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs]");
            DebugForm.rtf.WriteLine("ContentStore = " + regInfo.ContentStore, textColor);
            DebugForm.rtf.WriteLine("");

            List<CatalogItem> catalogList = CatalogList.GetCatalogs();
            catalogList.ForEach(delegate(CatalogItem catalog)
            {
                DebugForm.rtf.WriteLine(@"[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs\" + catalog.CatalogID + "]");
                DebugForm.rtf.WriteLine("   LocationPath = " + catalog.LocationPath, textColor);
                catalog.Locales.ForEach(delegate(CatalogLocaleItem catlocale)
                {
                    DebugForm.rtf.WriteLine(@"   [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs\" + catalog.CatalogID + "\\" + catlocale.Locale + "]");
                    DebugForm.rtf.WriteLine("   CatalogName = " + catlocale.CatalogName, textColor);
                    DebugForm.rtf.WriteLine("   SeedFilePath = " + catlocale.SeedFilePath, textColor);
                });
            });
        }

        private void FillCatalogListBox(int iSelect)   //fill with _catalogList items (flattened)
        {
            catListBox.DataSource = null;
            List<string> items = new List<string>();

            //Fill list box from flat list
            _flatCalaogList = FlatCatalogList.MakeList(_catalogList);  //Take the catalog list list can flatten it out
            _flatCalaogList.ForEach(delegate(FlatCatalogListItem catalog)
            {
                //artbitary display
                if (String.IsNullOrEmpty(catalog.Locale))
                    items.Add(catalog.CatalogID);   
                else
                    items.Add(catalog.CatalogID + @"\" + catalog.Locale);
            });

            catListBox.DataSource = items;    //fills listbox with items

            if (iSelect >= 0 && iSelect < catListBox.Items.Count)
                catListBox.SelectedIndex = iSelect;

            EnableDisable();
        }

        private void EnableDisable()
        {
            catListBox.Enabled = !Globals.catalog.IsOpen;
            OpenCatalogBtn.Enabled = catListBox.SelectedIndex >= 0 && !Globals.catalog.IsOpen;
            AddCatalogFolderBtn.Enabled = !Globals.catalog.IsOpen;
            AddMshxFileBtn.Enabled = !Globals.catalog.IsOpen;
            CloseCatalogBtn.Enabled = Globals.catalog.IsOpen;
            ContentPathBtn.Enabled = Globals.catalog.IsOpen;
        }

        public void DoOpenCatalog(String path, String locale)
        {
            DoCloseCatalog();  //close anything currently open
            EnableDisable();

            // Path is a standalone .mshx file?
            if (File.Exists(path))
            {
                try
                {
                    Globals.catalog.OpenMshx(path);
                }
                catch
                {
                    MessageBox.Show("Exception: OpenMshxCatalog() failed or timed out: " + path);
                }
            }
            // Path is a catalog directory
            else if (Directory.Exists(path))
            {
                String[] aPrioritizedLocales = { locale };            //eg. "en-US"
                try
                {
                    Globals.catalog.Open(path, aPrioritizedLocales);
                    if (Globals.catalog.IsOpen)
                        Globals.SetWindowTitle(path);
                    else 
                        MessageBox.Show("Failed to open catalog. IsOpen returned false!");
                }
                catch
                {
                    MessageBox.Show("Exception: Open() failed or timed out: " +path);
                }
            }
            else
            {
                MessageBox.Show("Path not found: " + path);
                return;
            }

            
            EnableDisable();
        }

        
        public void DoOpenSelectedCatalog()
        {
            int iCat = catListBox.SelectedIndex;
            if (iCat < 0)
                return;

            DoOpenCatalog(_flatCalaogList[iCat].LocationPath, _flatCalaogList[iCat].Locale);

            if (Globals.catalog.IsOpen)
            {
                MsxhelpProtocol.Catalog = Globals.catalog;  // pluggle protocol needs to know where to get topics and assets from
                Globals.selectedCatalog = new FlatCatalogListItem(_flatCalaogList[iCat]);
            }
            else
            {
                MsxhelpProtocol.Catalog = null;
                Globals.selectedCatalog = null;
            }
        }

        private void OpenCatalogBtn_Click(object sender, EventArgs e)
        {
            DoOpenSelectedCatalog();
        }

        private void GetCatalogReadWriteLockBtn_Click(object sender, EventArgs e)
        {
            int iCat = catListBox.SelectedIndex;
            if (iCat >= 0)
            {
                ICatalogReadWriteLock RWLock= Globals.catalog.GetReadWriteLock(_flatCalaogList[iCat].LocationPath);
                MessageBox.Show("Catalog: " + _flatCalaogList[iCat].LocationPath + "\n\n"
                               + "ICatalogReadWriteLock.IsCurrentlyNonBlockingWriteLocked = " + RWLock.IsCurrentlyNonBlockingWriteLocked.ToString() + "\n"
                               + "ICatalogReadWriteLock.IsCurrentlyWriteLocked = " + RWLock.IsCurrentlyWriteLocked.ToString() + "\n"
                               + RWLock.GetType().ToString() + "\n\n"
                               + "Note: Call to GetReadWriteLock() returns ICatalogReadWriteLock that can be used to lock\\unlock a catalog for reads & write etc.",
                               "Catalog.GetReadWriteLock()");
            }
        }

        private void CloseCatalogBtn_Click(object sender, EventArgs e)
        {
            DoCloseCatalog();
            EnableDisable();
        }

        private static void ClearPageData()
        {
            //Clear page data
            if (Page_Search.self != null)
                Page_Search.self.ClearAll();
            if (Page_TOC.self != null)
                Page_TOC.self.ClearAll();
            if (Page_Keywords.self != null)
                Page_Keywords.self.ClearAll();
            if (Page_Topics.self != null)
                Page_Topics.self.ClearAll();
            if (Page_Store.self != null)
                Page_Store.self.ClearAll();
        }

        public static void DoCloseCatalog()
        {
            if (Globals.catalog.IsOpen)
                Globals.catalog.Close();
            Globals.selectedCatalog = null;
            Globals.SetWindowTitle("");
            ClearPageData();
        }

        private void ShowDetails()
        {
            richTextBoxDetails.Clear();
            int iCat = catListBox.SelectedIndex;
            if (iCat >= 0)
            {
                FlatCatalogListItem catItem = _flatCalaogList[iCat];
                RichTextBoxFuncs rtbf = new RichTextBoxFuncs(richTextBoxDetails);

                rtbf.WriteLine("FromRegistry:", catItem.FromRegistry.ToString());
                rtbf.WriteLine("CatalogID:", catItem.CatalogID);

                if (Directory.Exists(catItem.LocationPath))
                    rtbf.WriteLine("LocationPath:", "file:/" + catItem.LocationPath);
                else
                    rtbf.WriteLine("LocationPath:", catItem.LocationPath);

                String dir1 = Path.Combine(catItem.LocationPath, "ContentStore\\");
                String dir2 = Path.Combine(catItem.LocationPath, "IndexStore\\");
                if (Directory.Exists(dir1))
                    rtbf.WriteLine("ContentStore:", "file:/" + dir1);
                if (Directory.Exists(dir2))
                    rtbf.WriteLine("IndexStore:", "file:/" + dir2);

                rtbf.WriteLine("");
                rtbf.WriteLine("Locale:", catItem.Locale);
                rtbf.WriteLine("CatalogName:", catItem.CatalogName);
                rtbf.WriteLine("SeedFilePath:", catItem.SeedFilePath);
            }
        }

        private void catListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ShowDetails();
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_CATALOGS);
        }

        private void ContentPathBtn_Click(object sender, EventArgs e)
        {
             try
             {
                 MessageBox.Show("Content Path of Catalog: \n" + Globals.catalog.ContentPath, "Catalog.ContentPath");
             }
             catch
             {
                 MessageBox.Show("Exception calling HelpCatalog.ContentPath");
             }
        }

 

        private void IsLangSupportAvailableBtn_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            try
            {
                //Lets try all MSDN language Locales

                MSLocales mslocales = new MSLocales();
                String results = "";
                for (int i = 0; i < mslocales.Locales.Count; i++)
                {
                    var item = mslocales.Locales.ElementAt(i);
                    String locale = item.Key;
                    String desc = item.Value;
                    var iRet = Globals.catalog.IsLanguageSupportAvailable(locale);
                    results = results + "Available(" + iRet.ToString("X") + "): " + locale + ". " + desc + "\n";
                }

                results = results + "\n 0=OK";
                results = results + "\n 0x3e8=System.Globalization.CultureInfo() call failed";
                results = results + "\n 0x3e9=ISearchLanguageSupport not available (tquery.dll, SearchAPI.h)";
                results = results + "\n 0x3ea=IWordBreaker not available (infotech.h)";
                results = results + "\n 0x3eb=IStemmer not available (infotech.h)";

                MessageBox.Show("IsLanguageSupportAvailable: \n" + results, "Catalog.IsLanguageSupportAvailable()");
            }
            catch
            {
                MessageBox.Show("Exception calling HelpCatalog.IsLanguageSupportAvailable");
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }
        }

        private void ShowDebugLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            DebugForm.self.Show();
        }

        private void richTextBoxDetails_LinkClicked(object sender, LinkClickedEventArgs e)
        {
            Process p = new Process();
            p = Process.Start(e.LinkText, "");
        }

        private void AddCatalogFolderBtn_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
            if (_flatCalaogList.Count > 0)
                folderBrowserDialog1.SelectedPath = ((FlatCatalogListItem)_flatCalaogList[0]).LocationPath;
            folderBrowserDialog1.Description = "Add a catalog folder";

            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                String folder = folderBrowserDialog1.SelectedPath;

                //Add catalog folder to list
                int origCount = _catalogList.Count;
                int i = CatalogList.WildAddCollection(folder, _catalogList, "");

                if (i < 0)
                    MessageBox.Show("Directory does not contain a \"ContentStore\" folder.\n" + folder);
                else
                    FillCatalogListBox(i);
            }
        }

        private void AddMshxFileBtn_Click(object sender, EventArgs e)
        {
            DoCloseCatalog();  //close anything currently open
            EnableDisable();

            // Displays an OpenFileDialog for *.mshx
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.Filter = "MSHX Help Filess|*.mshx";
            openFileDialog1.Title = "Select an MSHX file";
            openFileDialog1.CheckFileExists = true;
            openFileDialog1.CheckPathExists = true;
            openFileDialog1.DefaultExt = ".mshx";
            //openFileDialog1.InitialDirectory
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                String filePath = openFileDialog1.FileName;

                //Add catalog file to list
                int origCount = _catalogList.Count;
                int i = CatalogList.WildAddCollection(filePath, _catalogList, "");
                if (i >= 0)
                    FillCatalogListBox(i);
            }
        }



        /* Reflector code
         
        public int IsLanguageSupportAvailable(string lang)
        {
            ISearchLanguageSupport support = null;
            IWordBreaker ppWordBreaker = null;
            IStemmer ppStemmer = null;
            CultureInfo info = null;
            try
            {
                info = new CultureInfo(lang);
            }
            catch (ArgumentException)
            {
                return 0x3e8;
            }
            try
            {
                Guid clsid = new Guid("{6A68CC80-4337-4dbc-BD27-FBFB1053820B}");
                support = (ISearchLanguageSupport) Activator.CreateInstance(Type.GetTypeFromCLSID(clsid));
            }
            catch (COMException)
            {
                return 0x3e9;
            }
            try
            {
                Guid riid = new Guid("{D53552C8-77E3-101A-B552-08002B33B0E6}");
                support.LoadWordBreaker(info.LCID, ref riid, out ppWordBreaker);
            }
            catch (COMException)
            {
                return 0x3ea;
            }
            try
            {
                Guid guid3 = new Guid("{efbaf140-7f42-11ce-be57-00aa0051fe20}");
                support.LoadStemmer(info.LCID, ref guid3, out ppStemmer);
            }
            catch (COMException)
            {
                return 0x3eb;
            }
            return 0;
        }          
          
          
         */




    }
}
