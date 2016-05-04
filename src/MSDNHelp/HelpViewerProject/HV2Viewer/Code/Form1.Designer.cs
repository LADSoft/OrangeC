namespace HV2Viewer
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.ListViewGroup listViewGroup1 = new System.Windows.Forms.ListViewGroup("ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left);
            System.Windows.Forms.ListViewGroup listViewGroup2 = new System.Windows.Forms.ListViewGroup("ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left);
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.tabControlNav = new System.Windows.Forms.TabControl();
            this.tabPageTOC = new System.Windows.Forms.TabPage();
            this.tabPageIndex = new System.Windows.Forms.TabPage();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.indexListView = new System.Windows.Forms.ListView();
            this.listViewcolumnHeader = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader6 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader7 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.resultsListView = new System.Windows.Forms.ListView();
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader5 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.indexTextBox = new System.Windows.Forms.TextBox();
            this.tabPageSearch = new System.Windows.Forms.TabPage();
            this.searchListView = new System.Windows.Forms.ListView();
            this.searchColumnHeader = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.panel1 = new System.Windows.Forms.Panel();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.firstLink = new System.Windows.Forms.LinkLabel();
            this.prevLink = new System.Windows.Forms.LinkLabel();
            this.nextLink = new System.Windows.Forms.LinkLabel();
            this.lastLink = new System.Windows.Forms.LinkLabel();
            this.statusLabel = new System.Windows.Forms.Label();
            this.SearchCbo = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.pageSizeEdit = new System.Windows.Forms.NumericUpDown();
            this.FindBtn = new System.Windows.Forms.Button();
            this.sourceSplitter = new System.Windows.Forms.SplitContainer();
            this.webBrowser1 = new System.Windows.Forms.WebBrowser();
            this.richTextBoxCodeView = new System.Windows.Forms.RichTextBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
            this.labelUnrendered = new System.Windows.Forms.Label();
            this.imageUnrendered = new System.Windows.Forms.PictureBox();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.backBtn = new System.Windows.Forms.ToolStripButton();
            this.forwardBtn = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.openCatalogDropDown = new System.Windows.Forms.ToolStripDropDownButton();
            this.mnuOpenCatalogFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuOpenCatalogFile = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.tocSyncBtn = new System.Windows.Forms.ToolStripSplitButton();
            this.autoSyncOption = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuOptions = new System.Windows.Forms.ToolStripDropDownButton();
//            this.mnuVSRenderer = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.mnuShowRawSource = new System.Windows.Forms.ToolStripMenuItem();
            this.helpBtn = new System.Windows.Forms.ToolStripButton();
            this.mnuMsdnOnline = new System.Windows.Forms.ToolStripButton();
            this.headerPanel = new System.Windows.Forms.Panel();
            this.GoBtn = new System.Windows.Forms.Button();
            this.urlTextBox = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.tabControlNav.SuspendLayout();
            this.tabPageIndex.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.tabPageSearch.SuspendLayout();
            this.panel1.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pageSizeEdit)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.sourceSplitter)).BeginInit();
            this.sourceSplitter.Panel1.SuspendLayout();
            this.sourceSplitter.Panel2.SuspendLayout();
            this.sourceSplitter.SuspendLayout();
            this.panel2.SuspendLayout();
            this.flowLayoutPanel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.imageUnrendered)).BeginInit();
            this.toolStrip1.SuspendLayout();
            this.headerPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 63);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.tabControlNav);
            this.splitContainer1.Panel1.Padding = new System.Windows.Forms.Padding(4);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.sourceSplitter);
            this.splitContainer1.Size = new System.Drawing.Size(835, 569);
            this.splitContainer1.SplitterDistance = 311;
            this.splitContainer1.TabIndex = 2;
            // 
            // tabControlNav
            // 
            this.tabControlNav.Controls.Add(this.tabPageTOC);
            this.tabControlNav.Controls.Add(this.tabPageIndex);
            this.tabControlNav.Controls.Add(this.tabPageSearch);
            this.tabControlNav.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControlNav.ItemSize = new System.Drawing.Size(54, 18);
            this.tabControlNav.Location = new System.Drawing.Point(4, 4);
            this.tabControlNav.Name = "tabControlNav";
            this.tabControlNav.Padding = new System.Drawing.Point(22, 3);
            this.tabControlNav.SelectedIndex = 0;
            this.tabControlNav.Size = new System.Drawing.Size(303, 561);
            this.tabControlNav.TabIndex = 0;
            this.tabControlNav.Resize += new System.EventHandler(this.tabControlNav_Resize);
            // 
            // tabPageTOC
            // 
            this.tabPageTOC.BackColor = System.Drawing.Color.White;
            this.tabPageTOC.Location = new System.Drawing.Point(4, 22);
            this.tabPageTOC.Name = "tabPageTOC";
            this.tabPageTOC.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageTOC.Size = new System.Drawing.Size(295, 535);
            this.tabPageTOC.TabIndex = 0;
            this.tabPageTOC.Text = "Contents";
            // 
            // tabPageIndex
            // 
            this.tabPageIndex.BackColor = System.Drawing.Color.White;
            this.tabPageIndex.Controls.Add(this.splitContainer2);
            this.tabPageIndex.Controls.Add(this.indexTextBox);
            this.tabPageIndex.Location = new System.Drawing.Point(4, 22);
            this.tabPageIndex.Name = "tabPageIndex";
            this.tabPageIndex.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageIndex.Size = new System.Drawing.Size(295, 535);
            this.tabPageIndex.TabIndex = 1;
            this.tabPageIndex.Text = "Index";
            this.tabPageIndex.Layout += new System.Windows.Forms.LayoutEventHandler(this.tabPageIndex_Layout);
            // 
            // splitContainer2
            // 
            this.splitContainer2.BackColor = System.Drawing.Color.LightBlue;
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer2.Location = new System.Drawing.Point(3, 23);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.indexListView);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.resultsListView);
            this.splitContainer2.Size = new System.Drawing.Size(289, 509);
            this.splitContainer2.SplitterDistance = 341;
            this.splitContainer2.TabIndex = 1;
            this.splitContainer2.Resize += new System.EventHandler(this.tabControlNav_Resize);
            // 
            // indexListView
            // 
            this.indexListView.BackColor = System.Drawing.SystemColors.Window;
            this.indexListView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.indexListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.listViewcolumnHeader,
            this.columnHeader6,
            this.columnHeader7});
            this.indexListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.indexListView.FullRowSelect = true;
            listViewGroup1.Header = "ListViewGroup";
            listViewGroup1.Name = "listViewGroup1";
            this.indexListView.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup1});
            this.indexListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.indexListView.HideSelection = false;
            this.indexListView.Location = new System.Drawing.Point(0, 0);
            this.indexListView.MultiSelect = false;
            this.indexListView.Name = "indexListView";
            this.indexListView.Size = new System.Drawing.Size(289, 341);
            this.indexListView.TabIndex = 1;
            this.indexListView.UseCompatibleStateImageBehavior = false;
            this.indexListView.View = System.Windows.Forms.View.Details;
            this.indexListView.VirtualMode = true;
            this.indexListView.RetrieveVirtualItem += new System.Windows.Forms.RetrieveVirtualItemEventHandler(this.indexListView_RetrieveVirtualItem);
            this.indexListView.SelectedIndexChanged += new System.EventHandler(this.keywordsListView_SelectedIndexChanged);
            // 
            // listViewcolumnHeader
            // 
            this.listViewcolumnHeader.Text = "Item";
            this.listViewcolumnHeader.Width = 44;
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "Keywords";
            this.columnHeader6.Width = 195;
            // 
            // columnHeader7
            // 
            this.columnHeader7.Text = "Topics";
            this.columnHeader7.Width = 46;
            // 
            // resultsListView
            // 
            this.resultsListView.BackColor = System.Drawing.SystemColors.Window;
            this.resultsListView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.resultsListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader3,
            this.columnHeader4,
            this.columnHeader5});
            this.resultsListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.resultsListView.FullRowSelect = true;
            this.resultsListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.resultsListView.HideSelection = false;
            this.resultsListView.Location = new System.Drawing.Point(0, 0);
            this.resultsListView.MultiSelect = false;
            this.resultsListView.Name = "resultsListView";
            this.resultsListView.ShowItemToolTips = true;
            this.resultsListView.Size = new System.Drawing.Size(289, 164);
            this.resultsListView.TabIndex = 2;
            this.resultsListView.UseCompatibleStateImageBehavior = false;
            this.resultsListView.View = System.Windows.Forms.View.Details;
            this.resultsListView.ItemSelectionChanged += new System.Windows.Forms.ListViewItemSelectionChangedEventHandler(this.resultsListView_ItemSelectionChanged);
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Item";
            this.columnHeader3.Width = 33;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Topic Result";
            this.columnHeader4.Width = 166;
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "Location";
            this.columnHeader5.Width = 96;
            // 
            // indexTextBox
            // 
            this.indexTextBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.indexTextBox.Location = new System.Drawing.Point(3, 3);
            this.indexTextBox.Name = "indexTextBox";
            this.indexTextBox.Size = new System.Drawing.Size(289, 20);
            this.indexTextBox.TabIndex = 0;
            this.indexTextBox.TextChanged += new System.EventHandler(this.indexTextBox_TextChanged);
            // 
            // tabPageSearch
            // 
            this.tabPageSearch.BackColor = System.Drawing.Color.White;
            this.tabPageSearch.Controls.Add(this.searchListView);
            this.tabPageSearch.Controls.Add(this.panel1);
            this.tabPageSearch.Location = new System.Drawing.Point(4, 22);
            this.tabPageSearch.Name = "tabPageSearch";
            this.tabPageSearch.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageSearch.Size = new System.Drawing.Size(295, 535);
            this.tabPageSearch.TabIndex = 2;
            this.tabPageSearch.Text = "Search";
            this.tabPageSearch.Layout += new System.Windows.Forms.LayoutEventHandler(this.tabPageSearch_Layout);
            // 
            // searchListView
            // 
            this.searchListView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.searchListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.searchColumnHeader,
            this.columnHeader1,
            this.columnHeader2});
            this.searchListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.searchListView.FullRowSelect = true;
            listViewGroup2.Header = "ListViewGroup";
            listViewGroup2.Name = "listViewGroup1";
            this.searchListView.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup2});
            this.searchListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.searchListView.HideSelection = false;
            this.searchListView.Location = new System.Drawing.Point(3, 90);
            this.searchListView.Name = "searchListView";
            this.searchListView.Size = new System.Drawing.Size(289, 442);
            this.searchListView.TabIndex = 1;
            this.searchListView.UseCompatibleStateImageBehavior = false;
            this.searchListView.View = System.Windows.Forms.View.Details;
            this.searchListView.VirtualMode = true;
            this.searchListView.RetrieveVirtualItem += new System.Windows.Forms.RetrieveVirtualItemEventHandler(this.searchListView_RetrieveVirtualItem);
            this.searchListView.SelectedIndexChanged += new System.EventHandler(this.searchListView_SelectedIndexChanged);
            this.searchListView.Resize += new System.EventHandler(this.tabControlNav_Resize);
            // 
            // searchColumnHeader
            // 
            this.searchColumnHeader.Text = "Item";
            this.searchColumnHeader.Width = 35;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Topic";
            this.columnHeader1.Width = 185;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Version";
            this.columnHeader2.Width = 68;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.flowLayoutPanel1);
            this.panel1.Controls.Add(this.SearchCbo);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.pageSizeEdit);
            this.panel1.Controls.Add(this.FindBtn);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(289, 87);
            this.panel1.TabIndex = 0;
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.firstLink);
            this.flowLayoutPanel1.Controls.Add(this.prevLink);
            this.flowLayoutPanel1.Controls.Add(this.nextLink);
            this.flowLayoutPanel1.Controls.Add(this.lastLink);
            this.flowLayoutPanel1.Controls.Add(this.statusLabel);
            this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 65);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(289, 22);
            this.flowLayoutPanel1.TabIndex = 26;
            this.flowLayoutPanel1.WrapContents = false;
            // 
            // firstLink
            // 
            this.firstLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.firstLink.AutoSize = true;
            this.firstLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.firstLink.Location = new System.Drawing.Point(3, 0);
            this.firstLink.Name = "firstLink";
            this.firstLink.Size = new System.Drawing.Size(26, 13);
            this.firstLink.TabIndex = 0;
            this.firstLink.TabStop = true;
            this.firstLink.Text = "First";
            this.firstLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.firstLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.firstLink_LinkClicked);
            // 
            // prevLink
            // 
            this.prevLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.prevLink.AutoSize = true;
            this.prevLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.prevLink.Location = new System.Drawing.Point(35, 0);
            this.prevLink.Name = "prevLink";
            this.prevLink.Size = new System.Drawing.Size(38, 13);
            this.prevLink.TabIndex = 1;
            this.prevLink.TabStop = true;
            this.prevLink.Text = "< Prev";
            this.prevLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.prevLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.prevLink_LinkClicked);
            // 
            // nextLink
            // 
            this.nextLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.nextLink.AutoSize = true;
            this.nextLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.nextLink.Location = new System.Drawing.Point(79, 0);
            this.nextLink.Name = "nextLink";
            this.nextLink.Size = new System.Drawing.Size(38, 13);
            this.nextLink.TabIndex = 2;
            this.nextLink.TabStop = true;
            this.nextLink.Text = "Next >";
            this.nextLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.nextLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.nextLink_LinkClicked);
            // 
            // lastLink
            // 
            this.lastLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.lastLink.AutoSize = true;
            this.lastLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.lastLink.Location = new System.Drawing.Point(123, 0);
            this.lastLink.Name = "lastLink";
            this.lastLink.Size = new System.Drawing.Size(27, 13);
            this.lastLink.TabIndex = 3;
            this.lastLink.TabStop = true;
            this.lastLink.Text = "Last";
            this.lastLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.lastLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lastLink_LinkClicked);
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(156, 0);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(61, 13);
            this.statusLabel.TabIndex = 4;
            this.statusLabel.Text = "statusLabel";
            // 
            // SearchCbo
            // 
            this.SearchCbo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.SearchCbo.DropDownHeight = 1;
            this.SearchCbo.FormattingEnabled = true;
            this.SearchCbo.IntegralHeight = false;
            this.SearchCbo.Location = new System.Drawing.Point(5, 6);
            this.SearchCbo.Name = "SearchCbo";
            this.SearchCbo.Size = new System.Drawing.Size(225, 21);
            this.SearchCbo.TabIndex = 0;
            this.SearchCbo.DropDown += new System.EventHandler(this.SearchCbo_DropDown);
            this.SearchCbo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.SearchCbo_KeyDown);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(58, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Page Size:";
            // 
            // pageSizeEdit
            // 
            this.pageSizeEdit.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.pageSizeEdit.Location = new System.Drawing.Point(70, 33);
            this.pageSizeEdit.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.pageSizeEdit.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.pageSizeEdit.Name = "pageSizeEdit";
            this.pageSizeEdit.Size = new System.Drawing.Size(65, 20);
            this.pageSizeEdit.TabIndex = 3;
            this.pageSizeEdit.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            // 
            // FindBtn
            // 
            this.FindBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.FindBtn.Location = new System.Drawing.Point(236, 5);
            this.FindBtn.Name = "FindBtn";
            this.FindBtn.Size = new System.Drawing.Size(50, 23);
            this.FindBtn.TabIndex = 1;
            this.FindBtn.Text = "&Find";
            this.FindBtn.UseVisualStyleBackColor = true;
            this.FindBtn.Click += new System.EventHandler(this.FindBtn_Click);
            // 
            // sourceSplitter
            // 
            this.sourceSplitter.Dock = System.Windows.Forms.DockStyle.Fill;
            this.sourceSplitter.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.sourceSplitter.Location = new System.Drawing.Point(0, 0);
            this.sourceSplitter.Name = "sourceSplitter";
            this.sourceSplitter.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // sourceSplitter.Panel1
            // 
            this.sourceSplitter.Panel1.Controls.Add(this.webBrowser1);
            // 
            // sourceSplitter.Panel2
            // 
            this.sourceSplitter.Panel2.Controls.Add(this.richTextBoxCodeView);
            this.sourceSplitter.Panel2.Controls.Add(this.panel2);
            this.sourceSplitter.Size = new System.Drawing.Size(520, 569);
            this.sourceSplitter.SplitterDistance = 408;
            this.sourceSplitter.TabIndex = 1;
            // 
            // webBrowser1
            // 
            this.webBrowser1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.webBrowser1.Location = new System.Drawing.Point(0, 0);
            this.webBrowser1.Margin = new System.Windows.Forms.Padding(5);
            this.webBrowser1.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowser1.Name = "webBrowser1";
            this.webBrowser1.Size = new System.Drawing.Size(520, 408);
            this.webBrowser1.TabIndex = 0;
            this.webBrowser1.Navigated += new System.Windows.Forms.WebBrowserNavigatedEventHandler(this.webBrowser1_Navigated);
            // 
            // richTextBoxCodeView
            // 
            this.richTextBoxCodeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.richTextBoxCodeView.Location = new System.Drawing.Point(0, 21);
            this.richTextBoxCodeView.Name = "richTextBoxCodeView";
            this.richTextBoxCodeView.ReadOnly = true;
            this.richTextBoxCodeView.Size = new System.Drawing.Size(520, 136);
            this.richTextBoxCodeView.TabIndex = 0;
            this.richTextBoxCodeView.Text = "";
            this.richTextBoxCodeView.WordWrap = false;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.flowLayoutPanel2);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(520, 21);
            this.panel2.TabIndex = 1;
            // 
            // flowLayoutPanel2
            // 
            this.flowLayoutPanel2.Controls.Add(this.labelUnrendered);
            this.flowLayoutPanel2.Controls.Add(this.imageUnrendered);
            this.flowLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.flowLayoutPanel2.Location = new System.Drawing.Point(0, 0);
            this.flowLayoutPanel2.Name = "flowLayoutPanel2";
            this.flowLayoutPanel2.Size = new System.Drawing.Size(520, 21);
            this.flowLayoutPanel2.TabIndex = 0;
            this.flowLayoutPanel2.WrapContents = false;
            // 
            // labelUnrendered
            // 
            this.labelUnrendered.AutoSize = true;
            this.labelUnrendered.Location = new System.Drawing.Point(0, 3);
            this.labelUnrendered.Margin = new System.Windows.Forms.Padding(0, 3, 0, 3);
            this.labelUnrendered.Name = "labelUnrendered";
            this.labelUnrendered.Size = new System.Drawing.Size(103, 13);
            this.labelUnrendered.TabIndex = 1;
            this.labelUnrendered.Text = "Unrendered Source:";
            // 
            // imageUnrendered
            // 
            this.imageUnrendered.Image = global::HV2Viewer.Properties.Resources.infoTip13x13;
            this.imageUnrendered.Location = new System.Drawing.Point(103, 4);
            this.imageUnrendered.Margin = new System.Windows.Forms.Padding(0, 4, 3, 3);
            this.imageUnrendered.Name = "imageUnrendered";
            this.imageUnrendered.Size = new System.Drawing.Size(13, 13);
            this.imageUnrendered.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.imageUnrendered.TabIndex = 2;
            this.imageUnrendered.TabStop = false;
            // 
            // toolStrip1
            // 
            this.toolStrip1.BackColor = System.Drawing.Color.AliceBlue;
            this.toolStrip1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.backBtn,
            this.forwardBtn,
            this.toolStripSeparator1,
            this.openCatalogDropDown,
            this.tocSyncBtn,
            this.mnuOptions,
            this.helpBtn,
            this.mnuMsdnOnline});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Padding = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.toolStrip1.Size = new System.Drawing.Size(835, 31);
            this.toolStrip1.TabIndex = 0;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // backBtn
            // 
            this.backBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.backBtn.Image = global::HV2Viewer.Properties.Resources.LeftArrow;
            this.backBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.backBtn.Name = "backBtn";
            this.backBtn.Size = new System.Drawing.Size(28, 28);
            this.backBtn.Text = "Back (Backspace, Alt+Left)";
            this.backBtn.Click += new System.EventHandler(this.backBtn_Click);
            // 
            // forwardBtn
            // 
            this.forwardBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.forwardBtn.Image = global::HV2Viewer.Properties.Resources.RightArrow;
            this.forwardBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.forwardBtn.Name = "forwardBtn";
            this.forwardBtn.Size = new System.Drawing.Size(28, 28);
            this.forwardBtn.Text = "Forward (Alt+Right)";
            this.forwardBtn.Click += new System.EventHandler(this.forwardBtn_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 31);
            // 
            // openCatalogDropDown
            // 
            this.openCatalogDropDown.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuOpenCatalogFolder,
            this.mnuOpenCatalogFile,
            this.toolStripSeparator2});
            this.openCatalogDropDown.Image = global::HV2Viewer.Properties.Resources.OpenHelpCatalog;
            this.openCatalogDropDown.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.openCatalogDropDown.Name = "openCatalogDropDown";
            this.openCatalogDropDown.Size = new System.Drawing.Size(117, 28);
            this.openCatalogDropDown.Text = "&Open Catalog";
            // 
            // mnuOpenCatalogFolder
            // 
            this.mnuOpenCatalogFolder.Name = "mnuOpenCatalogFolder";
            this.mnuOpenCatalogFolder.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.mnuOpenCatalogFolder.Size = new System.Drawing.Size(286, 22);
            this.mnuOpenCatalogFolder.Text = "Open Catalog Folder...";
            this.mnuOpenCatalogFolder.Click += new System.EventHandler(this.mnuOpenCatalogFolder_Click);
            // 
            // mnuOpenCatalogFile
            // 
            this.mnuOpenCatalogFile.Name = "mnuOpenCatalogFile";
            this.mnuOpenCatalogFile.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift) 
            | System.Windows.Forms.Keys.O)));
            this.mnuOpenCatalogFile.Size = new System.Drawing.Size(286, 22);
            this.mnuOpenCatalogFile.Text = "Open Catalog .mshx File...";
            this.mnuOpenCatalogFile.Click += new System.EventHandler(this.mnuOpenCatalogFile_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(283, 6);
            // 
            // tocSyncBtn
            // 
            this.tocSyncBtn.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.autoSyncOption});
            this.tocSyncBtn.Image = global::HV2Viewer.Properties.Resources.TocSync;
            this.tocSyncBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tocSyncBtn.Name = "tocSyncBtn";
            this.tocSyncBtn.Size = new System.Drawing.Size(99, 28);
            this.tocSyncBtn.Text = "&TOC Sync";
            this.tocSyncBtn.ToolTipText = "Sync TOC to Topic (Ctrl+S)";
            this.tocSyncBtn.Click += new System.EventHandler(this.tocSyncBtn_Click);
            // 
            // autoSyncOption
            // 
            this.autoSyncOption.AutoToolTip = true;
            this.autoSyncOption.Checked = true;
            this.autoSyncOption.CheckState = System.Windows.Forms.CheckState.Checked;
            this.autoSyncOption.Name = "autoSyncOption";
            this.autoSyncOption.Size = new System.Drawing.Size(152, 22);
            this.autoSyncOption.Text = "Auto Sync";
            this.autoSyncOption.ToolTipText = "TOC sync automatically as the topic changes";
            this.autoSyncOption.Click += new System.EventHandler(this.autoSyncOption_Click);
            // 
            // mnuOptions
            // 
            this.mnuOptions.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mnuOptions.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
