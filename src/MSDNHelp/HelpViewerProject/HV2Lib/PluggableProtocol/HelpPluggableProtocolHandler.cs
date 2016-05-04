using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.InteropServices.ComTypes;
using System.Globalization;
using System.Collections.Generic;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
//using Microsoft.VisualStudio.Help;
using System.Diagnostics;
using HVProject.HV2Lib.Render;
using System.Text;

//http://msdn.microsoft.com/en-us/library/aa767916(VS.85).aspx

namespace HVProject.HV2Lib.PluggableProtocol
{
    [ComVisible(true)]
    [Guid("06955412-24B7-457d-A11F-8584C50F35CD")]
    [ClassInterface(ClassInterfaceType.None)]
    class HelpAsyncPluggableProtocolHandler : IInternetProtocol, IInternetProtocolRoot, IDisposable
    {
        // Member variables
        internal Stream _stream = new MemoryStream(0x8000);
        private byte[] _streamBuffer = new byte[0x8000];
        
//        private static ITopicRenderer _vsRenderer = null;
        private static CustomRenderer _customRenderer = null;


        #region IDisposable interface methods

        // Track whether Dispose has been called.
        private bool disposed;

        // Implement IDisposable.
        // Do not make this method virtual.
        // A derived class should not be able to override this method.
        public void Dispose()
        {
            Dispose(true);
            // This object will be cleaned up by the Dispose method.
            // Therefore, you should call GC.SupressFinalize to
            // take this object off the finalization queue
            // and prevent finalization code for this object
            // from executing a second time.
            GC.SuppressFinalize(this);
        }

        // Dispose(bool disposing) executes in two distinct scenarios.
        // If disposing equals true, the method has been called directly
        // or indirectly by a user's code. Managed and unmanaged resources
        // can be disposed.
        // If disposing equals false, the method has been called by the
        // runtime from inside the finalizer and you should not reference
        // other objects. Only unmanaged resources can be disposed.
        private void Dispose(bool disposing)
        {
            // Check to see if Dispose has already been called.
            if (!this.disposed)
            {
                // If disposing equals true, dispose all managed
                // and unmanaged resources.
                if (disposing)
                {
                    // Dispose managed resources.
                    if (_stream != null)
                        _stream.Dispose();
                }

                // Note disposing has been done.
                disposed = true;
            }
        }

        #endregion

        #region IInternetProtocolRoot interface methods
        // IInternetProtocolRoot Implementation
        void IInternetProtocolRoot.Continue(ref _tagPROTOCOLDATA pProtocolData)
        {
            //Debug.WriteLine("xxContinue");
        }

        void IInternetProtocolRoot.Abort(int hrReason, uint dwOptions)
        {
            //Debug.WriteLine("xxAbort");
        }

        void IInternetProtocolRoot.Terminate(uint dwOptions)
        {
            //Debug.WriteLine("xxTerminate");
        }

        void IInternetProtocolRoot.Suspend()
        {
            //Debug.WriteLine("xxSuspend");
        }

        void IInternetProtocolRoot.Resume()
        {
            //Debug.WriteLine("xxResume");
        }
        #endregion

        #region IInternetProtocol interface methods
        // IInternetProtocol Implementation
        void IInternetProtocol.Resume()
        {
            //Debug.WriteLine("Resume");
        }

        void IInternetProtocol.Terminate(uint dwOptions)
        {
            //Debug.WriteLine("Terminate");
        }

        void IInternetProtocol.Seek(_LARGE_INTEGER dlibMove, uint dwOrigin, out _ULARGE_INTEGER plibNewPosition)
        {
            //Debug.WriteLine("Seek");
            plibNewPosition = new _ULARGE_INTEGER();
        }

        void IInternetProtocol.LockRequest(uint dwOptions)
        {
            //Debug.WriteLine("LockRequest");
        }

        void IInternetProtocol.UnlockRequest()
        {
            //Debug.WriteLine("UnlockRequest");
        }

        void IInternetProtocol.Abort(int hrReason, uint dwOptions)
        {
            //Debug.WriteLine("Abort");
        }

        void IInternetProtocol.Suspend()
        {
            //Debug.WriteLine("Suspend");
        }

        void IInternetProtocol.Continue(ref _tagPROTOCOLDATA pProtocolData)
        {
            //Debug.WriteLine("Continue");
        }

        UInt32 IInternetProtocol.Read(System.IntPtr pv, uint cb, out uint pcbRead)
        {
            pcbRead = 0;
            if (this._stream != null)
            {
                pcbRead = (uint)Math.Min(cb, _streamBuffer.Length);
                pcbRead = (uint)_stream.Read(_streamBuffer, 0, (int)pcbRead);
                Marshal.Copy(_streamBuffer, 0, pv, (int)pcbRead);
                if ((pcbRead == 0) && (this._stream != null))
                {
                    this._stream.Close();
                    this._stream = null;
                }
            }
            UInt32 response = (pcbRead == 0) ? (UInt32)HRESULT.S_FALSE : (UInt32)HRESULT.S_OK;
            return response;
        }
        #endregion

