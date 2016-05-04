using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;
using HVProject.HV2Lib.Common;

namespace HVProject.HV2Lib.Render
{

    /// <summary>
    /// Given a Topic stream we fix it up up so paths start withs ms-xhelp:/// protocol
    /// HelpQuery contains the TopicVersion, TopicLocale type parameters used to expand link and img paths.
    /// </summary>
    public class TopicStreamExpand
    {
        readonly MetaData metaData;
        readonly HelpQuery _helpQuery;

        public static readonly string msxhelpPrefix = "ms-xhelp:///?";
        public static readonly string DefaultBrandPathBase = "76555C51-8CF1-4FEB-81C4-BED857D94EBB_";
        public readonly String xbrandingPath;   //eg. "ms-xhelp:///?76555C51-8CF1-4FEB-81C4-BED857D94EBB_en-US_Microsoft

        //eg ms-xhelp:///?method=asset&amp;id=SomeAssetPath&amp;package=SomePackage.mshc&amp;topiclocale=EN-US
        public readonly String xAssetPathTemplate;

        public TopicStreamExpand(Stream stream, HelpQuery helpQuery)  
        {
            _helpQuery = new HelpQuery(helpQuery.ToString());
            if (String.IsNullOrEmpty(_helpQuery.Locale))
                _helpQuery.Locale = MSLocales.ThreadLocale;

            xbrandingPath = msxhelpPrefix + DefaultBrandPathBase + _helpQuery.Locale + "_Microsoft";

            //eg ms-xhelp:///?method=asset&amp;id=SomeAssetPath&amp;package=SomePackage.mshc&amp;topiclocale=EN-US
            xAssetPathTemplate = msxhelpPrefix + "method=asset&amp;id={0}&amp;package=" + helpQuery.Package + "&amp;topiclocale=" + _helpQuery.Locale;

            //Rip all meta data
            metaData = new MetaData(stream);

            //Render
            DoRender(stream);
        }


        public void DoRender(Stream stream)
        {
            Boolean selfBranded = metaData.IsSelfBranded;
            stream.Position = 0;

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(stream);

            //Omitt the XML Declaration (not needed for HTML)
            //if (xmlDoc.FirstChild.NodeType == XmlNodeType.XmlDeclaration)
            //    xmlDoc.RemoveChild(xmlDoc.FirstChild);

            foreach (XmlNode node in xmlDoc.DocumentElement)  //all list under root element "HTML"
            {
                if (node.NodeType == XmlNodeType.Element)
                {
                    String name = node.Name.ToLower();
                    if (name == "head")
                    {
                        XmlNode headNode = node;

                        //-------------------------------------------------------------------------------------------------
                        // We need to prefix asset links with "ms-xhelp:///?method=asset&amp;id=xx&amp;package=Visual_Studio_21800791_VS_100_en-us_6.mshc&amp;topiclocale=EN-US" and make \ splashes
                        //   <link rel="parent" href="serviceapilive/content/dd831853/en-us;vs.100" />
                        // to 
                        //   <link rel="parent" href="ms-xhelp:///?method=asset&amp;id=serviceapilive\content\dd831853\en-us;vs.100&amp;package=Visual_Studio_21800791_VS_100_en-us_6.mshc&amp;topiclocale=EN-US" />

                        foreach (XmlNode linkNode in headNode.ChildNodes)
                        {
                            if (linkNode.NodeType == XmlNodeType.Element && linkNode.Name.ToLower() == "link" && linkNode.Attributes["href"] != null && linkNode.Attributes["href"].InnerXml != "")
                            {
                                linkNode.Attributes["href"].InnerXml = String.Format(xAssetPathTemplate, linkNode.Attributes["href"].InnerXml.Replace('/', '\\'));
                            }
                        }

                        //-------------------------------------------------------------------------------------------------
                        // First Insert Microsoft branding

                        if (!selfBranded)
                        {
                            // <meta name="Microsoft.Help.TopicVendor" content="Microsoft" />
                            headNode.InsertBefore(CreateXmlElement(xmlDoc, "meta", "name", "Microsoft.Help.TopicVendor", "content", "Microsoft"), headNode.FirstChild);

                            //This makes the page crash because we don't have the tag ids I suspect
                            // <script src="ms-xhelp:///?76555C51-8CF1-4FEB-81C4-BED857D94EBB_en-US_Microsoft;/branding.js" type="text/javascript"></script>
                            // headNode.InsertBefore(CreateXmlElement(xmlDoc, "script", "src", xbrandingPath + ";/branding.js", "type", "text/javascript"), headNode.FirstChild);

                            // <style type=\"text/css\">.cl_CollapsibleArea_expanding {background: url('" + xbrandingPath + ";/imageSprite.png') -1px -3px;}.cl_CollapsibleArea_collapsing {background: url('" + xbrandingPath + ";/imageSprite.png') -16px -3px;}.OH_CodeSnippetContainerTabLeftActive, .OH_CodeSnippetContainerTabLeft,.OH_CodeSnippetContainerTabLeftDisabled {background-image: url('" + xbrandingPath ";/tabLeftBG.gif')}.OH_CodeSnippetContainerTabRightActive, .OH_CodeSnippetContainerTabRight,.OH_CodeSnippetContainerTabRightDisabled {background-image: url('" + xbrandingPath + ";/tabRightBG.gif')}.OH_footer { background-image: url('" + xbrandingPath + ";/footer_slice.gif'); background-position:top; background-repeat:repeat-x}</style>
                            // <link rel="stylesheet" type="text/css" href="ms-xhelp:///?76555C51-8CF1-4FEB-81C4-BED857D94EBB_en-US_Microsoft;/branding.css" />
                            headNode.InsertBefore(CreateXmlElement(xmlDoc, "link", "rel", "stylesheet", "type", "text/css", "href", xbrandingPath + ";/branding.css"), headNode.FirstChild);

                            // <link rel="SHORTCUT ICON" href="ms-xhelp:///?76555C51-8CF1-4FEB-81C4-BED857D94EBB_en-US_Microsoft;/favicon.ico" />
                            headNode.InsertBefore(CreateXmlElement(xmlDoc, "link", "rel", "SHORTCUT ICON", "href", xbrandingPath + ";/favicon.ico"), headNode.FirstChild);
                            // <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE9" />
                            headNode.InsertBefore(CreateXmlElement(xmlDoc, "meta", "http-equiv", "X-UA-Compatible", "content", "IE=EmulateIE9"), headNode.FirstChild);
                        }
                    }
                    if (name == "body")
                    {
                        List<XmlNode> nodes = GetSubElementNodes(node);
                        foreach (XmlNode xNode in nodes)
                        {
                            if (xNode.Name.ToLower() == "a")
                                ExpandAnchorHref(xNode);
                            else if (xNode.Name.ToLower() == "img")
                                    ExpandImageSrc(xNode);
                            else if (xNode.Name.ToLower() == "codesnippet")
                                    ExpandCodeSnippet(xmlDoc, xNode);
                            else if (xNode.Name.ToLower() == "table")
                                    ExpandTable(xmlDoc, xNode);
                        }
                    }

                }
            }

            // Save changes back in the correct encoding (XmlDocument is always Unicode). Don't worry about ANSI.
            // UTF8 BOM = 0xEF,0xBB,0xBF
            // UTF16BE = 0xFE, 0xFF
            // UTF16LE = 0xFF, 0xFE  (normal Unicode)

            //Read BOM for encoding
            stream.Position = 0;
            Byte b1 = (Byte)stream.ReadByte();
            Byte b2 = (Byte)stream.ReadByte();
            Byte b3 = (Byte)stream.ReadByte();

            stream.Position = 0;
            if (b1 == 0xEF && b2 == 0xBB && b3 == 0xBF)
                xmlDoc.Save(new StreamWriter(stream, Encoding.UTF8));    //Rob: I find the stream is usually UTF-8
            else if (b1 == 0xFE && b2 == 0xFF)
                xmlDoc.Save(new StreamWriter(stream, Encoding.BigEndianUnicode));
            else
                xmlDoc.Save(new StreamWriter(stream, Encoding.Unicode));
            
            stream.Position = 0;
        }

