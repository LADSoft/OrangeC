
// A Query can be passed to the hlpViewer.exe using the /helpQuery command line switch
// eg. HelpViewer.exe /catalogName VisualStudio11 /helpQuery method=f1&query=VS.TextEditor&LCID=1033&LCID=1033&TargetFrameworkMoniker=.NETFramework,Version%3Dv4.0&DevLang=csharp /locale en-US /sku 3000 /launchingApp Microsoft,VisualStudio,11.0

// Or a page URI 
// eg. ms-xhelp:///?method=f1&query=VS.TextEditor&locale=en-US&format=html&targetFrameworkMoniker=.NETFramework,Version%3Dv4.0&devLang=csharp
// eg. ms-xhelp:///?method=page&id=-1&format=html&product=VS&productVersion=100&locale=en-us

// method=page, children, ancestors, toc, asset  ---- All use a queryName="&id="
// All other: method=f1, keywords, search,  --- All use a queryName ==> &query=

// Use Uri.UnescapeDataString() on some parameters before calling

// ms-xhelp:///?method=page&id=SOUTHPARK.INDEX&locale=en-US&vendor=ACME%20Company&format=html&topicversion=-1&topiclocale=EN-US


namespace HVProject.HV2Lib.Render
{
    using System;
    using System.Linq;
    using System.Runtime.InteropServices;
    using System.Text;

    public class HelpQuery
    {
        private string assetId;
        private string devLang;
        private bool isCompress;           //mshx
        private bool isExactTopicMatch;
        private bool isPrint;
        private string locale;
        private string method;            // eg. method=page or method=f1
        private string package;
        private string queryName;         // "&id=" or "&query=" depending on method
        private string queryValue;       
        private string returnFormat;
        private string targetFrameworkMoniker;
        private string targetOS;
        private string topicLocale;
        private string topicVersion;
        private string vendor;


        public HelpQuery(string helpQuery, bool isPrint = false, bool isCompress = true)
        {
            this.isPrint = isPrint;
            this.isCompress = isCompress;
            this.ParseHelpQuery(helpQuery);
            this.SetQueryNameBasedOnMethod(this.method);
        }

        public HelpQuery(string method, string query, string returnFormat, string topicVersion, string topicLocale, string locale, string vendor, bool isPrint = false, bool isCompress = true)
        {
            this.method = method;
            if (string.IsNullOrEmpty(method))
            {
                throw new ArgumentNullException("method");
            }
            if (string.IsNullOrEmpty(query))
            {
                throw new ArgumentNullException("query");
            }
            this.SetQueryNameBasedOnMethod(method);
            if (this.queryName.Equals("&id=", StringComparison.OrdinalIgnoreCase))
            {
                this.assetId = query;
            }
            else
            {
                this.queryValue = query;
            }
            //this.returnFormat = string.IsNullOrEmpty(this.returnFormat) ? "html" : returnFormat;
            this.returnFormat = returnFormat;
            this.topicVersion = topicVersion;
            this.topicLocale = topicLocale;
            this.locale = locale;
            this.vendor = vendor;
            this.isPrint = isPrint;
            this.isCompress = isCompress;
        }

        private string BuildQueryString()
        {
            StringBuilder builder = new StringBuilder();
            if (!string.IsNullOrEmpty(this.method))
            {
                builder.Append("method=");
                builder.Append(this.method);
            }
            if (!string.IsNullOrEmpty(this.assetId) || !string.IsNullOrEmpty(this.queryValue))
            {
                builder.Append(this.queryName);
                builder.Append(this.queryName.Equals("&id=", StringComparison.OrdinalIgnoreCase) ? this.assetId : this.queryValue);
            }
            if (!string.IsNullOrEmpty(this.locale))
            {
                builder.Append("&locale=");
                builder.Append(this.locale);
            }
            if (!string.IsNullOrEmpty(this.vendor))
            {
                builder.Append("&vendor=");
                builder.Append(this.vendor);
            }
            if (!string.IsNullOrEmpty(this.returnFormat))
            {
                builder.Append("&format=");
                builder.Append(this.returnFormat);
            }
            if (!string.IsNullOrEmpty(this.topicVersion))
            {
                builder.Append("&topicversion=");
                builder.Append(this.topicVersion);
            }
            if (!string.IsNullOrEmpty(this.topicLocale))
            {
                builder.Append("&topiclocale=");
                builder.Append(this.topicLocale);
            }
            if (!string.IsNullOrEmpty(this.targetFrameworkMoniker))
            {
                builder.Append("&targetFrameworkMoniker=");
                builder.Append(this.targetFrameworkMoniker);
            }
            if (!string.IsNullOrEmpty(this.devLang))
            {
                builder.Append("&devLang=");
                builder.Append(this.devLang);
            }
            if (!string.IsNullOrEmpty(this.targetOS))
            {
                builder.Append("&targetOS=");
                builder.Append(this.targetOS);
            }
            if (this.isPrint)
            {
                builder.Append("&print=true");
            }
            if (this.isExactTopicMatch)
            {
                builder.Append("&isExactTopicMatch=true");
            }
            if (!string.IsNullOrEmpty(this.package))
            {
                builder.Append("&package=");
                builder.Append(this.package);
            }
            return builder.ToString();
        }

