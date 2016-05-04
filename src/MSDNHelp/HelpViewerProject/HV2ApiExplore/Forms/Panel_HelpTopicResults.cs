using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
using System.Runtime.InteropServices.ComTypes;
using Helpware.Misc;
using System.Threading;
using System.IO;
using HVProject.HV2Lib.View;
using HVProject.HV2Lib.PluggableProtocol;
using HVProject.HV2Lib.Render;

namespace HV2ApiExplore.Forms
{
    /// <summary>
    /// HelpTopicResultsPanel
    /// </summary>
    /// View for HelpTopics

    public partial class Panel_HelpTopicResults : UserControl
    {
        public int startListViewNumbering = 1;   //Start of numbering down column 0 in the ListView

        private ITopicCollection _helpTopics = null;   //list of Help Topics to fill virtual listView
        private ITopic _helpTopic = null;


        // Constructor 

        public Panel_HelpTopicResults(Control parentControl, DockStyle dockStyle)
        {
            InitializeComponent();

            //parent, dock ourself
            this.Parent = parentControl;
            this.Dock = DockStyle.Fill;
            this.BringToFront();

            new SuperTip(HlpViewerBtn, "Show in HlpViewer", "Show selected help topic in HlpViewer.exe");
            new SuperTip(tipUnrendered, "View Unrendered Source", 
                "Displays the raw unrendered topic source directly from the .mshc help file.",
                "Use 'View Source' (webBrowser context menu) to view the 'rendered' source.");
        }

        // Set Data for panel

        public void SetVirtualList(ITopicCollection helpTopics) 
        {
            Clear();
            this._helpTopics = helpTopics;
            this.searchResultLV.VirtualListSize = helpTopics.Count;

            //Auto=select first linkNode & show destails
            if (searchResultLV.Items.Count > 0)
            {
                searchResultLV.Items[0].Selected = true;
                UpdateTopicView(0);
            }
        }


        public void Clear()
        {
            detailsRichTextBox.Clear();
            webBrowser1.Navigate("about:blank");

            searchResultLV.VirtualListSize = 0;  //Clear ListView
            _helpTopics = null;
            _helpTopic = null;
        }


        // Virtual ListView wants data

        private void searchResultLV_RetrieveVirtualItem(object sender, RetrieveVirtualItemEventArgs e)
        {
            if (_helpTopics == null || e.ItemIndex >= _helpTopics.Count)
                return;

            //Get Topic record

            int idx = e.ItemIndex;
            _helpTopics.MoveTo(idx);
            ITopic topic = (ITopic)_helpTopics.Current;

            String itemNo = (startListViewNumbering + idx).ToString();  //rank as returned by runtime
            string[] subItems = new string[searchResultLV.Columns.Count - 1];  //first column is title
            subItems[0] = topic.Title;
            subItems[1] = topic.DisplayVersion;
            subItems[2] = topic.Vendor;
            subItems[3] = topic.Locale;
            subItems[4] = topic.Id;

            ListViewItem li = new ListViewItem(itemNo);
            li.SubItems.AddRange(subItems);
            e.Item = li;
        }


        // Update Topic info (Right side)

        private String ExpandStringList(String[] stringList)
        {
            String ss = "";
            foreach (String s in stringList)
                if (ss.Length == 0) ss = s; else ss = ss + " | " + s;
            return ss;
        }

        //eg. "ms-xhelp:///?method=page&id=xxx&locale=en-US&vendor=Microsoft&topicVersion=&topicLocale=EN-US"

        private String UrlFromTopic(Topic topic)
        {
            String url = "ms-xhelp:///?method=page&id=" + topic.Id;

            if (!String.IsNullOrEmpty(topic.Locale))
                url += "&locale=" + topic.Locale;
            if (!String.IsNullOrEmpty(topic.Vendor))
                url += "&vendor=" + topic.Vendor;
            if (!String.IsNullOrEmpty(topic.TopicVersion))
                url += "&topicVersion=" + topic.TopicVersion;
            if (!String.IsNullOrEmpty(topic.TopicLocale))
                url += "&topicLocale=" + topic.TopicLocale;
            return url;
        }

        private void UpdateTopicDetails(ITopic helpTopic)
        {
            if (helpTopic == null)
                return;
            _helpTopic = helpTopic;     //remember in global

            // Details View

            detailsRichTextBox.Clear();
            RichTextBoxFuncs rtf = new RichTextBoxFuncs(detailsRichTextBox);

            rtf.WriteLine("topic.Category", ExpandStringList(_helpTopic.Category()));
            rtf.WriteLine("topic.ContentFilter", ExpandStringList(_helpTopic.ContentFilter()));
            rtf.WriteLine("topic.ContentType", ExpandStringList(_helpTopic.ContentType()));
            rtf.WriteLine("topic.Description", _helpTopic.Description);
            rtf.WriteLine("topic.DisplayVersion", _helpTopic.DisplayVersion);
            rtf.WriteLine("topic.Id", _helpTopic.Id);
            rtf.WriteLine("topic.Locale", _helpTopic.Locale);
            rtf.WriteLine("topic.Package", _helpTopic.Package);
            rtf.WriteLine("topic.ParentId", _helpTopic.ParentId);
            rtf.WriteLine("topic.TableOfContentsHasChildren", _helpTopic.TableOfContentsHasChildren.ToString());
            rtf.WriteLine("topic.TableOfContentsPosition", _helpTopic.TableOfContentsPosition.ToString());
            rtf.WriteLine("topic.Title", _helpTopic.Title);
            rtf.WriteLine("topic.TopicLocale", _helpTopic.TopicLocale);
            rtf.WriteLine("topic.TopicVersion", _helpTopic.TopicVersion);
            rtf.WriteLine("topic.Url", _helpTopic.Url);
            rtf.WriteLine("topic.Vendor", _helpTopic.Vendor);
        }

