using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
using HVProject.HV2Lib.Common;
using HVProject.HV2Lib.View;
using HVProject.HV2Lib.PluggableProtocol;
using HVProject.HV2Lib.Render;
using System.Runtime.InteropServices.ComTypes;
using System.Diagnostics;
using Helpware.Misc;
using HV2Viewer.Code;

namespace HV2Viewer
{
    public partial class Form1 : Form
    {
        // Help Viewer 2 API calls
        private Catalog _catalog;
        private CatalogRead _catalogRead;
        private IKeywordCollection _helpKeywords = null;
        private String _CatalogLocale = null;

        private List<FlatCatalogListItem> _calalogList;
        private List<ToolStripMenuItem> _catalogMnu = new List<ToolStripMenuItem>();
        private Panel_Toc tocPanel= null;
        private readonly String formTitle = "";
        private MsxhelpProtocol msxhelpProtocol;
        private IndexCache indexCache = new IndexCache();
        private Boolean inSyncMethod = false;

		private String initialQuery = null;
		
        String UIDataStoreKey = @"software\fabrikam\hv2viewer\userdata\";

        public Form1(string[] args)
        {
            InitializeComponent();

            //Theme the listViews
            Native.SetWindowTheme(this.indexListView.Handle, "explorer");
            Native.SetWindowTheme(this.resultsListView.Handle, "explorer");
            Native.SetWindowTheme(this.searchListView.Handle, "explorer");
            
            formTitle = this.Text;

            //
            // Help API:  Access a Catalog and it's Data
            //
            _catalog = new Catalog();                   
            _catalogRead = new CatalogRead();

            //
            // TOC Tab
            //
            tocPanel = new Panel_Toc(tabPageTOC);  //parent the TOC panel
            tocPanel.Dock = DockStyle.Fill;
            tocPanel.AddSelectCallback(TocNodeSelectionEvent);

            //Fill Catalog List (menu) with registered VS 11 catalogs
            FillCatalogList();

            // Associate this application with the ms-xhelp:/// protocol
            msxhelpProtocol = new MsxhelpProtocol();
            msxhelpProtocol.AddRawCodeCallback(RawCodeChangeEvent);
		
			if (args.Length==1)
			{
				initialQuery = args[0];
			}
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            sourceSplitter.Panel2Collapsed = true;
            //Supertips
            new SuperTip(imageUnrendered, "Unrendered Source (F6)", "Displays the original topic source from the .mshc help file.", "To see the 'rendered' source right-click and 'View Source' the WebBrowser control.");

            //Load the first registered catalog we find
            LoadCatalog();

            //load persistence UI data
            using (UserData userData = new UserData(UIDataStoreKey))
            {
                userData.RestoreFormPos(this);
                splitContainer1.SplitterDistance = userData.ReadInt("NavSplitterPos", splitContainer1.SplitterDistance);
                sourceSplitter.SplitterDistance = userData.ReadInt("SourceSplitterPos", sourceSplitter.SplitterDistance);
                ShowUnrenderedSource(userData.ReadBool("ShowUnrendered", !sourceSplitter.Panel2Collapsed));
//                SetVSRenderMode(userData.ReadBool("VsRenderer", mnuVSRenderer.Checked));
                autoSyncOption.Checked = userData.ReadBool("AutoSync", autoSyncOption.Checked);
            }

            EnableDisable();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (_catalog.IsOpen)
                _catalog.Close();

            //save persistence UI data
            using (UserData userData = new UserData(UIDataStoreKey))
            {
                userData.SaveFormPos(this);
                userData.WriteInt("NavSplitterPos", splitContainer1.SplitterDistance);
                userData.WriteInt("SourceSplitterPos", sourceSplitter.SplitterDistance);
                userData.WriteBool("ShowUnrendered", !sourceSplitter.Panel2Collapsed);
//                userData.WriteBool("VsRenderer", mnuVSRenderer.Checked);
                userData.WriteBool("AutoSync", autoSyncOption.Checked);
            }

        }


