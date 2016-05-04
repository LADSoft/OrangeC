namespace VisualStudioHelpDownloader2012
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components;

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
            System.Windows.Forms.Label _labelDirectory;
            System.Windows.Forms.Label _labelFilter;
            System.Windows.Forms.ColumnHeader bookName;
            System.Windows.Forms.ColumnHeader totalSize;
            System.Windows.Forms.ColumnHeader totalPackages;
            System.Windows.Forms.ColumnHeader downloadSize;
            System.Windows.Forms.ColumnHeader packagesToDownload;
            System.Windows.Forms.Label label1;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.loadBooks = new System.Windows.Forms.Button();
            this.cacheDirectory = new System.Windows.Forms.TextBox();
            this.browseDirectory = new System.Windows.Forms.Button();
            this.downloadProgress = new System.Windows.Forms.ProgressBar();
            this.startupTip = new System.Windows.Forms.Label();
            this.downloadBooks = new System.Windows.Forms.Button();
            this.loadingBooksTip = new System.Windows.Forms.Label();
            this.booksList = new System.Windows.Forms.ListView();
            this.languageSelection = new System.Windows.Forms.ComboBox();
            this.vsVersion = new System.Windows.Forms.ComboBox();
            _labelDirectory = new System.Windows.Forms.Label();
            _labelFilter = new System.Windows.Forms.Label();
            bookName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            totalSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            totalPackages = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            downloadSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            packagesToDownload = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // _labelDirectory
            // 
            _labelDirectory.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            _labelDirectory.AutoSize = true;
            _labelDirectory.Location = new System.Drawing.Point(12, 411);
            _labelDirectory.Name = "_labelDirectory";
            _labelDirectory.Size = new System.Drawing.Size(71, 15);
            _labelDirectory.TabIndex = 10;
            _labelDirectory.Text = "Store files in";
            // 
            // _labelFilter
            // 
            _labelFilter.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            _labelFilter.AutoSize = true;
            _labelFilter.Location = new System.Drawing.Point(239, 355);
            _labelFilter.Name = "_labelFilter";
            _labelFilter.Size = new System.Drawing.Size(105, 15);
            _labelFilter.TabIndex = 8;
            _labelFilter.Text = "2. Select Language";
            // 
            // bookName
            // 
            bookName.Text = "Book";
            bookName.Width = 400;
            // 
            // totalSize
            // 
            totalSize.Text = "Total Size (MB)";
            totalSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            totalSize.Width = 93;
            // 
            // totalPackages
            // 
            totalPackages.Text = "# Packages";
            totalPackages.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            totalPackages.Width = 73;
            // 
            // downloadSize
            // 
            downloadSize.Text = "Download Size (MB)";
            downloadSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            downloadSize.Width = 119;
            // 
            // packagesToDownload
            // 
            packagesToDownload.Text = "Num Downloads";
            packagesToDownload.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            packagesToDownload.Width = 105;
            // 
            // label1
            // 
            label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(9, 355);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(129, 15);
            label1.TabIndex = 12;
            label1.Text = "1. Visual Studio Version";
            // 
            // loadBooks
            // 
            this.loadBooks.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.loadBooks.Enabled = false;
            this.loadBooks.Location = new System.Drawing.Point(645, 349);
            this.loadBooks.Name = "loadBooks";
            this.loadBooks.Size = new System.Drawing.Size(87, 27);
            this.loadBooks.TabIndex = 3;
            this.loadBooks.Text = "3. Load Books";
            this.loadBooks.UseVisualStyleBackColor = true;
            this.loadBooks.Click += new System.EventHandler(this.LoadBooksClick);
            // 
            // cacheDirectory
            // 
            this.cacheDirectory.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cacheDirectory.Location = new System.Drawing.Point(105, 408);
            this.cacheDirectory.Name = "cacheDirectory";
            this.cacheDirectory.ReadOnly = true;
            this.cacheDirectory.Size = new System.Drawing.Size(683, 23);
            this.cacheDirectory.TabIndex = 5;
            // 
            // browseDirectory
            // 
            this.browseDirectory.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.browseDirectory.Enabled = false;
            this.browseDirectory.Image = ((System.Drawing.Image)(resources.GetObject("browseDirectory.Image")));
            this.browseDirectory.Location = new System.Drawing.Point(794, 405);
            this.browseDirectory.Name = "browseDirectory";
            this.browseDirectory.Size = new System.Drawing.Size(31, 27);
            this.browseDirectory.TabIndex = 2;
            this.browseDirectory.UseVisualStyleBackColor = true;
            this.browseDirectory.Click += new System.EventHandler(this.BrowseDirectoryClick);
            // 
            // downloadProgress
            // 
            this.downloadProgress.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.downloadProgress.Location = new System.Drawing.Point(12, 383);
            this.downloadProgress.MarqueeAnimationSpeed = 25;
            this.downloadProgress.Name = "downloadProgress";
            this.downloadProgress.Size = new System.Drawing.Size(813, 11);
            this.downloadProgress.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.downloadProgress.TabIndex = 9;
            // 
            // startupTip
            // 
            this.startupTip.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.startupTip.AutoSize = true;
            this.startupTip.BackColor = System.Drawing.SystemColors.Window;
            this.startupTip.Location = new System.Drawing.Point(141, 163);
            this.startupTip.Name = "startupTip";
            this.startupTip.Size = new System.Drawing.Size(514, 15);
            this.startupTip.TabIndex = 6;
            this.startupTip.Text = "Select a Visual Studio version and language, then press \"Load\" to  retrieve th" +
    "e available books";
            this.startupTip.Visible = false;
            // 
            // downloadBooks
            // 
            this.downloadBooks.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.downloadBooks.Enabled = false;
            this.downloadBooks.Location = new System.Drawing.Point(738, 349);
            this.downloadBooks.Name = "downloadBooks";
            this.downloadBooks.Size = new System.Drawing.Size(87, 27);
            this.downloadBooks.TabIndex = 5;
            this.downloadBooks.Text = "4. Download";
            this.downloadBooks.UseVisualStyleBackColor = true;
            this.downloadBooks.Click += new System.EventHandler(this.DownloadBooksClick);
            // 
            // loadingBooksTip
            // 
            this.loadingBooksTip.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.loadingBooksTip.AutoSize = true;
            this.loadingBooksTip.BackColor = System.Drawing.SystemColors.Window;
            this.loadingBooksTip.Location = new System.Drawing.Point(338, 163);
            this.loadingBooksTip.Name = "loadingBooksTip";
            this.loadingBooksTip.Size = new System.Drawing.Size(148, 15);
            this.loadingBooksTip.TabIndex = 7;
            this.loadingBooksTip.Text = "Downloading please wait...";
            // 
            // booksList
            // 
            this.booksList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.booksList.CheckBoxes = true;
            this.booksList.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            bookName,
            totalSize,
            totalPackages,
            downloadSize,
            packagesToDownload});
            this.booksList.FullRowSelect = true;
            this.booksList.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.booksList.HoverSelection = true;
            this.booksList.Location = new System.Drawing.Point(12, 13);
            this.booksList.Name = "booksList";
            this.booksList.ShowItemToolTips = true;
            this.booksList.Size = new System.Drawing.Size(813, 330);
            this.booksList.TabIndex = 4;
            this.booksList.UseCompatibleStateImageBehavior = false;
            this.booksList.View = System.Windows.Forms.View.Details;
            this.booksList.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.BooksListItemChecked);
            // 
            // languageSelection
            // 
            this.languageSelection.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.languageSelection.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.languageSelection.Enabled = false;
            this.languageSelection.FormattingEnabled = true;
            this.languageSelection.Location = new System.Drawing.Point(350, 352);
            this.languageSelection.Name = "languageSelection";
            this.languageSelection.Size = new System.Drawing.Size(121, 23);
            this.languageSelection.Sorted = true;
            this.languageSelection.TabIndex = 1;
            this.languageSelection.SelectedIndexChanged += new System.EventHandler(this.BookOptionsChanged);
            // 
            // vsVersion
            // 
            this.vsVersion.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.vsVersion.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.vsVersion.FormattingEnabled = true;
            this.vsVersion.Items.AddRange(new object[] {
            "2012",
            "2013",
            "2015"});
            this.vsVersion.Location = new System.Drawing.Point(144, 352);
            this.vsVersion.Name = "vsVersion";
            this.vsVersion.Size = new System.Drawing.Size(74, 23);
            this.vsVersion.TabIndex = 0;
            this.vsVersion.SelectedIndexChanged += new System.EventHandler(this.VsVersionChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(845, 452);
            this.Controls.Add(label1);
            this.Controls.Add(this.vsVersion);
            this.Controls.Add(this.languageSelection);
            this.Controls.Add(_labelFilter);
            this.Controls.Add(_labelDirectory);
            this.Controls.Add(this.loadingBooksTip);
            this.Controls.Add(this.startupTip);
            this.Controls.Add(this.downloadProgress);
            this.Controls.Add(this.browseDirectory);
            this.Controls.Add(this.cacheDirectory);
            this.Controls.Add(this.downloadBooks);
            this.Controls.Add(this.loadBooks);
            this.Controls.Add(this.booksList);
            this.Font = new System.Drawing.Font("Segoe UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(861, 490);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "%TITLE%";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

		private System.Windows.Forms.Button loadBooks;
        private System.Windows.Forms.TextBox cacheDirectory;
        private System.Windows.Forms.Button browseDirectory;
        private System.Windows.Forms.ProgressBar downloadProgress;
        private System.Windows.Forms.Label startupTip;
		private System.Windows.Forms.Button downloadBooks;
		private System.Windows.Forms.Label loadingBooksTip;
		private System.Windows.Forms.ListView booksList;
		private System.Windows.Forms.ComboBox languageSelection;
		private System.Windows.Forms.ComboBox vsVersion;
    }
}