        private void UpdateTopicView(ITopic helpTopic)
        {
            if (helpTopic == null)
                return;
            _helpTopic = helpTopic;     //remember in global

            // Fill Browser -- The work here is done by our pluggable protocole code. It does all the rendering.
            // This will also trigger a filling of the Details pane on Navigate complete event
            
            String url = UrlFromTopic((Topic)helpTopic);
            webBrowser1.Navigate(url);
        }

        private void UpdateTopicView(int index)
        {
            if (index < 0 && searchResultLV.SelectedIndices.Count > 0)  //use ListView selection
                index = searchResultLV.SelectedIndices[0];

            if (index >= 0 && index < _helpTopics.Count)    // in range
            {
                _helpTopics.MoveTo(index);
                ITopic helpTopic = (ITopic)_helpTopics.Current;
                UpdateTopicView(helpTopic);
            }
            else
            {
                UpdateTopicView(_helpTopic);
            }
        }

        private void searchResultLV_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (searchResultLV.SelectedIndices.Count > 0)
            {
                UpdateTopicView(searchResultLV.SelectedIndices[0]);
            }
        }

        // Public - Call to display just a HelpTopic

        public void DisplayHelpTopic(ITopic helpTopic)
        {
            UpdateTopicView(helpTopic);
        }

        // Show selected topic in HlpViewer.exe

        private void HlpViewerBtn_Click(object sender, EventArgs e)
        {
            String pageId = "-1";
            if (_helpTopic != null)
                pageId = _helpTopic.Id.Trim();

            Globals.StartHelpViewer(pageId);
        }

        private void searchResultLV_DoubleClick(object sender, EventArgs e)
        {
            HlpViewerBtn_Click(sender, null);
        }


        //Force ListView to stay selected

        private void searchResultLV_MouseUp(object sender, MouseEventArgs e)
        {
            try
            {
                if (searchResultLV.Items.Count > 0
                    && searchResultLV.FocusedItem != null
                    && searchResultLV.FocusedItem.Index >= 0)
                {
                    if (!searchResultLV.Items[searchResultLV.FocusedItem.Index].Selected)
                        searchResultLV.Items[searchResultLV.FocusedItem.Index].Selected = true;
                }
            }
            catch
            {
                //swallow the exception (although the only time I've ever seen one is accessing searchResultLV.SelectedIndices.Count while double clicking)
            }
        }


        // View

        public enum HelpTopicResultsView { AllTopics, HelpTopic, RawContent };

        public void SetupView(HelpTopicResultsView view)
        {
            splitContainer1.Panel1Collapsed = (view != HelpTopicResultsView.AllTopics);
            if (view == HelpTopicResultsView.RawContent)  //Minimum View - Just Raw data
            {
                // ..
            }
        }


        // Update Details view when user clicks a link in the browser

        private void webBrowser1_Navigating(object sender, WebBrowserNavigatingEventArgs e)
        {
            String url = e.Url.AbsoluteUri;
            if (url.StartsWith("ms-xhelp:", StringComparison.OrdinalIgnoreCase))
            {
                HelpQuery helpQuery = new HelpQuery(url);
                if (helpQuery.Method.Equals("page", StringComparison.OrdinalIgnoreCase))
                {
                    //get topic from ID
                    try
                    {
                        Topic topic = (Topic)Globals.catalogRead.GetIndexedTopicDetails(Globals.catalog, helpQuery.AssetId, null);
                        UpdateTopicDetails(topic); //update details only
                    }
                    catch
                    {
                    }
                }
            }
        }

        // View Unrendered Source

        private void linkUnrendered_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if (_helpTopic == null)
                return;

            try
            {
                Stream stream = (Stream)_helpTopic.FetchContent();
                if (stream != null)
                {
                    //Browser Pane
                    //webBrowser1.DocumentStream = stream;

                    //Details Pane
                    StreamReader reader = new StreamReader(stream);
                    detailsRichTextBox.Clear();
                    detailsRichTextBox.Text = reader.ReadToEnd();

                    //good idea to close all open streams
                    stream.Close();
                }
            }
            catch
            {
                detailsRichTextBox.Clear();
                detailsRichTextBox.Text = "** Error fetching topic stream **";
            }
        }

        private void linkBackToDetails_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            UpdateTopicDetails(_helpTopic);
        }

        // Public - View Stream 

        public void ViewStream(Stream stream)
        {
            if (stream != null)
            {
                //Details Pane
                StreamReader reader = new StreamReader(stream);
                detailsRichTextBox.Clear();
                detailsRichTextBox.Text = reader.ReadToEnd();

                //Browser Pane
                webBrowser1.DocumentText = detailsRichTextBox.Text;
            }
        }







    }
}
