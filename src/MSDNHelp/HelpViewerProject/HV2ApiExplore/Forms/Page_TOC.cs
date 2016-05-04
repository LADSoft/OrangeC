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
using Helpware.Misc;
using HVProject.HV2Lib.View;

namespace HV2ApiExplore.Forms
{
    public partial class Page_TOC : UserControl
    {
        public static Page_TOC self = null;
        private readonly Panel_HelpTopicResults resultsPanel;

        internal static Page_TOC Create()
        {
            self = new Page_TOC();
            return self;
        }

        Panel_Toc tocPanel = null;
        
        // Constructor

        public Page_TOC()
        {
            InitializeComponent();
            self = this;

            //Panel for to show HelpTopic results
            resultsPanel = new Panel_HelpTopicResults(this, DockStyle.Fill);

            //Toc Select panel
            tocPanel = new Panel_Toc(this, TopicIdCbo, Globals.catalog);

            //Persistence: Load settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            {
                TopicIdCbo.Text = userData.ReadStr("Toc.TopicIdCbo", "");
            }

            //Fill TocReturnDetail Enumeration dropdown
            tocDetailsCbo.Items.Clear();
            foreach (TocReturnDetail value in Enum.GetValues(typeof(TocReturnDetail)))
                tocDetailsCbo.Items.Add(value.ToString());
            tocDetailsCbo.SelectedIndex = 0;

            //SuperTips
            new SuperTip(TopicIDPictureBox, "Topic ID", 
                "Enter topic ID or select one from the drop down.",
                "Select the TocReturnDetail you want and click the Get button.",
                "Use an Id=\"-1\" to specify the virtual root page of the catalog");
            
            new SuperTip(tocDetailsCboTip,  ToolTipIcon.Warning, "Warning", "Using 'TocReturnDetail.TocDescendants' can take a long time to complete",
                      "since a top level node can return many 1000's of child/sub-child items.",
                      "In this case be patient. The call will return eventually.");

            new SuperTip(HlpViewerBtn, "Show in HlpViewer", "Show selected help topic in HlpViewer.exe");

        }

        public void ClearAll()   //call when Catalog is closed
        {
            resultsPanel.Clear();
            tocPanel.Clear();
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_TOC);
        }


        // TOC Command

        private void DoTOCCmd()
        {
            if (!Globals.catalog.IsOpen)
            {
                MessageBox.Show("Open a catalog first!");
                return;
            }

            String TopicId = TopicIdCbo.Text.Trim();
            if (String.IsNullOrEmpty(TopicId))
            {
                MessageBox.Show("Topic ID required. ");
                return;
            }

            HelpFilter filter = null;  //no fancy adv filtering yet
            resultsPanel.Clear();

            TocReturnDetail tocDetail = (TocReturnDetail)tocDetailsCbo.SelectedIndex;
            Cursor.Current = Cursors.WaitCursor;
            try
            {
                ITopicCollection helpTopics = Globals.catalogRead.GetTableOfContents(Globals.catalog, TopicId, filter, tocDetail);
                resultsPanel.SetVirtualList(helpTopics);
            }
            catch
            {
                resultsPanel.Clear();
                MessageBox.Show("Exception calling ICatalogRead.GetTableOfContents(..)");
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }

        }

        private void GetTocBtn_Click(object sender, EventArgs e)
        {
            DoTOCCmd();
        }

        private void TopicIdCbo_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
                DoTOCCmd();
        }

        private void TopicIdCbo_TextChanged(object sender, EventArgs e)
        {
            //Persistence: Save settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            {
                userData.WriteStr("Toc.TopicIdCbo", TopicIdCbo.Text);
            }
        }

        // HlpViewer.exe

        private void HlpViewerBtn_Click(object sender, EventArgs e)
        {
            Globals.StartHelpViewer(TopicIdCbo.Text.Trim());
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
