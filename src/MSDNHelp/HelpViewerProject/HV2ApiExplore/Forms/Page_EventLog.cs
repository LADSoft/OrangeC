using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Diagnostics;
using System.Web;
using Helpware.Misc;


namespace HV2ApiExplore.Forms
{
    public partial class Page_EventLog : UserControl
    {
        public static Page_EventLog self = null;
        readonly TabPage _hostTabPage = null;
        readonly String _hostTabPageText = "";

        internal static Page_EventLog Create(TabPage hostTabPage)
        {
            self = new Page_EventLog(hostTabPage);
            return self;
        }

        EventLog eventlog = null;
        Timer watchTimer = null;
        int _newItemsSinceStartup = 0;
        int _LastRecordID = -1;

        List<EventLogEntry> _cache = new List<EventLogEntry>();

        public Page_EventLog(TabPage hostTabPage)
        {
            InitializeComponent();
            self = this;

            _hostTabPage = hostTabPage;
            if (_hostTabPage != null)
                _hostTabPageText = _hostTabPage.Text;

            //Persistence: Load settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            {
                rdoAll.Checked = userData.ReadBool("EventLog.ShowAll", rdoAll.Checked);
                rdoHelp.Checked = !rdoAll.Checked;
                cbxIncludeHV1.Checked = userData.ReadBool("EventLog.cbxIncludeHV1", cbxIncludeHV1.Checked);
                EnableDisable();
            }
                

            //Init the log Obj after Loading settings incase setting checkboxes fires refilling
            eventlog = new EventLog("Application");

            //Check for new updates on a timer
            watchTimer = new Timer();
            watchTimer.Interval = 1000;  //msec
            watchTimer.Tick += new EventHandler(Timer_Tick);
            watchTimer.Start();

            new SuperTip(EventLogTip, "Application Event Log",
                "This page just echos the Windows Application Event Log.",
                "Latest entries are at the top. The page tab displays the",
                "number of entries that have arrived since startup.",
                "Blue or purple text = Help Viewer 1.x or 2.x events.");
            new SuperTip(ShowLogTip, "Show",
                "Show All Events - Shows entire Application Event Log",
                "Show HV2 Events - Scans the last 1000 events for HV2 events",
                "  (with the option to include Help Viewer 1.x events).");
        }

        public void Cleanup()  // Call on shutdown
        {
            watchTimer.Stop();
            eventlog.Close();
        }

        private void EnableDisable()
        {
            cbxIncludeHV1.Enabled = !rdoAll.Checked;
        }


        readonly String[] HV1Source = new String[] { "HELPLIBAGENT", "HELPLIBMANAGER", "HELPMANAGER", "HELP ZIP", "HELP CACHELIB", "HELP PROTOCOL", "HELP INDEX" };
        readonly String[] HV2Source = new String[] { "HLPCTNMGR", "HLPVIEWER", "HELP CACHELIB2", "HELP CATALOG MANAGER", "HELP INDEX2", "HELP ZIP2", "HELPRUNTIME" };
        const int CacheMaxSize = 1000;


        private EventLogEntry GetEventLogEntryFiltered(int i)   //get linkNode from sytem (with filtering is enabled)
        {
            if (i >= 0 || i < eventlog.Entries.Count)
            {
                EventLogEntry entry = eventlog.Entries[i];
                if (rdoAll.Checked)
                    return entry;
                else //help
                {
                    if (entry.Source.Length > 1 && entry.Source[0] == 'H' || entry.Source[0] == 'h')   //All help event start with 'H'
                    {
                        if (HV2Source.Contains(entry.Source.ToUpper()))
                            return entry;
                        else if (cbxIncludeHV1.Checked && HV1Source.Contains(entry.Source.ToUpper()))
                            return entry;
                    }
                }
            }
            return null; 
        }

