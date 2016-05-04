namespace HV2ApiExplore.Forms
{
    partial class Page_TOC
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Page_TOC));
            this.panel1 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.helpLinkLabel = new System.Windows.Forms.LinkLabel();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.panelControls = new System.Windows.Forms.Panel();
            this.tocDetailsCboTip = new System.Windows.Forms.PictureBox();
            this.HlpViewerBtn = new System.Windows.Forms.Button();
            this.TopicIDPictureBox = new System.Windows.Forms.PictureBox();
            this.label2 = new System.Windows.Forms.Label();
            this.TopicIdCbo = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tocDetailsCbo = new System.Windows.Forms.ComboBox();
            this.GetTocBtn = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.panelControls.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tocDetailsCboTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.TopicIDPictureBox)).BeginInit();
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
            this.panel1.Size = new System.Drawing.Size(675, 67);
            this.panel1.TabIndex = 16;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.ForeColor = System.Drawing.Color.White;
            this.label7.Location = new System.Drawing.Point(26, 29);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(286, 13);
            this.label7.TabIndex = 5;
            this.label7.Text = "- Demonstrate call ICatalogRead.GetTableOfContents()";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(569, 24);
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
            this.helpLinkLabel.Location = new System.Drawing.Point(588, 24);
            this.helpLinkLabel.Name = "helpLinkLabel";
            this.helpLinkLabel.Size = new System.Drawing.Size(62, 13);
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
            this.label4.Size = new System.Drawing.Size(193, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "- Create a basic TOC (see dropdown)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Segoe UI", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(9, 5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(128, 20);
            this.label3.TabIndex = 0;
            this.label3.Text = "Table Of Contents";
            // 
            // panelControls
            // 
            this.panelControls.Controls.Add(this.tocDetailsCboTip);
            this.panelControls.Controls.Add(this.HlpViewerBtn);
            this.panelControls.Controls.Add(this.TopicIDPictureBox);
            this.panelControls.Controls.Add(this.label2);
            this.panelControls.Controls.Add(this.TopicIdCbo);
            this.panelControls.Controls.Add(this.label1);
            this.panelControls.Controls.Add(this.tocDetailsCbo);
            this.panelControls.Controls.Add(this.GetTocBtn);
            this.panelControls.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelControls.Location = new System.Drawing.Point(0, 67);
            this.panelControls.Name = "panelControls";
            this.panelControls.Size = new System.Drawing.Size(675, 96);
            this.panelControls.TabIndex = 17;
            // 
            // tocDetailsCboTip
            // 
            this.tocDetailsCboTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.tocDetailsCboTip.Location = new System.Drawing.Point(101, 55);
            this.tocDetailsCboTip.Name = "tocDetailsCboTip";
            this.tocDetailsCboTip.Size = new System.Drawing.Size(13, 13);
            this.tocDetailsCboTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.tocDetailsCboTip.TabIndex = 7;
            this.tocDetailsCboTip.TabStop = false;
            // 
            // HlpViewerBtn
            // 
            this.HlpViewerBtn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.HlpViewerBtn.Image = global::HV2ApiExplore.Properties.Resources.eye_16;
            this.HlpViewerBtn.Location = new System.Drawing.Point(574, 25);
            this.HlpViewerBtn.Name = "HlpViewerBtn";
            this.HlpViewerBtn.Size = new System.Drawing.Size(34, 24);
            this.HlpViewerBtn.TabIndex = 6;
            this.HlpViewerBtn.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.HlpViewerBtn.UseVisualStyleBackColor = true;
            this.HlpViewerBtn.Click += new System.EventHandler(this.HlpViewerBtn_Click);
            // 
            // TopicIDPictureBox
            // 
            this.TopicIDPictureBox.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.TopicIDPictureBox.Location = new System.Drawing.Point(101, 28);
            this.TopicIDPictureBox.Name = "TopicIDPictureBox";
            this.TopicIDPictureBox.Size = new System.Drawing.Size(13, 13);
            this.TopicIDPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.TopicIDPictureBox.TabIndex = 5;
            this.TopicIDPictureBox.TabStop = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(0, 28);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(51, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Topic ID:";
            // 
            // TopicIdCbo
            // 
            this.TopicIdCbo.DropDownHeight = 1;
            this.TopicIdCbo.FormattingEnabled = true;
            this.TopicIdCbo.IntegralHeight = false;
            this.TopicIdCbo.Location = new System.Drawing.Point(120, 25);
            this.TopicIdCbo.Name = "TopicIdCbo";
            this.TopicIdCbo.Size = new System.Drawing.Size(325, 21);
            this.TopicIdCbo.TabIndex = 3;
            this.TopicIdCbo.DropDown += new System.EventHandler(this.TopicIdCbo_DropDown);
            this.TopicIdCbo.TextChanged += new System.EventHandler(this.TopicIdCbo_TextChanged);
            this.TopicIdCbo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TopicIdCbo_KeyDown);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(0, 55);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(88, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "TocReturnDetail:";
            // 
            // tocDetailsCbo
            // 
            this.tocDetailsCbo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.tocDetailsCbo.FormattingEnabled = true;
            this.tocDetailsCbo.Location = new System.Drawing.Point(120, 52);
            this.tocDetailsCbo.Name = "tocDetailsCbo";
            this.tocDetailsCbo.Size = new System.Drawing.Size(173, 21);
            this.tocDetailsCbo.TabIndex = 1;
            // 
            // GetTocBtn
            // 
            this.GetTocBtn.Location = new System.Drawing.Point(451, 25);
            this.GetTocBtn.Name = "GetTocBtn";
            this.GetTocBtn.Size = new System.Drawing.Size(117, 24);
            this.GetTocBtn.TabIndex = 0;
            this.GetTocBtn.Text = "Get TOC Data";
            this.GetTocBtn.UseVisualStyleBackColor = true;
            this.GetTocBtn.Click += new System.EventHandler(this.GetTocBtn_Click);
            // 
            // Page_TOC
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.panelControls);
            this.Controls.Add(this.panel1);
            this.Name = "Page_TOC";
            this.Size = new System.Drawing.Size(675, 606);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.panelControls.ResumeLayout(false);
            this.panelControls.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tocDetailsCboTip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.TopicIDPictureBox)).EndInit();
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
        private System.Windows.Forms.ComboBox tocDetailsCbo;
        private System.Windows.Forms.Button GetTocBtn;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox TopicIdCbo;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox TopicIDPictureBox;
        private System.Windows.Forms.Button HlpViewerBtn;
        private System.Windows.Forms.PictureBox tocDetailsCboTip;
    }
}
