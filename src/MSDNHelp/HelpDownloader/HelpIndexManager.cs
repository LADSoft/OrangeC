namespace VisualStudioHelpDownloader2012
{
	using System;
	using System.Collections.Generic;
	using System.Globalization;
	using System.IO;
	using System.Linq;
	using System.Xml;
	using System.Xml.Linq;

	/// <summary>
	///     Helper class for parsing and creating documents with help indexes.
	/// </summary>
	internal static class HelpIndexManager
	{
		/// <summary>
		/// Find the available locales for the help collections
		/// </summary>
		/// <param name="data">
		/// The page data downloaded containing the locale catalog data
		/// </param>
		/// <returns>
		/// Collection of locales in which the help collection may be downloaded
		/// </returns>
		/// <exception cref="ArgumentNullException">
		/// If data is null
		/// </exception>
		/// <exception cref="XmlException">
		/// If there was a error processing the xml data
		/// </exception>
		/// <exception cref="InvalidOperationException">
		/// If there was a error processing the xml data
		/// </exception>
		public static ICollection<Locale> LoadLocales( byte[] data )
		{
			XDocument document;
			List<Locale> result = new List<Locale>();

			if ( data == null )
			{
				throw new ArgumentNullException( "data" );
			}

			using ( MemoryStream stream = new MemoryStream( data ) )
			{
				document = XDocument.Load( stream );
			}

			if ( document.Root != null )
			{
				IEnumerable<XElement> query =
					document.Root.Elements()
							.Where( x => x.GetClassName() == "catalogLocales" )
							.Take( 1 )
							.Single()
							.Elements()
							.Where( x => x.GetClassName() == "catalog-locale-list" )
							.Take( 1 )
							.Single()
							.Elements()
							.Where( x => x.GetClassName() == "catalog-locale" );

				result.AddRange(
					query.Select(
						x =>
						new Locale
							{
								Code = x.GetChildClassValue( "locale" ),
								CatalogLink = x.GetChildClassAttributeValue( "locale-link", "href" )
							} ) );
			}
			else
			{
				throw new XmlException( "Missing document root" );
			}

			return result;
		}

		/// <summary>
		/// Find the available book groups, books, and packages for the selected language
		/// </summary>
		/// <param name="data">
		/// The page data downloaded containing the book data
		/// </param>
		/// <returns>
		/// Collection of book groups for the help collection
		/// </returns>
		/// <exception cref="ArgumentNullException">
		/// If data is null
		/// </exception>
		/// <exception cref="XmlException">
		/// If there was a error processing the xml data
		/// </exception>
		/// <exception cref="InvalidOperationException">
		/// If there was a error processing the xml data
		/// </exception>
		public static ICollection<BookGroup> LoadBooks( byte[] data )
		{
			XDocument document;
			using ( MemoryStream stream = new MemoryStream( data ) )
			{
				document = XDocument.Load( stream );
			}

			if ( data == null )
			{
				throw new ArgumentNullException( "data" );
			}

			List<BookGroup> result = new List<BookGroup>();

			if ( document.Root != null )
			{
				IEnumerable<XElement> groups =
					document.Root.Elements()
							.Where( x => x.GetClassName() == "book-list" )
							.Take( 1 )
							.Single()
							.Elements()
							.Where( x => x.GetClassName() == "book-groups" )
							.Take( 1 )
							.Single()
							.Elements()
							.Where( x => x.GetClassName() == "book-group" );
				foreach ( XElement group in groups )
				{
					BookGroup bookGroup = new BookGroup
										{
											Name = group.GetChildClassValue( "name" ),
											Code = group.GetChildClassValue( "id" ),
											Books = new List<Book>(),
											Locale = new Locale { Code = "en-us" },
											Description = group.GetChildClassValue( "name" )
										};

					result.Add( bookGroup );

					IEnumerable<XElement> books = group.Elements().Where( x => x.GetClassName() == "book" );
					foreach ( XElement book in books )
					{
						XElement path =
							book.Elements()
								.Where( x => x.GetClassName() == "properties" )
								.Take( 1 )
								.Single()
								.Elements()
								.Where( x => x.GetClassName() == "paths" )
								.Take( 1 )
								.Single()
								.Elements()
								.Where( x => x.GetClassName() == "path" )
								.Take( 1 )
								.Single();
						string bookPath = path.GetChildClassValue( "name" ).TrimStart( new[] { '\\' } );

						Book b = new Book
									{
										Name = book.GetChildClassValue( "name" ),
										Description = book.GetChildClassValue( "description" ),
										Code = book.GetChildClassValue( "id" ),
										Packages = new List<Package>(),
										Locale = new Locale { Code = "en-us" },
										Category = bookPath
									};

						bookGroup.Name = bookPath;
						bookGroup.Books.Add( b );
						IEnumerable<XElement> packages =
							book.Elements()
								.Where( x => x.GetClassName() == "packages" )
								.Take( 1 )
								.Single()
								.Elements()
								.Where( x => x.GetClassName() == "package" );
						foreach ( XElement package in packages )
						{
							Package p = new Package
											{
												LastModified = DateTime.Parse( package.GetChildClassValue( "last-modified" ), CultureInfo.InvariantCulture ),
												Link = package.GetChildClassValue( "current-link" ),
												Tag = package.GetChildClassValue( "package-etag" ),
												Name = package.GetChildClassValue( "name" ),
												Size = long.Parse( package.GetChildClassValue( "package-size-bytes" ), CultureInfo.InvariantCulture )
											};

							b.Packages.Add( p );
						}
					}
				}				
			}
			else
			{
				throw new XmlException( "Missing document root" );
			}

			return result;
		}

		/// <summary>
		/// Creates main help setup index.
		/// </summary>
		/// <param name="bookGroups">
		/// A collection of book groups to add to the index
		/// </param>
		/// <returns>
		/// The xml document text
		/// </returns>
		public static string CreateSetupIndex( IEnumerable<BookGroup> bookGroups )
		{
			XDocument document = new XDocument( new XDeclaration( "1.0", "utf-8", null ), CreateElement( "html", null, null ) );

			XElement bodyElement = CreateElement( "body", "product-list", null );

			foreach ( BookGroup bookGroup in bookGroups )
			{
				XElement productElement = CreateElement( "div", "product", null );

				XElement linkElement = CreateElement( "a", "product-link", null );
				linkElement.SetAttributeValue( XName.Get( "href", string.Empty ), bookGroup.CreateFileName() );

				productElement.Add(
					CreateElement( "span", "name", bookGroup.Name ), 
					CreateElement( "span", "locale", bookGroup.Locale.Code ), 
					CreateElement( "span", "description", bookGroup.Description ), 
					linkElement );

				bodyElement.Add( productElement );
			}

			if ( document.Root != null )
			{
				document.Root.Add( bodyElement );
			}

			return document.ToStringWithDeclaration();
		}

		/// <summary>
		/// Create book group books index.
		/// </summary>
		/// <param name="bookGroup">
		/// The book group to create the index for.
		/// </param>
		/// <returns>
		/// The xml document text
		/// </returns>
		public static string CreateBookGroupBooksIndex( BookGroup bookGroup )
		{
			XDocument document = new XDocument( new XDeclaration( "1.0", "utf-8", null ), CreateElement( "html", null, null ) );

			XElement headElement = CreateElement( "head", null, null );
			XElement metaDateElemet = CreateElement( "meta", null, null );
			metaDateElemet.SetAttributeValue( XName.Get( "http-equiv", string.Empty ), "Date" );
			metaDateElemet.SetAttributeValue( XName.Get( "content", string.Empty ), DateTime.Now.ToString( "R", CultureInfo.InvariantCulture ) );
			headElement.Add( metaDateElemet );

			XElement bodyElement = CreateElement( "body", "product", null );
			XElement detailsElement = CreateElement( "div", "details", null );
			detailsElement.Add(
				CreateElement( "span", "name", bookGroup.Name ), 
				CreateElement( "span", "locale", bookGroup.Locale.Code ), 
				CreateElement( "span", "description", bookGroup.Description ) );
			XElement bookListElement = CreateElement( "div", "book-list", null );

			foreach ( Book book in bookGroup.Books )
			{
				if ( book.Wanted )
				{
					XElement bookElement = CreateElement( "div", "book", null );

					XElement linkElement = CreateElement( "a", "book-link", null );
					linkElement.SetAttributeValue( XName.Get( "href", string.Empty ), book.CreateFileName() );

					bookElement.Add(
						CreateElement( "span", "name", book.Name ),
						CreateElement( "span", "locale", book.Locale.Code ),
						CreateElement( "span", "description", book.Description ),
						linkElement );

					bookListElement.Add( bookElement );					
				}
			}

			bodyElement.Add( detailsElement, bookListElement );
			if ( document.Root != null )
			{
				document.Root.Add( headElement, bodyElement );
			}

			return document.ToStringWithDeclaration();
		}

		/// <summary>
		/// Create book packages index.
		/// </summary>
		/// <param name="bookGroup">
		/// The book Group associated with the book.
		/// </param>
		/// <param name="book">
		/// The book associated with the packages
		/// </param>
		/// <returns>
		/// The xml document text
		/// </returns>
		public static string CreateBookPackagesIndex( BookGroup bookGroup, Book book )
		{
			XDocument document = new XDocument( new XDeclaration( "1.0", "utf-8", null ), CreateElement( "html", null, null ) );

			XElement headElement = CreateElement( "head", null, null );
			XElement metaDateElemet = CreateElement( "meta", null, null );
			metaDateElemet.SetAttributeValue( XName.Get( "http-equiv", string.Empty ), "Date" );
			metaDateElemet.SetAttributeValue( XName.Get( "content", string.Empty ), DateTime.Now.ToString( "R", CultureInfo.InvariantCulture ) );
			headElement.Add( metaDateElemet );

			XElement bodyElement = CreateElement( "body", "book", null );
			XElement detailsElement = CreateElement( "div", "details", null );

			XElement brandingPackageElement1 = CreateElement( "a", "branding-package-link", null );
			XElement brandingPackageElement2 = CreateElement( "a", "branding-package-link", null );
			XElement productLinkElement = CreateElement( "a", "product-link", null );

			productLinkElement.SetAttributeValue( XName.Get( "href", string.Empty ), bookGroup.CreateFileName() );

			detailsElement.Add(
				CreateElement( "span", "name", book.Name ), 
				CreateElement( "span", "description", book.Description ), 
				CreateElement( "span", "vendor", "Microsoft" ), 
				CreateElement( "span", "locale", book.Locale.Code ), 
				brandingPackageElement1, 
				brandingPackageElement2 );

			XElement packageListElement = CreateElement( "div", "package-list", null );

			foreach ( Package package in book.Packages )
			{
				XElement packageElement = CreateElement( "div", "package", null );
				XElement linkElement = CreateElement( "a", "current-link", null );

				linkElement.SetAttributeValue(
					XName.Get( "href", string.Empty ), 
					string.Format( CultureInfo.InvariantCulture, @"Packages\{0}", package.CreateFileName() ) );

				packageElement.Add(
					CreateElement( "span", "name", package.Name ), 
					CreateElement( "span", "deployed", "true" ), 
					CreateElement( "span", "package-etag", package.Tag ), 
					CreateElement( "span", "last-modified", package.LastModified.ToUniversalTime().ToString( "O", CultureInfo.InvariantCulture ) ), 
					linkElement );

				packageListElement.Add( packageElement );
			}

			bodyElement.Add( detailsElement, packageListElement );
			if ( document.Root != null )
			{
				document.Root.Add( headElement, bodyElement );
			}

			return document.ToStringWithDeclaration();
		}

		/// <summary>
		/// Create a new xml element
		/// </summary>
		/// <param name="name">
		/// The name of the element
		/// </param>
		/// <param name="className">
		/// The class attribute value (may be null)
		/// </param>
		/// <param name="value">
		/// The element content (may be null)
		/// </param>
		/// <returns>
		/// The created element
		/// </returns>
		private static XElement CreateElement( string name, string className, string value )
		{
			XElement element = new XElement( XName.Get( name, "http://www.w3.org/1999/xhtml" ) );

			if ( className != null )
			{
				element.SetAttributeValue( XName.Get( "class", string.Empty ), className );
			}

			if ( value != null )
			{
				element.Value = value;
			}

			return element;
		}

		/// <summary>
		/// Get the name of the class of an xml element
		/// </summary>
		/// <param name="element">
		/// The element to get the class name of
		/// </param>
		/// <returns>
		/// The class name or null if there is no class name
		/// </returns>
		private static string GetClassName( this XElement element )
		{
			return GetAttributeValue( element, "class" );
		}

		/// <summary>
		/// The the value of an attribute of an xml element
		/// </summary>
		/// <param name="element">
		/// The element to get the attribute value from
		/// </param>
		/// <param name="name">
		/// The name of the attrbute to query
		/// </param>
		/// <returns>
		/// The attribute value or null if the attribute was not found
		/// </returns>
		private static string GetAttributeValue( this XElement element, string name )
		{
			XAttribute attribute = element.Attribute( XName.Get( name, string.Empty ) );

			return attribute == null ? null : attribute.Value;
		}

		/// <summary>
		/// Get the value of the first child element of the specified element with the class attribute that matched
		/// the specified name
		/// </summary>
		/// <param name="element">
		/// The element to get the child class value from
		/// </param>
		/// <param name="name">
		/// The class name to find
		/// </param>
		/// <returns>
		/// The value of the child class element
		/// </returns>
		/// <exception cref="InvalidOperationException">
		/// If there was no child element with the class attribute
		/// </exception>
		private static string GetChildClassValue( this XElement element, string name )
		{
			XElement result = element.Elements().Where( x => x.GetClassName() == name ).Take( 1 ).Single();

			return null != result ? result.Value : null;
		}

		/// <summary>
		/// Get the value of the specified attribute of the first child element of the specified element with the 
		/// class attribute that matched the specified name
		/// </summary>
		/// <param name="element">
		/// The element to get the child attribute value from
		/// </param>
		/// <param name="name">
		/// The class name to find
		/// </param>
		/// <param name="attribute">
		/// The attribute name to find
		/// </param>
		/// <returns>
		/// The value of the attribute
		/// </returns>
		/// <exception cref="InvalidOperationException">
		/// If there was no child element with the class attribute
		/// </exception>
		private static string GetChildClassAttributeValue( this XElement element, string name, string attribute )
		{
			XElement result = element.Elements().Where( x => x.GetClassName() == name ).Take( 1 ).Single();

			return null != result ? result.GetAttributeValue( attribute ) : null;
		}

		/// <summary>
		/// XDocument extension method to get the XML text including the declaration from an XDocument
		/// </summary>
		/// <param name="document">
		/// The document to get the xml text from
		/// </param>
		/// <returns>
		/// The xml text for the document
		/// </returns>
		private static string ToStringWithDeclaration( this XDocument document )
		{
			return document.Declaration == null ?
				document.ToString() :
				string.Format( CultureInfo.InvariantCulture, "{0}{1}{2}", document.Declaration, Environment.NewLine, document );
		}
	}
}
