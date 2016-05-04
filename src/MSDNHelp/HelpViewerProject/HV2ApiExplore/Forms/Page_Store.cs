using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HVProject.HV2Lib.LocalStore;
using HVProject.HV2Lib.Common;
using HVProject.HV2Lib.View;
using System.IO;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
using Helpware.Misc;
using System.Runtime.InteropServices.ComTypes;
using System.Reflection;
using System.Drawing.Imaging;

namespace HV2ApiExplore.Forms
{
    public partial class Page_Store : UserControl
    {
        public static Page_Store self = null;

        internal static Page_Store Create()
        {
            self = new Page_Store();
            return self;
        }

        private String _monitorCatalogPath = "";
        private Timer monitorTimer = new Timer();
        private readonly SuperTip monitorLocksTip = null;

        private CatalogParser _catalogXml = null;
        private Dictionary<String, PackageMetaData> _catalogPackageMetaData = null;
        private readonly String defaultMonitorTip;

        public Page_Store()
        {
            InitializeComponent();
            self = this;

        
            // Init Catalog Cbo with catalog paths from the registry
            List<CatalogItem> _catalogList = CatalogList.GetCatalogs();   //Get list from the HV2 registry
            catalogPathCbo.Items.Clear();
            foreach (CatalogItem catalog in _catalogList)
                catalogPathCbo.Items.Add(catalog.LocationPath);
            if (catalogPathCbo.Items.Count > 0)  //Select something
                catalogPathCbo.SelectedIndex = 0;

            //Monitoring Catalog Locks
            monitorTimer.Interval = 1000;  //Poll for locks ever 1 sec
            monitorTimer.Tick += new EventHandler(monitorTimer_Tick);

            //Init SuperTip and tips
            defaultMonitorTip = "Here we display the catalog write lock status on a timer.\nTry performing a catalog update (Help Manager) and watch the status change.\nUseful to apps like this which access catalog files directly.";
            monitorLocksTip = new SuperTip(CatalogLocksTip, "Monitor Catalog Locks", defaultMonitorTip);
            new SuperTip(LoadButtonTip, "Load", 
                "Given a valid 'managed' catalog location, the Load button code rips",
                "the various catalog XML files for all catalog configuration data.",
                "* CatalogType.xml - Required to enable User Managed content.",
                @"* ContentStore\installedBooks.xml - Lists all catalog books and packages.",
                @"* ContentStore\<locale>\<packageName>.metadata - Package download related info.");
            new SuperTip(warningTip, "Bypassing the API Warning",
                "Here we are sneaking around the Help Runtime API and accessing",
                "help catalog files directly. While this is all very interesting",
                "as we learn how Help Viewer 2.0 works, the Help API normally",
                "serves most of our needs, and we don't want to block or upset",
                "the normal operation of the help system.");
            CopyBtn.ToolTipText = "Copy node & sub-node text\n(Ctrl+C)";
            ExplorerBtn.ToolTipText = "Show the associated XML file in Explorer\n(Ctrl+E)";

            EnableDisable();
        }

        public void ClearAll() 
        {
            DoMonitorLocks = false;
            EnableDisable();
        }

        private String GetCatalogContentStoreLocation()  // from combo box text
        {
            if (Directory.Exists(Path.Combine(catalogPathCbo.Text.Trim(), "ContentStore")))
            {
                return catalogPathCbo.Text.Trim();
            }
            return "";
        }

        private void LoadButton_Click(object sender, EventArgs e)
        {
            treeView1.Nodes.Clear();
            nodeCatalogTypeXml = null;
            EnableDisable();

            String path = GetCatalogContentStoreLocation();
            if (path == "")
            {
                MessageBox.Show("Catalog path is invalid or does not contain a \"ContentStore\" folder.");
                return;
            }

            // First check for locks (ie. Catalog is in flux being rebuilt)

            ICatalogReadWriteLock RWLock = Globals.catalog.GetReadWriteLock(path);
            if (RWLock.IsCurrentlyNonBlockingWriteLocked || RWLock.IsCurrentlyWriteLocked)
            {
                MessageBox.Show("Catalog is busy. Try again soon.\nCatalog is currently write locked.");
                return;
            }

            // Load Catalog XML -- Locking as we go so others don't try a modify it while we read

            RWLock.StartNonBlockingWriteOperation(10000);   //10 second time out (usually reads all XML under 1 second on my laptop VM)
            Cursor.Current = Cursors.WaitCursor;
            try
            {
                _catalogXml = new CatalogParser(path);
                _catalogPackageMetaData = _catalogXml.GetPackageMetaData();
                BuildToc();
                EnableDisable();

                if (_catalogXml.InstalledBooks == null)
                {
                    MessageBox.Show("No data found! Note that only VS 11 catalog folders contain HV2 XML data.");
                }
            }
            finally
            {
                Cursor.Current = Cursors.Default;
                RWLock.ExitNonBlockingWriteOperation();
            }
        }


