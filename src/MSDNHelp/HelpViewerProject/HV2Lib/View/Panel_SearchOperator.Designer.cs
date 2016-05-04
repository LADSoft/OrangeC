namespace HVProject.HV2Lib.View
{
    partial class Panel_SearchOperator
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.panelContent = new System.Windows.Forms.Panel();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.linkTitle = new System.Windows.Forms.LinkLabel();
            this.linkKeyword = new System.Windows.Forms.LinkLabel();
            this.linkCode = new System.Windows.Forms.LinkLabel();
            this.linkCodeCS = new System.Windows.Forms.LinkLabel();
            this.linkCodeVB = new System.Windows.Forms.LinkLabel();
            this.linkCodeCpp = new System.Windows.Forms.LinkLabel();
            this.linkCodeJScript = new System.Windows.Forms.LinkLabel();
            this.linkCodeFS = new System.Windows.Forms.LinkLabel();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.panelContent.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 24);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(217, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Valid operators: AND, OR, NOT, NEAR, (, )";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(165, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Use a trailing * for wild search.";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 72);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(121, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Valid prefix operators:";
            // 
            // panelContent
            // 
            this.panelContent.BackColor = System.Drawing.Color.White;
            this.panelContent.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelContent.Controls.Add(this.label6);
            this.panelContent.Controls.Add(this.flowLayoutPanel1);
            this.panelContent.Controls.Add(this.label5);
            this.panelContent.Controls.Add(this.label4);
            this.panelContent.Controls.Add(this.label1);
            this.panelContent.Controls.Add(this.label3);
            this.panelContent.Controls.Add(this.label2);
            this.panelContent.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelContent.ForeColor = System.Drawing.Color.MidnightBlue;
            this.panelContent.Location = new System.Drawing.Point(0, 0);
            this.panelContent.Name = "panelContent";
            this.panelContent.Size = new System.Drawing.Size(322, 152);
            this.panelContent.TabIndex = 3;
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.flowLayoutPanel1.Controls.Add(this.linkTitle);
            this.flowLayoutPanel1.Controls.Add(this.linkKeyword);
            this.flowLayoutPanel1.Controls.Add(this.linkCode);
            this.flowLayoutPanel1.Controls.Add(this.linkCodeCS);
            this.flowLayoutPanel1.Controls.Add(this.linkCodeVB);
            this.flowLayoutPanel1.Controls.Add(this.linkCodeCpp);
            this.flowLayoutPanel1.Controls.Add(this.linkCodeJScript);
            this.flowLayoutPanel1.Controls.Add(this.linkCodeFS);
            this.flowLayoutPanel1.Location = new System.Drawing.Point(31, 88);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(283, 33);
            this.flowLayoutPanel1.TabIndex = 6;
            // 
            // linkTitle
            // 
            this.linkTitle.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkTitle.AutoSize = true;
            this.linkTitle.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkTitle.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkTitle.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkTitle.Location = new System.Drawing.Point(3, 0);
            this.linkTitle.Name = "linkTitle";
            this.linkTitle.Size = new System.Drawing.Size(30, 13);
            this.linkTitle.TabIndex = 5;
            this.linkTitle.TabStop = true;
            this.linkTitle.Text = "title:";
            this.linkTitle.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkTitle.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // linkKeyword
            // 
            this.linkKeyword.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkKeyword.AutoSize = true;
            this.linkKeyword.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkKeyword.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkKeyword.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkKeyword.Location = new System.Drawing.Point(39, 0);
            this.linkKeyword.Name = "linkKeyword";
            this.linkKeyword.Size = new System.Drawing.Size(54, 13);
            this.linkKeyword.TabIndex = 6;
            this.linkKeyword.TabStop = true;
            this.linkKeyword.Text = "keyword:";
            this.linkKeyword.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkKeyword.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // linkCode
            // 
            this.linkCode.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCode.AutoSize = true;
            this.linkCode.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCode.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkCode.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCode.Location = new System.Drawing.Point(99, 0);
            this.linkCode.Name = "linkCode";
            this.linkCode.Size = new System.Drawing.Size(35, 13);
            this.linkCode.TabIndex = 7;
            this.linkCode.TabStop = true;
            this.linkCode.Text = "code:";
            this.linkCode.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCode.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // linkCodeCS
            // 
            this.linkCodeCS.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCS.AutoSize = true;
            this.linkCodeCS.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCS.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCS.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCS.Location = new System.Drawing.Point(140, 0);
            this.linkCodeCS.Name = "linkCodeCS";
            this.linkCodeCS.Size = new System.Drawing.Size(50, 13);
            this.linkCodeCS.TabIndex = 8;
            this.linkCodeCS.TabStop = true;
            this.linkCodeCS.Text = "code:c#:";
            this.linkCodeCS.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCS.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // linkCodeVB
            // 
            this.linkCodeVB.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeVB.AutoSize = true;
            this.linkCodeVB.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeVB.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeVB.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeVB.Location = new System.Drawing.Point(196, 0);
            this.linkCodeVB.Name = "linkCodeVB";
            this.linkCodeVB.Size = new System.Drawing.Size(50, 13);
            this.linkCodeVB.TabIndex = 9;
            this.linkCodeVB.TabStop = true;
            this.linkCodeVB.Text = "code:vb:";
            this.linkCodeVB.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeVB.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // linkCodeCpp
            // 
            this.linkCodeCpp.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCpp.AutoSize = true;
            this.linkCodeCpp.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCpp.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCpp.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCpp.Location = new System.Drawing.Point(3, 13);
            this.linkCodeCpp.Name = "linkCodeCpp";
            this.linkCodeCpp.Size = new System.Drawing.Size(59, 13);
            this.linkCodeCpp.TabIndex = 10;
            this.linkCodeCpp.TabStop = true;
            this.linkCodeCpp.Text = "code:c++:";
            this.linkCodeCpp.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeCpp.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // linkCodeJScript
            // 
            this.linkCodeJScript.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeJScript.AutoSize = true;
            this.linkCodeJScript.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeJScript.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeJScript.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeJScript.Location = new System.Drawing.Point(68, 13);
            this.linkCodeJScript.Name = "linkCodeJScript";
            this.linkCodeJScript.Size = new System.Drawing.Size(69, 13);
            this.linkCodeJScript.TabIndex = 11;
            this.linkCodeJScript.TabStop = true;
            this.linkCodeJScript.Text = "code:jscript:";
            this.linkCodeJScript.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeJScript.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // linkCodeFS
            // 
            this.linkCodeFS.ActiveLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeFS.AutoSize = true;
            this.linkCodeFS.DisabledLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeFS.ForeColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeFS.LinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeFS.Location = new System.Drawing.Point(143, 13);
            this.linkCodeFS.Name = "linkCodeFS";
            this.linkCodeFS.Size = new System.Drawing.Size(49, 13);
            this.linkCodeFS.TabIndex = 12;
            this.linkCodeFS.TabStop = true;
            this.linkCodeFS.Text = "code:f#:";
            this.linkCodeFS.VisitedLinkColor = System.Drawing.Color.RoyalBlue;
            this.linkCodeFS.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkTitle_LinkClicked);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(11, 37);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(243, 13);
            this.label5.TabIndex = 4;
            this.label5.Text = "If no operator is specified then AND assumed.";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Segoe UI", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.ForeColor = System.Drawing.Color.LightSkyBlue;
            this.label4.Location = new System.Drawing.Point(3, 3);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(126, 19);
            this.label4.TabIndex = 3;
            this.label4.Text = "Search Operators";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 124);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(295, 13);
            this.label6.TabIndex = 7;
            this.label6.Text = "(prefixes can be combined and added to all query terms)";
            // 
            // Panel_SearchOperator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.panelContent);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Panel_SearchOperator";
            this.Size = new System.Drawing.Size(322, 152);
            this.VisibleChanged += new System.EventHandler(this.SearchOperatorPanel_VisibleChanged);
            this.Leave += new System.EventHandler(this.SearchOperatorPanel_Leave);
            this.panelContent.ResumeLayout(false);
            this.panelContent.PerformLayout();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Panel panelContent;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.LinkLabel linkTitle;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.LinkLabel linkKeyword;
        private System.Windows.Forms.LinkLabel linkCode;
        private System.Windows.Forms.LinkLabel linkCodeCS;
        private System.Windows.Forms.LinkLabel linkCodeVB;
        private System.Windows.Forms.LinkLabel linkCodeCpp;
        private System.Windows.Forms.LinkLabel linkCodeJScript;
        private System.Windows.Forms.LinkLabel linkCodeFS;
        private System.Windows.Forms.Label label6;
    }
}