        private void FillLog()   //Retrieves the Event Log Entries of the specified event log.
        {
            EventLogEntry entry;

            if (eventlog.Entries.Count > 0)
                _LastRecordID = eventlog.Entries[eventlog.Entries.Count - 1].Index;
            else
                _LastRecordID = -1;

            //
            // Append Action -- Find new items and append to current list
            //
            if (_cache.Count > 0 && _startupCount >= 0 && eventlog.Entries.Count > 0)        // _startupCount < 0 is a reset -- Don't append if a reset
            {
                int currentListTopId = _cache[0].Index;

                int count = 0;
                List<EventLogEntry> tempCache = new List<EventLogEntry>();
                for (int i = eventlog.Entries.Count - 1; i >= 0; i--)   //count from bottom (last linkNode will be our top linkNode)
                {
                    if (eventlog.Entries[i].Index == currentListTopId)  //done - we have caught up to the current cache content
                    {
                        if (tempCache.Count > 0)    //Found new items to add to top?
                        {
                            tempCache.AddRange(_cache);
                            _cache = tempCache;           //now cache has all new items on top

                            if (rdoHelp.Checked)
                                listView1.VirtualListSize = _cache.Count;  //show just filtered list
                            else
                                listView1.VirtualListSize = eventlog.Entries.Count;      //show all items, beyond this cache
                        }
                        return;
                    }
                    count++;
                    if (count > 100)   //Should only have to read a few items to get the latest events 
                        break;
                    entry = GetEventLogEntryFiltered(i);
                    if (entry != null)
                        tempCache.Add(entry);
                }
            }

            //Clear & Build new list
            listView1.VirtualListSize = 0;
            _cache.Clear();

            if (eventlog.Entries.Count > 0)
            {
                Cursor.Current = Cursors.WaitCursor;
                try
                {
                    int logCount = eventlog.Entries.Count;

                    //Fill cache -- Normally only interested in the last 50 items. User can still scroll to see the entire list but it's slower to access.
                    int cacheSize = CacheMaxSize;
                    if (cacheSize > logCount)
                        cacheSize = logCount;

                    for (int i = 0; i < cacheSize; i++)
                    {
                        entry = GetEventLogEntryFiltered(logCount - i - 1);
                        if (entry != null)
                            _cache.Add(entry);
                    }

                    if (rdoHelp.Checked)
                        listView1.VirtualListSize = _cache.Count;  //show just filtered list
                    else
                        listView1.VirtualListSize = eventlog.Entries.Count;      //show all items, beyond this cache
                }
                finally
                {
                    Cursor.Current = Cursors.Default;
                }
            }
        }

        //returns log zipEntry in reverse order (newest first) and from cache if possible
        private EventLogEntry GetEventLogEntryFromCache(int i)
        {
            int c = listView1.VirtualListSize;
            EventLogEntry entry = null;
            if (i >= 0)
            {
                if (i < _cache.Count)  //in cache
                    entry = _cache[i];
                else if (i < eventlog.Entries.Count)      //fetch from system array (working from last linkNode up)
                    entry = eventlog.Entries[c - i - 1];
            }
            return entry;
        }

        // Virtual ListView wants an linkNode

        private void listView1_RetrieveVirtualItem(object sender, RetrieveVirtualItemEventArgs e)
        {
            EventLogEntry entry = GetEventLogEntryFromCache(e.ItemIndex);

            if (entry != null)
            {
                ListViewItem li = new ListViewItem(GetErrorType(entry.EntryType));
                if (entry.EntryType == EventLogEntryType.Warning)
                    li.ImageIndex = 1;
                else if (entry.EntryType == EventLogEntryType.Error)
                    li.ImageIndex = 2;
                else if (entry.EntryType == EventLogEntryType.FailureAudit)
                    li.ImageIndex = 3;
                else if (entry.EntryType == EventLogEntryType.SuccessAudit)
                    li.ImageIndex = 3;
                else 
                    li.ImageIndex = 0; //info & default

                DateTime dt = entry.TimeWritten;
                String sDateTime;
                if (dt.Date == DateTime.Today)
                    sDateTime = dt.ToShortTimeString();
                else
                    sDateTime = dt.ToShortDateString() + " " + dt.ToShortTimeString();

                if (e.ItemIndex < _newItemsSinceStartup)    //mark latest with color
                {
                    li.BackColor = Color.Cornsilk;
                    sDateTime = sDateTime + " (" + (_newItemsSinceStartup - e.ItemIndex).ToString() + ")";
                }

                //color source items of the help event items
                String source = entry.Source.ToUpper();
                if (HV1Source.Contains(source))
                    li.ForeColor = Color.Blue;
                else if (HV2Source.Contains(source))
                    li.ForeColor = Color.DarkViolet;  

                String[] subItems = new String[] { sDateTime, entry.Source, entry.Index.ToString(), entry.Message };
                li.SubItems.AddRange(subItems);
                e.Item = li;
            }
            else  //unlikely
            {
                ListViewItem li = new ListViewItem("?");
                String[] subItems = new String[] { "-", "-", "-", "-" };
                li.SubItems.AddRange(subItems);
                li.ImageIndex = -1;
                e.Item = li;
            }
        }


