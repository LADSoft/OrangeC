using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;

namespace HVProject.HV2Lib.Render
{
    public class MetaData
    {
        public readonly Dictionary<String, String> metaData = null;   //key is lower case

        public static readonly String MetaKey_MicrosoftHelpSelfbranded = "Microsoft.Help.Selfbranded";

        public MetaData(Stream stream)
        {
            try
            {
                metaData = GetMetaTags(stream);
            }
            catch
            {
                throw new Exception("reading meta tags via XmlReader");
            }
        }

        public Boolean IsSelfBranded
        {
            get
            {
                if (metaData.ContainsKey(MetaKey_MicrosoftHelpSelfbranded.ToLower()))
                {
                    String value = metaData[MetaKey_MicrosoftHelpSelfbranded.ToLower()];
                    return String.Equals(value, "true", StringComparison.OrdinalIgnoreCase);
                }
                return false;  //MS default is not SelfBranded
            }
        }

        public Dictionary<String, String> GetMetaTags(Stream stream)   // eg. <meta name="SelfBranded" content="false" />
        {
            stream.Position = 0;
            Dictionary<String, String> metaData = new Dictionary<string, string>();

            //Important to ignore a few things otherwise some MS Docs eg <DocType ..> tend to crash the reader.Read() call.
            XmlReaderSettings settings = new XmlReaderSettings();
            settings.IgnoreComments = true;
            settings.IgnoreProcessingInstructions = true;
            settings.IgnoreWhitespace = true;
            settings.DtdProcessing = DtdProcessing.Ignore;

            using (XmlReader reader = XmlReader.Create(stream, settings))
            {
                while (reader.Read())
                {
                    if (reader.NodeType == XmlNodeType.Element)
                    {
                        if (reader.Name.Equals("head", StringComparison.OrdinalIgnoreCase))
                            break;
                        else
                            continue;
                    }
                }

                //Read meta tags
                while (reader.Read())
                {
                    if (reader.NodeType == XmlNodeType.Element)
                    {
                        if (reader.Name.Equals("meta", StringComparison.OrdinalIgnoreCase))
                        {
                            String name = reader.GetAttribute("name");
                            if (string.IsNullOrEmpty(name))                    // maybe <meta http-equiv="X-UA-Compatible" content="IE=EmulateIE9" /> or <meta http-equiv="Content-Location" content="http://localhost/serviceapilive/content/dd831853/en-us;vs.100/primary/mtps.offline" />
                                name = reader.GetAttribute("http-equiv");
                            String value = reader.GetAttribute("content");

                            if (!string.IsNullOrEmpty(name) && (value != null))
                            {
                                if (!metaData.ContainsKey(name.ToLower()))
                                {
                                    metaData.Add(name.ToLower(), value);
                                }
                            }
                        }
                        else if (reader.Name.Equals("body", StringComparison.OrdinalIgnoreCase))  //don't go into body
                            break;
                    }
                }
            }
            stream.Position = 0;
            return metaData;
        }


    }
}