        // === Registered catalog list with registered catalogs ===

        int _SelectedCatalog = -1;

        private void FillCatalogList()
        {
            _calalogList = FlatCatalogList.GetAll();
            int i = 0;
            foreach (FlatCatalogListItem catalog in _calalogList)
            {
                ToolStripMenuItem mnuItem = (ToolStripMenuItem)openCatalogDropDown.DropDownItems.Add(catalog.CatalogName + " | " + catalog.Locale, null, openCatalogDropDown_Click);
                mnuItem.AutoToolTip = true;
                mnuItem.ToolTipText = "LocalPath: " + catalog.LocationPath + Environment.NewLine
                    + "Catalog Alias: " + catalog.CatalogID + Environment.NewLine
                    + "Catalog Name: " + catalog.CatalogName + Environment.NewLine
                    + "Locale: " + catalog.Locale + Environment.NewLine
                    + "SeedFilePath: " + catalog.SeedFilePath + Environment.NewLine;
                mnuItem.Tag = i++;
                _catalogMnu.Add(mnuItem);
            }
            if (_calalogList.Count > 0)
                _SelectedCatalog = 0;
        }

        private void openCatalogDropDown_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem menuitem = (ToolStripMenuItem)sender;
            _SelectedCatalog = (int)menuitem.Tag;
            LoadCatalog();
        }

        private void CheckCatalogMnu(int iChecked)
        {
            for (int i = 0; i < _catalogMnu.Count; i++)
                _catalogMnu[i].Checked = (i == iChecked);
        }



        // --------------------------------------------------------------------------------------------------
        // Open/Close Catalog
        // --------------------------------------------------------------------------------------------------

        private void CloseCatalog()
        {
            try
            {
                if (_catalog.IsOpen)
                    _catalog.Close();
            }
            finally
            {
                this.Text = formTitle;
                CheckCatalogMnu(-1);
                webBrowser1.Navigate("about:blank");  //Clear browser
                richTextBoxCodeView.Text = "";
                indexListView.VirtualListSize = 0;    //Clear Index & cache
                indexCache.Clear();
                resultsListView.Items.Clear();
                searchListView.VirtualListSize = 0;   //Clear Search
                SearchInit();
                tocPanel.HelpCatalog = _catalog;      //Clear TOC
                MsxhelpProtocol.UserTopicText = "";
            }
        }

        private void LoadCatalog()
        {
            LoadCatalog(null, null);
        }

        private void LoadCatalog(String mshxFile)
        {
            LoadCatalog(mshxFile, null);
        }

        private void LoadCatalog(String catalogPath, String locale)
        {
            String displayName = "";
            _CatalogLocale = locale;

            try
            {
                CloseCatalog();   

                // == Open Catalog from Registered Catalog dropdown list ==

                if (String.IsNullOrEmpty(catalogPath))
                {
                    if (_calalogList.Count == 0)
                    {
                        MessageBox.Show("No registered catalogs found. Is help configured?");
                        return;
                    }
                    else
                    {
                        int i = _SelectedCatalog;
                        _catalog.Open(_calalogList[i].LocationPath, new String[] { _calalogList[i].Locale });
                        if (!String.IsNullOrEmpty(_calalogList[i].CatalogName))
                            displayName = _calalogList[i].CatalogName;
                        else
                            displayName = _calalogList[i].LocationPath + " | " + _calalogList[i].Locale;
                        CheckCatalogMnu(i);
                    }
                }

                // == Open a .mshx File

                else if (File.Exists(catalogPath)) // .mshx file
                {
                    _catalog.OpenMshx(catalogPath);
                    displayName = catalogPath;
                }

                // == Open catalog folder

                else if (Directory.Exists(catalogPath))
                {
                    _catalog.Open(catalogPath, new String[] { locale });
                    displayName = catalogPath + " | " + locale;
                }
                else
                    return;

                //Sucess?
                if (!_catalog.IsOpen)
                    MessageBox.Show("Unknown error opening _catalog \n" + _calalogList[0].LocationPath + " | " + _calalogList[0].Locale);

                else
                {
                    MsxhelpProtocol.Catalog = _catalog;

                    this.Text = formTitle + " - " + displayName;

                    //Fill TOC
                    tocPanel.HelpCatalog = _catalog;
                    tocPanel.SelectFirstNode();

                    // Index Tab
                    _helpKeywords = _catalogRead.GetKeywords(_catalog, true);        //true = caching to speed up loading
                    indexListView.VirtualListSize = _helpKeywords.Count;             //tell virtual list to fill itself 

					try
					{
						if (initialQuery != null)
						{
							DoNavigateByF1Keyword(initialQuery);
						}
					}
					catch
					{
					}
				}
            }
            catch
            {
                MessageBox.Show("Exception acessing catalog. Is catalog valid? \n" + catalogPath + " | " + locale);
            }
        }

