using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Helpware.Misc;
#if VSDLL
using Microsoft.VisualStudio.Help.Runtime;
#else
using Windows.Help.Runtime;
#endif
using System.Runtime.InteropServices.ComTypes;
using HVProject.HV2Lib.View;
using System.IO;

namespace HV2ApiExplore.Forms
{
    public partial class Page_Topics : UserControl
    {
        public static Page_Topics self = null;
        private readonly Panel_HelpTopicResults resultsPanel = null;   //Index linkNode results panel
        private readonly Panel_Toc tocPanel = null;

        internal static Page_Topics Create()
        {
            self = new Page_Topics();
            return self;
        }

        public Page_Topics()
        {
            InitializeComponent();
            self = this;

            //Panel for to show HelpTopic results
            resultsPanel = new Panel_HelpTopicResults(this, DockStyle.Fill);
            resultsPanel.SetupView(Panel_HelpTopicResults.HelpTopicResultsView.HelpTopic);

            //Toc Select panel
            tocPanel = new Panel_Toc(this, TopicFromIdCbo, Globals.catalog);

            //Persistence: Load settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            {
                TopicFromIdCbo.Text = userData.ReadStr("Keyword.TopicFromIdCbo", "");
                TopicFromF1TextBox.Text = userData.ReadStr("Keyword.TopicFromF1TextBox", "");
            }

            // Super Tips 
            new SuperTip(tipFileStreamOnly, "File Stream Only",
                "The raw unrendered topic source from the .mshc help file.",
                "Expect Script errors. Being unrendered it won't be WebBrowser friendly.");

            new SuperTip(TopicFromIdTip, "Get Topic for Topic ID",
                "ITopic = ICatalogRead.GetIndexedTopicDetails()",
                "IStream = ICatalogRead.GetIndexedTopic()",
                "Enter a topic ID for the required topic.",
                "If successful the call returns a topic file ITopic object or topic IStream.",
                "Note that the topic object also contains a convenient method to Fetch the stream.");
            new SuperTip(TopicFromF1Tip, "Get Topic for F1 Keyword(s)",
                "ITopic = ICatalogRead.GetTopicDetailsForF1Keyword()",
                "IStream = ICatalogRead.GetTopicForF1Keyword()",
                "Enter a prioritized list of F1 keywords separated by '|' chars (arbitary).",
                "If successful the call returns a ITopic Object or a topic IStream.",
                "Note that the topic object also contains a convenient method to Fetch the stream.");
            new SuperTip(GetAssetTip, "IStream catalogRead.GetLinkedAsset(catalog, packageName, path, locale)",
                "Example:    ",
                "   Stream stream = catalogRead.GetLinkedAsset(catalog, ",
                "        \"Visual_Studio_21800791_VS_100_en-us_6.mshc\",",
                "        \"\\R583.htm\", \"en-US\");",
                "For the purpose of this quick demo we get an asset path from ",
                "a Topic obj retieved using the specified topic ID.",
                "However this can be used to retrieve any asset file (image, JS, CSS etc).");

        }

