using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Helpware.Misc;
using HVProject.HV2Lib.Common;
using System.Diagnostics;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
using System.IO;
using HV2ApiExplore;
using HV2ApiExplore.Forms;
using System.Reflection;
using HVProject.HV2Lib.LocalStore;
using HVProject.HV2Lib.View;
using HVProject.HV2Lib.PluggableProtocol;

// Note on Deployment: http://msdn.microsoft.com/en-us/library/ms973802.aspx

namespace HV2ApiExplore.Forms
{
    public partial class Form_H2Main : Form
    {
        private List<string> _navItems = new List<string>();  //Content of Catalog ListBox
        private List<UserControl> _pages = new List<UserControl>();
        private MsxhelpProtocol msxhelpProtocol;

        public Form_H2Main()
        {
            InitializeComponent();

            DebugForm.Create();
            Globals.Initialize(this);

            //Create API Test _pages in order

            _pages.Add(Page_Notes.Create());
            _navItems.Add("Introduction");

            _pages.Add(Page_Catalogs.Create());
            _navItems.Add("Catalogs");

            _pages.Add(Page_TOC.Create());
            _navItems.Add("TOC");

            _pages.Add(Page_Keywords.Create());
            _navItems.Add("Keywords");

            _pages.Add(Page_Search.Create());
            _navItems.Add("Search");

            _pages.Add(Page_Topics.Create());
            _navItems.Add("Topics");

            navListBox.DataSource = _navItems;

            //Rest of initialization

            InitOtherPageTabs();
        }


        Page_EventLog eventLogPage;

        private void InitOtherPageTabs()
        {
            // Associate this application with the ms-xhelp:/// protocol
            msxhelpProtocol = new MsxhelpProtocol();

            // About page
            InitAboutPageTab();

            //Store Page 
            Page_Store storePage = new Page_Store();
            storePage.Parent = tabPagContentStore;
            storePage.Dock = DockStyle.Fill;

            //Create MSHX Page
            Page_CreateMSHX mshxPage = new Page_CreateMSHX();
            mshxPage.Parent = tabPageMshx;
            mshxPage.Dock = DockStyle.Fill;
            
            //Event Log Page
            eventLogPage = new Page_EventLog(tabPageEventLog);
            eventLogPage.Parent = tabPageEventLog;
            eventLogPage.Dock = DockStyle.Fill;

            //Other
            new SuperTip(imageRenderWithVS, "Render with VS", 
                "Topics extracted from a catalog's .mshc help file are not yet rendered for viewing in the browser.",
                "EG. Links need to be expanded to use the ms-xhelp:/// protocol and point to the correct location in the catalog",
                "By default we provide a pluggle protocol and code that renders the HTML.",
                "Check this box to render using the VS rendering code in Microsoft.VisualStudio.Help.dll");
        }

        private String[] GetFileInfo(String fileName)
        {
            String path = Path.Combine(HV2Reg.AppRoot, fileName);
            if (File.Exists(path))
            {
                FileVersionInfo fileInfo = FileVersionInfo.GetVersionInfo(path);
                return new String[] { path, "    Description: " + fileInfo.FileDescription, "    Version: " + fileInfo.FileVersion };
            }
            else
            {
                return new String[] {"File not found: " + path};
            }
        }

        private void InitAboutPageTab()  
        {
            infoRichTextBox.Clear();
            RichTextBoxFuncs rtf = new RichTextBoxFuncs(infoRichTextBox);
            Assembly thisAssembly = System.Reflection.Assembly.GetExecutingAssembly();
            int _indent = 20;
            Color infoColor = Color.DimGray;
            Color titleColor = Color.SaddleBrown;

            rtf.WriteLine("Free example code by Helpware");
            rtf.WriteLine("http://helpviewer2.codeplex.com/");
            rtf.WriteLine("http://hv2.helpmvp.com");
            rtf.WriteLine("");

            rtf.WriteLine("This Assembly:", titleColor);
            String version = thisAssembly.GetName().Version.ToString();
            rtf.WriteLine("Version: " + version, infoColor, _indent);
            toolStripLabelVersion.Text = "Version: " + version;
            rtf.WriteLine(thisAssembly.FullName, infoColor, _indent);
            rtf.WriteLine(thisAssembly.Location, infoColor, _indent);

            rtf.WriteLine("");
            rtf.WriteLine("References:", titleColor);
            AssemblyName[] asmNames = thisAssembly.GetReferencedAssemblies();
            foreach (AssemblyName asmName in asmNames)
            {
                rtf.WriteLine(asmName.FullName, infoColor, _indent);
            }

            rtf.WriteLine("");
            rtf.WriteLine("ImageRuntimeVersion: " + thisAssembly.ImageRuntimeVersion, infoColor);

            rtf.WriteLine("");
            rtf.WriteLine("Help Runtime:", titleColor);
            rtf.WriteLine(GetFileInfo("HlpCtntMgr.exe"), infoColor, _indent);
            rtf.WriteLine(GetFileInfo("HlpViewer.exe"), infoColor, _indent);
            rtf.WriteLine(GetFileInfo("Microsoft.mshtml.dll"), infoColor, _indent);
            rtf.WriteLine(GetFileInfo("Microsoft.VisualStudio.Help.dll"), infoColor, _indent);
            rtf.WriteLine(GetFileInfo("Windows.Help.Runtime.dll"), infoColor, _indent);
        }