        private void mnuOpenCatalogFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.DefaultExt = ".mshx";
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                LoadCatalog(openFileDialog1.FileName);
            }
        }

        private String GetFirstContentStoreLocale(String contentStorePath)
        {
            foreach (String path in Directory.EnumerateDirectories(contentStorePath))
            {
                if (path.Length > 6 && path[path.Length - 3] == '-' && path[path.Length - 6] == '\\')  //looks like emds in focale folder "...xxx\en-US"
                {
                    return path.Substring(path.Length - 5);   // return locale folder under "\contentStore\"
                }
            }
            return "";
        }


        String _lastPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);

        private void mnuOpenCatalogFolder_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
            folderBrowserDialog1.Description = @"Select the folderName folder of a catalog eg. <catalog>\contentStore\en-US";
            folderBrowserDialog1.SelectedPath = _lastPath;
            if (folderBrowserDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                String path = folderBrowserDialog1.SelectedPath;
                _lastPath = path;
                path.TrimEnd(new char[] { '\\'});
                String folderName = path.Substring(path.LastIndexOf('\\') + 1);
                if (folderName.Length == 5 && folderName[2] == '-')   // folderName = say "en-us" -- probably a valid <catalog>\ContentStore\<Locale> path
                {
                    String catalogPath = Directory.GetParent(path).Parent.FullName;
                    LoadCatalog(catalogPath, folderName);
                }
                else
                {
                    String contentPath = path + "\\ContentStore";
                    if (Directory.Exists(contentPath))
                    {
                        String locale = GetFirstContentStoreLocale(contentPath);
                        if (locale != "")
                            LoadCatalog(path, locale);
                        else
                            MessageBox.Show(@"Please select a valid folderName folder. eg. <catalog>\ContentStore\en-US\");
                    }
                    else
                        MessageBox.Show(@"Please select a valid folderName folder. eg. <catalog>\ContentStore\en-US\");
                }
            }
        }



        // --------------------------------------------------------------------------------------------------
        // TOC 
        // --------------------------------------------------------------------------------------------------

        public void TocNodeSelectionEvent(String helpTopicId)   //TOC tells us selection has changed
        {
            if (inSyncMethod)
                return;
            DoNavigateByID(helpTopicId);
        }



        // --------------------------------------------------------------------------------------------------
        // Index
        // --------------------------------------------------------------------------------------------------

        // Virtual Index List wants an linkNode

        private void indexListView_RetrieveVirtualItem(object sender, RetrieveVirtualItemEventArgs e)
        {
            int i = e.ItemIndex;
            ListViewItem li = new ListViewItem((i + 1).ToString());

            if (_helpKeywords == null || i >= _helpKeywords.Count)  //range check
            {
                li.SubItems.AddRange(new string[] { "?", "?" });
                e.Item = li;
                return;
            }

            //Note: We can improve speed and smoothness with some local caching

            int topicCount;
            Keyword keyword = indexCache.GetKW(i, out topicCount);
            if (keyword == null)
            {
                _helpKeywords.MoveTo(i);
                keyword = (Keyword)_helpKeywords.Current;
                topicCount = keyword.Topics.Count;
                indexCache.Add(i, keyword, topicCount);
            }

            String kwText = keyword.DisplayValue;
            if (keyword.IsSubkey)
                kwText = "    " + kwText;
            li.SubItems.AddRange(new string[] { kwText, topicCount.ToString() });
            e.Item = li;
        }

        List<Topic> _results = new List<Topic>();

        private void keywordsListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            resultsListView.Items.Clear();
            _results.Clear();

            if (indexListView.SelectedIndices.Count <= 0)
                return;
            int index = indexListView.SelectedIndices[0];

            _helpKeywords.MoveTo(index);
            IKeyword keyword = (IKeyword)_helpKeywords.Current;

            if (keyword.Topics.Count > 0)
            {
                ITopicCollection topics = keyword.Topics;
                topics.MoveTo(0);

                for (int i = 0; i < topics.Count; i++)
			    {
                    _results.Add((Topic)topics.Current);
                    topics.MoveNext();
                }

                //Load ListView with results
                for (int i = 0; i < topics.Count; i++)
			    {
                    ListViewItem li = new ListViewItem((i+1).ToString());
                    li.ToolTipText = MakeTopicToolTip(_results[i]); // just for dev info -- Hover over the first column to see this tip
                    li.SubItems.AddRange(new String[] { _results[i].Title, _results[i].DisplayVersion });
                    resultsListView.Items.Add(li);
                }

                //Select linkNode #1 -- and trigger display of topic in webBrowser1
                resultsListView.Items[0].Selected = true;
            }
        }

        private String ExpandStringList(String[] stringList)
        {
            String ss = "";
            foreach (String s in stringList)
                if (ss.Length == 0) ss = s; else ss = ss + " | " + s;
            return ss;
        }
        
        private String MakeTopicToolTip(Topic topic)  //Debug (only of interest to developers)
        {
            String s = "Topic info:"
                + "\rtopic.Catalog.ContentPath: " + topic.Catalog.ContentPath
                + "\rtopic.Category: " + ExpandStringList(topic.Category())
                + "\rtopic.ContentFilter: " + ExpandStringList(topic.ContentFilter())
                + "\rtopic.ContentType: " + ExpandStringList(topic.ContentType())
                + "\rtopic.Description: " + topic.Description
                + "\rtopic.DisplayVersion: " + topic.DisplayVersion
                + "\rtopic.Id: " + topic.Id
                + "\rtopic.Locale: " + topic.Locale
                + "\rtopic.Package: " + topic.Package
                + "\rtopic.ParentId: " + topic.ParentId
                + "\rtopic.TableOfContentsHasChildren: " + topic.TableOfContentsHasChildren.ToString()
                + "\rtopic.TableOfContentsPosition: " + topic.TableOfContentsPosition.ToString()
                + "\rtopic.Title: " + topic.Title
                + "\rtopic.TopicLocale: " + topic.TopicLocale
                + "\rtopic.TopicVersion: " + topic.TopicVersion
                + "\rtopic.Url: " + topic.Url
                + "\rtopic.Vendor: " + topic.Vendor;
            return s;

        }

        private void resultsListView_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            if (e.IsSelected)
            {
                DoNavigateByTopic(_results[e.ItemIndex]);
            }
        }


        // Editbox wordwheel

        private void indexTextBox_TextChanged(object sender, EventArgs e)  //type kwText, word wheel down to the matching kwText
        {
            int iSelected = -1;
            if (indexListView.SelectedIndices.Count > 0)
                iSelected = indexListView.SelectedIndices[0];

            String text = indexTextBox.Text.Trim();
            if (_helpKeywords != null && _helpKeywords.Count > 0)
            {
                int i = 0;
                if (!String.IsNullOrEmpty(text))
                    i = _helpKeywords.MoveToKeyword(text);
                if (i >= 0 && i <= _helpKeywords.Count && i != iSelected)
                {
                    //Select and scroll match into view
                    indexListView.BeginUpdate();           /// This keeps selection traffic to a min
                    try
                    {
                        indexListView.SelectedIndices.Clear();
                        indexListView.SelectedIndices.Add(i);
                        indexListView.Items[i].EnsureVisible();
                        indexListView.TopItem = indexListView.Items[i];
                    }
                    finally
                    {
                        indexListView.EndUpdate();
                    }
                }
            }

        }



        // --------------------------------------------------------------------------------------------------
        // Search
        // --------------------------------------------------------------------------------------------------

        String _sQuery = "";
        int _pageNumber = 1;
        int _pageSize = 50;
        int _totalAvailableHits = 0;
        int _startListViewNumbering = 1;
        Panel_SearchOperator searchOperatorsPanel = null;
        ITopicCollection _searchTopics;

        private void SearchInit()
        {
            //Search Operator panel
            searchOperatorsPanel = new Panel_SearchOperator(this, SearchCbo);

            statusLabel.Text = "--";
            UpdateStatus(false);
        }

        private void DoSearch()
        {
            if (!_catalog.IsOpen)
            {
                MessageBox.Show("Open a _catalog first!");
                return;
            }

            HelpFilter filter = null;  //no fancy adv filtering yet
            UpdateStatus(false);
            searchListView.VirtualListSize = 0;

            Cursor.Current = Cursors.WaitCursor;
            try
            {
                _searchTopics = _catalogRead.GetSearchResults(_catalog, _sQuery, filter, SearchOptions.None, _pageSize, _pageNumber, out _totalAvailableHits);
                searchListView.VirtualListSize = _searchTopics.Count;
                UpdateStatus(true);
            }
            catch
            {
                searchListView.VirtualListSize = 0;
                _pageNumber = 1;
                _totalAvailableHits = 0;
                MessageBox.Show("Exception calling ICatalogRead.GetSearchResults(..)");
                return;
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }

        }

        private void SearchBtnSearch()
        {
            _sQuery = SearchCbo.Text.Trim();
            if (String.IsNullOrEmpty(_sQuery))
                return;

            _pageSize = (Int32)pageSizeEdit.Value;
            _pageNumber = 1;             //When searching with the Search Button we always start from Page 1
            _startListViewNumbering = 1;
            _totalAvailableHits = 0;

            DoSearch();
        }

        private void FindBtn_Click(object sender, EventArgs e)
        {
            SearchBtnSearch();
        }

        private void SearchCbo_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
                SearchBtnSearch();
        }

        private void SearchCbo_DropDown(object sender, EventArgs e)
        {
            searchOperatorsPanel.SmartShow();
        }

        // == Pages ==

        #region *** Change pages << < > >>

        private int PageCount
        {
            get
            {
                int pages = _totalAvailableHits / _pageSize;
                if (pages * _pageSize < _totalAvailableHits)
                    pages++;   //last page is incomplete
                return pages;
            }
        }

        private void PageChange(int newPageNumber)
        {
            if (newPageNumber != _pageNumber && _sQuery != "")  //change && a query was run
            {
                _pageNumber = newPageNumber;
                _startListViewNumbering = (_pageNumber - 1) * _pageSize + 1;
                DoSearch();
            }
        }

        private void UpdateStatus(Boolean aShow)
        {
            if (aShow && _totalAvailableHits == 0)
                statusLabel.Text = " " + _totalAvailableHits.ToString("#,##0") + " total results";
            else if (aShow)
                statusLabel.Text = " Page: " + _pageNumber.ToString("#,##0") + " of " + PageCount.ToString("#,##0") + ";  " + _totalAvailableHits.ToString("#,##0") + " total";
            else
                statusLabel.Text = "";
            // Enable buttons
            int pgCount = PageCount;
            firstLink.Enabled = aShow && _pageNumber != 1;
            prevLink.Enabled = aShow && (_pageNumber > 1) && _pageNumber > 1;
            nextLink.Enabled = aShow && (_pageNumber < pgCount) && pgCount > 1;
            lastLink.Enabled = aShow && (_pageNumber != pgCount) && pgCount > 1;
        }

        private void firstLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            PageChange(1);
        }

        private void prevLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            int p = _pageNumber - 1;
            if (p < 1)
                p = 1;
            PageChange(p);
        }

        private void nextLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            int p = _pageNumber + 1;
            if (p > PageCount)
                p = PageCount;
            PageChange(p);
        }

        private void lastLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            PageChange(PageCount);
        }

        #endregion

        // Selected? Display the topic

        private void searchListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (searchListView.SelectedIndices.Count > 0)
            {
                int index = searchListView.SelectedIndices[0];

                _searchTopics.MoveTo(index);
                Topic topic = (Topic)_searchTopics.Current;
                DoNavigateByTopic(topic);
            }
        }

        // Search Virtual ListView wants an linkNode

        private void searchListView_RetrieveVirtualItem(object sender, RetrieveVirtualItemEventArgs e)
        {
            ListViewItem li;
            if (_searchTopics == null || e.ItemIndex >= _searchTopics.Count)
            {
                li = new ListViewItem("?");
                li.SubItems.AddRange(new String[] {"?", "?"});
                return;
            }

            //Get record
            int idx = e.ItemIndex;
            _searchTopics.MoveTo(idx);
            ITopic topic = (ITopic)_searchTopics.Current;

            String itemNo = (_startListViewNumbering + idx).ToString();  //rank as returned by runtime
            string[] subItems = new String[] { topic.Title, topic.DisplayVersion };
            li = new ListViewItem(itemNo);
            li.SubItems.AddRange(subItems);
            e.Item = li;
        }


        // Resize - Adjust columns for autosize (index and search pages)

        private void DoResizeColumns()
        {
            // Index ListView - Autosize 2 of 3 columns
            indexListView.Columns[1].Width = indexListView.ClientSize.Width - indexListView.Columns[0].Width - indexListView.Columns[2].Width - 8;

            // Index Results - Autosize 2 of 3 columns
            resultsListView.Columns[1].Width = resultsListView.ClientSize.Width - resultsListView.Columns[0].Width - resultsListView.Columns[2].Width - 8;

            // search Results - Autosize 2 of 3 columns
            searchListView.Columns[1].Width = searchListView.ClientSize.Width - searchListView.Columns[0].Width - searchListView.Columns[2].Width - 8;
        }

        private void tabPageIndex_Layout(object sender, LayoutEventArgs e)
        {
            DoResizeColumns();
        }

        private void tabPageSearch_Layout(object sender, LayoutEventArgs e)
        {
            DoResizeColumns();
        }

        private void tabControlNav_Resize(object sender, EventArgs e)
        {
            DoResizeColumns();
        }

        // --------------------------------------------------------------------------------------------------
        // Navigation Address Bar
        // --------------------------------------------------------------------------------------------------

        private void urlTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
                DoNavigate(urlTextBox.Text);
        }

        private void GoBtn_Click(object sender, EventArgs e)
        {
            DoNavigate(urlTextBox.Text);
        }

        private void DoNavigate(String url)
        {
            MsxhelpProtocol.UserTopicText = "";  //While we are here clear User Edited text

            if (String.IsNullOrEmpty(url))
                return;
            url = url.Trim();
            if (url == "")
                return;
            urlTextBox.Text = url;
            webBrowser1.Navigate(url);
        }

        private void DoNavigateByID(String topicId)
        {
            DoNavigate(UrlFromId(topicId));
        }

        private void DoNavigateByTopic(Topic topic)
        {
            DoNavigate(UrlFromTopic(topic));
        }
		public void DoNavigateByF1Keyword(String keyword)
		{
			DoNavigate(UrlFromF1Keyword(keyword));
		}
		public String UrlFromF1Keyword(String keyword)
		{
			string[] stringSeparators = new string[] {"%5C"};
			String[] prioritizedF1Keywords = keyword.Split(stringSeparators, StringSplitOptions.None);
			Topic topic = (Topic)_catalogRead.GetTopicDetailsForF1Keyword(_catalog, prioritizedF1Keywords, null);
            return UrlFromTopic(topic);
		}
		
        private String UrlFromId(String topicId)
        {
            if (topicId == null)
                return null;
            topicId = topicId.Trim();
            if (topicId == "")
                return null;

            if (!_catalog.IsOpen)
            {
                MessageBox.Show("First open a catalog!");
                return null;
            }

            Topic topic = (Topic)_catalogRead.GetIndexedTopicDetails(_catalog, topicId, null);
            return UrlFromTopic(topic);
        }

        //Let's try and form a nice looking URL (same as MS Viewer does in the topic property box)
        //eg. "ms-xhelp:///?method=page&id=xxx&folderName=en-US&vendor=Microsoft&topicVersion=&topicLocale=EN-US"

        private String UrlFromTopic(Topic topic)
        {
            if (topic == null)
                return null;

            if (!_catalog.IsOpen)
            {
                MessageBox.Show("First open a catalog!");
                return null;
            }

            String url = "ms-xhelp:///?method=page&id=" + topic.Id;

            if (!String.IsNullOrEmpty(_CatalogLocale))
                url += "&folderName=" + _CatalogLocale;
            else if (!String.IsNullOrEmpty(topic.Locale))
                url += "&folderName=" + topic.Locale;

            if (!String.IsNullOrEmpty(topic.Vendor))
                url += "&vendor=" + topic.Vendor;
            if (!String.IsNullOrEmpty(topic.TopicVersion))
                url += "&topicVersion=" + topic.TopicVersion;
            if (!String.IsNullOrEmpty(topic.TopicLocale))
                url += "&topicLocale=" + topic.TopicLocale;
            return url;
        }

        private void webBrowser1_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            urlTextBox.Text = webBrowser1.Url.AbsoluteUri;
            EnableDisable();
            if (autoSyncOption.Checked)
                DoSync();
        }


        private void EnableDisable()
        {
            backBtn.Enabled = webBrowser1.CanGoBack;
            forwardBtn.Enabled = webBrowser1.CanGoForward;
        }

        private void backBtn_Click(object sender, EventArgs e)
        {
            if (webBrowser1.CanGoBack)
                webBrowser1.GoBack();
        }

        private void forwardBtn_Click(object sender, EventArgs e)
        {
            if (webBrowser1.CanGoForward)
                webBrowser1.GoForward();
        }



        // Render Type 

        private void ShowUnrenderedSource(Boolean showSource)
        {
            sourceSplitter.Panel2Collapsed = !showSource;
            mnuShowRawSource.Checked = showSource;
        }

        private void mnuUnrenderedSource_Click(object sender, EventArgs e)
        {
            ShowUnrenderedSource(sourceSplitter.Panel2Collapsed);   //toggle
        }


