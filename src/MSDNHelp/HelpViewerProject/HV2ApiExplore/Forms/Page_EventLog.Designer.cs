namespace HV2ApiExplore.Forms
{
    partial class Page_EventLog
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Page_EventLog));
            System.Windows.Forms.ListViewGroup listViewGroup1 = new System.Windows.Forms.ListViewGroup("ListViewGroup", System.Windows.Forms.HorizontalAlignment.Left);
            this.panel2 = new System.Windows.Forms.Panel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.helpLinkLabel = new System.Windows.Forms.LinkLabel();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.cbxIncludeHV1 = new System.Windows.Forms.CheckBox();
            this.rdoHelp = new System.Windows.Forms.RadioButton();
            this.label3 = new System.Windows.Forms.Label();
            this.rdoAll = new System.Windows.Forms.RadioButton();
            this.ShowLogTip = new System.Windows.Forms.PictureBox();
            this.EventLogTip = new System.Windows.Forms.PictureBox();
            this.EventLogLink = new System.Windows.Forms.LinkLabel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader5 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ShowLogTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.EventLogTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.Color.DarkBlue;
            this.panel2.Controls.Add(this.pictureBox1);
            this.panel2.Controls.Add(this.helpLinkLabel);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.label2);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(627, 71);
            this.panel2.TabIndex = 26;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(530, 24);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(16, 16);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox1.TabIndex = 6;
            this.pictureBox1.TabStop = false;
            // 
            // helpLinkLabel
            // 
            this.helpLinkLabel.ActiveLinkColor = System.Drawing.Color.Lavender;
            this.helpLinkLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.helpLinkLabel.AutoSize = true;
            this.helpLinkLabel.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.helpLinkLabel.LinkColor = System.Drawing.Color.Lavender;
            this.helpLinkLabel.Location = new System.Drawing.Point(549, 24);
            this.helpLinkLabel.Name = "helpLinkLabel";
            this.helpLinkLabel.Size = new System.Drawing.Size(69, 13);
            this.helpLinkLabel.TabIndex = 5;
            this.helpLinkLabel.TabStop = true;
            this.helpLinkLabel.Text = "Online Help";
            this.helpLinkLabel.VisitedLinkColor = System.Drawing.Color.Lavender;
            this.helpLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.helpLinkLabel_LinkClicked);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(21, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(78, 21);
            this.label1.TabIndex = 1;
            this.label1.Text = "Event Log";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point(22, 39);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(417, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "All errors reported by the help system go to the Windows Application Event Log";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.cbxIncludeHV1);
            this.panel1.Controls.Add(this.rdoHelp);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Controls.Add(this.rdoAll);
            this.panel1.Controls.Add(this.ShowLogTip);
            this.panel1.Controls.Add(this.EventLogTip);
            this.panel1.Controls.Add(this.EventLogLink);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 71);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(627, 69);
            this.panel1.TabIndex = 27;
            // 
            // cbxIncludeHV1
            // 
            this.cbxIncludeHV1.AutoSize = true;
            this.cbxIncludeHV1.Checked = true;
            this.cbxIncludeHV1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbxIncludeHV1.ForeColor = System.Drawing.Color.Blue;
            this.cbxIncludeHV1.Location = new System.Drawing.Point(142, 37);
            this.cbxIncludeHV1.Name = "cbxIncludeHV1";
            this.cbxIncludeHV1.Size = new System.Drawing.Size(88, 17);
            this.cbxIncludeHV1.TabIndex = 10;
            this.cbxIncludeHV1.Text = "Include HV1";
            this.cbxIncludeHV1.UseVisualStyleBackColor = true;
            this.cbxIncludeHV1.CheckedChanged += new System.EventHandler(this.cbxIncludeHV1_CheckedChanged);
            // 
            // rdoHelp
            // 
            this.rdoHelp.AutoSize = true;
            this.rdoHelp.Checked = true;
            this.rdoHelp.ForeColor = System.Drawing.Color.DarkViolet;
            this.rdoHelp.Location = new System.Drawing.Point(54, 36);
            this.rdoHelp.Name = "rdoHelp";
            this.rdoHelp.Size = new System.Drawing.Size(82, 17);
            this.rdoHelp.TabIndex = 9;
            this.rdoHelp.TabStop = true;
            this.rdoHelp.Text = "HV2 Events";
            this.rdoHelp.UseVisualStyleBackColor = true;
            this.rdoHelp.CheckedChanged += new System.EventHandler(this.rdoHelp_CheckedChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 15);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(39, 13);
            this.label3.TabIndex = 8;
            this.label3.Text = "Show:";
            // 
            // rdoAll
            // 
            this.rdoAll.AutoSize = true;
            this.rdoAll.Location = new System.Drawing.Point(54, 13);
            this.rdoAll.Name = "rdoAll";
            this.rdoAll.Size = new System.Drawing.Size(74, 17);
            this.rdoAll.TabIndex = 7;
            this.rdoAll.Text = "All Events";
            this.rdoAll.UseVisualStyleBackColor = true;
            this.rdoAll.CheckedChanged += new System.EventHandler(this.rdoAll_CheckedChanged);
            // 
            // ShowLogTip
            // 
            this.ShowLogTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.ShowLogTip.Location = new System.Drawing.Point(27, 38);
            this.ShowLogTip.Name = "ShowLogTip";
            this.ShowLogTip.Size = new System.Drawing.Size(13, 13);
            this.ShowLogTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.ShowLogTip.TabIndex = 6;
            this.ShowLogTip.TabStop = false;
            // 
            // EventLogTip
            // 
            this.EventLogTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.EventLogTip.Location = new System.Drawing.Point(306, 16);
            this.EventLogTip.Name = "EventLogTip";
            this.EventLogTip.Size = new System.Drawing.Size(13, 13);
            this.EventLogTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.EventLogTip.TabIndex = 6;
            this.EventLogTip.TabStop = false;
            // 
            // EventLogLink
            // 
            this.EventLogLink.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.EventLogLink.AutoSize = true;
            this.EventLogLink.ForeColor = System.Drawing.Color.RoyalBlue;
            this.EventLogLink.LinkColor = System.Drawing.Color.RoyalBlue;
            this.EventLogLink.Location = new System.Drawing.Point(322, 15);
            this.EventLogLink.Name = "EventLogLink";
            this.EventLogLink.Size = new System.Drawing.Size(136, 13);
            this.EventLogLink.TabIndex = 0;
            this.EventLogLink.TabStop = true;
            this.EventLogLink.Text = "Show Event Log Window";
            this.EventLogLink.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.EventLogLink.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.EventLogLink_LinkClicked);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer1.Location = new System.Drawing.Point(0, 140);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.listView1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.richTextBox1);
            this.splitContainer1.Size = new System.Drawing.Size(627, 469);
            this.splitContainer1.SplitterDistance = 302;
            this.splitContainer1.TabIndex = 28;
            // 
            // listView1
            // 
            this.listView1.AllowColumnReorder = true;
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader5,
            this.columnHeader3,
            this.columnHeader2,
            this.columnHeader1,
            this.columnHeader4});
            this.listView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView1.FullRowSelect = true;
            listViewGroup1.Header = "ListViewGroup";
            listViewGroup1.Name = "listViewGroup1";
            this.listView1.Groups.AddRange(new System.Windows.Forms.ListViewGroup[] {
            listViewGroup1});
            this.listView1.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listView1.HideSelection = false;
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(627, 302);
            this.listView1.SmallImageList = this.imageList1;
            this.listView1.TabIndex = 0;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            this.listView1.VirtualMode = true;
            this.listView1.RetrieveVirtualItem += new System.Windows.Forms.RetrieveVirtualItemEventHandler(this.listView1_RetrieveVirtualItem);
            this.listView1.SelectedIndexChanged += new System.EventHandler(this.listView1_SelectedIndexChanged);
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "Type";
            this.columnHeader5.Width = 109;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Date";
            this.columnHeader3.Width = 144;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Source";
            this.columnHeader2.Width = 155;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Record #";
            this.columnHeader1.Width = 66;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Message";
            this.columnHeader4.Width = 999;
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "errorLog_Info.png");
            this.imageList1.Images.SetKeyName(1, "errorLog_Warn.png");
            this.imageList1.Images.SetKeyName(2, "errorLog_Error.png");
            this.imageList1.Images.SetKeyName(3, "errorLog_Key.png");
            // 
            // richTextBox1
            // 
            this.richTextBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.richTextBox1.DetectUrls = false;
            this.richTextBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.richTextBox1.Location = new System.Drawing.Point(0, 0);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(627, 163);
            this.richTextBox1.TabIndex = 0;
            this.richTextBox1.Text = "";
            // 
            // Page_EventLog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.panel2);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Page_EventLog";
            this.Size = new System.Drawing.Size(627, 609);
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ShowLogTip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.EventLogTip)).EndInit();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.LinkLabel EventLogLink;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.LinkLabel helpLinkLabel;
        private System.Windows.Forms.PictureBox EventLogTip;
        private System.Windows.Forms.RadioButton rdoHelp;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.RadioButton rdoAll;
        private System.Windows.Forms.CheckBox cbxIncludeHV1;
        private System.Windows.Forms.PictureBox ShowLogTip;
    }
}
