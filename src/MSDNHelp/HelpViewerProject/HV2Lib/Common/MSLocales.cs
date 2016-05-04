using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace HVProject.HV2Lib.Common
{

    /// <summary>
    /// At the time of writing these Locales are used by VS 11.
    /// </summary>
    
    public class MSLocales
    {
        public readonly Dictionary<String, String> Locales = new Dictionary<string,string>();
        
        public MSLocales()
        {
            //VS Languages shipped
            Locales.Add("zh-TW", "Chinese - Taiwan (Traditional)");
            Locales.Add("zh-CN", "Chinese - China (Simplified)");
            Locales.Add("en-US", "English - US");
            Locales.Add("fr-FR", "French - France");
            Locales.Add("de-DE", "German - Germany");
            Locales.Add("It-IT", "Italian - Italy");
            Locales.Add("ja-JP", "Japanese - Japan");
            Locales.Add("ko-KR", "Korean - Korea");
            Locales.Add("ru-RU", "Russian - Russia");
            Locales.Add("es-ES", "Spanish - Spain");

            // Other language codes used by MSDN Online
            Locales.Add("ar-SA", "Arabic - Saudi Arabia");
            Locales.Add("cs-CZ", "Czech - Czech Republic");
            Locales.Add("pl-PL", "Polish - Poland");
            Locales.Add("pt-BR", "Portuguese - Brazilian");
            Locales.Add("tr-TR", "Turkish - Turkey");
 	    }

        public static String ThreadLocale   //Microsoft code typically defaults to the thread locale if no locale is specified
        {
            get { return Thread.CurrentThread.CurrentUICulture.Name; }
        }

    }
}