/*
        private void SetVSRenderMode(Boolean vsRenderMode)
        {
            mnuVSRenderer.Checked = vsRenderMode;
            MsxhelpProtocol.RenderUsingVS = vsRenderMode;
        }

        private void mnuRenderingMode_Click(object sender, EventArgs e)
        {
            SetVSRenderMode(!mnuVSRenderer.Checked); //toggle
            webBrowser1.Refresh(WebBrowserRefreshOption.Completely); //re-render
        }
*/

        public void RawCodeChangeEvent(String text)  //MsxhelpProtocol Pluggable protocol sending us raw help text
        {
            richTextBoxCodeView.Text = text;
        }


        // Help

        private void helpBtn_Click(object sender, EventArgs e)
        {
            Process.Start("http://hv2.helpmvp.com/code/viewer/");
        }


        // Toc Sync

        private void autoSyncOption_Click(object sender, EventArgs e)
        {
            autoSyncOption.Checked = !autoSyncOption.Checked;
        }

        private void tocSyncBtn_Click(object sender, EventArgs e)
        {
            if (!tocSyncBtn.DropDownButtonPressed)
            {
                if (tabControlNav.SelectedIndex != 0)
                    tabControlNav.SelectedIndex = 0;

                DoSync();
            }
        }

        private void DoSync()
        {
            String topicID = TopicLink.GetMshcTopicID(webBrowser1);
            if (!String.IsNullOrEmpty(topicID))
            {
                inSyncMethod = true;
                try
                {
                    tocPanel.Sync(topicID);
                }
                finally
                {
                    inSyncMethod = false;
                }
            }
        }



        // UserTopicText Render (not unused & untested)

        private void UserTopicRender()
        {
            MsxhelpProtocol.UserTopicText = 0xEF + 0xBB + 0xBF + richTextBoxCodeView.Text;   //Grab text from raw code view window
            webBrowser1.Refresh(WebBrowserRefreshOption.Completely);
        }

        //Keys 

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (keyData == (Keys.Alt | Keys.Left) && webBrowser1.CanGoBack)  // Back - And the Backspace key will works only when the Browser has the focus
            {
                webBrowser1.GoBack();
                EnableDisable();
                return true;
            }
            if (keyData == (Keys.Alt | Keys.Right) && webBrowser1.CanGoForward)  //Forward
            {
                webBrowser1.GoForward();
                EnableDisable();
                return true;
            }
            if (keyData == (Keys.Control | Keys.S))  // Toc Sync
            {
                DoSync();
                EnableDisable();
                return true;
            }
            if ((keyData == (Keys.Control | Keys.D1)) || (keyData == (Keys.Alt | Keys.C)))   //Contents Tab
            {
                tabControlNav.SelectedIndex = 0;
                tocPanel.DoFocus();
                return true;
            }
            if ((keyData == (Keys.Control | Keys.D2)) || (keyData == (Keys.Alt | Keys.I)))   //Index Tab
            {
                tabControlNav.SelectedIndex = 1;
                if (indexTextBox.CanFocus && !indexTextBox.Focused)
                    indexTextBox.Focus();
                return true;
            }
            if ((keyData == (Keys.Control | Keys.D3)) || (keyData == (Keys.Alt | Keys.S)))   //Search Tab
            {
                tabControlNav.SelectedIndex = 2;
                if (SearchCbo.CanFocus && !SearchCbo.Focused)
                    SearchCbo.Focus();
                return true;
            }
            return base.ProcessCmdKey(ref msg, keyData);
        }

        // MSDN Online Link

        private void OnlineLinkBtn_Click(object sender, EventArgs e)
        {
            String topicId = TopicLink.GetMshcTopicID(webBrowser1);
            if (!String.IsNullOrEmpty(topicId))
            {
                Topic topic = (Topic)_catalogRead.GetIndexedTopicDetails(_catalog, topicId, null);
                String url = TopicLink.MakeMsdnLibraryUrl(topicId, "", topic.TopicVersion);
                if (!String.IsNullOrEmpty(url) && topic.Vendor.Equals("Microsoft", StringComparison.InvariantCultureIgnoreCase))
                {
                    webBrowser1.Navigate(url);
                    richTextBoxCodeView.Text = "";
                    return;
                }
            }
            //defaul to MSDN Library home page
            webBrowser1.Navigate(TopicLink.GetMSDNLibraryHome(_CatalogLocale));
            richTextBoxCodeView.Text = "";
        }









    }



}
