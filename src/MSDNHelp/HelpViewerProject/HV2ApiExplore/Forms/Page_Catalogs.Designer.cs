namespace HV2ApiExplore.Forms
{
    partial class Page_Catalogs
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
            this.label2 = new System.Windows.Forms.Label();
            this.richTextBoxDetails = new System.Windows.Forms.RichTextBox();
            this.CloseCatalogBtn = new System.Windows.Forms.Button();
            this.OpenCatalogBtn = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.catListBox = new System.Windows.Forms.ListBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.helpLinkLabel = new System.Windows.Forms.LinkLabel();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.AddMshxFileBtn = new System.Windows.Forms.Button();
            this.ContentPathBtn = new System.Windows.Forms.Button();
            this.IsLangSupportAvailableBtn = new System.Windows.Forms.Button();
            this.ShowDebugLink = new System.Windows.Forms.LinkLabel();
            this.GetCatalogReadWriteLockBtn = new System.Windows.Forms.Button();
            this.AddCatalogFolderBtn = new System.Windows.Forms.Button();
            this.OpenCatalogBtnTip = new System.Windows.Forms.PictureBox();
            this.GetCatalogReadWriteLockBtnTip = new System.Windows.Forms.PictureBox();
            this.AvailableCatalogsTip = new System.Windows.Forms.PictureBox();
            this.AddBtnTip = new System.Windows.Forms.PictureBox();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.OpenCatalogBtnTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.GetCatalogReadWriteLockBtnTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.AvailableCatalogsTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.AddBtnTip)).BeginInit();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(0, 314);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(134, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Selected Catalog Details:";
            // 
            // richTextBoxDetails
            // 
            this.richTextBoxDetails.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.richTextBoxDetails.BackColor = System.Drawing.SystemColors.Window;
            this.richTextBoxDetails.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.richTextBoxDetails.Location = new System.Drawing.Point(3, 330);
            this.richTextBoxDetails.Name = "richTextBoxDetails";
            this.richTextBoxDetails.ReadOnly = true;
            this.richTextBoxDetails.Size = new System.Drawing.Size(798, 219);
            this.richTextBoxDetails.TabIndex = 12;
            this.richTextBoxDetails.Text = "";
            this.richTextBoxDetails.WordWrap = false;
            this.richTextBoxDetails.LinkClicked += new System.Windows.Forms.LinkClickedEventHandler(this.richTextBoxDetails_LinkClicked);
            // 
            // CloseCatalogBtn
            // 
            this.CloseCatalogBtn.Location = new System.Drawing.Point(255, 131);
            this.CloseCatalogBtn.Name = "CloseCatalogBtn";
            this.CloseCatalogBtn.Size = new System.Drawing.Size(134, 24);
            this.CloseCatalogBtn.TabIndex = 11;
            this.CloseCatalogBtn.Text = "Close";
            this.CloseCatalogBtn.UseVisualStyleBackColor = true;
            this.CloseCatalogBtn.Click += new System.EventHandler(this.CloseCatalogBtn_Click);
            // 
            // OpenCatalogBtn
            // 
            this.OpenCatalogBtn.Location = new System.Drawing.Point(255, 101);
            this.OpenCatalogBtn.Name = "OpenCatalogBtn";
            this.OpenCatalogBtn.Size = new System.Drawing.Size(134, 24);
            this.OpenCatalogBtn.TabIndex = 10;
            this.OpenCatalogBtn.Text = "Open";
            this.OpenCatalogBtn.UseVisualStyleBackColor = true;
            this.OpenCatalogBtn.Click += new System.EventHandler(this.OpenCatalogBtn_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(0, 84);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(104, 13);
            this.label1.TabIndex = 9;
            this.label1.Text = "Available Catalogs:";
            // 
            // catListBox
            // 
            this.catListBox.FormattingEnabled = true;
            this.catListBox.Location = new System.Drawing.Point(3, 100);
            this.catListBox.Name = "catListBox";
            this.catListBox.ScrollAlwaysVisible = true;
            this.catListBox.Size = new System.Drawing.Size(246, 199);
            this.catListBox.TabIndex = 8;
            this.catListBox.SelectedIndexChanged += new System.EventHandler(this.catListBox_SelectedIndexChanged);
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
            this.panel1.Size = new System.Drawing.Size(804, 67);
            this.panel1.TabIndex = 14;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.ForeColor = System.Drawing.Color.White;
            this.label7.Location = new System.Drawing.Point(26, 29);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(148, 13);
            this.label7.TabIndex = 5;
            this.label7.Text = "- Exercise ICatalog interface";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Image = global::HV2ApiExplore.Properties.Resources.help_16;
            this.pictureBox1.Location = new System.Drawing.Point(698, 24);
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
            this.helpLinkLabel.Location = new System.Drawing.Point(717, 24);
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
            this.label4.Size = new System.Drawing.Size(233, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "- Open a catalog to perform other help calls";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Segoe UI", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(9, 5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(67, 20);
            this.label3.TabIndex = 0;
            this.label3.Text = "Catalogs";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(445, 84);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(176, 13);
            this.label6.TabIndex = 15;
            this.label6.Text = "Other ICatalog:: Commands:";
            // 
            // AddMshxFileBtn
            // 
            this.AddMshxFileBtn.Location = new System.Drawing.Point(255, 266);
            this.AddMshxFileBtn.Name = "AddMshxFileBtn";
            this.AddMshxFileBtn.Size = new System.Drawing.Size(134, 24);
            this.AddMshxFileBtn.TabIndex = 16;
            this.AddMshxFileBtn.Text = "Add .MSHX File...";
            this.AddMshxFileBtn.UseVisualStyleBackColor = true;
            this.AddMshxFileBtn.Click += new System.EventHandler(this.AddMshxFileBtn_Click);
            // 
            // ContentPathBtn
            // 
            this.ContentPathBtn.Location = new System.Drawing.Point(448, 101);
            this.ContentPathBtn.Name = "ContentPathBtn";
            this.ContentPathBtn.Size = new System.Drawing.Size(202, 24);
            this.ContentPathBtn.TabIndex = 17;
            this.ContentPathBtn.Text = "ICatalog.ContentPath";
            this.ContentPathBtn.UseVisualStyleBackColor = true;
            this.ContentPathBtn.Click += new System.EventHandler(this.ContentPathBtn_Click);
            // 
            // IsLangSupportAvailableBtn
            // 
            this.IsLangSupportAvailableBtn.Location = new System.Drawing.Point(448, 131);
            this.IsLangSupportAvailableBtn.Name = "IsLangSupportAvailableBtn";
            this.IsLangSupportAvailableBtn.Size = new System.Drawing.Size(202, 24);
            this.IsLangSupportAvailableBtn.TabIndex = 18;
            this.IsLangSupportAvailableBtn.Text = "IsLanguageSupportAvailable()";
            this.IsLangSupportAvailableBtn.UseVisualStyleBackColor = true;
            this.IsLangSupportAvailableBtn.Click += new System.EventHandler(this.IsLangSupportAvailableBtn_Click);
            // 
            // ShowDebugLink
            // 
            this.ShowDebugLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.ShowDebugLink.AutoSize = true;
            this.ShowDebugLink.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.ShowDebugLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.ShowDebugLink.Location = new System.Drawing.Point(445, 189);
            this.ShowDebugLink.Name = "ShowDebugLink";
            this.ShowDebugLink.Size = new System.Drawing.Size(199, 13);
            this.ShowDebugLink.TabIndex = 19;
            this.ShowDebugLink.TabStop = true;
            this.ShowDebugLink.Text = "HV 2.0 Registry Info (Debug Window)";
            this.ShowDebugLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.ShowDebugLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.ShowDebugLink_LinkClicked);
            // 
            // GetCatalogReadWriteLockBtn
            // 
            this.GetCatalogReadWriteLockBtn.Location = new System.Drawing.Point(255, 183);
            this.GetCatalogReadWriteLockBtn.Name = "GetCatalogReadWriteLockBtn";
            this.GetCatalogReadWriteLockBtn.Size = new System.Drawing.Size(134, 24);
            this.GetCatalogReadWriteLockBtn.TabIndex = 20;
            this.GetCatalogReadWriteLockBtn.Text = "Show Locks";
            this.GetCatalogReadWriteLockBtn.UseVisualStyleBackColor = true;
            this.GetCatalogReadWriteLockBtn.Click += new System.EventHandler(this.GetCatalogReadWriteLockBtn_Click);
            // 
            // AddCatalogFolderBtn
            // 
            this.AddCatalogFolderBtn.Location = new System.Drawing.Point(255, 236);
            this.AddCatalogFolderBtn.Name = "AddCatalogFolderBtn";
            this.AddCatalogFolderBtn.Size = new System.Drawing.Size(134, 24);
            this.AddCatalogFolderBtn.TabIndex = 21;
            this.AddCatalogFolderBtn.Text = "Add Folder...";
            this.AddCatalogFolderBtn.UseVisualStyleBackColor = true;
            this.AddCatalogFolderBtn.Click += new System.EventHandler(this.AddCatalogFolderBtn_Click);
            // 
            // OpenCatalogBtnTip
            // 
            this.OpenCatalogBtnTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.OpenCatalogBtnTip.Location = new System.Drawing.Point(394, 107);
            this.OpenCatalogBtnTip.Name = "OpenCatalogBtnTip";
            this.OpenCatalogBtnTip.Size = new System.Drawing.Size(13, 13);
            this.OpenCatalogBtnTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.OpenCatalogBtnTip.TabIndex = 22;
            this.OpenCatalogBtnTip.TabStop = false;
            // 
            // GetCatalogReadWriteLockBtnTip
            // 
            this.GetCatalogReadWriteLockBtnTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.GetCatalogReadWriteLockBtnTip.Location = new System.Drawing.Point(394, 189);
            this.GetCatalogReadWriteLockBtnTip.Name = "GetCatalogReadWriteLockBtnTip";
            this.GetCatalogReadWriteLockBtnTip.Size = new System.Drawing.Size(13, 13);
            this.GetCatalogReadWriteLockBtnTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.GetCatalogReadWriteLockBtnTip.TabIndex = 24;
            this.GetCatalogReadWriteLockBtnTip.TabStop = false;
            // 
            // AvailableCatalogsTip
            // 
            this.AvailableCatalogsTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.AvailableCatalogsTip.Location = new System.Drawing.Point(110, 84);
            this.AvailableCatalogsTip.Name = "AvailableCatalogsTip";
            this.AvailableCatalogsTip.Size = new System.Drawing.Size(13, 13);
            this.AvailableCatalogsTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.AvailableCatalogsTip.TabIndex = 26;
            this.AvailableCatalogsTip.TabStop = false;
            // 
            // AddBtnTip
            // 
            this.AddBtnTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.AddBtnTip.Location = new System.Drawing.Point(396, 256);
            this.AddBtnTip.Name = "AddBtnTip";
            this.AddBtnTip.Size = new System.Drawing.Size(13, 13);
            this.AddBtnTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.AddBtnTip.TabIndex = 27;
            this.AddBtnTip.TabStop = false;
            // 
            // Page_Catalogs
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.AddBtnTip);
            this.Controls.Add(this.AvailableCatalogsTip);
            this.Controls.Add(this.GetCatalogReadWriteLockBtnTip);
            this.Controls.Add(this.OpenCatalogBtnTip);
            this.Controls.Add(this.AddCatalogFolderBtn);
            this.Controls.Add(this.GetCatalogReadWriteLockBtn);
            this.Controls.Add(this.ShowDebugLink);
            this.Controls.Add(this.IsLangSupportAvailableBtn);
            this.Controls.Add(this.ContentPathBtn);
            this.Controls.Add(this.AddMshxFileBtn);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.richTextBoxDetails);
            this.Controls.Add(this.CloseCatalogBtn);
            this.Controls.Add(this.OpenCatalogBtn);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.catListBox);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Page_Catalogs";
            this.Size = new System.Drawing.Size(804, 552);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.OpenCatalogBtnTip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.GetCatalogReadWriteLockBtnTip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.AvailableCatalogsTip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.AddBtnTip)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.RichTextBox richTextBoxDetails;
        private System.Windows.Forms.Button CloseCatalogBtn;
        private System.Windows.Forms.Button OpenCatalogBtn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListBox catListBox;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.LinkLabel helpLinkLabel;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button AddMshxFileBtn;
        private System.Windows.Forms.Button ContentPathBtn;
        private System.Windows.Forms.Button IsLangSupportAvailableBtn;
        private System.Windows.Forms.LinkLabel ShowDebugLink;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button GetCatalogReadWriteLockBtn;
        private System.Windows.Forms.Button AddCatalogFolderBtn;
        private System.Windows.Forms.PictureBox OpenCatalogBtnTip;
        private System.Windows.Forms.PictureBox GetCatalogReadWriteLockBtnTip;
        private System.Windows.Forms.PictureBox AvailableCatalogsTip;
        private System.Windows.Forms.PictureBox AddBtnTip;
    }
}
