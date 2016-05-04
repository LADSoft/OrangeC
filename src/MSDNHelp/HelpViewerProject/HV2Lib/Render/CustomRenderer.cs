using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using HVProject.HV2Lib.Common;
using HVProject.HV2Lib.PluggableProtocol;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif

namespace HVProject.HV2Lib.Render
{
    public class CustomRenderer
    {
        public static CustomRenderer self = null;

        private HelpQuery _helpQuery = new HelpQuery("");
        private Topic _topic = null;


        public static CustomRenderer Create()
        {
            self = new CustomRenderer();
            return self;
        }

        public CustomRenderer()
        {
            self = this;
        }


        public enum URLType { Topic, Asset, Branding, F1Keyword, Unknown }


        public Boolean IsTopicUrl(String sURL, out String topicId)
        {
            HelpQuery helpQuery;
            URLType urlType = UrlInfo(sURL, out helpQuery);
            topicId = helpQuery.AssetId;
            return urlType == URLType.Topic;
        }

        public URLType UrlInfo(String sURL, out HelpQuery helpQuery)
        {
            String url = sURL.Replace("&amp;", "&");
            helpQuery = new HelpQuery(url);

            // Indexed Topic -- ms-xhelp:///?method=page&amp;id=SouthPark.Index&amp;vendor=ACME Company&amp;topicVersion=-1&amp;topicLocale=EN-US
            // or an unrendered url: ms-xhelp:///?Id=SouthPark.EricCartman 

            if ((!String.IsNullOrEmpty(helpQuery.AssetId)) && (String.IsNullOrEmpty(helpQuery.Method) || helpQuery.Method.ToLower() == "page")) 
                return URLType.Topic;

			// F1Keyword query "ms-xhelp:///?method=f1&amp;query=SouthPark.Text"
			
            if ((!String.IsNullOrEmpty(helpQuery.QueryValue)) && !String.IsNullOrEmpty(helpQuery.Method) && helpQuery.Method.ToLower() == "f1") 
                return URLType.F1Keyword;

            // Branding package path "ms-xhelp:///?76555C51-8CF1-4FEB-81C4-BED857D94EBB_EN-US_Microsoft;/branding.js"

            else if (url.IndexOf("?76555C51-8CF1-4FEB-81C4-BED857D94EBB_", StringComparison.OrdinalIgnoreCase) > 0)  // ms-xhelp:///?xxxxx.css 
                return URLType.Branding;

            // Asset  -- "ms-xhelp:///?method=asset&id=XyzStyles.css&package=SOURCE HELP.mshc&topiclocale=EN-US"
            // Or some unrendered asset link ms-xhelp:///?xxxxx.css 

            else if ((!String.IsNullOrEmpty(helpQuery.AssetId) && !String.IsNullOrEmpty(helpQuery.Method) && helpQuery.Method.ToLower() == "asset") // ms-xhelp:///?method=asset&id=xxx
            || (String.IsNullOrEmpty(helpQuery.Method) && String.IsNullOrEmpty(helpQuery.AssetId))) // ms-xhelp:///?xxxxx.css 
                return URLType.Asset;

            else 
                return URLType.Unknown;
        }


        public Stream UrlToStream(String sURL)
        {
            return UrlToStream(sURL, true);
        }