        TreeNode nodeCatalogTypeXml;

        private void BuildToc()
        {
            InstalledBooks xml = _catalogXml.InstalledBooks;
            if (xml == null)
                return;

            treeView1.BeginUpdate();
            try
            {
                TreeNode tn;
                TreeNode tn2;
                TreeNode tn3;
                TreeNode tn4;

                treeView1.Nodes.Add("Catalog Location: " + _catalogXml.CatalogLocation);
                treeView1.Nodes.Add("Catalog Version: " + xml.version);
                if (_catalogXml.CatalogTypeXmlFound)
                    nodeCatalogTypeXml = treeView1.Nodes.Add("UserManaged=" + _catalogXml.UserManaged);
                else
                    nodeCatalogTypeXml = treeView1.Nodes.Add("UserManaged=<CatalogType.xml not found>");
                nodeCatalogTypeXml.ToolTipText = "CatalogType.xml";

                Color infoColor = Color.DimGray;
                Color packageColor = Color.SaddleBrown;

                foreach (LocaleListItem localeListItem in xml.localeList)
                {
                    tn = treeView1.Nodes.Add(localeListItem.locale);
                    foreach (BookListItem bookListItem in localeListItem.bookList)
                    {
                        tn2 = tn.Nodes.Add(bookListItem.displayname);
                        tn2.ToolTipText = "Book display name";
                        tn2.Nodes.Add("kind: " + bookListItem.kind);
                        tn2.Nodes.Add("path: " + bookListItem.path);
                        tn2.Nodes.Add("productgroupname: " + bookListItem.productgroupname);
                        tn2.Nodes.Add("productname: " + bookListItem.productname);
                        tn2.Nodes.Add("ref: " + bookListItem.ref_);
                        tn2.Nodes.Add("source: " + bookListItem.source);
                        tn2.Nodes.Add("vendorname: " + bookListItem.vendorname);

                        tn3 = tn2.Nodes.Add("packages");
                        foreach (String package in bookListItem.packageList)
                        {
                            tn4 = tn3.Nodes.Add(package);
                            tn4.ForeColor = packageColor;
                            tn4.ToolTipText = "Package name. Sub-items are extracted from\n" + package + ".metadata";

                            //Show meta data (extracted from <package>.metadata file)
                            if (!String.IsNullOrEmpty(package) && _catalogPackageMetaData.Count > 0)
                            {
                                PackageMetaData metadata = _catalogPackageMetaData[package];
                                if (metadata != null)
                                {
                                    tn4.Nodes.Add("Date: " + metadata.Date).ForeColor = infoColor;
                                    tn4.Nodes.Add("ETag: " + metadata.ETag).ForeColor = infoColor;
                                    tn4.Nodes.Add("PackageType: " + metadata.PackageType).ForeColor = infoColor;
                                    tn4.Nodes.Add("Size: " + metadata.Size).ForeColor = infoColor;
                                    tn4.Nodes.Add("Url: " + metadata.Url).ForeColor = infoColor;
                                    tn4.Nodes.Add("VendorName: " + metadata.VendorName).ForeColor = infoColor;
                                }
                            }
                        }

                    }
                }


                tn = treeView1.Nodes.Add("Branding Packages");
                foreach (BrandingPackage brandingPackage in xml.brandingPackages)
                {
                    tn2 = tn.Nodes.Add(brandingPackage.package);
                    tn2.Nodes.Add("vendorName:" +brandingPackage.vendorName);
                    tn2.Nodes.Add("locale:" + brandingPackage.locale);
                }

            }
            finally
            {
                treeView1.EndUpdate();

                //Select something
                if (treeView1.Nodes.Count > 0)
                {
                    treeView1.SelectedNode = treeView1.Nodes[0];
                    treeView1.Select();
                }
            }

            
        }

        private void monitorCbx_Click(object sender, EventArgs e)
        {
            if (monitorCbx.Checked)    //currently On -- OK to turn off at any time
            {
                DoMonitorLocks = false;
                monitorLocksTip.toolTip.SetToolTip(CatalogLocksTip, defaultMonitorTip);
            }
            else
            {
                String path = GetCatalogContentStoreLocation();
                if (path == "")
                {
                    MessageBox.Show("Invalid catalog path! Does not contain a \"ContentStore\" folder.");
                    return;
                }
                _monitorCatalogPath = path;
                DoMonitorLocks = true;
                monitorLocksTip.toolTip.SetToolTip(CatalogLocksTip, "Monitoring: " + _monitorCatalogPath);
            }
        }