        public static bool IsValid(string url)
        {
            return url.StartsWith("ms-xhelp:///?", StringComparison.OrdinalIgnoreCase);
        }

        private void ParseHelpQuery(string helpQuery)
        {
            if (!string.IsNullOrEmpty(helpQuery))
            {
                int index = helpQuery.IndexOf('?');
                if (index > -1)
                {
                    helpQuery = helpQuery.Substring(index + 1);
                }
                string[] strArray = helpQuery.Split(new char[] { '&' });
                if (strArray != null)
                {
                    for (int i = 0; i < strArray.Length; i++)
                    {
                        string[] strArray2 = strArray[i].Split(new char[] { '=' });
                        if ((strArray2 != null) && (strArray2.Length == 2))
                        {
                            this.SetValues(strArray2[0].ToUpperInvariant(), strArray2[1]);
                        }
                    }
                }
            }
        }

        private void SetQueryNameBasedOnMethod(string method)
        {
            string str;
            if (((str = method) != null) && (((str == "page") || (str == "children")) || ((str == "ancestors") || (str == "toc") || (str == "asset"))))
            {
                this.queryName = "&id=";
            }
            else
            {
                this.queryName = "&query=";
            }
        }

        private void SetValues(string key, string value)
        {
            switch (key)
            {
                case "METHOD":
                    this.method = value;
                    return;

                case "QUERY":
                    this.queryValue = value;
                    return;

                case "LOCALE":
                    this.locale = value;
                    return;

                case "VENDOR":
                    this.vendor = value;
                    return;

                case "ID":
                    this.assetId = value;
                    return;

                case "FORMAT":
                    this.returnFormat = value;
                    return;

                case "TOPICVERSION":
                    this.topicVersion = value;
                    return;

                case "TOPICLOCALE":
                    this.topicLocale = value;
                    return;

                case "TARGETFRAMEWORKMONIKER":
                    this.targetFrameworkMoniker = value;
                    return;

                case "DEVLANG":
                    this.devLang = value;
                    return;

                case "TARGETOS":
                    this.targetOS = value;
                    return;

                case "PACKAGE":
                    this.package = value;
                    return;
            }
        }

        public override string ToString()
        {
            string query = this.BuildQueryString();
            if (!query.StartsWith("ms-xhelp:///?", StringComparison.OrdinalIgnoreCase))
            {
                query = "ms-xhelp:///?" + query;
            }
            return query;
        }

        public static HelpQuery ViewerF1Query(string query)
        {
            return new HelpQuery("f1", query, "html", null, null, null, "Microsoft", false, true);
        }

        public string AssetId
        {
            get
            {
                return this.assetId;
            }
            set
            {
                this.assetId = value;
            }
        }

        public string DevLang
        {
            get
            {
                return this.devLang;
            }
            set
            {
                this.devLang = value;
            }
        }

        public bool IsExactTopicMatch
        {
            get
            {
                return this.isExactTopicMatch;
            }
            set
            {
                this.isExactTopicMatch = value;
            }
        }

        public bool IsViewerF1
        {
            get
            {
                return (!String.IsNullOrEmpty(this.method) && this.method.ToUpperInvariant() == "F1" && !String.IsNullOrEmpty(this.queryValue));
            }
        }

        public string Locale
        {
            get
            {
                return this.locale;
            }
            set
            {
                this.locale = value;
            }
        }

        public string Method
        {
            get
            {
                return this.method;
            }
            set
            {
                this.method = value;
            }
        }

        public string QueryValue
        {
            get
            {
                return this.queryValue;
            }
            set
            {
                this.queryValue = value;
            }
        }

        public string ReturnFormat
        {
            get
            {
                return this.returnFormat;
            }
            set
            {
                this.returnFormat = value;
            }
        }

        public string TargetFrameworkMoniker
        {
            get
            {
                return this.targetFrameworkMoniker;
            }
            set
            {
                this.targetFrameworkMoniker = value;
            }
        }

        public string TargetOS
        {
            get
            {
                return this.targetOS;
            }
            set
            {
                this.targetOS = value;
            }
        }

        public string Package
        {
            get
            {
                return this.package;
            }
            set
            {
                this.package = value;
            }
        }


        public string TopicLocale
        {
            get
            {
                return this.topicLocale;
            }
            set
            {
                this.topicLocale = value;
            }
        }

        public string TopicVersion
        {
            get
            {
                return this.topicVersion;
            }
            set
            {
                this.topicVersion = value;
            }
        }

        public string Vendor
        {
            get
            {
                return this.vendor;
            }
            set
            {
                this.vendor = value;
            }
        }


    }
}
