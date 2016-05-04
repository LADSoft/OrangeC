namespace HV2ApiExplore.Forms
{
    partial class Form_H2Main
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_H2Main));
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPageApiTests = new System.Windows.Forms.TabPage();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.navListBox = new System.Windows.Forms.ListBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.checkBoxRenderWithVS = new System.Windows.Forms.CheckBox();
            this.imageRenderWithVS = new System.Windows.Forms.PictureBox();
            this.labelOptions = new System.Windows.Forms.Label();
            this.tabPagContentStore = new System.Windows.Forms.TabPage();
            this.tabPageMshx = new System.Windows.Forms.TabPage();
            this.tabPageEventLog = new System.Windows.Forms.TabPage();
            this.tabPageAbout = new System.Windows.Forms.TabPage();
            this.infoRichTextBox = new System.Windows.Forms.RichTextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.mnuDebugWindow = new System.Windows.Forms.ToolStripButton();
            this.RunVSHlpViewerElevatedBtn = new System.Windows.Forms.ToolStripSplitButton();
            this.mnuOnlineHelp = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabelVersion = new System.Windows.Forms.ToolStripLabel();
            this.tabControl1.SuspendLayout();
            this.tabPageApiTests.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.imageRenderWithVS)).BeginInit();
            this.tabPageAbout.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPageApiTests);
            this.tabControl1.Controls.Add(this.tabPagContentStore);
            this.tabControl1.Controls.Add(this.tabPageMshx);
            this.tabControl1.Controls.Add(this.tabPageEventLog);
            this.tabControl1.Controls.Add(this.tabPageAbout);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 25);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(839, 520);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPageApiTests
            // 
            this.tabPageApiTests.BackColor = System.Drawing.Color.White;
            this.tabPageApiTests.Controls.Add(this.splitContainer1);
            this.tabPageApiTests.Location = new System.Drawing.Point(4, 22);
            this.tabPageApiTests.Name = "tabPageApiTests";
            this.tabPageApiTests.Padding = new System.Windows.Forms.Padding(8);
            this.tabPageApiTests.Size = new System.Drawing.Size(831, 494);
            this.tabPageApiTests.TabIndex = 1;
            this.tabPageApiTests.Text = "Help Runtime API";
            // 
            // splitContainer1
            // 
            this.splitContainer1.BackColor = System.Drawing.Color.White;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(8, 8);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.Color.White;
            this.splitContainer1.Panel1.Controls.Add(this.navListBox);
            this.splitContainer1.Panel1.Controls.Add(this.panel1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.BackColor = System.Drawing.Color.White;
            this.splitContainer1.Size = new System.Drawing.Size(815, 478);
            this.splitContainer1.SplitterDistance = 147;
            this.splitContainer1.SplitterWidth = 12;
            this.splitContainer1.TabIndex = 0;
            // 
            // navListBox
            // 
            this.navListBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.navListBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.navListBox.Font = new System.Drawing.Font("Segoe UI", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.navListBox.FormattingEnabled = true;
            this.navListBox.IntegralHeight = false;
            this.navListBox.ItemHeight = 17;
            this.navListBox.Items.AddRange(new object[] {
            "Test 1",
            "Test 2"});
            this.navListBox.Location = new System.Drawing.Point(0, 0);
            this.navListBox.Name = "navListBox";
            this.navListBox.Size = new System.Drawing.Size(147, 318);
            this.navListBox.TabIndex = 0;
            this.navListBox.SelectedIndexChanged += new System.EventHandler(this.navListBox_SelectedIndexChanged);
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.panel1.Controls.Add(this.checkBoxRenderWithVS);
            this.panel1.Controls.Add(this.imageRenderWithVS);
            this.panel1.Controls.Add(this.labelOptions);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 318);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(147, 160);
            this.panel1.TabIndex = 1;
            // 
            // checkBoxRenderWithVS
            // 
            this.checkBoxRenderWithVS.AutoSize = true;
            this.checkBoxRenderWithVS.Location = new System.Drawing.Point(9, 30);
            this.checkBoxRenderWithVS.Margin = new System.Windows.Forms.Padding(9, 8, 0, 3);
            this.checkBoxRenderWithVS.Name = "checkBoxRenderWithVS";
            this.checkBoxRenderWithVS.Size = new System.Drawing.Size(105, 17);
            this.checkBoxRenderWithVS.TabIndex = 10;
            this.checkBoxRenderWithVS.Text = "Render with VS";
            this.checkBoxRenderWithVS.UseVisualStyleBackColor = true;
            this.checkBoxRenderWithVS.CheckedChanged += new System.EventHandler(this.checkBoxRenderWithVS_CheckedChanged);
            // 
            // imageRenderWithVS
            // 
            this.imageRenderWithVS.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.imageRenderWithVS.Location = new System.Drawing.Point(117, 30);
            this.imageRenderWithVS.Margin = new System.Windows.Forms.Padding(0, 8, 3, 3);
            this.imageRenderWithVS.Name = "imageRenderWithVS";
            this.imageRenderWithVS.Padding = new System.Windows.Forms.Padding(0, 2, 0, 0);
            this.imageRenderWithVS.Size = new System.Drawing.Size(13, 15);
            this.imageRenderWithVS.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.imageRenderWithVS.TabIndex = 11;
            this.imageRenderWithVS.TabStop = false;
            // 
            // labelOptions
            // 
            this.labelOptions.AutoSize = true;
            this.labelOptions.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelOptions.Location = new System.Drawing.Point(5, 9);
            this.labelOptions.Name = "labelOptions";
            this.labelOptions.Size = new System.Drawing.Size(48, 13);
            this.labelOptions.TabIndex = 0;
            this.labelOptions.Text = "Options";
            // 
            // tabPagContentStore
            // 
            this.tabPagContentStore.Location = new System.Drawing.Point(4, 22);
            this.tabPagContentStore.Name = "tabPagContentStore";
            this.tabPagContentStore.Padding = new System.Windows.Forms.Padding(8);
            this.tabPagContentStore.Size = new System.Drawing.Size(831, 494);
            this.tabPagContentStore.TabIndex = 2;
            this.tabPagContentStore.Text = "Content Store";
            this.tabPagContentStore.UseVisualStyleBackColor = true;
            // 
            // tabPageMshx
            // 
            this.tabPageMshx.Location = new System.Drawing.Point(4, 22);
            this.tabPageMshx.Name = "tabPageMshx";
            this.tabPageMshx.Padding = new System.Windows.Forms.Padding(8);
            this.tabPageMshx.Size = new System.Drawing.Size(831, 494);
            this.tabPageMshx.TabIndex = 4;
            this.tabPageMshx.Text = "HelpIndexer.exe";
            this.tabPageMshx.UseVisualStyleBackColor = true;
            // 
            // tabPageEventLog
            // 
            this.tabPageEventLog.Location = new System.Drawing.Point(4, 22);
            this.tabPageEventLog.Name = "tabPageEventLog";
            this.tabPageEventLog.Padding = new System.Windows.Forms.Padding(8);
            this.tabPageEventLog.Size = new System.Drawing.Size(831, 494);
            this.tabPageEventLog.TabIndex = 3;
            this.tabPageEventLog.Text = "Event Log";
            this.tabPageEventLog.UseVisualStyleBackColor = true;
            // 
            // tabPageAbout
            // 
            this.tabPageAbout.BackColor = System.Drawing.Color.White;
            this.tabPageAbout.Controls.Add(this.infoRichTextBox);
            this.tabPageAbout.Controls.Add(this.label1);
            this.tabPageAbout.Location = new System.Drawing.Point(4, 22);
            this.tabPageAbout.Name = "tabPageAbout";
            this.tabPageAbout.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageAbout.Size = new System.Drawing.Size(831, 494);
            this.tabPageAbout.TabIndex = 0;
            this.tabPageAbout.Text = "About";
            // 
            // infoRichTextBox
            // 
            this.infoRichTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.infoRichTextBox.BackColor = System.Drawing.Color.White;
            this.infoRichTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.infoRichTextBox.Cursor = System.Windows.Forms.Cursors.Default;
            this.infoRichTextBox.Location = new System.Drawing.Point(35, 59);
            this.infoRichTextBox.Name = "infoRichTextBox";
            this.infoRichTextBox.ReadOnly = true;
            this.infoRichTextBox.Size = new System.Drawing.Size(753, 392);
            this.infoRichTextBox.TabIndex = 1;
            this.infoRichTextBox.Text = "";
            this.infoRichTextBox.WordWrap = false;
            this.infoRichTextBox.LinkClicked += new System.Windows.Forms.LinkClickedEventHandler(this.infoRichTextBox_LinkClicked);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(30, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(207, 21);
            this.label1.TabIndex = 0;
            this.label1.Text = "Help Viewer 2.0 API Explorer";
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuDebugWindow,
            this.RunVSHlpViewerElevatedBtn,
            this.mnuOnlineHelp,
            this.toolStripSeparator2,
            this.toolStripLabelVersion});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(839, 25);
            this.toolStrip1.TabIndex = 1;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // mnuDebugWindow
            // 
            this.mnuDebugWindow.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mnuDebugWindow.Image = global::HV2ApiExplore.Properties.Resources.Document;
            this.mnuDebugWindow.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mnuDebugWindow.Name = "mnuDebugWindow";
            this.mnuDebugWindow.Size = new System.Drawing.Size(23, 22);
            this.mnuDebugWindow.Text = "Debug Window";
            this.mnuDebugWindow.ToolTipText = "Debug Window (Ctrl+D)";
            this.mnuDebugWindow.Click += new System.EventHandler(this.mnuDebugWindow_Click);
            // 
            // RunVSHlpViewerElevatedBtn
            // 
            this.RunVSHlpViewerElevatedBtn.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.RunVSHlpViewerElevatedBtn.Image = global::HV2ApiExplore.Properties.Resources.eye_16;
            this.RunVSHlpViewerElevatedBtn.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.RunVSHlpViewerElevatedBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.RunVSHlpViewerElevatedBtn.Name = "RunVSHlpViewerElevatedBtn";
            this.RunVSHlpViewerElevatedBtn.Size = new System.Drawing.Size(36, 22);
            this.RunVSHlpViewerElevatedBtn.Text = "Run VS 11 HelpViewer as Admin";
            this.RunVSHlpViewerElevatedBtn.ToolTipText = "Run VS 11 HelpViewer as Admin (required for installing non-signed content)";
            this.RunVSHlpViewerElevatedBtn.ButtonClick += new System.EventHandler(this.RunVSHlpViewerElevatedBtn_ButtonClick);
            this.RunVSHlpViewerElevatedBtn.DropDownOpening += new System.EventHandler(this.RunVSHlpViewerElevatedBtn_DropDownOpening);
            this.RunVSHlpViewerElevatedBtn.DropDownItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.RunVSHlpViewerElevatedBtn_DropDownItemClicked);
            // 
            // mnuOnlineHelp
            // 
            this.mnuOnlineHelp.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.mnuOnlineHelp.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mnuOnlineHelp.Image = global::HV2ApiExplore.Properties.Resources.help_16;
            this.mnuOnlineHelp.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mnuOnlineHelp.Name = "mnuOnlineHelp";
            this.mnuOnlineHelp.Size = new System.Drawing.Size(23, 22);
            this.mnuOnlineHelp.Text = "Online &Help";
            this.mnuOnlineHelp.ToolTipText = "Online Help (F1)";
            this.mnuOnlineHelp.Click += new System.EventHandler(this.mnuOnlineHelp_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripLabelVersion
            // 
            this.toolStripLabelVersion.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.toolStripLabelVersion.Name = "toolStripLabelVersion";
            this.toolStripLabelVersion.Size = new System.Drawing.Size(67, 22);
            this.toolStripLabelVersion.Text = "Version: ---";
            // 
            // Form_H2Main
            // 
            this.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.ClientSize = new System.Drawing.Size(839, 545);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.toolStrip1);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.KeyPreview = true;
            this.Name = "Form_H2Main";
            this.Text = "Help Viewer 2.0 API Explore";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.H2MainForm_FormClosing);
            this.Shown += new System.EventHandler(this.H2MainForm_Shown);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.H2MainForm_KeyDown);
            this.tabControl1.ResumeLayout(false);
            this.tabPageApiTests.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.imageRenderWithVS)).EndInit();
            this.tabPageAbout.ResumeLayout(false);
            this.tabPageAbout.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPageAbout;
        private System.Windows.Forms.TabPage tabPageApiTests;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ListBox navListBox;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton mnuDebugWindow;
        private System.Windows.Forms.ToolStripButton mnuOnlineHelp;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RichTextBox infoRichTextBox;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripLabel toolStripLabelVersion;
        private System.Windows.Forms.TabPage tabPagContentStore;
        private System.Windows.Forms.TabPage tabPageEventLog;
        private System.Windows.Forms.ToolStripSplitButton RunVSHlpViewerElevatedBtn;
        private System.Windows.Forms.TabPage tabPageMshx;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label labelOptions;
        private System.Windows.Forms.CheckBox checkBoxRenderWithVS;
        private System.Windows.Forms.PictureBox imageRenderWithVS;
        

    }
}

