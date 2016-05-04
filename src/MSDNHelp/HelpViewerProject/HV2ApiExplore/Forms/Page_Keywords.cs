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
using System.Threading;
using HVProject.HV2Lib.View;
using System.IO;


namespace HV2ApiExplore.Forms
{
    public partial class Page_Keywords : UserControl
    {
        public static Page_Keywords self = null;
        private readonly Panel_HelpTopicResults resultsPanel;   //Index linkNode results panel
        private System.Windows.Forms.Timer oneshotTimer = null;
        private IKeywordCollection _helpKeywords = null;
        private IndexCache indexCache = new IndexCache();

        internal static Page_Keywords Create()
        {
            self = new Page_Keywords();
            return self;
        }


        // Constructor

        public Page_Keywords()
        {
            InitializeComponent();
            self = this;

            //Panel for to show HelpTopic results
            resultsPanel = new Panel_HelpTopicResults(splitContainer1.Panel2, DockStyle.Fill);
            UpdateStatus();

            // our oneshot watchTimer
            oneshotTimer = new System.Windows.Forms.Timer();
            oneshotTimer.Interval = 200;
            oneshotTimer.Tick += new EventHandler(oneshotTimer_Tick);

            // Super Tips 
            new SuperTip(GetKeywordsTip, "CatalogRead.GetKeywords()", 
                "This call returns a list of all visible keywords.", 
                "Try typing into the search box (word-wheel).");
        }

        public void ClearAll()   //call when Catalog is closed
        {
            resultsPanel.Clear();
            keywordsListView.VirtualListSize = 0;
            indexCache.Clear();
            _helpKeywords = null;
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_KEYWORDS);
        }


        // GetKeywords()

        private void GetKeywordsBtn_Click(object sender, EventArgs e)
        {
            ClearAll();

            if (!Globals.catalog.IsOpen)
            {
                MessageBox.Show("Open a catalog first!");
                return;
            }

            Cursor.Current = Cursors.WaitCursor;
            try
            {
                //Setup screen 
                splitContainer1.Panel1Collapsed = false;
                resultsPanel.SetupView(Panel_HelpTopicResults.HelpTopicResultsView.AllTopics);

                _helpKeywords = Globals.catalogRead.GetKeywords(Globals.catalog, true);  //true = caching. This really speeds up loading
                keywordsListView.VirtualListSize = _helpKeywords.Count;
                UpdateStatus();
            }
            catch
            {
                ClearAll();
                MessageBox.Show("Exception while calling catalogRead.GetKeywords()");
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }
        }



        // Find Keyword

        private void searchTextBox_TextChanged(object sender, EventArgs e)
        {
            //Crude but effective -- Use a oneshot watchTimer so we can type ahead without blocking the thread
            oneshotTimer.Stop();   //Stop current activity
            oneshotTimer.Start();  //Kick off watchTimer - Calls Timer_Tick() in xx ms
        }

        private void oneshotTimer_Tick(object sender, EventArgs e)
        {
            oneshotTimer.Stop();  //done with watchTimer
            SearchForText();
        }

        private void SearchForText()
        {
            int iSelected = -1;
            if (keywordsListView.SelectedIndices.Count > 0)
                iSelected = keywordsListView.SelectedIndices[0];

            String text = searchTextBox.Text.Trim();
            if (_helpKeywords != null && _helpKeywords.Count > 0)
            {
                int i = 0;
                if (!String.IsNullOrEmpty(text))
                    i = _helpKeywords.MoveToKeyword(text);
                if (i >= 0 && i <= _helpKeywords.Count && i != iSelected)
                {
                    //Select and scroll match into view
                    keywordsListView.BeginUpdate();           /// This is really important on a Virtual List as we are doing several list changes. Will crash without it.
                    try
                    {
                        keywordsListView.SelectedIndices.Clear();
                        keywordsListView.SelectedIndices.Add(i);
                        keywordsListView.Items[i].EnsureVisible();
                        keywordsListView.TopItem = keywordsListView.Items[i];
                    }
                    finally
                    {
                        keywordsListView.EndUpdate();
                    }
                }
            }
        }


