namespace HV2ApiExplore.Forms
{
    partial class Page_Topics
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Page_Topics));
            this.panel1 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.helpLinkLabel = new System.Windows.Forms.LinkLabel();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.panelControls = new System.Windows.Forms.Panel();
            this.GetAssetTip = new System.Windows.Forms.PictureBox();
            this.GetAssetBtn = new System.Windows.Forms.Button();
            this.TopicFromIdTip = new System.Windows.Forms.PictureBox();
            this.GetTopicFromIdBtn = new System.Windows.Forms.Button();
            this.TopicFromIdCbo = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.rdoGetTopicDetail = new System.Windows.Forms.RadioButton();
            this.rdoGetTopicStream = new System.Windows.Forms.RadioButton();
            this.TopicFromF1Tip = new System.Windows.Forms.PictureBox();
            this.TopicFromF1TextBox = new System.Windows.Forms.TextBox();
            this.GetTopicFromF1Btn = new System.Windows.Forms.Button();
            this.tipFileStreamOnly = new System.Windows.Forms.PictureBox();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.panelControls.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GetAssetTip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.TopicFromIdTip)).BeginInit();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.TopicFromF1Tip)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.tipFileStreamOnly)).BeginInit();
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
            this.panel1.Size = new System.Drawing.Size(754, 67);
            this.panel1.TabIndex = 17;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.ForeColor = System.Drawing.Color.White;
            this.label7.Location = new System.Drawing.Point(26, 29);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(209, 13);
            this.label7.TabIndex = 5;
            this.label7.Text = "- Get a topic (stream or details+stream) ";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(648, 24);
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
            this.helpLinkLabel.Location = new System.Drawing.Point(667, 24);
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
            this.label4.Size = new System.Drawing.Size(192, 13);
            this.label4.TabIndex = 1;
            this.label4.Text = "   by either Topic ID or F1 topicID(s)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Segoe UI", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(9, 5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(52, 20);
            this.label3.TabIndex = 0;
            this.label3.Text = "Topics";
            // 
            // panelControls
            // 
            this.panelControls.Controls.Add(this.GetAssetTip);
            this.panelControls.Controls.Add(this.GetAssetBtn);
            this.panelControls.Controls.Add(this.TopicFromIdTip);
            this.panelControls.Controls.Add(this.GetTopicFromIdBtn);
            this.panelControls.Controls.Add(this.TopicFromIdCbo);
            this.panelControls.Controls.Add(this.label2);
            this.panelControls.Controls.Add(this.label1);
            this.panelControls.Controls.Add(this.panel3);
            this.panelControls.Controls.Add(this.TopicFromF1Tip);
            this.panelControls.Controls.Add(this.TopicFromF1TextBox);
            this.panelControls.Controls.Add(this.GetTopicFromF1Btn);
            this.panelControls.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelControls.Location = new System.Drawing.Point(0, 67);
            this.panelControls.Name = "panelControls";
            this.panelControls.Size = new System.Drawing.Size(754, 138);
            this.panelControls.TabIndex = 18;
            // 
            // GetAssetTip
            // 
            this.GetAssetTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.GetAssetTip.Location = new System.Drawing.Point(659, 72);
            this.GetAssetTip.Name = "GetAssetTip";
            this.GetAssetTip.Size = new System.Drawing.Size(13, 13);
            this.GetAssetTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.GetAssetTip.TabIndex = 21;
            this.GetAssetTip.TabStop = false;
            // 
            // GetAssetBtn
            // 
            this.GetAssetBtn.Location = new System.Drawing.Point(578, 67);
            this.GetAssetBtn.Name = "GetAssetBtn";
            this.GetAssetBtn.Size = new System.Drawing.Size(75, 22);
            this.GetAssetBtn.TabIndex = 20;
            this.GetAssetBtn.Text = "Get Asset";
            this.GetAssetBtn.UseVisualStyleBackColor = true;
            this.GetAssetBtn.Click += new System.EventHandler(this.GetAssetBtn_Click);
            // 
            // TopicFromIdTip
            // 
            this.TopicFromIdTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.TopicFromIdTip.Location = new System.Drawing.Point(555, 72);
            this.TopicFromIdTip.Name = "TopicFromIdTip";
            this.TopicFromIdTip.Size = new System.Drawing.Size(13, 13);
            this.TopicFromIdTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.TopicFromIdTip.TabIndex = 19;
            this.TopicFromIdTip.TabStop = false;
            // 
            // GetTopicFromIdBtn
            // 
            this.GetTopicFromIdBtn.Location = new System.Drawing.Point(440, 67);
            this.GetTopicFromIdBtn.Name = "GetTopicFromIdBtn";
            this.GetTopicFromIdBtn.Size = new System.Drawing.Size(109, 23);
            this.GetTopicFromIdBtn.TabIndex = 17;
            this.GetTopicFromIdBtn.Text = "Get Topic for ID";
            this.GetTopicFromIdBtn.UseVisualStyleBackColor = true;
            this.GetTopicFromIdBtn.Click += new System.EventHandler(this.GetTopicFromIdBtn_Click);
            // 
            // TopicFromIdCbo
            // 
            this.TopicFromIdCbo.DropDownHeight = 1;
            this.TopicFromIdCbo.FormattingEnabled = true;
            this.TopicFromIdCbo.IntegralHeight = false;
            this.TopicFromIdCbo.Location = new System.Drawing.Point(118, 68);
            this.TopicFromIdCbo.Name = "TopicFromIdCbo";
            this.TopicFromIdCbo.Size = new System.Drawing.Size(314, 21);
            this.TopicFromIdCbo.TabIndex = 16;
            this.TopicFromIdCbo.DropDown += new System.EventHandler(this.TopicIdCbo_DropDown);
            this.TopicFromIdCbo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TopicFromIdCbo_KeyDown);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(-3, 72);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(81, 13);
            this.label2.TabIndex = 11;
            this.label2.Text = "Enter Topic ID:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(-3, 103);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(110, 13);
            this.label1.TabIndex = 10;
            this.label1.Text = "Enter F1 Keyword(s):";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.tipFileStreamOnly);
            this.panel3.Controls.Add(this.label5);
            this.panel3.Controls.Add(this.rdoGetTopicDetail);
            this.panel3.Controls.Add(this.rdoGetTopicStream);
            this.panel3.Location = new System.Drawing.Point(0, 30);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(512, 26);
            this.panel3.TabIndex = 15;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(37, 6);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(72, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Get Topic as:";
            // 
            // rdoGetTopicDetail
            // 
            this.rdoGetTopicDetail.AutoSize = true;
            this.rdoGetTopicDetail.Checked = true;
            this.rdoGetTopicDetail.Location = new System.Drawing.Point(118, 4);
            this.rdoGetTopicDetail.Name = "rdoGetTopicDetail";
            this.rdoGetTopicDetail.Size = new System.Drawing.Size(90, 17);
            this.rdoGetTopicDetail.TabIndex = 9;
            this.rdoGetTopicDetail.TabStop = true;
            this.rdoGetTopicDetail.Text = "Topic Details";
            this.rdoGetTopicDetail.UseVisualStyleBackColor = true;
            // 
            // rdoGetTopicStream
            // 
            this.rdoGetTopicStream.AutoSize = true;
            this.rdoGetTopicStream.Location = new System.Drawing.Point(221, 4);
            this.rdoGetTopicStream.Name = "rdoGetTopicStream";
            this.rdoGetTopicStream.Size = new System.Drawing.Size(108, 17);
            this.rdoGetTopicStream.TabIndex = 8;
            this.rdoGetTopicStream.Text = "File Stream Only";
            this.rdoGetTopicStream.UseVisualStyleBackColor = true;
            // 
            // TopicFromF1Tip
            // 
            this.TopicFromF1Tip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.TopicFromF1Tip.Location = new System.Drawing.Point(555, 103);
            this.TopicFromF1Tip.Name = "TopicFromF1Tip";
            this.TopicFromF1Tip.Size = new System.Drawing.Size(13, 13);
            this.TopicFromF1Tip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.TopicFromF1Tip.TabIndex = 6;
            this.TopicFromF1Tip.TabStop = false;
            // 
            // TopicFromF1TextBox
            // 
            this.TopicFromF1TextBox.Location = new System.Drawing.Point(118, 100);
            this.TopicFromF1TextBox.Name = "TopicFromF1TextBox";
            this.TopicFromF1TextBox.Size = new System.Drawing.Size(314, 22);
            this.TopicFromF1TextBox.TabIndex = 4;
            this.TopicFromF1TextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TopicFromF1TextBox_KeyDown);
            // 
            // GetTopicFromF1Btn
            // 
            this.GetTopicFromF1Btn.Location = new System.Drawing.Point(440, 98);
            this.GetTopicFromF1Btn.Name = "GetTopicFromF1Btn";
            this.GetTopicFromF1Btn.Size = new System.Drawing.Size(109, 23);
            this.GetTopicFromF1Btn.TabIndex = 3;
            this.GetTopicFromF1Btn.Text = "Get Topic for F1";
            this.GetTopicFromF1Btn.UseVisualStyleBackColor = true;
            this.GetTopicFromF1Btn.Click += new System.EventHandler(this.GetTopicFromF1Btn_Click);
            // 
            // tipFileStreamOnly
            // 
            this.tipFileStreamOnly.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.tipFileStreamOnly.Location = new System.Drawing.Point(335, 6);
            this.tipFileStreamOnly.Name = "tipFileStreamOnly";
            this.tipFileStreamOnly.Size = new System.Drawing.Size(13, 13);
            this.tipFileStreamOnly.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.tipFileStreamOnly.TabIndex = 20;
            this.tipFileStreamOnly.TabStop = false;
            // 
            // Page_Topics
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.panelControls);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Page_Topics";
            this.Size = new System.Drawing.Size(754, 572);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.panelControls.ResumeLayout(false);
            this.panelControls.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GetAssetTip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.TopicFromIdTip)).EndInit();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.TopicFromF1Tip)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.tipFileStreamOnly)).EndInit();
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
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.RadioButton rdoGetTopicDetail;
        private System.Windows.Forms.RadioButton rdoGetTopicStream;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox TopicFromF1Tip;
        private System.Windows.Forms.TextBox TopicFromF1TextBox;
        private System.Windows.Forms.Button GetTopicFromF1Btn;
        private System.Windows.Forms.ComboBox TopicFromIdCbo;
        private System.Windows.Forms.Button GetTopicFromIdBtn;
        private System.Windows.Forms.PictureBox TopicFromIdTip;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button GetAssetBtn;
        private System.Windows.Forms.PictureBox GetAssetTip;
        private System.Windows.Forms.PictureBox tipFileStreamOnly;
    }
}
