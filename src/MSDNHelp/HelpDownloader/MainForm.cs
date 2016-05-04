namespace VisualStudioHelpDownloader2012
{
	using System;
	using System.Collections.Generic;
	using System.Globalization;
	using System.IO;
	using System.Threading.Tasks;
	using System.Windows.Forms;
    using System.Diagnostics;
	/// <summary>
	///     Main application form.
	/// </summary>
	internal sealed partial class MainForm : Form, MyProgress
	{
		/// <summary>
		/// The products.
		/// </summary>
		private ICollection<BookGroup> products;

		private string OrangeCHelpFileDirectory = null;
		/// <summary>
		/// Initializes a new instance of the <see cref="MainForm"/> class.
		/// </summary>
		public MainForm(string [] args)
		{
			InitializeComponent();

			Text = Application.ProductName;
			products = new List<BookGroup>();
			startupTip.Visible = false;
			cacheDirectory.Text = Path.Combine( Environment.GetFolderPath( Environment.SpecialFolder.UserProfile ), "Downloads", "MSDN Library" );
//			if (args.Length == 1 && args[0] == "/orangec")
			{
				OrangeCHelpFileDirectory = Path.Combine( Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Orange C", "MSDN  Library" );
			}
		}

		/// <summary>
		/// Reports a progress update.
		/// </summary>
		/// <param name="value">
		/// The value of the updated progress. (percentage complete)
		/// </param>
		public void Report( int value )
		{
			Invoke(
				new MethodInvoker(
					delegate
						{
							downloadProgress.Value = value;
						} ) );
		}

		/// <summary>
		/// Called when the form is loaded. Start retrieving the list of available
		/// languages in the background.
		/// </summary>
		/// <param name="e">
		/// The parameter is not used.
		/// </param>
		protected override void OnLoad( EventArgs e )
		{
			base.OnLoad( e );
            loadingBooksTip.Visible = false;
            startupTip.Visible = true;
        }

        /// <summary>
        /// Called to update the available locales for the selected version of visual studio
        /// </summary>
        private void UpdateLocales()
        {
            if ( vsVersion.SelectedIndex == -1 )
            {
                return;
            }

            string[] vsVersions = { "visualstudio11", "visualstudio12", "dev14" };
            string version = vsVersions[vsVersion.SelectedIndex];
            loadingBooksTip.Visible = true;
            startupTip.Visible = false;
            languageSelection.Items.Clear();
            downloadProgress.Style = ProgressBarStyle.Marquee;
            Task.Factory.StartNew(
                () =>
                {
                    using (Downloader downloader = new Downloader())
                    {
                        return downloader.LoadAvailableLocales( version );
                    }
                })
            .ContinueWith(
                        t =>
                        {
                            languageSelection.DisplayMember = "Name";
                            t.Result.ForEach(x => languageSelection.Items.Add(x));
                            ClearBusyState();
                            startupTip.Visible = true;
                        },
                        TaskScheduler.FromCurrentSynchronizationContext());
        }

		/// <summary>
		/// Called when the download books button is clicked. Start downloading in a background thread
		/// </summary>
		/// <param name="sender">
		/// The parameter is not used.
		/// </param>
		/// <param name="e">
		/// The parameter is not used.
		/// </param>
		private void DownloadBooksClick( object sender, EventArgs e )
		{
			SetBusyState();
			downloadProgress.Style = ProgressBarStyle.Continuous;
			downloadProgress.Value = 0;
			Task.Factory.StartNew(
				() =>
					{
						using ( Downloader downloader = new Downloader() )
						{
							downloader.DownloadBooks( products, cacheDirectory.Text, OrangeCHelpFileDirectory, (Locale)languageSelection.SelectedItem, this );
						}
					} )
			.ContinueWith(
						t =>
							{
								if ( t.Status == TaskStatus.Faulted )
								{
									string message = string.Format(
										CultureInfo.CurrentCulture,
										"Download failed - {0}",
										t.Exception == null ? "Unknown error" : t.Exception.GetBaseException().Message );
									MessageBox.Show(
										message,
										Application.ProductName,
										MessageBoxButtons.OK,
										MessageBoxIcon.Error,
										MessageBoxDefaultButton.Button1,
										0 );
								}
								else
								{
									MessageBox.Show(
										"Download completed successfully",
										Application.ProductName,
										MessageBoxButtons.OK,
										MessageBoxIcon.Information,
										MessageBoxDefaultButton.Button1,
										0 );
								}

								ClearBusyState();
								DisplayBooks();
								downloadProgress.Value = 0;
							}, 
						TaskScheduler.FromCurrentSynchronizationContext() );
		}

		/// <summary>
		/// Called when the load books button is clicked. Load the list of available books for the selected
		/// language
		/// </summary>
		/// <param name="sender">
		/// The parameter is not used.
		/// </param>
		/// <param name="e">
		/// The parameter is not used.
		/// </param>
		private void LoadBooksClick( object sender, EventArgs e )
		{
			string path = ((Locale)languageSelection.SelectedItem).CatalogLink;

			SetBusyState();
			downloadProgress.Style = ProgressBarStyle.Marquee;
			startupTip.Visible = false;
			loadingBooksTip.Visible = true;

			Task.Factory.StartNew(
				() =>
					{
						using ( Downloader downloader = new Downloader() )
						{
							return downloader.LoadBooksInformation( path );
						}
					} ).ContinueWith(
						t =>
							{
								if ( t.Status == TaskStatus.Faulted )
								{
									string message = string.Format(
										CultureInfo.CurrentCulture,
										"Failed to retrieve book information - {0}",
										t.Exception == null ? "Unknown error" : t.Exception.GetBaseException().Message );
									MessageBox.Show(
										message,
										Application.ProductName,
										MessageBoxButtons.OK,
										MessageBoxIcon.Error,
										MessageBoxDefaultButton.Button1,
										0 );
								}
								else
								{
									products = t.Result;
									DisplayBooks();
								}

								ClearBusyState();
							}, 
						TaskScheduler.FromCurrentSynchronizationContext() );
		}

		/// <summary>
		/// Enable/disable, hide/show controls for when the program is not busy 
		/// </summary>
		private void ClearBusyState()
		{
			vsVersion.Enabled = true;
			languageSelection.Enabled = true;
			loadBooks.Enabled = true;
			downloadBooks.Enabled = (booksList.Items.Count > 0) && !string.IsNullOrEmpty( cacheDirectory.Text );
			browseDirectory.Enabled = true;
			downloadProgress.Style = ProgressBarStyle.Continuous;
			startupTip.Visible = false;
			loadingBooksTip.Visible = false;
		}

		/// <summary>
		/// Enable/disable, hide/show controls for when the program is busy 
		/// </summary>
		private void SetBusyState()
		{
			vsVersion.Enabled = false;
			languageSelection.Enabled = false;
			loadBooks.Enabled = false;
			downloadBooks.Enabled = false;
			browseDirectory.Enabled = false;
		}

		/// <summary>
		/// Populate the list view control with the books available for download
		/// </summary>
		private void DisplayBooks()
		{
			booksList.Items.Clear();
			if ( !string.IsNullOrEmpty( cacheDirectory.Text ) )
			{
				Downloader.CheckPackagesStates( products, cacheDirectory.Text );
			}

			Dictionary<string, ListViewGroup> groups = new Dictionary<string, ListViewGroup>();
			foreach ( BookGroup product in products )
			{
				foreach ( Book book in product.Books )
				{
					// Calculate some details about any prospective download
					long totalSize = 0;
					long downloadSize = 0;
					int packagesOutOfDate = 0;
					int packagesCached = 0;
					foreach ( Package package in book.Packages )
					{
						totalSize += package.Size;
						if ( package.State != PackageState.Ready )
						{
							downloadSize += package.Size;
							packagesOutOfDate++;
						}

						if ( package.State != PackageState.NotDownloaded )
						{
							packagesCached++;
						}
					}

					// Make sure the groups aren't duplicated
					ListViewGroup itemGroup;
					if ( groups.ContainsKey( book.Category ) )
					{
						itemGroup = groups[book.Category];
					}
					else
					{
						itemGroup = booksList.Groups.Add( book.Category, book.Category );
						groups.Add( book.Category, itemGroup );
					}

					ListViewItem item = booksList.Items.Add( book.Name );
					item.SubItems.Add( (totalSize / 1000000).ToString( "F1", CultureInfo.CurrentCulture ) );
					item.SubItems.Add( book.Packages.Count.ToString( CultureInfo.CurrentCulture ) );
					item.SubItems.Add( (downloadSize / 1000000).ToString( "F1", CultureInfo.CurrentCulture ) );
					item.SubItems.Add( packagesOutOfDate.ToString( CultureInfo.CurrentCulture ) );
					item.ToolTipText = book.Description;
					item.Checked = packagesCached > 1;
					book.Wanted = item.Checked;
					item.Tag = book;
					item.Group = itemGroup;
				}
			}
		}

		/// <summary>
		/// Called when the browse for directory button is clicked. Show an folder browser to allow the
		/// user to select a directory to store the cached file in
		/// </summary>
		/// <param name="sender">
		/// The parameter is not used.
		/// </param>
		/// <param name="e">
		/// The parameter is not used.
		/// </param>
		private void BrowseDirectoryClick( object sender, EventArgs e )
		{
			using ( FolderBrowserDialog dialog = new FolderBrowserDialog() )
			{
				dialog.RootFolder = Environment.SpecialFolder.MyComputer;
				dialog.SelectedPath = cacheDirectory.Text;
				dialog.ShowNewFolderButton = true;
				dialog.Description = "Select local cache folder to store selected MSDN Library books";

				if ( DialogResult.OK == dialog.ShowDialog( this ) )
				{
					cacheDirectory.Text = dialog.SelectedPath;
					downloadBooks.Enabled = (booksList.Items.Count > 0) && !string.IsNullOrEmpty( cacheDirectory.Text );
					DisplayBooks();
				}
			}
		}

		/// <summary>
		/// Called when the checkbox of one of the listview items is checked or unchecked. Mark the associated book state
		/// </summary>
		/// <param name="sender">
		/// The parameter is not used.
		/// </param>
		/// <param name="e">
		/// Details about the item checked/unchecked
		/// </param>
		private void BooksListItemChecked( object sender, ItemCheckedEventArgs e )
		{
			Book book = e.Item.Tag as Book;
			if ( book != null )
			{
				book.Wanted = e.Item.Checked;
			}
		}

		/// <summary>
		/// Called when the language combobox selection is changed. Clear the
		/// currently list of available books and reshow the instruction.
		/// </summary>
		/// <param name="sender">
		/// The parameter is not used.
		/// </param>
		/// <param name="e">
		/// The parameter is not used.
		/// </param>
		private void BookOptionsChanged( object sender, EventArgs e )
		{
			booksList.Items.Clear();
			downloadBooks.Enabled = false;
			startupTip.Visible = true;
        }

        /// <summary>
        /// Called when the visual studio language combobox selection is changed. Clear the
        /// currently list of available books and reshow the instruction.
        /// </summary>
        /// <param name="sender">
        /// The parameter is not used.
        /// </param>
        /// <param name="e">
        /// The parameter is not used.
        /// </param>
        private void VsVersionChanged(object sender, EventArgs e)
        {
            booksList.Items.Clear();
            languageSelection.Items.Clear();
            languageSelection.SelectedItem = -1;
            UpdateLocales();
        }
	}
}
