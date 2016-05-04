namespace VisualStudioHelpDownloader2012
{
	using System;
	using System.Globalization;

	/// <summary>
	/// The possible download states for a package
	/// </summary>
	public enum PackageState
	{
		/// <summary>
		/// The package has not been downloaded yet
		/// </summary>
		NotDownloaded, 

		/// <summary>
		/// The package has been previously downloaded but is now out of date
		/// </summary>
		OutOfDate, 

		/// <summary>
		/// The package has been downloaded and is up to date
		/// </summary>
		Ready
	}

	/// <summary>
	///     Represents an MSDN package
	/// </summary>
	internal sealed class Package
	{
		/// <summary>
		///     Gets or sets the last modified time.
		/// </summary>
		public DateTime LastModified
		{
			get;
			set;
		}

		/// <summary>
		///     Gets or sets the tag associated with the package
		/// </summary>
		public string Tag
		{
			get;
			set;
		}

		/// <summary>
		///     Gets or sets the package relative URL for downloading.
		/// </summary>
		public string Link
		{
			get;
			set;
		}

		/// <summary>
		///     Gets or sets the package name
		/// </summary>
		public string Name
		{
			get;
			set;
		}

		/// <summary>
		/// Gets or sets the size in bytes.
		/// </summary>
		public long Size
		{
			get;
			set;
		}

		/// <summary>
		/// Gets or sets the state.
		/// </summary>
		public PackageState State
		{
			get;
			set;
		}

		/// <summary>
		/// Create a file name for the package file
		/// </summary>
		/// <returns>
		/// A string containing the file name
		/// </returns>
		public string CreateFileName()
		{
			return string.Format( CultureInfo.InvariantCulture, "{0}.cab", Name.ToUpperInvariant() );
		}

		/// <summary>
		/// Returns a string representing the object
		/// </summary>
		/// <returns>
		/// String representing the object
		/// </returns>
		public override string ToString()
		{
			return Name ?? "NULL";
		}
	}
}
