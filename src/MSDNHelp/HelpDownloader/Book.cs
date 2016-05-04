namespace VisualStudioHelpDownloader2012
{
	using System.Collections.Generic;
	using System.Globalization;

	/// <summary>
	///     Represents an MSDN book
	/// </summary>
	internal sealed class Book : ItemBase
	{
		/// <summary>
		/// Gets or sets the display category for the book
		/// </summary>
		public string Category
		{
			get;
			set;
		}

		/// <summary>
		/// Gets or sets a value indicating whether the a download of the book has been requested.
		/// </summary>
		public bool Wanted
		{
			get;
			set;
		}

		/// <summary>
		///    Gets or sets the collection of packages associated with the book
		/// </summary>
		public ICollection<Package> Packages
		{
			get;
			set;
		}

		/// <summary>
		/// Create a file name for the book index file
		/// </summary>
		/// <returns>
		/// A string containing the file name
		/// </returns>
		public string CreateFileName()
		{
			return string.Format( CultureInfo.InvariantCulture, "Book-{0}-{1}.xml", Code, Locale.Name );
		}
	}
}
