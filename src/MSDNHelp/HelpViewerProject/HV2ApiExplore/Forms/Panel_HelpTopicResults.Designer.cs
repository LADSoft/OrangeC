namespace HV2ApiExplore.Forms
{
    partial class Panel_HelpTopicResults
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
            System.Windows.Forms.ListViewGroup listViewGroup2 = new System.Windows.Forms.ListViewGroup("ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left);
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.searchResultLV = new System.Windows.Forms.ListView();
            this.columnHeader_Rank = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Title = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_DisplayVersion = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Vendor = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_Locale = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader_TopicID = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.splitSource = new System.Windows.Forms.SplitContainer();
            this.webBrowser1 = new System.Windows.Forms.WebBrowser();
            this.detailsRichTextBox = new System.Windows.Forms.RichTextBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
            this.HlpViewerBtn = new System.Windows.Forms.Button();
            this.linkUnrendered = new System.Windows.Forms.LinkLabel();
            this.tipUnrendered = new System.Windows.Forms.PictureBox();
            this.linkBackToDetails = new System.Windows.Forms.LinkLabel();
            this.label1 = new System.Windows.Forms.Label();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.label2 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitSource)).BeginInit();
            this.splitSource.Panel1.SuspendLayout();
            this.splitSource.Panel2.SuspendLayout();
            this.splitSource.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel3.SuspendLayout();
            this.flowLayoutPanel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tipUnrendered)).BeginInit();
            this.flowLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.searchResultLV);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitSource);
            this.splitContainer1.Panel2.Controls.Add(this.panel3);
            this.splitContainer1.Size = new System.Drawing.Size(904, 656);
            this.splitContainer1.SplitterDistance = 418;
            this.splitContainer1.TabIndex = 27;
            // 
            // searchResultLV
            // 
            this.searchResultLV.AllowColumnReorder = true;
            this.searchResultLV.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.searchResultLV.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader_Rank,
            this.columnHeader_Title,
            this.columnHeader_DisplayVersion,
            this.columnHeader_Vendor,
            this.columnHeader_Locale,
            this.columnHeader_TopicID});
            this.searchResultLV.Dock = System.Windows.Forms.DockStyle.Fill;
            this.searchResultLV.FullRowSelect = true;
            listViewGroup2.Header = "ListViewGroup";
            listViewGroup2.Name = "listViewGroup1";
            this.searchResultLV.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup2});
            this.searchResultLV.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.searchResultLV.HideSelection = false;
            this.searchResultLV.Location = new System.Drawing.Point(0, 0);
            this.searchResultLV.Name = "searchResultLV";
            this.searchResultLV.Size = new System.Drawing.Size(416, 654);
            this.searchResultLV.TabIndex = 26;
            this.searchResultLV.UseCompatibleStateImageBehavior = false;
            this.searchResultLV.View = System.Windows.Forms.View.Details;
            this.searchResultLV.VirtualMode = true;
            this.searchResultLV.RetrieveVirtualItem += new System.Windows.Forms.RetrieveVirtualItemEventHandler(this.searchResultLV_RetrieveVirtualItem);
            this.searchResultLV.SelectedIndexChanged += new System.EventHandler(this.searchResultLV_SelectedIndexChanged);
            this.searchResultLV.DoubleClick += new System.EventHandler(this.searchResultLV_DoubleClick);
            this.searchResultLV.MouseUp += new System.Windows.Forms.MouseEventHandler(this.searchResultLV_MouseUp);
            // 
            // columnHeader_Rank
            // 
            this.columnHeader_Rank.Text = "Item";
            this.columnHeader_Rank.Width = 39;
            // 
            // columnHeader_Title
            // 
            this.columnHeader_Title.Text = "Topic Title";
            this.columnHeader_Title.Width = 135;
            // 
            // columnHeader_DisplayVersion
            // 
            this.columnHeader_DisplayVersion.Text = "DisplayVersions";
            this.columnHeader_DisplayVersion.Width = 121;
            // 
            // columnHeader_Vendor
            // 
            this.columnHeader_Vendor.Text = "Vendor";
            this.columnHeader_Vendor.Width = 65;
            // 
            // columnHeader_Locale
            // 
            this.columnHeader_Locale.Text = "Locale";
            this.columnHeader_Locale.Width = 44;
            // 
            // columnHeader_TopicID
            // 
            this.columnHeader_TopicID.Text = "Id";
            this.columnHeader_TopicID.Width = 200;
            // 
            // splitSource
            // 
            this.splitSource.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitSource.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitSource.Location = new System.Drawing.Point(0, 30);
            this.splitSource.Name = "splitSource";
            this.splitSource.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitSource.Panel1
            // 
            this.splitSource.Panel1.Controls.Add(this.webBrowser1);
            // 
            // splitSource.Panel2
            // 
            this.splitSource.Panel2.Controls.Add(this.detailsRichTextBox);
            this.splitSource.Panel2.Controls.Add(this.panel1);
            this.splitSource.Size = new System.Drawing.Size(480, 624);
            this.splitSource.SplitterDistance = 333;
            this.splitSource.TabIndex = 3;
            // 
            // webBrowser1
            // 
            this.webBrowser1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.webBrowser1.Location = new System.Drawing.Point(0, 0);
            this.webBrowser1.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowser1.Name = "webBrowser1";
            this.webBrowser1.Size = new System.Drawing.Size(480, 333);
            this.webBrowser1.TabIndex = 2;
            this.webBrowser1.Navigating += new System.Windows.Forms.WebBrowserNavigatingEventHandler(this.webBrowser1_Navigating);
            // 
            // detailsRichTextBox
            // 
            this.detailsRichTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.detailsRichTextBox.Location = new System.Drawing.Point(0, 21);
            this.detailsRichTextBox.Name = "detailsRichTextBox";
            this.detailsRichTextBox.Size = new System.Drawing.Size(480, 266);
            this.detailsRichTextBox.TabIndex = 0;
            this.detailsRichTextBox.Text = "";
            this.detailsRichTextBox.WordWrap = false;
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.panel1.Controls.Add(this.flowLayoutPanel1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(480, 21);
            this.panel1.TabIndex = 1;
            // 
            // panel3
            // 
            this.panel3.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.panel3.Controls.Add(this.flowLayoutPanel2);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel3.Location = new System.Drawing.Point(0, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(480, 30);
            this.panel3.TabIndex = 1;
            // 
            // flowLayoutPanel2
            // 
            this.flowLayoutPanel2.Controls.Add(this.HlpViewerBtn);
            this.flowLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.flowLayoutPanel2.Location = new System.Drawing.Point(0, 0);
            this.flowLayoutPanel2.Name = "flowLayoutPanel2";
            this.flowLayoutPanel2.Size = new System.Drawing.Size(480, 30);
            this.flowLayoutPanel2.TabIndex = 9;
            // 
            // HlpViewerBtn
            // 
            this.HlpViewerBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.HlpViewerBtn.Image = global::HV2ApiExplore.Properties.Resources.eye_16;
            this.HlpViewerBtn.Location = new System.Drawing.Point(3, 3);
            this.HlpViewerBtn.Name = "HlpViewerBtn";
            this.HlpViewerBtn.Size = new System.Drawing.Size(34, 24);
            this.HlpViewerBtn.TabIndex = 7;
            this.HlpViewerBtn.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.HlpViewerBtn.UseVisualStyleBackColor = true;
            this.HlpViewerBtn.Click += new System.EventHandler(this.HlpViewerBtn_Click);
            // 
            // linkUnrendered
            // 
            this.linkUnrendered.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkUnrendered.AutoSize = true;
            this.linkUnrendered.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkUnrendered.Location = new System.Drawing.Point(122, 3);
            this.linkUnrendered.Margin = new System.Windows.Forms.Padding(3, 3, 0, 0);
            this.linkUnrendered.Name = "linkUnrendered";
            this.linkUnrendered.Size = new System.Drawing.Size(135, 13);
            this.linkUnrendered.TabIndex = 6;
            this.linkUnrendered.TabStop = true;
            this.linkUnrendered.Text = "View Unrendered Source";
            this.linkUnrendered.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkUnrendered.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkUnrendered_LinkClicked);
            // 
            // tipUnrendered
            // 
            this.tipUnrendered.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.tipUnrendered.Location = new System.Drawing.Point(257, 2);
            this.tipUnrendered.Margin = new System.Windows.Forms.Padding(0, 2, 0, 3);
            this.tipUnrendered.Name = "tipUnrendered";
            this.tipUnrendered.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
            this.tipUnrendered.Size = new System.Drawing.Size(13, 15);
            this.tipUnrendered.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.tipUnrendered.TabIndex = 27;
            this.tipUnrendered.TabStop = false;
            // 
            // linkBackToDetails
            // 
            this.linkBackToDetails.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkBackToDetails.AutoSize = true;
            this.linkBackToDetails.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkBackToDetails.Location = new System.Drawing.Point(283, 3);
            this.linkBackToDetails.Margin = new System.Windows.Forms.Padding(0, 3, 3, 0);
            this.linkBackToDetails.Name = "linkBackToDetails";
            this.linkBackToDetails.Size = new System.Drawing.Size(83, 13);
            this.linkBackToDetails.TabIndex = 28;
            this.linkBackToDetails.TabStop = true;
            this.linkBackToDetails.Text = "Back to Details";
            this.linkBackToDetails.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkBackToDetails.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkBackToDetails_LinkClicked);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 3);
            this.label1.Margin = new System.Windows.Forms.Padding(3, 3, 44, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 13);
            this.label1.TabIndex = 29;
            this.label1.Text = "Topic Details";
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.label1);
            this.flowLayoutPanel1.Controls.Add(this.linkUnrendered);
            this.flowLayoutPanel1.Controls.Add(this.tipUnrendered);
            this.flowLayoutPanel1.Controls.Add(this.label2);
            this.flowLayoutPanel1.Controls.Add(this.linkBackToDetails);
            this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(480, 21);
            this.flowLayoutPanel1.TabIndex = 30;
            this.flowLayoutPanel1.WrapContents = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(273, 2);
            this.label2.Margin = new System.Windows.Forms.Padding(3, 2, 0, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(10, 13);
            this.label2.TabIndex = 30;
            this.label2.Text = "|";
            // 
            // Panel_HelpTopicResults
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.splitContainer1);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Panel_HelpTopicResults";
            this.Size = new System.Drawing.Size(904, 656);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitSource.Panel1.ResumeLayout(false);
            this.splitSource.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitSource)).EndInit();
            this.splitSource.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.flowLayoutPanel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.tipUnrendered)).EndInit();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListView searchResultLV;
        private System.Windows.Forms.ColumnHeader columnHeader_Rank;
        private System.Windows.Forms.ColumnHeader columnHeader_Title;
        private System.Windows.Forms.ColumnHeader columnHeader_DisplayVersion;
        private System.Windows.Forms.ColumnHeader columnHeader_Vendor;
        private System.Windows.Forms.ColumnHeader columnHeader_Locale;
        private System.Windows.Forms.WebBrowser webBrowser1;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Button HlpViewerBtn;
        private System.Windows.Forms.ColumnHeader columnHeader_TopicID;
        private System.Windows.Forms.SplitContainer splitSource;
        private System.Windows.Forms.RichTextBox detailsRichTextBox;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
        private System.Windows.Forms.LinkLabel linkUnrendered;
        private System.Windows.Forms.PictureBox tipUnrendered;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.LinkLabel linkBackToDetails;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.Label label2;
    }
}