        public void ClearAll()   //call when Catalog is closed
        {
            resultsPanel.Clear();
            tocPanel.Clear();
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_TOPICS);
        }


        // Get Topic by ID

        private void DoGetTopicFromId(Boolean getDetails)  //false to return just the file IStream
        {
            if (!Globals.catalog.IsOpen)
            {
                MessageBox.Show("Open a catalog first!");
                return;
            }

            ClearAll();

            IHelpFilter filter = null;  // no filtering

            String topicID = TopicFromIdCbo.Text.Trim();
            if (String.IsNullOrEmpty(topicID))
                return;

            Cursor.Current = Cursors.WaitCursor;
            try
            {
                if (getDetails)  //Returns detail object
                {
                    resultsPanel.SetupView(Panel_HelpTopicResults.HelpTopicResultsView.HelpTopic);

                    ITopic helpTopic = Globals.catalogRead.GetIndexedTopicDetails(Globals.catalog, topicID, filter);

                    if (helpTopic == null)
                        MessageBox.Show("No results found! Returned null.");
                    else  //display result
                        resultsPanel.DisplayHelpTopic(helpTopic);
                }
                else //Returns just the stream
                {
                    resultsPanel.SetupView(Panel_HelpTopicResults.HelpTopicResultsView.RawContent);

                    //Get Stream only
                    Stream stream = (Stream)Globals.catalogRead.GetIndexedTopic(Globals.catalog, topicID, filter);
                    if (stream == null)
                        MessageBox.Show("No results found! Returned null.");
                    else 
                    {
                        resultsPanel.ViewStream(stream);
                        stream.Close();
                    }
                }
            }
            catch
            {
                MessageBox.Show("No results. Exception while calling catalogRead.GetIndexedTopic...()");
            }
            finally
            {
                Cursor.Current = Cursors.Default;

                //Persistence: Save settings
                using (UserData userData = new UserData(Globals.DataRegKey))
                {
                    userData.WriteStr("Keyword.TopicFromIdCbo", TopicFromIdCbo.Text);
                }
            }
        }


        // Get Linked Asset == IStream GetLinkedAsset(ICatalog catalog, string packageName, string path, string locale)

        private void GetAssetBtn_Click(object sender, EventArgs e)
        {
            if (!Globals.catalog.IsOpen)
            {
                MessageBox.Show("Open a catalog first!");
                return;
            }

            ClearAll();

            IHelpFilter filter = null;  // no filtering

            String keyword = TopicFromIdCbo.Text.Trim();
            if (String.IsNullOrEmpty(keyword))
                return;

            Cursor.Current = Cursors.WaitCursor;
            try
            {
                //Returns topic detail
                ITopic helpTopic = Globals.catalogRead.GetIndexedTopicDetails(Globals.catalog, keyword, filter);
                if (helpTopic == null)
                    MessageBox.Show("No results found! GetIndexedTopicDetails returned null.");
                else  //display result
                {
                    resultsPanel.SetupView(Panel_HelpTopicResults.HelpTopicResultsView.RawContent);

                    // Now try an Asset call
                    // The call above returns helpTopic.Package == helpTopic.Url = package;\\path (so we need to split this out into Package & internal path
                    //Note: This is test is rather artificial but gives you an idea of how to access the GetLinkedAsset() call.

                    String[] url = helpTopic.Url.Split(new char[] {';'});
                    if (url.Length == 2)
                    {
                        Stream stream = (Stream)Globals.catalogRead.GetLinkedAsset(Globals.catalog, url[0], url[1], helpTopic.Locale);
                        if (stream == null)
                            MessageBox.Show("GetLinkedAsset() returned null.");
                        else
                        {
                            resultsPanel.ViewStream(stream);
                            stream.Close();
                        }
                    }
                    else
                    {
                        MessageBox.Show("Could Test. Unable to extract Package name and internal path from \""+helpTopic.Url+"\"");
                    }
                }
            }
            catch
            {
                MessageBox.Show("No results. Exception!");
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }
        }


        // Get Topic by F1 Keyword

        private void DoGetTopicFromF1(Boolean getDetails)  //false to return just the file IStream
        {
            if (!Globals.catalog.IsOpen)
            {
                MessageBox.Show("Open a catalog first!");
                return;
            }

            ClearAll();

            IHelpFilter filter = null;  // no filtering

            String keyword = TopicFromF1TextBox.Text.Trim();
            if (String.IsNullOrEmpty(keyword))
                return;

            String[] prioritizedF1Keywords = keyword.Split(new Char[] { '|' });   //arbitarily separate by ;
            for (int i = 0; i < prioritizedF1Keywords.Length; i++)
                prioritizedF1Keywords[i] = prioritizedF1Keywords[i].Trim();

            Cursor.Current = Cursors.WaitCursor;
            try
            {
                if (getDetails)  //Returns detail and stream
                {
                    resultsPanel.SetupView(Panel_HelpTopicResults.HelpTopicResultsView.HelpTopic);

                    //Do It
                    ITopic helpTopic = Globals.catalogRead.GetTopicDetailsForF1Keyword(Globals.catalog, prioritizedF1Keywords, filter);

                    if (helpTopic == null)
                        MessageBox.Show("No results found! Returned null.");
                    else  //display result
                        resultsPanel.DisplayHelpTopic(helpTopic);
                }
                else              //Returns just the stream
                {
                    resultsPanel.SetupView(Panel_HelpTopicResults.HelpTopicResultsView.RawContent);

                    //Do It
                    Stream stream = (Stream)Globals.catalogRead.GetTopicForF1Keyword(Globals.catalog, prioritizedF1Keywords, filter);

                    if (stream == null)
                        MessageBox.Show("No results found! Returned null.");
                    {
                        resultsPanel.ViewStream(stream);
                        stream.Close();
                    }
                }
            }
            catch
            {
                MessageBox.Show("No results. Exception while calling catalogRead.GetTopic...ForF1Keyword()");
            }
            finally
            {
                Cursor.Current = Cursors.Default;

                //Persistence: Save settings
                using (UserData userData = new UserData(Globals.DataRegKey))
                {
                    userData.WriteStr("Keyword.TopicFromF1TextBox", TopicFromF1TextBox.Text);
                }
            }
        }

        private void GetTopicFromIdBtn_Click(object sender, EventArgs e)
        {
            DoGetTopicFromId(rdoGetTopicDetail.Checked);
        }

        private void GetTopicFromF1Btn_Click(object sender, EventArgs e)
        {
            DoGetTopicFromF1(rdoGetTopicDetail.Checked);
        }

        private void TopicFromIdCbo_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
                DoGetTopicFromId(rdoGetTopicDetail.Checked);
        }

        private void TopicFromF1TextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
                DoGetTopicFromF1(rdoGetTopicDetail.Checked);
        }

        // Show / Hide TOC dropdown

        private void TopicIdCbo_DropDown(object sender, EventArgs e)
        {
            if (!tocPanel.Visible)
            {
                if (tocPanel.SmartShow())
                    tocPanel.HelpCatalog = Globals.catalog;  //rebuild TOC if required
            }
        }




    }
}
