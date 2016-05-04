using Microsoft.Win32;
using System;
using System.Configuration;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Security.AccessControl;
using System.Security.Principal;
using System.Text;
using System.Diagnostics;
using System.Xml;
using System.Collections.Generic;
using HVProject.HV2Lib.Common;

namespace HVProject.HV2Lib.LocalStore
{
    /// <summary>
    /// [ContentPath]\CatalogType.xml - This file is in the root of each VS catalog folder. 
    /// Windows Help Catalog will be locked (MS and partners only) but VS Help is marked "UserManaged".
    /// CatalogType.xml contains a value catalogType=UserManaged. if present the catalog can be managed by the user.
    /// <?xml version="1.0" encoding="utf-8"?>
    /// <catalogType>UserManaged</catalogType>
    /// 
    /// </summary>
    /// <example>
    /// CatalogInfo catalog = new CatalogInfo(@"C:\ProgramData\Microsoft\HelpLibrary2\Catalogs\VisualStudio11");
    /// </example>

    public class CatalogParser
    {
        public readonly Boolean CatalogTypeXmlFound = false;       //<catalogPath>\CatalogType.xml found
        public readonly Boolean UserManaged = false;                    //content of CatalogType.xml
        public readonly String CatalogLocation = "";
        
        private InstalledBooks _installedBooks = null;                  //Contents of <catalogPath>\ContentStore\InstalledBooks.xml

        public CatalogParser(String catalogLocation)  //constructor requires a valid catalog directory (something containing a "ContentStore" folder)
        {
            this.CatalogLocation = catalogLocation;

            CatalogTypeXmlFound = File.Exists(Path.Combine(catalogLocation, "CatalogType.xml"));
            if (CatalogTypeXmlFound)    // <CatalogLocation>\CatalogType.xml
            {
                UserManaged = ReadFile_CatalogTypeXml();
            }

            //Read <CatalogLocation>\ContentStore\BooksInstalled.xml
            ReadFile_InstalledBooksXml();
        }

        
        public InstalledBooks InstalledBooks
        {
            get
            {
                return _installedBooks;
            }
        }


        // File: <catalogPath>\CatalogType.xml
        // <?xml version="1.0" encoding="utf-8"?>
        // <catalogType>UserManaged</catalogType>

        private Boolean ReadFile_CatalogTypeXml()   //Returns if UserManaged is enabled (currently the only Catalog setting in this file)
        {
            String path = Path.Combine(CatalogLocation, "CatalogType.xml"); 

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(path);
            XmlNode node = xmlDoc.SelectSingleNode("//catalogType");       //root node
            if (node != null)
                return (node.InnerXml.Equals("UserManaged", StringComparison.OrdinalIgnoreCase));
            return false;
        }

        // File: <catalogPath>\ContentStore\installedBooks.5.xml

        private void ReadFile_InstalledBooksXml()   //pass in the filename without 
        {
            String baseFilePath = Path.Combine(CatalogLocation, "ContentStore", "installedBooks.xml");
            String path = FileUtility.GetFileName(baseFilePath);  //Returns the actualy (latest version) filename
            if (!File.Exists(path))
                return;

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(path);
            XmlNode nodeInstalledBooks = xmlDoc.SelectSingleNode("//installed-books");      //root node
            if (nodeInstalledBooks == null)
                return;

            this._installedBooks = new InstalledBooks();

            //Check version -- Should be 1.0
            if (nodeInstalledBooks.Attributes["version"] != null)
            {
                _installedBooks.version = nodeInstalledBooks.Attributes["version"].InnerXml;
            }

            //<installed-books>
            //  <branding-packages>
            //    <brandingpackage locale="EN-US" vendorname="MICROSOFT" package="VISUALSTUDIO_2011_BRANDING_EN-US" />
            //  </branding-packages>
            //</installed-books>

            XmlNode nodeBrandingPackages = nodeInstalledBooks.SelectSingleNode("//branding-packages");
            if (nodeBrandingPackages != null && nodeBrandingPackages.HasChildNodes)
            {
                foreach (XmlNode n in nodeBrandingPackages.ChildNodes)
                {
                    if (n.Name == "brandingpackage"
                        && n.Attributes["locale"] != null 
                        && n.Attributes["vendorname"] != null
                        && n.Attributes["package"] != null)
                    {
                        _installedBooks.brandingPackages.Add(new BrandingPackage(
                            n.Attributes["locale"].InnerXml,
                            n.Attributes["vendorname"].InnerXml,
                            n.Attributes["package"].InnerXml));
                    }
                }
            }

            //<installed-books version="3">
            //  <locale-membership>
            //     <locale name="EN-US">
            //     </locale>
            //     ...
            //  </locale-membership>
            //</installed-books>

            XmlNode nodeLocaleMembership = nodeInstalledBooks.SelectSingleNode("//locale-membership");
            if (nodeLocaleMembership == null || !nodeLocaleMembership.HasChildNodes)
                return;
            foreach (XmlNode n in nodeLocaleMembership.ChildNodes)   // <locale name="EN-US">... <locale name="fr-fr"> etc
            {
                if (n.Name == "locale" && n.Attributes["name"] != null)
                {
                    LocaleListItem localeItem = new LocaleListItem(n.Attributes["name"].InnerXml);
                    _installedBooks.localeList.Add(localeItem);

                    // Get Locale Book Lists
                    GetBookList(localeItem, n);

                }
            }
        }