        private Boolean DoMonitorLocks
        {
            get
            {
                return monitorCbx.Checked;
            }
            set
            {
                if (!value)
                    monitorLabel.Text = "--";
                else
                    DisplayLockStatus();      
                monitorCbx.Checked = value;
                monitorTimer.Enabled = value;
            }
        }



        // == Monitor Lock States ==

        private void DisplayLockStatus()
        {
            try
            {
                ICatalogReadWriteLock RWLock = Globals.catalog.GetReadWriteLock(_monitorCatalogPath);
                monitorLabel.Text = "NonBlockingWriteLocked: " + RWLock.IsCurrentlyNonBlockingWriteLocked.ToString()
                    + ", WriteLocked: " + RWLock.IsCurrentlyWriteLocked.ToString();
            }
            catch
            {
            }
        }

        private void monitorTimer_Tick(object sender, EventArgs e)
        {
            DisplayLockStatus();
        }

        private void EnableDisable()
        {
            Boolean nodeSelected = treeView1.SelectedNode != null;
            mnuCopy.Enabled = nodeSelected;
            CopyBtn.Enabled = nodeSelected;
            mnuExplorer.Enabled = nodeSelected;
            ExplorerBtn.Enabled = nodeSelected;
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            EnableDisable();
        }

        // ---------------------------------------------------------------------------------
        // Menu/Btns 
        //   - Copy Node text and sub node text
        //   - Show Explorer and select file
        //   - Unzip selected package
        // ---------------------------------------------------------------------------------

        private void treeView1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.C && e.Modifiers == Keys.Control)
            {
                HandleCtrlC_Copy();
            }
            if (e.KeyCode == Keys.E && e.Modifiers == Keys.Control)
            {
                HandleCtrlE_Explorer();
            }
        }

        const String sTab = "   ";

        private String GetChildText(TreeNode node, String text, String tab)
        {
            String sRet = text;
            foreach (TreeNode n in node.Nodes)
            {
                sRet = sRet + Environment.NewLine + tab + n.Text;
                if (n.Nodes.Count > 0)
                    sRet = sRet + GetChildText(n, text, tab + sTab);
            }
            return sRet;
        }

        private void HandleCtrlC_Copy()
        {
            if (treeView1.SelectedNode == null)
                return;
            TreeNode tn = treeView1.SelectedNode;
            String text = tn.Text;
            if (tn.Nodes.Count > 0)
                text = GetChildText(tn, text, sTab);

            Clipboard.SetText(text); 
        }

        private String LocaleFromNode(TreeNode node)
        {
            //Find locale (node text at level 0)
            TreeNode rootNode = node;
            String locale = "en-US";
            while (rootNode != null)
            {
                if (rootNode.Parent == null)
                    locale = rootNode.Text;
                rootNode = rootNode.Parent;
            }
            return locale;
        }
        

        private void HandleCtrlE_Explorer()
        {
            if (_catalogXml == null || _catalogXml.InstalledBooks == null)
                return;
            if (treeView1.SelectedNode == null)
                return;
            TreeNode tn = treeView1.SelectedNode;
            String catalogLocalePath = Path.Combine(_catalogXml.CatalogLocation, "ContentStore", LocaleFromNode(tn));

            String fn = "";
            if (tn == nodeCatalogTypeXml && _catalogXml.CatalogTypeXmlFound)
                fn = Path.Combine(_catalogXml.CatalogLocation, "CatalogType.xml");
            else if (tn.Level == 3 && catalogLocalePath != "")  //package name - Show Package.metadata file
                fn = FileUtility.GetFileName(Path.Combine(catalogLocalePath, tn.Text + ".metadata")); 
            else
                fn = FileUtility.GetFileName(Path.Combine(_catalogXml.CatalogLocation, "ContentStore\\installedBooks.xml"));  //GetFileName gets latest version eg. xxx.4.xml

            if (File.Exists(fn))
                WinExec.ShowExplorer(fn);  //Show file in explorer and select it
            else 
                MessageBox.Show("File not found: \n" +fn);
        }

        private void mnuCopy_Click(object sender, EventArgs e)
        {
            HandleCtrlC_Copy();
        }

        private void CopyBtn_Click(object sender, EventArgs e)
        {
            HandleCtrlC_Copy();
        }

        private void ExplorerBtn_Click(object sender, EventArgs e)
        {
            HandleCtrlE_Explorer();
        }

        private void mnuExplorer_Click(object sender, EventArgs e)
        {
            HandleCtrlE_Explorer();
        }

        // === Help ===

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_STORE);
        }



    }
}
