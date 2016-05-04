namespace HVProject.HV2Lib.Common
{
    using System;
	using System.IO;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    /// <summary>
    /// Read registered Catalog info from the VS 11 Help registry.
    /// [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs]
    /// Normally this is a list of categories found. Each catalog can contain one or more locales.
    /// The flat list flattens this list out to Catalog|Locale1, Catalog|Locale2, ...
    /// </summary>

    public class CatalogItem
    {
        public readonly Boolean FromRegistry = false;  //True if these items were read from the registry. False if we added for convenience.
        public readonly String CatalogID;              //eg VisualStudio11 (This is a key in the registry)     
        public readonly String LocationPath;           //eg C:\ProgramData\Microsoft\HelpLibrary2\Catalogs\VisualStudio11\
        public List<CatalogLocaleItem> Locales = new List<CatalogLocaleItem>();

        public CatalogItem(Boolean FromRegistry, String CatalogID, String LocationPath)
        {
            this.FromRegistry = FromRegistry;
            this.CatalogID = CatalogID;
            this.LocationPath = LocationPath;
        }

        public CatalogItem(String CatalogID, String LocationPath) : this(false, CatalogID, LocationPath) { }

        public void AddLocaleItem(String Locale, String CatalogName, String SeedFilePath)
        {
            Locales.Add(new CatalogLocaleItem(Locale, CatalogName, SeedFilePath));
        }
    }

    public class CatalogLocaleItem              //Each catalog has one or more localeList
    {
        public readonly String Locale;          //eg en-US
        public readonly String CatalogName;     //eg Visual Studio 2011 Help Documentation
        public readonly String SeedFilePath;    //eg C:\Program Files\Microsoft Help Viewer\v2.0\CatalogInfo\VS11_en-us.cab

        public CatalogLocaleItem(String Locale, String CatalogName, String SeedFilePath)
        {
            this.Locale = Locale;
            this.CatalogName = CatalogName;
            this.SeedFilePath = SeedFilePath;
        }
    }


    // Some times easier to flatten out the catalog list... means repeating CatalogID and LocationPath but simplifies displaying a list of catalogs

    public class FlatCatalogListItem
    {
        public readonly Boolean FromRegistry = false;  //True if these items were read from the registry. False if we added for convenience.
        public readonly String CatalogID;       //eg VisualStudio11 (This is the key in the registry)     
        public readonly String LocationPath;    //eg C:\ProgramData\Microsoft\HelpLibrary2\Catalogs\VisualStudio11\
        public readonly String Locale;          //eg en-US
        public readonly String CatalogName;     //eg Visual Studio 2011 Help Documentation
        public readonly String SeedFilePath;    //eg C:\Program Files\Microsoft Help Viewer\v2.0\CatalogInfo\VS11_en-us.cab

        public FlatCatalogListItem(Boolean FromRegistry, String CatalogID, String LocationPath, String Locale, String CatalogName, String SeedFilePath)
        {
            this.FromRegistry = FromRegistry;
            this.CatalogID = CatalogID;
            this.LocationPath = LocationPath;
            this.Locale = Locale;
            this.CatalogName = CatalogName;
            this.SeedFilePath = SeedFilePath;
        }

        public FlatCatalogListItem(FlatCatalogListItem item)
        {
            this.FromRegistry = item.FromRegistry;
            this.CatalogID = item.CatalogID;
            this.LocationPath = item.LocationPath;
            this.Locale = item.Locale;
            this.CatalogName = item.CatalogName;
            this.SeedFilePath = item.SeedFilePath;
        }
    }

    public static class FlatCatalogList
    {
        public static List<FlatCatalogListItem> MakeList(List<CatalogItem> catalogList)
        {
            List<FlatCatalogListItem> list = new List<FlatCatalogListItem>();

            catalogList.ForEach(delegate(CatalogItem catalog)
            {
                catalog.Locales.ForEach(delegate(CatalogLocaleItem catlocale)
                {
                    list.Add(new FlatCatalogListItem(catalog.FromRegistry, catalog.CatalogID, catalog.LocationPath, 
                        catlocale.Locale, catlocale.CatalogName, catlocale.SeedFilePath));
                });

                //We also allow .mshc files in the list -- In this case we don't know the locale
                if (catalog.Locales.Count == 0)
                {
                    list.Add(new FlatCatalogListItem(catalog.FromRegistry, catalog.CatalogID, catalog.LocationPath, "", "", ""));
                }

            });

            return list;
        }

        public static List<FlatCatalogListItem> GetAll()
        {
			string OrangeCHelpFileDirectory = Path.Combine( Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Orange C", "MSDN  Library" );
            List<CatalogItem> resultList = new List<CatalogItem>();
			CatalogList.WildAddCollection(OrangeCHelpFileDirectory, resultList, @"MSDN Library");
			return MakeList(resultList);
//            return  MakeList(CatalogList.GetCatalogs());
        }


    }


}