        int _startupCount = -1;

        private void Timer_Tick(object sender, EventArgs e)
        {
            //List is cleared 
            if (eventlog.Entries.Count == 0)
            {
                if (listView1.VirtualListSize != 0)
                    ResetList();
                return;
            }

            //There are new items -- Resync!
            if (_LastRecordID != eventlog.Entries[eventlog.Entries.Count - 1].Index)  //last record ID has changed
            {
                FillLog();
                int listViewCount = listView1.VirtualListSize;  //we have a new count
                
                //startup or mode change = Initialize startup count
                if (_startupCount < 0)
                    _startupCount = listViewCount;

                //Log was cleared? Reduce start count
                if (_startupCount > listViewCount)
                    _startupCount = listViewCount;

                //Show new linkNode count (since startup) in page tab
                _newItemsSinceStartup = listViewCount - _startupCount;
                if (_hostTabPage != null)
                {
                    if (_newItemsSinceStartup > 0)
                        _hostTabPage.Text = _hostTabPageText + " (" + _newItemsSinceStartup.ToString() + ")";
                    else if (_hostTabPage.Text != _hostTabPageText)
                        _hostTabPage.Text = _hostTabPageText;
                }
            }
        }

        private void EventLogLink_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            String path = "eventvwr.msc"; 
            String cmdLine = "/Page:Application";   //This arg is not working.. How do you show the Apps log via cmd-line
            Process.Start(path, cmdLine);
        }

        private String GetErrorType(EventLogEntryType et)
        {
            String ret = "Information";   //Windows defaults to this
            if (et == EventLogEntryType.Error)
                ret = "Error";
            else if (et == EventLogEntryType.FailureAudit)
                ret = "FailureAudit";
            else if (et == EventLogEntryType.Information)
                ret = "Information";
            else if (et == EventLogEntryType.SuccessAudit)
                ret = "SuccessAudit";
            else if (et == EventLogEntryType.Warning)
                ret = "Warning";
            return ret;
        }

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {
            richTextBox1.Clear();
            if (listView1.SelectedIndices.Count <= 0) 
                return;
            int iSel = listView1.SelectedIndices[0];

            EventLogEntry entry = GetEventLogEntryFromCache(iSel);
            if (entry != null)
            {
                RichTextBoxFuncs rtf = new RichTextBoxFuncs(richTextBox1);
                rtf.WriteLine("Id:\t", entry.InstanceId.ToString());
                rtf.WriteLine("Index: ", entry.Index.ToString());
                rtf.WriteLine("EntryType: ", GetErrorType(entry.EntryType));

                rtf.WriteLine("TimeWritten: ", entry.TimeWritten.ToString());
                rtf.WriteLine("Source: ", entry.Source);
                //rtf.WriteLine("Category: ", zipEntry.Category);
                //rtf.WriteLine("CategoryNumber: ", zipEntry.CategoryNumber.ToString());
                rtf.WriteLine("MachineName: ", entry.MachineName);
                rtf.WriteLine("UserName: ", entry.UserName);
                rtf.WriteLine("Message: ", entry.Message);
            }
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_EVENTLOG);
        }

        //Radio or Check change

        private void ResetList()
        {
            if (eventlog != null)  //initialized
            {
                if (_hostTabPage != null && _hostTabPage.Text != _hostTabPageText)   //clear xxx (x)
                    _hostTabPage.Text = _hostTabPageText;
                _newItemsSinceStartup = 0;
                _startupCount = -1;
                FillLog();
                _startupCount = listView1.VirtualListSize;

                //Persistence: Save settings
                using (UserData userData = new UserData(Globals.DataRegKey))
                {
                    userData.WriteBool("EventLog.ShowAll", rdoAll.Checked);
                    userData.WriteBool("EventLog.cbxIncludeHV1", cbxIncludeHV1.Checked);
                }
                EnableDisable();
            }
        }

        private void rdoAll_CheckedChanged(object sender, EventArgs e)
        {
            if (rdoAll.Checked)
                ResetList();
        }

        private void rdoHelp_CheckedChanged(object sender, EventArgs e)
        {
            if (rdoHelp.Checked)
                ResetList();
        }

        private void cbxIncludeHV1_CheckedChanged(object sender, EventArgs e)
        {
            ResetList();
        }








    }


}