        public Stream UrlToStream(String sURL, Boolean okToRender)  
        {
            if (MsxhelpProtocol.Catalog == null || !MsxhelpProtocol.Catalog.IsOpen)
                return null;

            HelpQuery helpQuery;
            URLType urltype = UrlInfo(sURL, out helpQuery);
            String url = sURL.Replace("&amp;", "&");

            Stream stream = null;
            String assetPath = "";
            CatalogRead _catalogRead = new CatalogRead();

            // Indexed Topic -- ms-xhelp:///?method=page&amp;id=SouthPark.Index&amp;vendor=ACME Company&amp;topicVersion=-1&amp;topicLocale=EN-US
            // or an unrendered url: ms-xhelp:///?Id=SouthPark.EricCartman 

            if (urltype == URLType.Topic)
            {
                _helpQuery = new HelpQuery(helpQuery.ToString());

                HelpFilter filter = new HelpFilter();
                if (!String.IsNullOrEmpty(_helpQuery.Vendor))
                    filter.Add("vendor", _helpQuery.Vendor);
                if (!String.IsNullOrEmpty(_helpQuery.TopicLocale))
                    filter.Add("topiclocale", _helpQuery.TopicLocale);
                if (!String.IsNullOrEmpty(_helpQuery.TopicVersion))
                    filter.Add("topicversion", _helpQuery.TopicVersion);

                //Get topic obj frome store
				String [] root = _helpQuery.AssetId.Split(new String[] {"%7c"}, StringSplitOptions.None);
				if (root.Length == 1)
				{
					_topic = (Topic)_catalogRead.GetIndexedTopicDetails(MsxhelpProtocol.Catalog, _helpQuery.AssetId, filter);
				}
				else if (root.Length == 3 && root[0] == "VS")
				{
					String[] path = root[2].Split(new string[] {"%5c"}, StringSplitOptions.None);
					if (root[1] == "winui")
					{
						// Create keyword list
						String name = path[path.Length-1];
						if (name.IndexOf(".") != -1)
							name = name.Substring(0, name.IndexOf("."));
						String[] prioritizedF1Keywords = new String[1];
						prioritizedF1Keywords[0] = "winuser/" + name;
						
						//Get topic obj frome store
						_topic = (Topic)_catalogRead.GetTopicDetailsForF1Keyword(MsxhelpProtocol.Catalog, prioritizedF1Keywords, filter);
						
					}
				}
                if (_topic != null) 
                {
                    // _topic.Package == _topic.Url = package.mshc;\\path (so we need to split to get the package)
                    String[] url2 = _topic.Url.Split(new char[] { ';' });
                    if (url2.Length > 0)
                        helpQuery.Package = url2[0];   // "package.mshc"
                    _helpQuery.Package = helpQuery.Package;

                    if (!String.IsNullOrEmpty(_topic.Locale))
                        _helpQuery.Locale = _topic.Locale;
                    if (String.IsNullOrEmpty(_helpQuery.Locale))
                        _helpQuery.Locale = MSLocales.ThreadLocale;

                    if (!String.IsNullOrEmpty(_topic.TopicLocale))
                        _helpQuery.TopicLocale = _topic.TopicLocale;
                    if (!String.IsNullOrEmpty(_topic.TopicVersion))
                        _helpQuery.TopicVersion = _topic.TopicVersion;
                    if (!String.IsNullOrEmpty(_topic.Vendor))
                        _helpQuery.Vendor = _topic.Vendor;

                    //Special Case: Force modified text
                    if (!String.IsNullOrEmpty(MsxhelpProtocol.UserTopicText))
                    {
                        stream = MsxhelpProtocol.GetUserTopicAsStream();
                    }
                    else
                    {
                        try
                        {
                            stream = (Stream)_topic.FetchContent();    //Normal fetch topic from store
                            //this would also return the stream
                            //stream = (Stream)_catalogRead.GetIndexedTopic(MsxhelpProtocol.Catalog, _topic.Id, null);  
                        }
                        catch
                        {
                            stream = null;
                        }
                    }
                    if (stream != null && okToRender)
                    {
                        new TopicStreamExpand(stream, helpQuery);   // expand all links etc in the stream
                    }
                }
            }

			// F1Keyword query "ms-xhelp:///?method=f1&amp;query=SouthPark.Text"

			else if (urltype == URLType.F1Keyword)
			{
                _helpQuery = new HelpQuery(helpQuery.ToString());
				MessageBox.Show(_helpQuery.QueryValue,"hi",MessageBoxButtons.OK);

                HelpFilter filter = new HelpFilter();
                if (!String.IsNullOrEmpty(_helpQuery.Vendor))
                    filter.Add("vendor", _helpQuery.Vendor);
                if (!String.IsNullOrEmpty(_helpQuery.TopicLocale))
                    filter.Add("topiclocale", _helpQuery.TopicLocale);
                if (!String.IsNullOrEmpty(_helpQuery.TopicVersion))
                    filter.Add("topicversion", _helpQuery.TopicVersion);

				// Create keyword list
				String[] prioritizedF1Keywords = new String[1];
				prioritizedF1Keywords[0] = _helpQuery.QueryValue;
				
                //Get topic obj frome store
                _topic = (Topic)_catalogRead.GetTopicDetailsForF1Keyword(MsxhelpProtocol.Catalog, prioritizedF1Keywords, filter);
                if (_topic != null) 
                {
                    // _topic.Package == _topic.Url = package.mshc;\\path (so we need to split to get the package)
                    String[] url2 = _topic.Url.Split(new char[] { ';' });
                    if (url2.Length > 0)
                        helpQuery.Package = url2[0];   // "package.mshc"
                    _helpQuery.Package = helpQuery.Package;

                    if (!String.IsNullOrEmpty(_topic.Locale))
                        _helpQuery.Locale = _topic.Locale;
                    if (String.IsNullOrEmpty(_helpQuery.Locale))
                        _helpQuery.Locale = MSLocales.ThreadLocale;

                    if (!String.IsNullOrEmpty(_topic.TopicLocale))
                        _helpQuery.TopicLocale = _topic.TopicLocale;
                    if (!String.IsNullOrEmpty(_topic.TopicVersion))
                        _helpQuery.TopicVersion = _topic.TopicVersion;
                    if (!String.IsNullOrEmpty(_topic.Vendor))
                        _helpQuery.Vendor = _topic.Vendor;

                    //Special Case: Force modified text
                    if (!String.IsNullOrEmpty(MsxhelpProtocol.UserTopicText))
                    {
                        stream = MsxhelpProtocol.GetUserTopicAsStream();
                    }
                    else
                    {
                        try
                        {
                            stream = (Stream)_topic.FetchContent();    //Normal fetch topic from store
                            //this would also return the stream
                            //stream = (Stream)_catalogRead.GetIndexedTopic(MsxhelpProtocol.Catalog, _topic.Id, null);  
                        }
                        catch
                        {
                            stream = null;
                        }
                    }
                    if (stream != null && okToRender)
                    {
                        new TopicStreamExpand(stream, helpQuery);   // expand all links etc in the stream
                    }
                }
			}

            // Branding package path "ms-xhelp:///?76555C51-8CF1-4FEB-81C4-BED857D94EBB_EN-US_Microsoft;/branding.js"

            else if (urltype == URLType.Branding)
            {
                if (url.StartsWith("76555C51-8CF1-4FEB-81C4-BED857D94EBB_", StringComparison.OrdinalIgnoreCase))  // xxxxx.css 
                    assetPath = url.Replace("/", @"\");
                else
                    assetPath = url.Substring("ms-xhelp:///?".Length).Replace("/", @"\");

                String[] parts = assetPath.Split(new char[] { '_' });
                String brandingLocale = parts[1];

                assetPath = url.Split(new char[] { ';' })[1];
                assetPath = assetPath.TrimStart(new char[] { '/' });

                // For now we have branding files in a folder ".\Branding"
                Assembly thisAssembly = System.Reflection.Assembly.GetExecutingAssembly();
                String BrandingDir = Path.Combine(Path.GetDirectoryName(thisAssembly.Location), "Branding");
                String filePath = Path.Combine(BrandingDir, assetPath);
                if (!Directory.Exists(BrandingDir))
                {
                    MessageBox.Show("Branding folder not found: " + BrandingDir);
                    stream = null;
                }
                else if (!File.Exists(filePath))
                {
                    MessageBox.Show("Branding file not found: " + filePath);
                    stream = null;
                }
                else
                {
                    FileStream fs = File.OpenRead(filePath);
                    byte[] b = new byte[fs.Length];
                    fs.Read(b, 0, b.Length);
                    fs.Close();
                    stream = new MemoryStream(b);
                }
            }

            // Asset  -- "ms-xhelp:///?method=asset&id=XyzStyles.css&package=SOURCE HELP.mshc&topiclocale=EN-US"
            // Or some unrendered asset link ms-xhelp:///?xxxxx.css 

            else if (urltype == URLType.Asset)
            {
                if (!String.IsNullOrEmpty(helpQuery.AssetId))
                    assetPath = helpQuery.AssetId.Replace("/", @"\");
                else
                    assetPath = url.Substring("ms-xhelp:///".Length).Replace("/", @"\");

                if (String.IsNullOrEmpty(_helpQuery.Package) && !String.IsNullOrEmpty(helpQuery.Package))
                    _helpQuery.Package = helpQuery.Package;

                if (!String.IsNullOrEmpty(_helpQuery.Package) && !String.IsNullOrEmpty(assetPath))
                    stream = (Stream)_catalogRead.GetLinkedAsset(MsxhelpProtocol.Catalog, _helpQuery.Package, assetPath, _helpQuery.Locale);
            }

            return stream;
        }

 

        
    }
}