        #region Public Methods

        // private static IRenderParameters _renderParameters = null;

        public HelpAsyncPluggableProtocolHandler()  // Constructor
        {
            //Debug.WriteLine("xxHelpAsyncPluggableProtocolHandler() class constructor");

            // requires VS  

                // _renderParameters = new RenderParameters();

                //_renderParameters = new RenderParameters();
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.CodeSnippetTabIndex, "0");
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.TopicOnlineLinkText, "View this topic <a href=\"{0}\" {1}>online</a>");
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.LogoPath, "?VISUALSTUDIO_2011_BRANDING_EN-US.mshc;/vs_logo_bk.gif");
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.Copyright, "© 2010 Microsoft Corporation. All rights reserved.");
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.SendFeedback, "<a href=\"{0}\" {1}>Send Feedback</a> on this topic to Microsoft.");
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.FeedbackLink, "mailto:DevDocs@Microsoft.com?subject=MSDN%5e*12345%5e*VS%5e*100%5e*JA-JP&amp;body=Visual%20Studio%20feedback%20policy%3a%20http%3a%2f%2fmsdn.microsoft.com%2fJA-JP%2flibrary%2fee264131(VS.100).aspx");
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.BrandingPackagePath, "e:\\Branding_poo_en-US.mshc");
                //_renderParameters.Add((int)TopicRenderer.RenderParameterKeys.CacheSize, "5242880");
        }


        public void Start(string szURL, IInternetProtocolSink sink, IInternetBindInfo pOIBindInfo, uint grfPI, uint dwReserved)
        {
            //Debug.WriteLine("xxHelpAsyncPluggableProtocolHandler.Start(" + szURL + ")");

            String mimeType = "text/html";
                // type = "text" / "image" / "audio" / "video" / "application" / extension-token
                //type/subtype eg. "text/html", "image/png", "image/gif", "video/mpeg", "text/css", and "audio/basic"
            
            if (MsxhelpProtocol.Catalog == null || !MsxhelpProtocol.Catalog.IsOpen)
            {
                _stream = null;
            }
            else
            {
                try
                {
                    if (_customRenderer == null)
                        _customRenderer = CustomRenderer.Create();

                    String topicID;
                    Boolean isTopicUrl = _customRenderer.IsTopicUrl(szURL, out topicID);

                    // -----------------------------------------------------------------------
                    // debug - Host wants to see raw HTML from .mshc

                    if (MsxhelpProtocol .RawCodeHandler != null) 
                    {
                        try
                        {
                            if (isTopicUrl && String.IsNullOrEmpty(MsxhelpProtocol.UserTopicText))  // Topic with no text overload
                            {
                                _stream = _customRenderer.UrlToStream(szURL, false);  // false = No Render. 
                                if (this._stream != null)
                                {
                                    _stream.Position = 0;
                                    StreamReader reader = new StreamReader(_stream);
                                    String text = reader.ReadToEnd();
                                    MsxhelpProtocol.RawCodeHandler(text);    //send text to main form
                                    this._stream.Close();
                                    this._stream = null;
                                }
                            }
                        }
                        catch
                        {
                            //suck it up.. the next bit is the important part
                        }
                    }

                    // -----------------------------------------------------------------------
                    // render - Web browser wants rendered code

//                    if (MsxhelpProtocol.RenderUsingVS)  // Requires VS 11 DLL (Microsoft.VisualStudio.Help.DLL)
//                    {
//                        if (_vsRenderer == null)                      //will crash if VS not installed
//                            _vsRenderer = new TopicRenderer();

 //                       _stream = (Stream)_vsRenderer.ProcessLink(szURL, (ICatalog)MsxhelpProtocol.Catalog, null/*_renderParameters*/);
//                    }
//                    else  // basic render code supplied with this SDK -- Good for Win 8 where VS 11 not installed
                    {
                        _stream = _customRenderer.UrlToStream(szURL);
                        _stream.Position = 0;
                    }
                }
                catch
                {
                    _stream = null;
                }
            }

            if (_stream != null)
            {
                //Set Mime type
                uint BINDSTATUS_MIMETYPEAVAILABLE = 13;
                if (mimeType != "")
                    sink.ReportProgress(BINDSTATUS_MIMETYPEAVAILABLE, mimeType); 
                
                sink.ReportData(BSCF.LASTDATANOTIFICATION | BSCF.DATAFULLYAVAILABLE, (uint)_stream.Length, (uint)_stream.Length);
                sink.ReportResult((uint)HRESULT.S_OK, 200, null);
            }
            else
                sink.ReportResult((uint)HRESULT.INET_E_OBJECT_NOT_FOUND, 404, "Cannot find the goods");
        }

        #endregion
    }
}
