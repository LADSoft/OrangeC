namespace HVProject.HV2Lib.Common
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;
    using Microsoft.Win32;


    /// <summary>
    /// Read registered Catalog info from the VS 11 Help registry.
    /// [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs\*]
    /// </summary>

    public static class CatalogList
    {

        public static List<CatalogItem> GetCatalogs()
        {
            List<CatalogItem> resultList = new List<CatalogItem>();
            CatalogItem catalogItem;
            HV2RegInfo hv2RegInfo = new HV2RegInfo();

            RegistryKey baseKeyName = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, hv2RegInfo.RegistryView);
            if (baseKeyName != null)
            {
                //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs] 
                RegistryKey subkey = baseKeyName.OpenSubKey(hv2RegInfo.KEY_Help_v20_Catalogs);
                if (subkey != null)
                {
                    //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs\*] 
                    String[] catList = subkey.GetSubKeyNames();
                    subkey.Close();

                    foreach (String catalogID in catList)
                    {
                        //eg. [HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs\VisualStudio11]
                        String key = hv2RegInfo.KEY_Help_v20_Catalogs + @"\" + catalogID;
                        subkey = baseKeyName.OpenSubKey(key);
                        if (subkey != null)
                        {
                            //eg. LocationPath=C:\ProgramData\Microsoft\HelpLibrary2\Catalogs\VisualStudio11\
                            String LocationPath = (string)subkey.GetValue("LocationPath");

                            //Create new recort
                            catalogItem = new CatalogItem(true, catalogID, LocationPath);  //true = valud is from the registry

                            //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs\VisualStudio11\*] 
                            String[] localeList = subkey.GetSubKeyNames();
                            subkey.Close();

                            //[HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Help\v2.0\Catalogs\VisualStudio11\en-US] 
                            foreach (String locale in localeList)
                            {
                                String localeKey = hv2RegInfo.KEY_Help_v20_Catalogs + @"\" + catalogID + @"\" + locale;
                                subkey = baseKeyName.OpenSubKey(localeKey);
                                if (subkey != null)
                                {
                                    //eg. catalogName=Visual Studio 2011 Help Documentation
                                    String catalogName = (string)subkey.GetValue("catalogName");
                                    //eg. SeedFilePath=C:\Program Files\Microsoft Help Viewer\v2.0\CatalogInfo\VS11_en-us.cab
                                    String SeedFilePath = (string)subkey.GetValue("SeedFilePath");

                                    //Add to results
                                    catalogItem.AddLocaleItem(locale, catalogName, SeedFilePath);
                                    subkey.Close();
                                }
                            }

                            //Add to result list
                            resultList.Add(catalogItem);
                        }
                    }
                }

                baseKeyName.Close();
            }
            return (resultList);
        }



        //There maybe be other collection directories (eg. Windows 8) that we can add to the list of catalogs. This also validates the path.
        public static int WildAddCollection(string path, List<CatalogItem> catList, String metaName)
        {
            if (Directory.Exists(path))
              if (!path.EndsWith("\\"))
                 path = path + "\\";      // for consistency

            //Already in catList? return that index!
            int i = IndexOfCollection(path, catList);
            if (i >= 0)
                return i;

            //A .mshx file?
            if (File.Exists(path))
                return AddCollection(path, catList, metaName);

            //Validate catalog path
            if (!Directory.Exists(path))
                return -1;

            //Is this path valid Collection path? ie. Contains "IndexStore" & "ContentStore" folders (and assume for now locale folders under these)
            if (Directory.Exists(Path.Combine(path, "IndexStore"))
             && Directory.Exists(Path.Combine(path, "ContentStore")))
            {
                return AddCollection(path, catList, metaName);
            }

            //Look a little deeper in the folder structure
            // LINQ query
            int iRet = -1;
            var dirs = Directory.EnumerateDirectories(path);
            foreach (var dir in dirs)
            {
                if (Directory.Exists(Path.Combine(dir, "IndexStore"))
                 && Directory.Exists(Path.Combine(dir, "ContentStore")))
                {
                    iRet = AddCollection(dir, catList, metaName);
                }
            }
            return iRet;
        }

        //No validation: Assumes that the path is a valid collection folder containing both a "IndexStore" and "ContentStore" folder
        private static int AddCollection(string path, List<CatalogItem> catList, String metaName)  //Meta name is optional
        {
            int iRet = catList.Count;
            path = path.TrimEnd(new char[] { '\\' });
            String CatalogID;
            if (String.IsNullOrEmpty(metaName))
                CatalogID = path.Substring(path.LastIndexOf("\\") + 1);   //last segment of path
            else 
                CatalogID = metaName;
            if (Directory.Exists(path))  //it could be an .mshx file
                path = path + "\\";
            CatalogItem catItem = new CatalogItem(CatalogID, path);

            //Add all localeList found under ContentStore
            String contentsStorePath = Path.Combine(path, "ContentStore\\");
            if (Directory.Exists(contentsStorePath))
            {
                var dirs = Directory.EnumerateDirectories(Path.Combine(path, "ContentStore\\"));
                foreach (var dir in dirs)
                {
                    String locale = dir.Substring(dir.LastIndexOf("\\") + 1);
                    if ((locale.Length == 5) && (locale[2] == '-'))   //Looks like a Locale form eg. "en-US"
                    {
                        catItem.AddLocaleItem(locale, metaName, "");
                    }
                }
            }

            catList.Add(catItem);
            return iRet;
        }

        // find dir path in catList. Returns -1 if not found.
        public static int IndexOfCollection(string path, List<CatalogItem> catList)
        {
            if (!path.EndsWith("\\"))
                path = path + "\\";

            for (int i = 0; i < catList.Count; i++)
            {
                String locationPath = catList[i].LocationPath;
                if (!locationPath.EndsWith("\\"))
                    locationPath = locationPath + "\\";
                if (path.Equals(locationPath, StringComparison.CurrentCultureIgnoreCase))
                {
                    return i;
                }
            }
            return -1;
        }

    }
}
