namespace HV2ApiExplore.Forms
{
    partial class Panel_HelpIndexer
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
            this.dosBox = new System.Windows.Forms.RichTextBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.HelpIndexerHelpBtn = new System.Windows.Forms.Button();
            this.RunBtnTip = new System.Windows.Forms.PictureBox();
            this.RunBtn = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.richTextBox1 = new System.Windows.Forms.RichTextBox();
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.rdoCombine = new System.Windows.Forms.RadioButton();
            this.rdoIndex = new System.Windows.Forms.RadioButton();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxVendorName = new System.Windows.Forms.TextBox();
            this.labelVendorName = new System.Windows.Forms.Label();
            this.textBoxOutputFilename = new System.Windows.Forms.TextBox();
            this.labelOutputFilename = new System.Windows.Forms.Label();
            this.buttonOutput = new System.Windows.Forms.Button();
            this.textBoxOutput = new System.Windows.Forms.TextBox();
            this.labelOutputPath = new System.Windows.Forms.Label();
            this.buttonInput = new System.Windows.Forms.Button();
            this.textBoxInput = new System.Windows.Forms.TextBox();
            this.InputLabel = new System.Windows.Forms.Label();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RunBtnTip)).BeginInit();
            this.flowLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // dosBox
            // 
            this.dosBox.BackColor = System.Drawing.Color.Black;
            this.dosBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.dosBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dosBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.dosBox.ForeColor = System.Drawing.Color.White;
            this.dosBox.Location = new System.Drawing.Point(0, 283);
            this.dosBox.Name = "dosBox";
            this.dosBox.ReadOnly = true;
            this.dosBox.Size = new System.Drawing.Size(743, 251);
            this.dosBox.TabIndex = 0;
            this.dosBox.Text = "";
            this.dosBox.WordWrap = false;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.HelpIndexerHelpBtn);
            this.panel1.Controls.Add(this.RunBtnTip);
            this.panel1.Controls.Add(this.RunBtn);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Controls.Add(this.richTextBox1);
            this.panel1.Controls.Add(this.flowLayoutPanel1);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.textBoxVendorName);
            this.panel1.Controls.Add(this.labelVendorName);
            this.panel1.Controls.Add(this.textBoxOutputFilename);
            this.panel1.Controls.Add(this.labelOutputFilename);
            this.panel1.Controls.Add(this.buttonOutput);
            this.panel1.Controls.Add(this.textBoxOutput);
            this.panel1.Controls.Add(this.labelOutputPath);
            this.panel1.Controls.Add(this.buttonInput);
            this.panel1.Controls.Add(this.textBoxInput);
            this.panel1.Controls.Add(this.InputLabel);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(743, 283);
            this.panel1.TabIndex = 1;
            // 
            // HelpIndexerHelpBtn
            // 
            this.HelpIndexerHelpBtn.Location = new System.Drawing.Point(379, 230);
            this.HelpIndexerHelpBtn.Name = "HelpIndexerHelpBtn";
            this.HelpIndexerHelpBtn.Size = new System.Drawing.Size(175, 28);
            this.HelpIndexerHelpBtn.TabIndex = 21;
            this.HelpIndexerHelpBtn.Text = "HelpIndexer.exe /?";
            this.HelpIndexerHelpBtn.UseVisualStyleBackColor = true;
            this.HelpIndexerHelpBtn.Click += new System.EventHandler(this.HelpIndexerHelpBtn_Click);
            // 
            // RunBtnTip
            // 
            this.RunBtnTip.Image = global::HV2ApiExplore.Properties.Resources.infoTip13x13;
            this.RunBtnTip.Location = new System.Drawing.Point(345, 238);
            this.RunBtnTip.Name = "RunBtnTip";
            this.RunBtnTip.Size = new System.Drawing.Size(13, 13);
            this.RunBtnTip.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.RunBtnTip.TabIndex = 20;
            this.RunBtnTip.TabStop = false;
            // 
            // RunBtn
            // 
            this.RunBtn.Location = new System.Drawing.Point(157, 230);
            this.RunBtn.Name = "RunBtn";
            this.RunBtn.Size = new System.Drawing.Size(183, 28);
            this.RunBtn.TabIndex = 14;
            this.RunBtn.Text = "&Run HelpIndexer.exe";
            this.RunBtn.UseVisualStyleBackColor = true;
            this.RunBtn.Click += new System.EventHandler(this.RunBtn_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(2, 162);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(140, 13);
            this.label3.TabIndex = 13;
            this.label3.Text = "Equivalent command-line:";
            // 
            // richTextBox1
            // 
            this.richTextBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.richTextBox1.BackColor = System.Drawing.Color.Teal;
            this.richTextBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.richTextBox1.ForeColor = System.Drawing.Color.White;
            this.richTextBox1.Location = new System.Drawing.Point(157, 162);
            this.richTextBox1.Name = "richTextBox1";
            this.richTextBox1.Size = new System.Drawing.Size(565, 52);
            this.richTextBox1.TabIndex = 12;
            this.richTextBox1.Text = "";
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.rdoCombine);
            this.flowLayoutPanel1.Controls.Add(this.rdoIndex);
            this.flowLayoutPanel1.Location = new System.Drawing.Point(156, 18);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(510, 27);
            this.flowLayoutPanel1.TabIndex = 11;
            // 
            // rdoCombine
            // 
            this.rdoCombine.AutoSize = true;
            this.rdoCombine.Checked = true;
            this.rdoCombine.Location = new System.Drawing.Point(3, 3);
            this.rdoCombine.Name = "rdoCombine";
            this.rdoCombine.Size = new System.Drawing.Size(168, 17);
            this.rdoCombine.TabIndex = 1;
            this.rdoCombine.TabStop = true;
            this.rdoCombine.Text = "Create .mshx file from .mshc";
            this.rdoCombine.UseVisualStyleBackColor = true;
            this.rdoCombine.CheckedChanged += new System.EventHandler(this.rdoCombine_CheckedChanged);
            // 
            // rdoIndex
            // 
            this.rdoIndex.AutoSize = true;
            this.rdoIndex.Location = new System.Drawing.Point(177, 3);
            this.rdoIndex.Name = "rdoIndex";
            this.rdoIndex.Size = new System.Drawing.Size(184, 17);
            this.rdoIndex.TabIndex = 0;
            this.rdoIndex.Text = "Create Index (.mshi) from .mshc";
            this.rdoIndex.UseVisualStyleBackColor = true;
            this.rdoIndex.CheckedChanged += new System.EventHandler(this.rdoIndex_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 23);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(73, 13);
            this.label2.TabIndex = 10;
            this.label2.Text = "Output type:";
            // 
            // textBoxVendorName
            // 
            this.textBoxVendorName.Location = new System.Drawing.Point(157, 129);
            this.textBoxVendorName.Name = "textBoxVendorName";
            this.textBoxVendorName.Size = new System.Drawing.Size(190, 22);
            this.textBoxVendorName.TabIndex = 9;
            this.textBoxVendorName.TextChanged += new System.EventHandler(this.textBoxVendorName_TextChanged);
            // 
            // labelVendorName
            // 
            this.labelVendorName.AutoSize = true;
            this.labelVendorName.Location = new System.Drawing.Point(3, 132);
            this.labelVendorName.Name = "labelVendorName";
            this.labelVendorName.Size = new System.Drawing.Size(141, 13);
            this.labelVendorName.TabIndex = 8;
            this.labelVendorName.Text = "Vendor name (Index only):";
            // 
            // textBoxOutputFilename
            // 
            this.textBoxOutputFilename.Location = new System.Drawing.Point(157, 103);
            this.textBoxOutputFilename.Name = "textBoxOutputFilename";
            this.textBoxOutputFilename.Size = new System.Drawing.Size(190, 22);
            this.textBoxOutputFilename.TabIndex = 7;
            this.textBoxOutputFilename.TextChanged += new System.EventHandler(this.textBoxOutputFilename_TextChanged);
            // 
            // labelOutputFilename
            // 
            this.labelOutputFilename.AutoSize = true;
            this.labelOutputFilename.Location = new System.Drawing.Point(3, 106);
            this.labelOutputFilename.Name = "labelOutputFilename";
            this.labelOutputFilename.Size = new System.Drawing.Size(148, 13);
            this.labelOutputFilename.TabIndex = 6;
            this.labelOutputFilename.Text = "Output filename (optional):";
            // 
            // buttonOutput
            // 
            this.buttonOutput.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOutput.Location = new System.Drawing.Point(693, 77);
            this.buttonOutput.Name = "buttonOutput";
            this.buttonOutput.Size = new System.Drawing.Size(29, 20);
            this.buttonOutput.TabIndex = 5;
            this.buttonOutput.Text = "...";
            this.buttonOutput.UseVisualStyleBackColor = true;
            this.buttonOutput.Click += new System.EventHandler(this.buttonOutput_Click);
            // 
            // textBoxOutput
            // 
            this.textBoxOutput.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxOutput.Location = new System.Drawing.Point(157, 77);
            this.textBoxOutput.Name = "textBoxOutput";
            this.textBoxOutput.Size = new System.Drawing.Size(530, 22);
            this.textBoxOutput.TabIndex = 4;
            this.textBoxOutput.TextChanged += new System.EventHandler(this.textBoxOutput_TextChanged);
            // 
            // labelOutputPath
            // 
            this.labelOutputPath.AutoSize = true;
            this.labelOutputPath.Location = new System.Drawing.Point(3, 80);
            this.labelOutputPath.Name = "labelOutputPath";
            this.labelOutputPath.Size = new System.Drawing.Size(96, 13);
            this.labelOutputPath.TabIndex = 3;
            this.labelOutputPath.Text = "Output directory:";
            // 
            // buttonInput
            // 
            this.buttonInput.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonInput.Location = new System.Drawing.Point(693, 51);
            this.buttonInput.Name = "buttonInput";
            this.buttonInput.Size = new System.Drawing.Size(29, 20);
            this.buttonInput.TabIndex = 2;
            this.buttonInput.Text = "...";
            this.buttonInput.UseVisualStyleBackColor = true;
            this.buttonInput.Click += new System.EventHandler(this.buttonInput_Click);
            // 
            // textBoxInput
            // 
            this.textBoxInput.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInput.Location = new System.Drawing.Point(157, 51);
            this.textBoxInput.Name = "textBoxInput";
            this.textBoxInput.Size = new System.Drawing.Size(530, 22);
            this.textBoxInput.TabIndex = 1;
            this.textBoxInput.TextChanged += new System.EventHandler(this.textBoxInput_TextChanged);
            // 
            // InputLabel
            // 
            this.InputLabel.AutoSize = true;
            this.InputLabel.Location = new System.Drawing.Point(3, 54);
            this.InputLabel.Name = "InputLabel";
            this.InputLabel.Size = new System.Drawing.Size(95, 13);
            this.InputLabel.TabIndex = 0;
            this.InputLabel.Text = "Input file (.mshc):";
            // 
            // Panel_HelpIndexer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.dosBox);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "Panel_HelpIndexer";
            this.Size = new System.Drawing.Size(743, 534);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RunBtnTip)).EndInit();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.flowLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RichTextBox dosBox;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox textBoxOutputFilename;
        private System.Windows.Forms.Label labelOutputFilename;
        private System.Windows.Forms.Button buttonOutput;
        private System.Windows.Forms.TextBox textBoxOutput;
        private System.Windows.Forms.Label labelOutputPath;
        private System.Windows.Forms.Button buttonInput;
        private System.Windows.Forms.TextBox textBoxInput;
        private System.Windows.Forms.Label InputLabel;
        private System.Windows.Forms.TextBox textBoxVendorName;
        private System.Windows.Forms.Label labelVendorName;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.RadioButton rdoCombine;
        private System.Windows.Forms.RadioButton rdoIndex;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.RichTextBox richTextBox1;
        private System.Windows.Forms.Button RunBtn;
        private System.Windows.Forms.PictureBox RunBtnTip;
        private System.Windows.Forms.Button HelpIndexerHelpBtn;
    }
}
