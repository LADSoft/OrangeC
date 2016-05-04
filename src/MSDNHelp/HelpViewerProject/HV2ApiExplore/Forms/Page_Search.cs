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

namespace HV2ApiExplore.Forms
{
    public partial class Page_Search : UserControl
    {
        public static Page_Search self = null;
        private readonly Panel_HelpTopicResults resultsPanel;   //search results inserted screen bottom

        internal static Page_Search Create()
        {
            self = new Page_Search();
            return self;
        }

        Panel_SearchOperator operatorsPanel = null;

        // Constructor

        public Page_Search()
        {
            InitializeComponent();
            self = this;

            //Panel for to show HelpTopic results
            resultsPanel = new Panel_HelpTopicResults(this, DockStyle.Fill);

            //Search Operator panel
            operatorsPanel = new Panel_SearchOperator(this, SearchCbo);

            //Persistence: Load settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            { 
                SearchCbo.Text = userData.ReadStr("Search.Query", "");
                pageSizeEdit.Value = userData.ReadInt("Search.PageSize", 50);  
            }

            statusLabel.Text = "--";
            UpdateStatus(false);

            //Super Tips
            new SuperTip(tipResultsPerPage, "Results pre Page", 
                "Often many 1000's of search results will be available.",
                "So search calls return just one page of results at a time.",
                "In this demo you can specify the size of a page and use",
                "next and previous links to step through all available pages.");
            new SuperTip(tipMatchHighlighting, "Match Highlighting",
                "When checked search results are wrapped in <span> tags",
                "to highlight search query terms. This would be useful if",
                "you were creating your own HTML page of search results.");
            new SuperTip(tipOrQueryTerms, "OR Query Terms",
                "Normally when you enter 2 or more search query terms, they",
                "are AND'd together. Check this box to 'OR' them instead.");
        }

        public void ClearAll()   //call when Catalog is closed
        {
            resultsPanel.Clear();
            UpdateStatus(false);
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_SEARCH);
        }


        //==== Search ====

        private String _sQuery = "";
        private SearchOptions _searchOptions = SearchOptions.None;
        public int _pageNumber = 1;           //1..n
        public int _pageSize = 50;            //max items per page (max items return from SEarch call)
        public int _totalAvailableHits = 0;   //total hits in all _pages -- Do a search call to fill each page

        private void DoSearch()
        {
            if (!Globals.catalog.IsOpen)
            {
                MessageBox.Show("Open a catalog first!");
                return;
            }

            HelpFilter filter = null;  //no fancy adv filtering yet
            UpdateStatus(false);
            Cursor.Current = Cursors.WaitCursor;
            resultsPanel.Clear();

            try
            {
                ITopicCollection helpTopics = Globals.catalogRead.GetSearchResults(Globals.catalog, _sQuery, filter, _searchOptions, _pageSize, _pageNumber, out _totalAvailableHits);
                resultsPanel.SetVirtualList(helpTopics);
                UpdateStatus(true);
            }
            catch
            {
                resultsPanel.Clear();
                _pageNumber = 1;
                _totalAvailableHits = 0;
                MessageBox.Show("Exception calling ICatalogRead.GetSearchResults(..)");
                return;
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }

        }

        private void SearchBtnSearch()
        {
            _sQuery = SearchCbo.Text.Trim();
            if (String.IsNullOrEmpty(_sQuery))
                return;

            _searchOptions = SearchOptions.None;
            if (SearchTermHighlightCbx.Checked)
                _searchOptions = _searchOptions | SearchOptions.SearchTermHighlight;
            if (OrSearchOverrideCbx.Checked)
                _searchOptions = _searchOptions | SearchOptions.OrSearchOverride;

            //When searching with the Search Button we always start from Page 1
            _pageSize = (Int32)pageSizeEdit.Value;
            _pageNumber = 1;
            _totalAvailableHits = 0;
            resultsPanel.startListViewNumbering = 1;

            DoSearch();

            //Persistence: Save settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            {
                userData.WriteStr("Search.Query", SearchCbo.Text);
                userData.WriteInt("Search.PageSize", (int)pageSizeEdit.Value);
            }
        }

        private void SearchBtn_Click(object sender, EventArgs e)
        {
            SearchBtnSearch();
        }

        private void SearchCbo_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return)
                SearchBtnSearch();
        }



        // == Pages ==

        #region *** Change _pages << < > >>  

        private int PageCount
        {
            get 
            {
                int pages = _totalAvailableHits / _pageSize;
                if (pages * _pageSize < _totalAvailableHits)
                    pages++;   //last page is incomplete
                return pages;
            }
        }

        private void PageChange(int newPageNumber)
        {
            if (newPageNumber != _pageNumber && _sQuery != "")  //change && a query was run
            {
                _pageNumber = newPageNumber;
                resultsPanel.startListViewNumbering = (_pageNumber - 1) * _pageSize + 1;
                DoSearch();
            }
        }

        private void UpdateStatus(Boolean aShow)
        {
            if (aShow && _totalAvailableHits == 0)
                statusLabel.Text = " " + _totalAvailableHits.ToString("#,##0") + " total results";
            else if (aShow)
                statusLabel.Text = " Page: " + _pageNumber.ToString("#,##0") + " of " + PageCount.ToString("#,##0") + ";  " + _totalAvailableHits.ToString("#,##0") + " total results";
            else
                statusLabel.Text = "";
            // Enable buttons
            int pgCount = PageCount;
            firstLink.Enabled = aShow && _pageNumber != 1;
            prevLink.Enabled = aShow && (_pageNumber > 1) && _pageNumber > 1;
            nextLink.Enabled = aShow && (_pageNumber < pgCount) && pgCount > 1;
            lastLink.Enabled = aShow && (_pageNumber != pgCount) && pgCount > 1;
        }

        private void firstLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            PageChange(1);
        }

        private void prevLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            int p = _pageNumber - 1;
            if (p < 1)
                p = 1;
            PageChange(p);
        }

        private void nextLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            int p = _pageNumber + 1;
            if (p > PageCount)
                p = PageCount;
            PageChange(p);
        }

        private void lastLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            PageChange(PageCount);
        }

        #endregion


        // Show / Hide TOC dropdown

        private void SearchCbo_DropDown(object sender, EventArgs e)
        {
            operatorsPanel.SmartShow();
        }




    }


    
}
