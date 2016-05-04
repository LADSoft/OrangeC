using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HVProject.HV2Lib.Render
{
    using System;

    internal static class HttpUtility
    {
        public static string HtmlEncode(string htmlToEncode)
        {
            if (!string.IsNullOrEmpty(htmlToEncode))
            {
                htmlToEncode = htmlToEncode.Replace("<", "&lt;");
                htmlToEncode = htmlToEncode.Replace(">", "&gt;");
                htmlToEncode = htmlToEncode.Replace("\"", "&quot;");
                htmlToEncode = htmlToEncode.Replace("&", "&amp;");
            }
            return htmlToEncode;
        }

        public static string KeywordDecode(string keywordToDecode)
        {
            return Uri.UnescapeDataString(keywordToDecode);
        }

        public static string UrlDecode(string urlToDecode)
        {
            return Uri.UnescapeDataString(urlToDecode);
        }

        public static string UrlEncode(string urlToEncode)
        {
            return Uri.EscapeDataString(urlToEncode);
        }
    }
}