namespace HV2ApiExplore.Forms
{
    partial class Page_Store
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Page_Store));
            this.panel1 = new System.Windows.Forms.Panel();
            this.warningTip = new System.Windows.Forms.PictureBox();
            this.treeViewToolStrip = new System.Windows.Forms.ToolStrip();
            this.CopyBtn = new System.Windows.Forms.ToolStripButton();
            this.ExplorerBtn = new System.Windows.Forms.ToolStripButton();
            this.panel2 = new System.Windows.Forms.Panel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.helpLinkLabel = new System.Windows.Forms.LinkLabel();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.LoadButtonTip = new System.Windows.Forms.PictureBox();
            this.CatalogLocksTip = new System.Windows.Forms.PictureBox();
            this.monitorLabel = new System.Windows.Forms.Label();
            this.monitorCbx = new System.Windows.Forms.CheckBox();
            this.LoadButton = new System.Windows.Forms.Button();
            this.catalogPathCbo = new System.Windows.Forms.ComboBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.treeViewContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.mnuCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuExplorer = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.warningTip)).BeginInit();
            this.treeViewToolStrip.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.LoadButtonTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.CatalogLocksTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.treeViewContextMenuStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.warningTip);
            this.panel1.Controls.Add(this.treeViewToolStrip);
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Controls.Add(this.LoadButtonTip);
            this.panel1.Controls.Add(this.CatalogLocksTip);
            this.panel1.Controls.Add(this.monitorLabel);
            this.panel1.Controls.Add(this.monitorCbx);
            this.panel1.Controls.Add(this.LoadButton);
            this.panel1.Controls.Add(this.catalogPathCbo);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(647, 173);
            this.panel1.TabIndex = 0;
            // 
            // warningTip
            // 
            this.warningTip.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.warningTip.Image = global::HV2ApiExplore.Properties.Resources.warning16x16;
            this.warningTip.Location = new System.Drawing.Point(614, 93);
            this.warningTip.Name = "warningTip";
            this.warningTip.Size = new System.Drawing.Size(16, 16);
            this.warningTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.warningTip.TabIndex = 3;
            this.warningTip.TabStop = false;
            // 
            // treeViewToolStrip
            // 
            this.treeViewToolStrip.AutoSize = false;
            this.treeViewToolStrip.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.treeViewToolStrip.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.treeViewToolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.treeViewToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.CopyBtn,
            this.ExplorerBtn});
            this.treeViewToolStrip.Location = new System.Drawing.Point(0, 148);
            this.treeViewToolStrip.Name = "treeViewToolStrip";
            this.treeViewToolStrip.Size = new System.Drawing.Size(647, 25);
            this.treeViewToolStrip.TabIndex = 26;
            this.treeViewToolStrip.Text = "toolStrip1";
            // 
            // CopyBtn
            // 
            this.CopyBtn.Image = global::HV2ApiExplore.Properties.Resources.Copy;
            this.CopyBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.CopyBtn.Name = "CopyBtn";
            this.CopyBtn.Size = new System.Drawing.Size(55, 22);
            this.CopyBtn.Text = "Copy";
            this.CopyBtn.ToolTipText = "Copy node and sub-node text";
            this.CopyBtn.Click += new System.EventHandler(this.CopyBtn_Click);
            // 
            // ExplorerBtn
            // 
            this.ExplorerBtn.Image = global::HV2ApiExplore.Properties.Resources.Explorer;
            this.ExplorerBtn.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.ExplorerBtn.Name = "ExplorerBtn";
            this.ExplorerBtn.Size = new System.Drawing.Size(69, 22);
            this.ExplorerBtn.Text = "Explorer";
            this.ExplorerBtn.ToolTipText = "Show the XML file in Explorer";
            this.ExplorerBtn.Click += new System.EventHandler(this.ExplorerBtn_Click);
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.Color.DarkSlateGray;
            this.panel2.Controls.Add(this.pictureBox1);
            this.panel2.Controls.Add(this.helpLinkLabel);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.label2);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(647, 71);
            this.panel2.TabIndex = 25;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(542, 27);
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
            this.helpLinkLabel.Location = new System.Drawing.Point(561, 27);
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
            this.label1.Size = new System.Drawing.Size(167, 21);
            this.label1.TabIndex = 1;
            this.label1.Text = "Catalog Content Store ";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point(22, 39);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(325, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Explore the XML files in a VS 11 (User Managed) catalog folder";
            // 
            // LoadButtonTip
            // 
            this.LoadButtonTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.LoadButtonTip.Location = new System.Drawing.Point(20, 93);
            this.LoadButtonTip.Name = "LoadButtonTip";
            this.LoadButtonTip.Size = new System.Drawing.Size(13, 13);
            this.LoadButtonTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.LoadButtonTip.TabIndex = 24;
            this.LoadButtonTip.TabStop = false;
            // 
            // CatalogLocksTip
            // 
            this.CatalogLocksTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.CatalogLocksTip.Location = new System.Drawing.Point(20, 118);
            this.CatalogLocksTip.Name = "CatalogLocksTip";
            this.CatalogLocksTip.Size = new System.Drawing.Size(13, 13);
            this.CatalogLocksTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.CatalogLocksTip.TabIndex = 23;
            this.CatalogLocksTip.TabStop = false;
            // 
            // monitorLabel
            // 
            this.monitorLabel.AutoSize = true;
            this.monitorLabel.Location = new System.Drawing.Point(190, 118);
            this.monitorLabel.Name = "monitorLabel";
            this.monitorLabel.Size = new System.Drawing.Size(15, 13);
            this.monitorLabel.TabIndex = 7;
            this.monitorLabel.Text = "--";
            // 
            // monitorCbx
            // 
            this.monitorCbx.AutoCheck = false;
            this.monitorCbx.AutoSize = true;
            this.monitorCbx.Location = new System.Drawing.Point(39, 117);
            this.monitorCbx.Name = "monitorCbx";
            this.monitorCbx.Size = new System.Drawing.Size(145, 17);
            this.monitorCbx.TabIndex = 5;
            this.monitorCbx.Text = "Monitor Catalog Locks:";
            this.monitorCbx.UseVisualStyleBackColor = true;
            this.monitorCbx.Click += new System.EventHandler(this.monitorCbx_Click);
            // 
            // LoadButton
            // 
            this.LoadButton.Location = new System.Drawing.Point(39, 88);
            this.LoadButton.Name = "LoadButton";
            this.LoadButton.Size = new System.Drawing.Size(71, 23);
            this.LoadButton.TabIndex = 4;
            this.LoadButton.Text = "Load";
            this.LoadButton.UseVisualStyleBackColor = true;
            this.LoadButton.Click += new System.EventHandler(this.LoadButton_Click);
            // 
            // catalogPathCbo
            // 
            this.catalogPathCbo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.catalogPathCbo.FormattingEnabled = true;
            this.catalogPathCbo.Location = new System.Drawing.Point(121, 90);
            this.catalogPathCbo.Name = "catalogPathCbo";
            this.catalogPathCbo.Size = new System.Drawing.Size(487, 21);
            this.catalogPathCbo.TabIndex = 3;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 173);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.treeView1);
            this.splitContainer1.Panel1MinSize = 222;
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.splitContainer2);
            this.splitContainer1.Panel2Collapsed = true;
            this.splitContainer1.Size = new System.Drawing.Size(647, 470);
            this.splitContainer1.SplitterDistance = 347;
            this.splitContainer1.TabIndex = 1;
            // 
            // treeView1
            // 
            this.treeView1.ContextMenuStrip = this.treeViewContextMenuStrip;
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView1.FullRowSelect = true;
            this.treeView1.HideSelection = false;
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Name = "treeView1";
            this.treeView1.ShowNodeToolTips = true;
            this.treeView1.Size = new System.Drawing.Size(647, 470);
            this.treeView1.TabIndex = 0;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            this.treeView1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treeView1_KeyDown);
            // 
            // treeViewContextMenuStrip
            // 
            this.treeViewContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuCopy,
            this.mnuExplorer});
            this.treeViewContextMenuStrip.Name = "treeViewContextMenuStrip";
            this.treeViewContextMenuStrip.Size = new System.Drawing.Size(157, 70);
            // 
            // mnuCopy
            // 
            this.mnuCopy.Name = "mnuCopy";
            this.mnuCopy.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
            this.mnuCopy.Size = new System.Drawing.Size(156, 22);
            this.mnuCopy.Text = "Copy";
            this.mnuCopy.Click += new System.EventHandler(this.mnuCopy_Click);
            // 
            // mnuExplorer
            // 
            this.mnuExplorer.Name = "mnuExplorer";
            this.mnuExplorer.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.E)));
            this.mnuExplorer.Size = new System.Drawing.Size(156, 22);
            this.mnuExplorer.Text = "Explorer";
            this.mnuExplorer.Click += new System.EventHandler(this.mnuExplorer_Click);
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.splitContainer2.Size = new System.Drawing.Size(96, 100);
            this.splitContainer2.SplitterDistance = 25;
            this.splitContainer2.TabIndex = 0;
            // 
            // Page_Store
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Page_Store";
            this.Size = new System.Drawing.Size(647, 643);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.warningTip)).EndInit();
            this.treeViewToolStrip.ResumeLayout(false);
            this.treeViewToolStrip.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.LoadButtonTip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.CatalogLocksTip)).EndInit();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.treeViewContextMenuStrip.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox catalogPathCbo;
        private System.Windows.Forms.Button LoadButton;
        private System.Windows.Forms.CheckBox monitorCbx;
        private System.Windows.Forms.Label monitorLabel;
        private System.Windows.Forms.PictureBox CatalogLocksTip;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.ContextMenuStrip treeViewContextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem mnuCopy;
        private System.Windows.Forms.PictureBox LoadButtonTip;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.ToolStrip treeViewToolStrip;
        private System.Windows.Forms.ToolStripButton CopyBtn;
        private System.Windows.Forms.ToolStripButton ExplorerBtn;
        private System.Windows.Forms.ToolStripMenuItem mnuExplorer;
        private System.Windows.Forms.PictureBox warningTip;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.LinkLabel helpLinkLabel;
    }
}