        private void infoRichTextBox_LinkClicked(object sender, LinkClickedEventArgs e)
        {
            try
            {
                Process.Start(e.LinkText);
            }
            catch (Exception x) 
            {
                MessageBox.Show(x.Message, this.Text);
            }
        }


        private void H2MainForm_Shown(object sender, EventArgs e)
        {
            //Restore form size/pos
            using (UserData userData = new UserData(Globals.DataRegKey)) 
                { userData.RestoreFormPos(this); }
        }

        private void H2MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!e.Cancel)
            {
                //Save form size/pos
                using (UserData userData = new UserData(Globals.DataRegKey))
                    { userData.SaveFormPos(this); }

                //Allow Debug form to save pos/size & close
                DebugForm.OkToClose = true;
                DebugForm.self.Close();

                eventLogPage.Cleanup();  //Close eventLog
                Page_CreateMSHX.self.helpIndexer.Cleanup();
            }
        }


        private void H2MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            //close any open catalogs
            Page_Catalogs.DoCloseCatalog();   
        }

        private void ShowPage(int idx)
        {
            //Hide other _pages
            for (int i = 0; i < _pages.Count; i++)
            {
                if (i != idx && _pages[i].Visible)
                    _pages[i].Hide();
            }

            //Show page
            if (idx < _pages.Count)
            {
                _pages[idx].Parent = splitContainer1.Panel2;
                _pages[idx].Dock = DockStyle.Fill;
                _pages[idx].Show();
            }
        }

        private void navListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            //Load selected page
            int iSelected = navListBox.SelectedIndex;
            if (iSelected >= 0)
                ShowPage(iSelected);
        }

        // == Main menu ==

        private void mnuOnlineHelp_Click(object sender, EventArgs e)
        {
            Globals.ShowAppHelp(-1);
        }

        private void mnuDebugWindow_Click(object sender, EventArgs e)
        {
            DebugForm.self.Show();
        }

        private void H2MainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Control && e.KeyCode == Keys.D)
                DebugForm.self.Show();
            if (!e.Control && e.KeyCode == Keys.F1)
                Globals.ShowAppHelp(-1);
        }


        // ToolBar - Launch Viewer Elevated

        List<FlatCatalogListItem> _catList = null;  //List of registered VS 11 catlogs

        private void RunVSHlpViewerElevatedBtn_DropDownOpening(object sender, EventArgs e)
        {
            RunVSHlpViewerElevatedBtn.DropDownItems.Clear();
            _catList = FlatCatalogList.GetAll();
            for (int i = 0; i < _catList.Count; i++)
            {
                ToolStripItem item = RunVSHlpViewerElevatedBtn.DropDownItems.Add(_catList[i].CatalogName + " | " + _catList[i].Locale);
                item.Tag = i;
            }
        }

        private void RunVSHlpViewerElevatedBtn_DropDownItemClicked(object sender, ToolStripItemClickedEventArgs e)  //fill catalog drop down list
        {
            int i = (int)e.ClickedItem.Tag;
            _catList = FlatCatalogList.GetAll();
            if (i >= 0 && i < _catList.Count)
            {
                HlpViewer.StartReturn sRet = HlpViewer.RunAsAdmin("/catalogName", _catList[i].CatalogID, "/locale", _catList[i].Locale);
                if (sRet != HlpViewer.StartReturn.ok)
                    MessageBox.Show("Error running HlpViewer.exe: " + sRet.ToString() + "\n" + HlpViewer.path);
            }
        }

        private void RunVSHlpViewerElevatedBtn_ButtonClick(object sender, EventArgs e)
        {
            _catList = FlatCatalogList.GetAll();
            if (_catList.Count > 0)
            {
                HlpViewer.StartReturn sRet = HlpViewer.RunAsAdmin("/catalogName", _catList[0].CatalogID, "/locale", _catList[0].Locale);
                if (sRet != HlpViewer.StartReturn.ok)
                    MessageBox.Show("Error running HlpViewer.exe: " + sRet.ToString() + "\n" + HlpViewer.path);
            }
            else
                MessageBox.Show("No VS 11 registered catalogs found.");
        }

        // Render checkbox

        private void checkBoxRenderWithVS_CheckedChanged(object sender, EventArgs e)
        {
            MsxhelpProtocol.RenderUsingVS = checkBoxRenderWithVS.Checked;    //Tell Render about the change
        }



    }
}