        //<installed-books version="3">
        //  <locale-membership>
        //     <locale name="EN-US">   <---- nodeLocale
        //        <book-list>
        //           <book ref="urn:852e209f-1bdd-1349-13ba-eed3c6d86c9f" vendorname="Microsoft" displayname=".NET Framework 4" source="http://b25ddepxvm17.redmond.corp.microsoft.com:202/catalogs/visualstudio11/en-us" kind="Microsoft" productname="" productgroupname="" path="\Visual Studio\Visual Studio 2011\Visual Studio 2011 Ultimate" />
        //           ..
        //        </book-list> 
        //        <book-membership>
        //           <book ref="urn:852e209f-1bdd-1349-13ba-eed3c6d86c9f"> 
        //              <package ref="VISUAL_STUDIO_21800791_VS_100_EN-US_2" />
        //              ..
        //           </book> 
        //           ..
        //        </book-membership>
        //     </locale>
        //     ...
        //  </locale-membership>
        //</installed-books>

        private void GetBookList(LocaleListItem localeItem, XmlNode nodeLocale)
        {
            // Get Book List

            XmlNode nodeBookList = nodeLocale.SelectSingleNode("//book-list");  // <locale name="EN-US"><book-list>
            if (nodeBookList == null || !nodeBookList.HasChildNodes)
                return;

            foreach (XmlNode n in nodeBookList.ChildNodes)  
            {
                if (n.Name == "book" && n.Attributes["ref"] != null)    // <book ref="urn:852e209f-1bdd-1349-13ba-eed3c6d86c9f" ...
                {
                    BookListItem bookListItem = new BookListItem(n.Attributes["ref"].InnerXml);
                    if (n.Attributes["vendorname"] != null)
                        bookListItem.vendorname = n.Attributes["vendorname"].InnerXml;
                    if (n.Attributes["displayname"] != null)
                        bookListItem.displayname = n.Attributes["displayname"].InnerXml;
                    if (n.Attributes["source"] != null)
                        bookListItem.source = n.Attributes["source"].InnerXml;
                    if (n.Attributes["kind"] != null)
                        bookListItem.kind = n.Attributes["kind"].InnerXml;
                    if (n.Attributes["productname"] != null)
                        bookListItem.productname = n.Attributes["productname"].InnerXml;
                    if (n.Attributes["productgroupname"] != null)
                        bookListItem.productgroupname = n.Attributes["productgroupname"].InnerXml;
                    if (n.Attributes["path"] != null)
                        bookListItem.path = n.Attributes["path"].InnerXml;

                    localeItem.bookList.Add(bookListItem);
                }
            }

            // Get Book Packages

            XmlNode nodeBookMembership = nodeLocale.SelectSingleNode("//book-membership");   // <locale name="EN-US"><book-membership>
            if (nodeBookMembership == null || !nodeBookMembership.HasChildNodes)
                return;

            foreach (XmlNode nodeBook in nodeBookMembership.ChildNodes)
            {
                if (nodeBook.Name == "book" && nodeBook.Attributes["ref"] != null)  // <book ref="urn:852e209f-1bdd-1349-13ba-eed3c6d86c9f"> 
                {
                    String BookRef = nodeBook.Attributes["ref"].InnerXml;

                    //Build a package list
                    String[] packageList = new String[nodeBook.ChildNodes.Count];
                    int x = 0;
                    foreach (XmlNode nodePackage in nodeBook.ChildNodes)
                    {
                        packageList[x] = nodePackage.Attributes["ref"].InnerXml;    //  <book ..><package ref="VISUAL_STUDIO_21800791_VS_100_EN-US_2" />...
                        x++;
                    }

                    // Insert this list into the appropriate BookListItem that we created above 
                    // First find the linkNode using the ref member

                    for (int i = 0; i < localeItem.bookList.Count; i++)
                    {
                        if (localeItem.bookList[i].ref_ == BookRef)
                        {
                            localeItem.bookList[i].packageList = packageList;
                            break;
                        }
                    }
                }
            }
        }




        // Step though all packages and read info from <catalogPath>\ContentStore\<locale>\<packageName>.metadata

        // <?xml version="1.0" encoding="utf-8"?>
        // <Metadata>
        //    <ETag>31fa5d6a-3352-37e3-21ad-d2d97e597cbb</ETag>
        //    <Date>08/05/2011 19:58:28</Date>
        //    <Url>http://packages.mtps.microsoft.com/visual_studio_27029139_vs_100_en-us_1(31fa5d6a-3352-37e3-21ad-d2d97e597cbb).cab</Url>
        //    <Size>7429904</Size>
        //    <PackageType>content</PackageType>
        //    <VendorName>Microsoft</VendorName>
        // </Metadata>