        private void UpdateStatus()
        {
            if (_helpKeywords == null || _helpKeywords.Count == 0)
                statusLabel.Text = "Total items:";
            else
            {
                statusLabel.Text = "Total items: " + _helpKeywords.Count.ToString("#,##0");
            }
        }

        // ListView wants data

        private ListViewItem GetListViewItem(int index, String sKeyword, Boolean fIsSubItem, int cTopics) //topicID.DisplayValue, topicID.IsSubkey, topicID.Topics.Count
        {
            String tab = "";
            if (fIsSubItem)
                tab = "    ";
            ListViewItem li = new ListViewItem((index + 1).ToString("#,##0"));
            li.SubItems.AddRange(new string[] { tab + Uri.UnescapeDataString(sKeyword), cTopics.ToString() });
            return li;
        }


        // Vitual ListView wants data

        private void keywordsListView_RetrieveVirtualItem(object sender, RetrieveVirtualItemEventArgs e)
        {
            int i = e.ItemIndex;
            ListViewItem li = new ListViewItem((i + 1).ToString());

            if (_helpKeywords == null || i >= _helpKeywords.Count)  //range check
            {
                e.Item = GetListViewItem(i, "??", false, 0);
                return;
            }

            //Note: We can improve speed and smoothness with some local caching

            int topicCount;
            Keyword keyword = indexCache.GetKW(i, out topicCount);
            if (keyword == null)
            {
                _helpKeywords.MoveTo(i);
                keyword = (Keyword)_helpKeywords.Current;
                topicCount = keyword.Topics.Count;
                indexCache.Add(i, keyword, topicCount);
            }

            String kwText = keyword.DisplayValue;
            if (keyword.IsSubkey)
                kwText = "    " + kwText;
            e.Item = GetListViewItem(i, kwText, keyword.IsSubkey, topicCount);
        }


        private int SelectedKeyword
        {
            get
            {
                int index = -1;
                if (keywordsListView.SelectedIndices.Count > 0)
                    index = keywordsListView.SelectedIndices[0];
                return index;
            }
        }

        private void keywordsListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = SelectedKeyword;
            if (index < 0 || index >= _helpKeywords.Count)
                return;

            _helpKeywords.MoveTo(index);
            IKeyword keyword = (IKeyword)_helpKeywords.Current;

            keywordsTextBox.Clear();
            RichTextBoxFuncs rtf = new RichTextBoxFuncs(keywordsTextBox);
            rtf.WriteLine("IKeyword Members:");
            rtf.WriteLine("DisplayValue", keyword.DisplayValue);
            rtf.WriteLine("IsSubkey", keyword.IsSubkey.ToString());
            rtf.WriteLine("Value", keyword.Value);
            rtf.WriteLine("Topics", keyword.Topics.Count.ToString());

            //Fill Results Panel
            resultsPanel.SetVirtualList(keyword.Topics);
        }



        //Resize - Autofit columns: Autosize column[1]

        private void panelKeywords_Resize(object sender, EventArgs e)
        {
            if (keywordsListView.Columns.Count == 3)
            {
                keywordsListView.Columns[1].Width = keywordsListView.ClientSize.Width - keywordsListView.Columns[0].Width - keywordsListView.Columns[2].Width - 16;
            }
        }

        // Nice Index things

        private void keywordsListView_KeyPress(object sender, KeyPressEventArgs e)
        {
            //If typing in the ListView move the focus and char to the Edit box

            e.Handled = true;
            String text = searchTextBox.Text;
            if (e.KeyChar < ' ')  // Low ascii
            {
                if (e.KeyChar == '\b')  //Back space
                {
                    if (text.Length > 0)
                        text = text.Substring(0, text.Length - 1);
                }
                else
                    return;
            }
            else //Ascii = add to current chars
            {
                text = text + e.KeyChar;
            }
            searchTextBox.Text = text;
            searchTextBox.SelectionStart = searchTextBox.Text.Length;
            searchTextBox.Focus();
        }

        private void searchTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            //If pressing up/dn keys in the edit box, send them to the ListView
            if (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down
             || e.KeyCode == Keys.PageUp || e.KeyCode == Keys.PageDown)
            {
                keywordsListView.Focus();
                //String keys = e.KeyCode.ToString();
                //SendKeys.Send("{"+keys+"}");
                e.SuppressKeyPress = true;
            }
        }



    }



    

}
