namespace HV2ApiExplore.Forms
{
    partial class Page_Keywords
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Page_Keywords));
            System.Windows.Forms.ListViewGroup listViewGroup1 = new System.Windows.Forms.ListViewGroup("ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left);
            this.panel1 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.helpLinkLabel = new System.Windows.Forms.LinkLabel();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.panelControls = new System.Windows.Forms.Panel();
            this.GetKeywordsTip = new System.Windows.Forms.PictureBox();
            this.GetKeywordsBtn = new System.Windows.Forms.Button();
            this.statusLabel = new System.Windows.Forms.Label();
            this.panelBase = new System.Windows.Forms.Panel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.panelKeywords = new System.Windows.Forms.Panel();
            this.keywordsListView = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.searchBoxPanel = new System.Windows.Forms.Panel();
            this.searchTextBox = new System.Windows.Forms.TextBox();
            this.keywordsTextBox = new System.Windows.Forms.RichTextBox();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.panelControls.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GetKeywordsTip)).BeginInit();
            this.panelBase.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panelKeywords.SuspendLayout();
            this.searchBoxPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.MidnightBlue;
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.label7);
            this.panel1.Controls.Add(this.pictureBox1);
            this.panel1.Controls.Add(this.helpLinkLabel);
            this.panel1.Controls.Add(this.label4);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(637, 67);
            this.panel1.TabIndex = 16;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.ForeColor = System.Drawing.Color.White;
            this.label7.Location = new System.Drawing.Point(26, 29);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(248, 13);
            this.label7.TabIndex = 5;
            this.label7.Text = "- Demonstrate call ICatalogRead.GetKeywords()";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(531, 24);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(16, 16);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox1.TabIndex = 4;
            this.pictureBox1.TabStop = false;
            // 
            // helpLinkLabel
            // 
            this.helpLinkLabel.ActiveLinkColor = System.Drawing.Color.Lavender;
            this.helpLinkLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.helpLinkLabel.AutoSize = true;
            this.helpLinkLabel.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.helpLinkLabel.LinkColor = System.Drawing.Color.Lavender;
            this.helpLinkLabel.Location = new System.Drawing.Point(550, 24);
            this.helpLinkLabel.Name = "helpLinkLabel";
            this.helpLinkLabel.Size = new System.Drawing.Size(69, 13);
            this.helpLinkLabel.TabIndex = 3;
            this.helpLinkLabel.TabStop = true;
            this.helpLinkLabel.Text = "Online Help";
            this.helpLinkLabel.VisitedLinkColor = System.Drawing.Color.Lavender;
            this.helpLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.helpLinkLabel_LinkClicked);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.ForeColor = System.Drawing.Color.White;
            this.label4.Location = new System.Drawing.Point(26, 44);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(207, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "- Demonstrate a basic navigation index";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Segoe UI", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(9, 5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(73, 20);
            this.label3.TabIndex = 0;
            this.label3.Text = "Keywords";
            // 
            // panelControls
            // 
            this.panelControls.Controls.Add(this.GetKeywordsTip);
            this.panelControls.Controls.Add(this.GetKeywordsBtn);
            this.panelControls.Controls.Add(this.statusLabel);
            this.panelControls.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelControls.Location = new System.Drawing.Point(0, 67);
            this.panelControls.Name = "panelControls";
            this.panelControls.Size = new System.Drawing.Size(637, 81);
            this.panelControls.TabIndex = 17;
            // 
            // GetKeywordsTip
            // 
            this.GetKeywordsTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.GetKeywordsTip.Location = new System.Drawing.Point(143, 35);
            this.GetKeywordsTip.Name = "GetKeywordsTip";
            this.GetKeywordsTip.Size = new System.Drawing.Size(13, 13);
            this.GetKeywordsTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.GetKeywordsTip.TabIndex = 7;
            this.GetKeywordsTip.TabStop = false;
            // 
            // GetKeywordsBtn
            // 
            this.GetKeywordsBtn.Location = new System.Drawing.Point(14, 30);
            this.GetKeywordsBtn.Name = "GetKeywordsBtn";
            this.GetKeywordsBtn.Size = new System.Drawing.Size(125, 23);
            this.GetKeywordsBtn.TabIndex = 2;
            this.GetKeywordsBtn.Text = "Get Keywords";
            this.GetKeywordsBtn.UseVisualStyleBackColor = true;
            this.GetKeywordsBtn.Click += new System.EventHandler(this.GetKeywordsBtn_Click);
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(14, 60);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(42, 13);
            this.statusLabel.TabIndex = 1;
            this.statusLabel.Text = "Status:";
            // 
            // panelBase
            // 
            this.panelBase.Controls.Add(this.splitContainer1);
            this.panelBase.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelBase.Location = new System.Drawing.Point(0, 148);
            this.panelBase.Name = "panelBase";
            this.panelBase.Size = new System.Drawing.Size(637, 471);
            this.panelBase.TabIndex = 18;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.panelKeywords);
            this.splitContainer1.Size = new System.Drawing.Size(637, 471);
            this.splitContainer1.SplitterDistance = 296;
            this.splitContainer1.TabIndex = 0;
            // 
            // panelKeywords
            // 
            this.panelKeywords.Controls.Add(this.keywordsListView);
            this.panelKeywords.Controls.Add(this.searchBoxPanel);
            this.panelKeywords.Controls.Add(this.keywordsTextBox);
            this.panelKeywords.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelKeywords.Location = new System.Drawing.Point(0, 0);
            this.panelKeywords.Name = "panelKeywords";
            this.panelKeywords.Size = new System.Drawing.Size(296, 471);
            this.panelKeywords.TabIndex = 4;
            this.panelKeywords.Resize += new System.EventHandler(this.panelKeywords_Resize);
            // 
            // keywordsListView
            // 
            this.keywordsListView.AllowColumnReorder = true;
            this.keywordsListView.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.keywordsListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
            this.keywordsListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.keywordsListView.FullRowSelect = true;
            listViewGroup1.Header = "ListViewGroup";
            listViewGroup1.Name = "listViewGroup1";
            this.keywordsListView.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup1});
            this.keywordsListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.keywordsListView.HideSelection = false;
            this.keywordsListView.Location = new System.Drawing.Point(0, 26);
            this.keywordsListView.MultiSelect = false;
            this.keywordsListView.Name = "keywordsListView";
            this.keywordsListView.Size = new System.Drawing.Size(296, 346);
            this.keywordsListView.TabIndex = 4;
            this.keywordsListView.UseCompatibleStateImageBehavior = false;
            this.keywordsListView.View = System.Windows.Forms.View.Details;
            this.keywordsListView.VirtualMode = true;
            this.keywordsListView.RetrieveVirtualItem += new System.Windows.Forms.RetrieveVirtualItemEventHandler(this.keywordsListView_RetrieveVirtualItem);
            this.keywordsListView.SelectedIndexChanged += new System.EventHandler(this.keywordsListView_SelectedIndexChanged);
            this.keywordsListView.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.keywordsListView_KeyPress);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Item";
            this.columnHeader1.Width = 57;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Keyword";
            this.columnHeader2.Width = 190;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Topics";
            this.columnHeader3.Width = 44;
            // 
            // searchBoxPanel
            // 
            this.searchBoxPanel.Controls.Add(this.searchTextBox);
            this.searchBoxPanel.Dock = System.Windows.Forms.DockStyle.Top;
            this.searchBoxPanel.Location = new System.Drawing.Point(0, 0);
            this.searchBoxPanel.Name = "searchBoxPanel";
            this.searchBoxPanel.Size = new System.Drawing.Size(296, 26);
            this.searchBoxPanel.TabIndex = 6;
            // 
            // searchTextBox
            // 
            this.searchTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.searchTextBox.Location = new System.Drawing.Point(0, 0);
            this.searchTextBox.Name = "searchTextBox";
            this.searchTextBox.Size = new System.Drawing.Size(296, 22);
            this.searchTextBox.TabIndex = 3;
            this.searchTextBox.TextChanged += new System.EventHandler(this.searchTextBox_TextChanged);
            this.searchTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.searchTextBox_KeyDown);
            // 
            // keywordsTextBox
            // 
            this.keywordsTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.keywordsTextBox.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.keywordsTextBox.Location = new System.Drawing.Point(0, 372);
            this.keywordsTextBox.Name = "keywordsTextBox";
            this.keywordsTextBox.ReadOnly = true;
            this.keywordsTextBox.Size = new System.Drawing.Size(296, 99);
            this.keywordsTextBox.TabIndex = 5;
            this.keywordsTextBox.Text = "";
            this.keywordsTextBox.WordWrap = false;
            // 
            // Page_Keywords
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.panelBase);
            this.Controls.Add(this.panelControls);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Page_Keywords";
            this.Size = new System.Drawing.Size(637, 619);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.panelControls.ResumeLayout(false);
            this.panelControls.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GetKeywordsTip)).EndInit();
            this.panelBase.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.panelKeywords.ResumeLayout(false);
            this.searchBoxPanel.ResumeLayout(false);
            this.searchBoxPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.LinkLabel helpLinkLabel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Panel panelControls;
        private System.Windows.Forms.Button GetKeywordsBtn;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.Panel panelBase;
        private System.Windows.Forms.Panel panelKeywords;
        private System.Windows.Forms.ListView keywordsListView;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.RichTextBox keywordsTextBox;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.TextBox searchTextBox;
        private System.Windows.Forms.Panel searchBoxPanel;
        private System.Windows.Forms.PictureBox GetKeywordsTip;
    }
}
