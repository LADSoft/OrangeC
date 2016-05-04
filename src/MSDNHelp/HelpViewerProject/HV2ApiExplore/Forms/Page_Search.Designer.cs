namespace HV2ApiExplore.Forms
{
    partial class Page_Search
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Page_Search));
            this.panel1 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.helpLinkLabel = new System.Windows.Forms.LinkLabel();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.SearchCbo = new System.Windows.Forms.ComboBox();
            this.SearchBtn = new System.Windows.Forms.Button();
            this.OrSearchOverrideCbx = new System.Windows.Forms.CheckBox();
            this.SearchTermHighlightCbx = new System.Windows.Forms.CheckBox();
            this.pageSizeEdit = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.panelControls = new System.Windows.Forms.Panel();
            this.label1 = new System.Windows.Forms.Label();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.firstLink = new System.Windows.Forms.LinkLabel();
            this.prevLink = new System.Windows.Forms.LinkLabel();
            this.nextLink = new System.Windows.Forms.LinkLabel();
            this.lastLink = new System.Windows.Forms.LinkLabel();
            this.spacerLabel = new System.Windows.Forms.Label();
            this.statusLabel = new System.Windows.Forms.Label();
            this.flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
            this.tipResultsPerPage = new System.Windows.Forms.PictureBox();
            this.tipMatchHighlighting = new System.Windows.Forms.PictureBox();
            this.tipOrQueryTerms = new System.Windows.Forms.PictureBox();
            this.label2 = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pageSizeEdit)).BeginInit();
            this.panelControls.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            this.flowLayoutPanel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tipResultsPerPage)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.tipMatchHighlighting)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.tipOrQueryTerms)).BeginInit();
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
            this.panel1.Size = new System.Drawing.Size(784, 67);
            this.panel1.TabIndex = 15;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.ForeColor = System.Drawing.Color.White;
            this.label7.Location = new System.Drawing.Point(26, 29);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(275, 13);
            this.label7.TabIndex = 5;
            this.label7.Text = "- Demonstrates call ICatalogRead.GetSearchResults()";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(678, 24);
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
            this.helpLinkLabel.Location = new System.Drawing.Point(697, 24);
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
            this.label4.Size = new System.Drawing.Size(160, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "- Perform basic catalog search";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Segoe UI", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(9, 5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 20);
            this.label3.TabIndex = 0;
            this.label3.Text = "Search";
            // 
            // SearchCbo
            // 
            this.SearchCbo.DropDownHeight = 1;
            this.SearchCbo.FormattingEnabled = true;
            this.SearchCbo.IntegralHeight = false;
            this.SearchCbo.Location = new System.Drawing.Point(99, 26);
            this.SearchCbo.Name = "SearchCbo";
            this.SearchCbo.Size = new System.Drawing.Size(372, 21);
            this.SearchCbo.TabIndex = 16;
            this.SearchCbo.DropDown += new System.EventHandler(this.SearchCbo_DropDown);
            this.SearchCbo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.SearchCbo_KeyDown);
            // 
            // SearchBtn
            // 
            this.SearchBtn.Location = new System.Drawing.Point(477, 26);
            this.SearchBtn.Name = "SearchBtn";
            this.SearchBtn.Size = new System.Drawing.Size(70, 24);
            this.SearchBtn.TabIndex = 18;
            this.SearchBtn.Text = "Search";
            this.SearchBtn.UseVisualStyleBackColor = true;
            this.SearchBtn.Click += new System.EventHandler(this.SearchBtn_Click);
            // 
            // OrSearchOverrideCbx
            // 
            this.OrSearchOverrideCbx.AutoSize = true;
            this.OrSearchOverrideCbx.Location = new System.Drawing.Point(387, 5);
            this.OrSearchOverrideCbx.Margin = new System.Windows.Forms.Padding(3, 5, 0, 3);
            this.OrSearchOverrideCbx.Name = "OrSearchOverrideCbx";
            this.OrSearchOverrideCbx.Size = new System.Drawing.Size(105, 17);
            this.OrSearchOverrideCbx.TabIndex = 21;
            this.OrSearchOverrideCbx.Text = "OR query terms";
            this.OrSearchOverrideCbx.UseVisualStyleBackColor = true;
            // 
            // SearchTermHighlightCbx
            // 
            this.SearchTermHighlightCbx.AutoSize = true;
            this.SearchTermHighlightCbx.Location = new System.Drawing.Point(229, 5);
            this.SearchTermHighlightCbx.Margin = new System.Windows.Forms.Padding(3, 5, 0, 3);
            this.SearchTermHighlightCbx.Name = "SearchTermHighlightCbx";
            this.SearchTermHighlightCbx.Size = new System.Drawing.Size(126, 17);
            this.SearchTermHighlightCbx.TabIndex = 22;
            this.SearchTermHighlightCbx.Text = "Match highlighting";
            this.SearchTermHighlightCbx.UseVisualStyleBackColor = true;
            // 
            // pageSizeEdit
            // 
            this.pageSizeEdit.Location = new System.Drawing.Point(135, 3);
            this.pageSizeEdit.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.pageSizeEdit.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.pageSizeEdit.Name = "pageSizeEdit";
            this.pageSizeEdit.Size = new System.Drawing.Size(59, 22);
            this.pageSizeEdit.TabIndex = 23;
            this.pageSizeEdit.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(33, 5);
            this.label5.Margin = new System.Windows.Forms.Padding(33, 5, 3, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(96, 13);
            this.label5.TabIndex = 24;
            this.label5.Text = "Results per page:";
            // 
            // panelControls
            // 
            this.panelControls.Controls.Add(this.label2);
            this.panelControls.Controls.Add(this.flowLayoutPanel2);
            this.panelControls.Controls.Add(this.label1);
            this.panelControls.Controls.Add(this.flowLayoutPanel1);
            this.panelControls.Controls.Add(this.SearchCbo);
            this.panelControls.Controls.Add(this.SearchBtn);
            this.panelControls.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelControls.Location = new System.Drawing.Point(0, 67);
            this.panelControls.Name = "panelControls";
            this.panelControls.Size = new System.Drawing.Size(784, 115);
            this.panelControls.TabIndex = 25;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(-7, 118);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(0, 13);
            this.label1.TabIndex = 26;
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.firstLink);
            this.flowLayoutPanel1.Controls.Add(this.prevLink);
            this.flowLayoutPanel1.Controls.Add(this.nextLink);
            this.flowLayoutPanel1.Controls.Add(this.lastLink);
            this.flowLayoutPanel1.Controls.Add(this.spacerLabel);
            this.flowLayoutPanel1.Controls.Add(this.statusLabel);
            this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.flowLayoutPanel1.Location = new System.Drawing.Point(0, 93);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(784, 22);
            this.flowLayoutPanel1.TabIndex = 25;
            this.flowLayoutPanel1.WrapContents = false;
            // 
            // firstLink
            // 
            this.firstLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.firstLink.AutoSize = true;
            this.firstLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.firstLink.Location = new System.Drawing.Point(3, 0);
            this.firstLink.Name = "firstLink";
            this.firstLink.Size = new System.Drawing.Size(29, 13);
            this.firstLink.TabIndex = 2;
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
            this.prevLink.Location = new System.Drawing.Point(38, 0);
            this.prevLink.Name = "prevLink";
            this.prevLink.Size = new System.Drawing.Size(61, 13);
            this.prevLink.TabIndex = 0;
            this.prevLink.TabStop = true;
            this.prevLink.Text = "< Previous";
            this.prevLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.prevLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.prevLink_LinkClicked);
            // 
            // nextLink
            // 
            this.nextLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.nextLink.AutoSize = true;
            this.nextLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.nextLink.Location = new System.Drawing.Point(105, 0);
            this.nextLink.Name = "nextLink";
            this.nextLink.Size = new System.Drawing.Size(41, 13);
            this.nextLink.TabIndex = 1;
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
            this.lastLink.Location = new System.Drawing.Point(152, 0);
            this.lastLink.Name = "lastLink";
            this.lastLink.Size = new System.Drawing.Size(27, 13);
            this.lastLink.TabIndex = 3;
            this.lastLink.TabStop = true;
            this.lastLink.Text = "Last";
            this.lastLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.lastLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lastLink_LinkClicked);
            // 
            // spacerLabel
            // 
            this.spacerLabel.AutoSize = true;
            this.spacerLabel.Location = new System.Drawing.Point(185, 0);
            this.spacerLabel.Name = "spacerLabel";
            this.spacerLabel.Size = new System.Drawing.Size(19, 13);
            this.spacerLabel.TabIndex = 4;
            this.spacerLabel.Text = "    ";
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(210, 0);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(65, 13);
            this.statusLabel.TabIndex = 5;
            this.statusLabel.Text = "statusLabel";
            // 
            // flowLayoutPanel2
            // 
            this.flowLayoutPanel2.Controls.Add(this.label5);
            this.flowLayoutPanel2.Controls.Add(this.pageSizeEdit);
            this.flowLayoutPanel2.Controls.Add(this.tipResultsPerPage);
            this.flowLayoutPanel2.Controls.Add(this.SearchTermHighlightCbx);
            this.flowLayoutPanel2.Controls.Add(this.tipMatchHighlighting);
            this.flowLayoutPanel2.Controls.Add(this.OrSearchOverrideCbx);
            this.flowLayoutPanel2.Controls.Add(this.tipOrQueryTerms);
            this.flowLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.flowLayoutPanel2.Location = new System.Drawing.Point(0, 53);
            this.flowLayoutPanel2.Name = "flowLayoutPanel2";
            this.flowLayoutPanel2.Size = new System.Drawing.Size(784, 40);
            this.flowLayoutPanel2.TabIndex = 27;
            // 
            // tipResultsPerPage
            // 
            this.tipResultsPerPage.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.tipResultsPerPage.Location = new System.Drawing.Point(197, 5);
            this.tipResultsPerPage.Margin = new System.Windows.Forms.Padding(0, 5, 16, 3);
            this.tipResultsPerPage.Name = "tipResultsPerPage";
            this.tipResultsPerPage.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
            this.tipResultsPerPage.Size = new System.Drawing.Size(13, 15);
            this.tipResultsPerPage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.tipResultsPerPage.TabIndex = 25;
            this.tipResultsPerPage.TabStop = false;
            // 
            // tipMatchHighlighting
            // 
            this.tipMatchHighlighting.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.tipMatchHighlighting.Location = new System.Drawing.Point(355, 5);
            this.tipMatchHighlighting.Margin = new System.Windows.Forms.Padding(0, 5, 16, 3);
            this.tipMatchHighlighting.Name = "tipMatchHighlighting";
            this.tipMatchHighlighting.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
            this.tipMatchHighlighting.Size = new System.Drawing.Size(13, 15);
            this.tipMatchHighlighting.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.tipMatchHighlighting.TabIndex = 26;
            this.tipMatchHighlighting.TabStop = false;
            // 
            // tipOrQueryTerms
            // 
            this.tipOrQueryTerms.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.tipOrQueryTerms.Location = new System.Drawing.Point(492, 5);
            this.tipOrQueryTerms.Margin = new System.Windows.Forms.Padding(0, 5, 16, 3);
            this.tipOrQueryTerms.Name = "tipOrQueryTerms";
            this.tipOrQueryTerms.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
            this.tipOrQueryTerms.Size = new System.Drawing.Size(13, 15);
            this.tipOrQueryTerms.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.tipOrQueryTerms.TabIndex = 27;
            this.tipOrQueryTerms.TabStop = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 29);
            this.label2.Margin = new System.Windows.Forms.Padding(3, 5, 3, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(78, 13);
            this.label2.TabIndex = 28;
            this.label2.Text = "Search Query:";
            // 
            // Page_Search
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.panelControls);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Page_Search";
            this.Size = new System.Drawing.Size(784, 627);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pageSizeEdit)).EndInit();
            this.panelControls.ResumeLayout(false);
            this.panelControls.PerformLayout();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.flowLayoutPanel2.ResumeLayout(false);
            this.flowLayoutPanel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tipResultsPerPage)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.tipMatchHighlighting)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.tipOrQueryTerms)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.LinkLabel helpLinkLabel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox SearchCbo;
        private System.Windows.Forms.Button SearchBtn;
        private System.Windows.Forms.CheckBox OrSearchOverrideCbx;
        private System.Windows.Forms.CheckBox SearchTermHighlightCbx;
        private System.Windows.Forms.NumericUpDown pageSizeEdit;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Panel panelControls;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.LinkLabel firstLink;
        private System.Windows.Forms.LinkLabel prevLink;
        private System.Windows.Forms.LinkLabel nextLink;
        private System.Windows.Forms.LinkLabel lastLink;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label spacerLabel;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
        private System.Windows.Forms.PictureBox tipResultsPerPage;
        private System.Windows.Forms.PictureBox tipMatchHighlighting;
        private System.Windows.Forms.PictureBox tipOrQueryTerms;
        private System.Windows.Forms.Label label2;
    }
}
