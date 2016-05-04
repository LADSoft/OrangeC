namespace VisualStudioHelpDownloader2012
{
	using System.Collections.Generic;
	using System.Globalization;

	/// <summary>
	///     Represents an MSDN book group
	/// </summary>
	internal sealed class BookGroup : ItemBase
	{
		/// <summary>
		///     Gets or sets the books associated with the book group
		/// </summary>
		public ICollection<Book> Books
		{
			get;
			set;
		}

		/// <summary>
		/// Create a file name for the book group index file
		/// </summary>
		/// <returns>
		/// A string containing the file name
		/// </returns>
		public string CreateFileName()
		{
			return string.Format( CultureInfo.InvariantCulture, "product-{0}.xml", Code );
		}
	}
}