//            this.mnuVSRenderer,
            this.toolStripSeparator3,
            this.mnuShowRawSource});
            this.mnuOptions.Image = global::HV2Viewer.Properties.Resources.Options;
            this.mnuOptions.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mnuOptions.Name = "mnuOptions";
            this.mnuOptions.Size = new System.Drawing.Size(37, 28);
            this.mnuOptions.Text = "Options";
            // 
            // mnuVSRenderer
            // 
/*
            this.mnuVSRenderer.AutoToolTip = true;
            this.mnuVSRenderer.Name = "mnuVSRenderer";
            this.mnuVSRenderer.Size = new System.Drawing.Size(222, 22);
            this.mnuVSRenderer.Text = "Render with VS 11 DLL";
            this.mnuVSRenderer.ToolTipText = "Uncheck to use customer renderer";
            this.mnuVSRenderer.Click += new System.EventHandler(this.mnuRenderingMode_Click);
*/
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(219, 6);
            // 
            // mnuShowRawSource
            // 
            this.mnuShowRawSource.Checked = true;
            this.mnuShowRawSource.CheckState = System.Windows.Forms.CheckState.Checked;
            this.mnuShowRawSource.Name = "mnuShowRawSource";
            this.mnuShowRawSource.ShortcutKeyDisplayString = "";
            this.mnuShowRawSource.ShortcutKeys = System.Windows.Forms.Keys.F6;
            this.mnuShowRawSource.Size = new System.Drawing.Size(222, 22);
            this.mnuShowRawSource.Text = "View Unrendered Source";
            this.mnuShowRawSource.Click += new System.EventHandler(this.mnuUnrenderedSource_Click);
            // 
            // helpBtn
            // 
            this.helpBtn.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.helpBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.helpBtn.Image = global::HV2Viewer.Properties.Resources.Help;
            this.helpBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.helpBtn.Name = "helpBtn";
            this.helpBtn.Size = new System.Drawing.Size(28, 28);
            this.helpBtn.Text = "Help";
            this.helpBtn.Click += new System.EventHandler(this.helpBtn_Click);
            // 
            // mnuMsdnOnline
            // 
            this.mnuMsdnOnline.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mnuMsdnOnline.Image = global::HV2Viewer.Properties.Resources.msdn1;
            this.mnuMsdnOnline.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mnuMsdnOnline.Name = "mnuMsdnOnline";
            this.mnuMsdnOnline.Size = new System.Drawing.Size(28, 28);
            this.mnuMsdnOnline.Text = "Msdn Online";
            this.mnuMsdnOnline.Click += new System.EventHandler(this.OnlineLinkBtn_Click);
            // 
            // headerPanel
            // 
            this.headerPanel.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.headerPanel.Controls.Add(this.GoBtn);
            this.headerPanel.Controls.Add(this.urlTextBox);
            this.headerPanel.Controls.Add(this.toolStrip1);
            this.headerPanel.Dock = System.Windows.Forms.DockStyle.Top;
            this.headerPanel.Location = new System.Drawing.Point(0, 0);
            this.headerPanel.Name = "headerPanel";
            this.headerPanel.Size = new System.Drawing.Size(835, 63);
            this.headerPanel.TabIndex = 3;
            // 
            // GoBtn
            // 
            this.GoBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.GoBtn.Location = new System.Drawing.Point(790, 35);
            this.GoBtn.Name = "GoBtn";
            this.GoBtn.Size = new System.Drawing.Size(37, 22);
            this.GoBtn.TabIndex = 2;
            this.GoBtn.Text = "&Go";
            this.GoBtn.UseVisualStyleBackColor = true;
            this.GoBtn.Click += new System.EventHandler(this.GoBtn_Click);
            // 
            // urlTextBox
            // 
            this.urlTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.urlTextBox.Location = new System.Drawing.Point(7, 36);
            this.urlTextBox.Name = "urlTextBox";
            this.urlTextBox.Size = new System.Drawing.Size(778, 20);
            this.urlTextBox.TabIndex = 1;
            this.urlTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.urlTextBox_KeyDown);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(835, 632);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.headerPanel);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.KeyPreview = true;
            this.Name = "Form1";
            this.Text = "HV2 Viewer";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.tabControlNav.ResumeLayout(false);
            this.tabPageIndex.ResumeLayout(false);
            this.tabPageIndex.PerformLayout();
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.tabPageSearch.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pageSizeEdit)).EndInit();
            this.sourceSplitter.Panel1.ResumeLayout(false);
            this.sourceSplitter.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.sourceSplitter)).EndInit();
            this.sourceSplitter.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.flowLayoutPanel2.ResumeLayout(false);
            this.flowLayoutPanel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.imageUnrendered)).EndInit();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.headerPanel.ResumeLayout(false);
            this.headerPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TabControl tabControlNav;
        private System.Windows.Forms.TabPage tabPageTOC;
        private System.Windows.Forms.TabPage tabPageIndex;
        private System.Windows.Forms.TabPage tabPageSearch;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.WebBrowser webBrowser1;
        private System.Windows.Forms.TextBox indexTextBox;
        private System.Windows.Forms.ListView indexListView;
        private System.Windows.Forms.ColumnHeader listViewcolumnHeader;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ListView searchListView;
        private System.Windows.Forms.ColumnHeader searchColumnHeader;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.Button FindBtn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown pageSizeEdit;
        private System.Windows.Forms.ComboBox SearchCbo;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.LinkLabel firstLink;
        private System.Windows.Forms.LinkLabel prevLink;
        private System.Windows.Forms.LinkLabel nextLink;
        private System.Windows.Forms.LinkLabel lastLink;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.ListView resultsListView;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.Panel headerPanel;
        private System.Windows.Forms.Button GoBtn;
        private System.Windows.Forms.TextBox urlTextBox;
        private System.Windows.Forms.ToolStripDropDownButton openCatalogDropDown;
        private System.Windows.Forms.ToolStripMenuItem mnuOpenCatalogFolder;
        private System.Windows.Forms.ToolStripMenuItem mnuOpenCatalogFile;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton backBtn;
        private System.Windows.Forms.ToolStripButton forwardBtn;
        private System.Windows.Forms.ToolStripDropDownButton mnuOptions;
 //       private System.Windows.Forms.ToolStripMenuItem mnuVSRenderer;
        private System.Windows.Forms.ToolStripMenuItem mnuShowRawSource;
        private System.Windows.Forms.ToolStripButton helpBtn;
        private System.Windows.Forms.ToolStripSplitButton tocSyncBtn;
        private System.Windows.Forms.ToolStripMenuItem autoSyncOption;
        private System.Windows.Forms.SplitContainer sourceSplitter;
        private System.Windows.Forms.RichTextBox richTextBoxCodeView;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
        private System.Windows.Forms.Label labelUnrendered;
        private System.Windows.Forms.PictureBox imageUnrendered;
        private System.Windows.Forms.ToolStripButton mnuMsdnOnline;
    }
}