        public Dictionary<String, PackageMetaData> GetPackageMetaData()
        {
            Dictionary<String, PackageMetaData> metadataDictionary = new Dictionary<String, PackageMetaData>();

            if (_installedBooks == null)
            {
                return metadataDictionary;
            }

            foreach (LocaleListItem localeListItem in _installedBooks.localeList)
            {
                // <catalogPath>\ContentStore\<locale>\<packageName>.metadata

                String localeDir = Path.Combine(CatalogLocation, "ContentStore", localeListItem.locale);
                if (!Directory.Exists(localeDir))
                    continue;

                foreach (BookListItem book in localeListItem.bookList)
	            {
                    //Each book hasa list of packages
                    for (int i = 0; i < book.packageList.Length; i++)
			        {
                        String packageName = book.packageList[i];

                        if (metadataDictionary.ContainsKey(packageName))  //Do we already have this package meta info?
                            continue;

                        String baseFilePath = Path.Combine(localeDir, packageName + ".metadata");
                        String metaDataFile = FileUtility.GetFileName(baseFilePath);
                        if (File.Exists(metaDataFile))
                        {
                            XmlDocument xmlDoc = new XmlDocument();
                            xmlDoc.Load(metaDataFile);
                            
                            XmlNode nodeMetaData = xmlDoc.SelectSingleNode("//Metadata");       //root node
                            if (nodeMetaData != null)
                            {
                                PackageMetaData metaData = new PackageMetaData();

                                foreach (XmlNode node in nodeMetaData.ChildNodes)
                                {
                                    if (node.Name == "ETag") metaData.ETag = node.InnerXml;
                                    else if (node.Name == "Date") metaData.Date = node.InnerXml;
                                    else if (node.Name == "Url") metaData.Url = node.InnerXml;
                                    else if (node.Name == "Size") metaData.Size = node.InnerXml;
                                    else if (node.Name == "PackageType") metaData.PackageType = node.InnerXml;
                                    else if (node.Name == "VendorName") metaData.VendorName = node.InnerXml;
                                }

                                metadataDictionary.Add(packageName, metaData);
                            }
                        }
			        }
	            }
            }

            return metadataDictionary;
        }
    }


    // =================================================================
    // InstalledBooks records
    // =================================================================

    public class InstalledBooks 
    {
        public String version = "";   //at time of writing (Develoepr Previewer version) this = "3"
        public List<LocaleListItem> localeList = new List<LocaleListItem>();   //list ready to add to -- Contains main book list
        public List<BrandingPackage> brandingPackages = new List<BrandingPackage>();   //list ready to add to
    }

    public class LocaleListItem
    {
        public readonly String locale = "";
        public List<BookListItem> bookList = new List<BookListItem>();

        public LocaleListItem(String locale)
        {
            this.locale = locale;
        }
    }

    public class BookListItem
    {
        public readonly String ref_ = ""; //eg. "urn:852e209f-1bdd-1349-13ba-eed3c6d86c9f" 
        public String vendorname = ""; //eg. "Microsoft" 
        public String displayname = ""; //eg. ".NET Framework 4" 
        public String source = ""; //eg. "http://b25ddepxvm17.redmond.corp.microsoft.com:202/catalogs/visualstudio11/en-us" 
        public String kind = ""; //eg. "Microsoft" 
        public String productname = ""; //eg. "" 
        public String productgroupname = ""; //eg. "" 
        public String path = ""; //eg. "\Visual Studio\Visual Studio 2011\Visual Studio 2011 Ultimate" />
        public String[] packageList;  //eg. "VISUAL_STUDIO_21800791_VS_100_EN-US_2" ... ...

        public BookListItem(String ref_)
        {
            this.ref_ = ref_;
        }
    }

    public class BrandingPackage
    {
        public readonly String locale;
        public readonly String vendorName;
        public readonly String package;

        public BrandingPackage(String locale, String vendorName, String package)
        {
            this.locale = locale;
            this.vendorName = vendorName;
            this.package = package;
        }
    }



    // .MetaData files: <catalogPath>\ContentStore\<locale>\<packageName>.metadata
    // One for each package...

    // <?xml version="1.0" encoding="utf-8"?>
    // <Metadata>
    //    <ETag>31fa5d6a-3352-37e3-21ad-d2d97e597cbb</ETag>
    //    <Date>08/05/2011 19:58:28</Date>
    //    <Url>http://packages.mtps.microsoft.com/visual_studio_27029139_vs_100_en-us_1(31fa5d6a-3352-37e3-21ad-d2d97e597cbb).cab</Url>
    //    <Size>7429904</Size>
    //    <PackageType>content</PackageType>
    //    <VendorName>Microsoft</VendorName>
    // </Metadata>

    public class PackageMetaData
    {
        public String ETag = "";
        public String Date = "";
        public String Url = "";
        public String Size = "";
        public String PackageType = "";
        public String VendorName = "";
    }


}
