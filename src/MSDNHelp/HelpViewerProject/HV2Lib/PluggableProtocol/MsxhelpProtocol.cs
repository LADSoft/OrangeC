using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using HVProject.HV2Lib.Render;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif

namespace HVProject.HV2Lib.PluggableProtocol
{
    public class MsxhelpProtocol
    {
        // ===============================================================================================================
        // Topic is user edited text  (untested)
        // ===============================================================================================================

        public static String UserTopicText = "";  //Main form can set. Handler will read.  == 0xFF + 0xFE (Unicode)

        public static Stream GetUserTopicAsStream()
        {
            return (Stream)new MemoryIStream(Encoding.UTF8.GetBytes(MsxhelpProtocol.UserTopicText));
        }

        // ===============================================================================================================
        // The Main window can request raw unrendered HTML topics when available (topic from the .mshc help file)
        // ===============================================================================================================

        public delegate void RawCodeDelegate(String text);
        public static RawCodeDelegate RawCodeHandler = null;

        public void AddRawCodeCallback(RawCodeDelegate callback)  // call by main form
        {
            RawCodeHandler = callback;
        }

        public static void SetRawCode(String text)  // call from handler
        {
            if (RawCodeHandler != null)
                RawCodeHandler(text); 
        }


        // ===============================================================================================================
        // Give Handler an Open Catalog (to get topics and assets from)
        // ===============================================================================================================

        private static Catalog _catalog = null;                      

        /// <summary>
        /// Pluggable Protocol needs to get data from a collection. You need to set this to the the open Catalog.
        /// </summary>
        public static Catalog Catalog
        {
            get { return _catalog; }
            set { _catalog = value; }
        }


        // ===============================================================================================================
        // Tell Handler which type of Rendering to use
        // ===============================================================================================================

        private static Boolean _renderUsingVS = false;

        /// <summary>
        /// RenderUsingVS=TRUE => best results for rendering VS SelfBranded=false content.  VS 11 Help (Microsoft.VisualStudio.Help.DLL) must be installed
        /// RenderUsingVS=FALSE => best for Windows 8 where VS may not be installed. Render SelfBranded=true content well. No so good with VS content. 
        /// </summary>
        public static Boolean RenderUsingVS
        {
            get { return _renderUsingVS; }
            set { _renderUsingVS = value; }
        }


        // ===============================================================================================================
        // Register a Handler for this application only
        // ===============================================================================================================

        // == Registers a pluggable namespace handler on the current process

        private IInternetSession protocolHandlerSession;
        private HelpClassFactory protocolHandlerFactory;

        public MsxhelpProtocol()  //constructor
        {
            // initialize async pluggable protocol handler
            NativeMethods.CoInternetGetSession(0, out protocolHandlerSession, 0);    
            protocolHandlerFactory = new HelpClassFactory();
            Guid guid = new Guid("078600C7-A606-4d28-99EF-9378A08FCE97");
            protocolHandlerSession.RegisterNameSpace(protocolHandlerFactory, ref guid, "ms-xhelp", 0, null, 0);

            //Now we will get pinged for content any time we navigate to ms-xhelp://...
            //webBrowser.Navigate("ms-xhelp:///?method=page&id=ba237712-f8c4-4d13-854b-ab4a16afa309&vendor=Microsoft&topicVersion=&topicLocale=EN-US");
        }

    }
}