        private void ExpandImageSrc(XmlNode node)  //only pass in <img > node. Expand the src=url
        {
            // Before: <img class="mtps-img-src" alt="Welcome to Visual Studio 2010" src="IC348421" />
            // After:  <img class="mtps-img-src" src="ms-xhelp:///?method=asset&amp;id=IC348421&amp;package=Visual_Studio_21800791_VS_100_en-us_6.mshc&amp;topiclocale=EN-US" alt="Welcome to Visual Studio 2010" title="Welcome to Visual Studio 2010" />
            // Windows help: <img src="mshelp://windows/?id=bf416877-c83f-4476-a3da-8ec98dcf5f10">
            // Windows help: <img src="mshelp://Help/?id=bf416877-c83f-4476-a3da-8ec98dcf5f10">      
            // Windows OEM:  <img src="mshelp://OEM/?id=TopicID493dg3789k4f"> ...

            int i;
            foreach (XmlAttribute attr in node.Attributes)
            {
                if (attr.Name.ToLower() == "src")
                {
                    String ID = attr.Value;
                    if (attr.Value.StartsWith("ms-xhelp:///?Id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("ms-xhelp:///?Id=".Length);
                    else if (attr.Value.StartsWith("mshelp://windows/?id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("mshelp://windows/?id=".Length);
                    else if (attr.Value.StartsWith("mshelp://help/?id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("mshelp://help/?id=".Length);
                    else if (attr.Value.StartsWith("mshelp://OEM/?id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("mshelp://OEM/?id=".Length);
                    else if (attr.Value.StartsWith("http", StringComparison.OrdinalIgnoreCase))
                        ID = "";

                    if (ID != "")  
                    {
                        //Windows Help weirdness  -- /basic8/Resources/file.jpg  --> /Resources/file.jpg
                        i = ID.IndexOf("/resources/", StringComparison.OrdinalIgnoreCase);  
                        if (i > 0 && ID[0] == '/')
                            ID = ID.Substring(i);

                        ID = ID.Replace('/', '\\');

                        //ms-xhelp:///?method=asset&amp;id=IC348421&amp;package=Visual_Studio_21800791_VS_100_en-us_6.mshc&amp;topiclocale=EN-US
                        HelpQuery helpQuery = new HelpQuery("asset", ID, null, _helpQuery.TopicVersion, _helpQuery.TopicLocale, _helpQuery.Locale, _helpQuery.Vendor);
                        helpQuery.Package = _helpQuery.Package; 
                        attr.Value = helpQuery.ToString(); //.Replace("&", "&amp;");
                    }
                    break;
                }
            }
        }

        private void ExpandAnchorHref(XmlNode node)  //only pass in <a > node. Expand the href=url
        {
            // Before: <a href="ms-xhelp:///?Id=SouthPark.KyleBroflovski">KyleBroflovski</a>
            // After:  <a href="ms-xhelp:///?method=page&amp;id=SouthPark.KyleBroflovski&amp;vendor=ACME Company&amp;topicVersion=-1&amp;topicLocale=EN-US">
            // Windows help: <a href="mshelp://windows/?id=bf416877-c83f-4476-a3da-8ec98dcf5f10">
            // Windows help: <a href="mshelp://Help/?id=bf416877-c83f-4476-a3da-8ec98dcf5f10">   //variation used by images
            // Windows OEM:  <a href="mshelp://OEM/?id=TopicID493dg3789k4f"> ...

            foreach (XmlAttribute attr in node.Attributes)
            {
                if (attr.Name.ToLower() == "href")
                {
                    String ID = "";
                    if (attr.Value.StartsWith("ms-xhelp:///?Id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("ms-xhelp:///?Id=".Length);
                    else if (attr.Value.StartsWith("mshelp://windows/?id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("mshelp://windows/?id=".Length);
                    else if (attr.Value.StartsWith("mshelp://help/?id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("mshelp://help/?id=".Length);
                    else if (attr.Value.StartsWith("mshelp://OEM/?id=", StringComparison.OrdinalIgnoreCase))
                        ID = attr.Value.Substring("mshelp://OEM/?id=".Length);

                    //update value with full path
                    if (ID != "")  
                    {
                        ID = ID.Replace('/', '\\');

                        //ms-xhelp:///?method=page&amp;id=SouthPark.KyleBroflovski&amp;vendor=ACME Company&amp;topicVersion=-1&amp;topicLocale=EN-US
                        HelpQuery helpQuery = new HelpQuery("page", ID, null, _helpQuery.TopicVersion, _helpQuery.TopicLocale, _helpQuery.Locale, _helpQuery.Vendor);
                        attr.Value = helpQuery.ToString(); //.Replace("&", "&amp;");
                    }
                    break;
                }
            }
        }
        private void ExpandCodeSnippet(XmlDocument doc, XmlNode node)
        {
            XmlNode innerNode = doc.CreateNode("element", "pre", "");
	    innerNode.InnerText = node.InnerText;
            node.InnerText = "";
            node.PrependChild(innerNode);
        }
        private void ExpandTable(XmlDocument doc, XmlNode node)
        {
            XmlAttribute attr = doc.CreateAttribute("border");
            attr.Value = "1";

            node.Attributes.SetNamedItem(attr);
            
        }
        
        // ------

        private List<XmlNode> GetSubElementNodes(XmlNode startNode)
        {
            List<XmlNode> list = new List<XmlNode>();
            list.Add(startNode);
            for (int i = 0; i < list.Count; i++)  //Find all child list of all 
			{
                if (list[i].HasChildNodes)
                {
                    foreach (XmlNode node in list[i].ChildNodes)    //Get children of node and append to end 
                    {
                        if (node.NodeType == XmlNodeType.Element)
                            list.Add(node);
                    }
                }
			}
            return list;
        }

        // ------

        private XmlElement CreateXmlElement(XmlDocument xmlDoc, String elementName, String x1, String x2, String y1, String y2, String z1, String z2)
        {
            XmlElement elem = xmlDoc.CreateElement(elementName, xmlDoc.DocumentElement.NamespaceURI);  //The root namespace suppresses xmlns="" being added

            XmlAttribute attribute = xmlDoc.CreateAttribute(x1);
            attribute.Value = x2;
            elem.Attributes.Append(attribute);

            if (!String.IsNullOrEmpty(y1))
            {
                attribute = xmlDoc.CreateAttribute(y1);
                attribute.Value = y2;
                elem.Attributes.Append(attribute);
            }
            if (!String.IsNullOrEmpty(z1))
            {
                attribute = xmlDoc.CreateAttribute(z1);
                attribute.Value = z2;
                elem.Attributes.Append(attribute);
            }
            return elem;
        }

        private XmlElement CreateXmlElement(XmlDocument xmlDoc, String elementName, String x1, String x2, String y1, String y2)
        {
            return CreateXmlElement(xmlDoc, elementName, x1, x2, y1, y2, null, null);
        }

        private XmlElement CreateXmlElement(XmlDocument xmlDoc, String elementName, String x1, String x2)
        {
            return CreateXmlElement(xmlDoc, elementName, x1, x2, null, null);
        }



    }
}
