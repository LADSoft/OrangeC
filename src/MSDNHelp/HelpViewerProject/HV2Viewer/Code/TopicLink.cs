using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HVProject.HV2Lib.Common;
using HVProject.HV2Lib.Render;

namespace HV2Viewer.Code
{
    public static class TopicLink
    {
        static private String msdnLibrary_Locale = "en-US";
        static private String msdnLibrary_Device = "";

        public static String MsdnLibrary_Locale
        {
            get { return msdnLibrary_Locale; }
            set { msdnLibrary_Locale = value; }
        }

        public static String MsdnLibrary_Device
        {
            get { return msdnLibrary_Device; }
            set { msdnLibrary_Device = value; }
        }



        public static String GetMshcTopicID(WebBrowser webBrowser1)
        {
            if (webBrowser1.Document != null && webBrowser1.Url != null)
                return GetMshcTopicID(webBrowser1.Url.AbsoluteUri);
            else
                return "";
        }

        public static String GetMshcTopicID(String url)  
        {
            if (!String.IsNullOrEmpty(url) &&  UrlType(url) == TopicLinkType.mshc)
            {
                HelpQuery helpQuery = new HelpQuery(url);
                if (!String.IsNullOrEmpty(helpQuery.Method) && helpQuery.Method.Equals("page", StringComparison.OrdinalIgnoreCase))
                {
                    return helpQuery.AssetId;  // ms-xhelp:///?method=page&id=AssetId       (return AssetID)
                }
            }
            return "";
        }

        public static TopicLinkType UrlType(String url)
        {
            if (url.Length > 6)
            {
                url = url.ToLower();
                if (url.StartsWith("http:") || url.StartsWith("https:"))
                    return TopicLinkType.web;
                if (url.StartsWith("ms-xhelp:"))
                    return TopicLinkType.mshc;
                if (url.StartsWith("file:"))
                    return TopicLinkType.file;
            }
            return TopicLinkType.unknown;
        }

        // ms-xhelp:///?method=page&id=topicid
        //eg. http://msdn.microsoft.com/[locale]/library/topicid.aspx
        //eg. http://msdn.microsoft.com/de-de/library/xxx
        //eg. http://msdn.microsoft.com/en-us/library/xxx(office.12).aspx
        //eg. http://msdn.microsoft.com/en-us/library/xxx(VS.90).aspx
        //eg. http://msdn.microsoft.com/en-us/library/xxx(v=VS.90, d=Printer).aspx

        public static String MakeMsdnLibraryUrl(String topicId, String locale, String topicVersion)   
        {
            if (String.IsNullOrEmpty(topicId) || locale == null || topicVersion == null)
                return "";

            if (topicVersion == "0")     //Can happen with PackageThis
                topicVersion = "";
            if (topicVersion != "" && !topicVersion.Contains('.'))   //maybe only 100
                topicVersion = "VS." + topicVersion;

            if (locale == "")
                locale = msdnLibrary_Locale;
            if (locale == "")
                locale = MSLocales.ThreadLocale;

            String param = "";
            if (msdnLibrary_Device != "" && topicVersion != "")
                param = "(v="+topicVersion+",d="+msdnLibrary_Device+")";
            else if (msdnLibrary_Device != "")
                param = "(" + msdnLibrary_Device + ")";
            else if (topicVersion != "")
                param = "(" + topicVersion + ")";

            return "http://msdn.microsoft.com/de-de/library/" + topicId + param + ".aspx";
        }


        public static String GetMSDNLibraryHome(String locale)
        {
            if (String.IsNullOrEmpty(locale))
                locale = MSLocales.ThreadLocale;
            String url = "http://msdn.microsoft.com/" + locale + "/library/default.aspx";
            return url;
        }


    }

    public enum TopicLinkType { web, mshc, file, unknown };

}
