namespace HV2ApiExplore.Forms
{
    partial class Form_RunHelpIndexer
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_RunHelpIndexer));
            this.SuspendLayout();
            // 
            // Form_RunHelpIndexer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(644, 527);
            this.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.KeyPreview = true;
            this.MinimizeBox = false;
            this.Name = "Form_RunHelpIndexer";
            this.Padding = new System.Windows.Forms.Padding(12);
            this.ShowInTaskbar = false;
            this.Text = "Run HelpIndexer.exe";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form_RunHelpIndexer_FormClosing);
            this.Shown += new System.EventHandler(this.Form_RunHelpIndexer_Shown);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form_RunHelpIndexer_KeyDown);
            this.ResumeLayout(false);

        }

        #endregion
    }
}