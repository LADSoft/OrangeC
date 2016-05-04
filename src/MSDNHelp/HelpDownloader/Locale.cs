namespace VisualStudioHelpDownloader2012
{
	/// <summary>
	/// The locale.
	/// </summary>
	public sealed class Locale
	{
		/// <summary>
		///  Gets the normalized locale name display.
		/// </summary>
		public string Name
		{
			get
			{
				return NormalizeLocale( Code );
			}
		}

		/// <summary>
		///   Gets or sets the locale code.
		/// </summary>
		public string Code
		{
			get;
			set;
		}

		/// <summary>
		/// Gets or sets the relative location of the catalog page associated with the locale
		/// </summary>
		public string CatalogLink
		{
			get;
			set;
		}

		/// <summary>
		/// Returns a string representing the object
		/// </summary>
		/// <returns>
		/// The Name of the locale
		/// </returns>
		public override string ToString()
		{
			return Name;
		}

		/// <summary>
		/// Normalizes the locale code 
		/// </summary>
		/// <param name="value">
		/// The locale code to normalize
		/// </param>
		/// <returns>
		/// The normalized locale code
		/// </returns>
		private static string NormalizeLocale( string value )
		{
			if ( null == value )
			{
				return string.Empty;
			}

			string[] parts = value.Split( '-' );

			if ( parts.Length != 2 )
			{
				return value;
			}

			return string.Join( "-", parts[0], parts[1].ToUpperInvariant() );
		}
	}
}
