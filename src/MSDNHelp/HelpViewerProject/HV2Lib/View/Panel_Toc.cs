namespace HVProject.HV2Lib.View
{
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
    using HVProject.HV2Lib.Common;

    
    /// <summary>
    /// A full catalog TOC in a userControl - Parent it whereever you want a TOC, or use a dropdown combo to make it appear as a dropdown.
    /// </summary>

    public partial class Panel_Toc : UserControl
    {
        public static Catalog _catalog = null;
        private CatalogRead _catalogRead = new CatalogRead();

        private ComboBox _cbo = null;
        private Timer oneshotTimer = null;
        private String _lastContentPath = null;
        private TocSync tocSync;

        public delegate void SelectChangeDelegate(string message);
        public SelectChangeDelegate selectChangeHandler = null;

        public Panel_Toc(Control parentControl, ComboBox comboBox, Catalog helpCatalog)
        {
            InitializeComponent();

            this._cbo = comboBox;
            this.HelpCatalog = helpCatalog;   //this will build the TOC

            // our oneshot timer
            oneshotTimer = new Timer();
            oneshotTimer.Interval = 10;
            oneshotTimer.Tick += new EventHandler(oneshotTimer_Tick);

            //Position under the ComboBox if one exists
            this.Parent = parentControl;
            if (_cbo != null)
            {
                // parent, size under the Cbo & Hide ourself
                Point cboPos = CalcCboPos();
                this.Left = cboPos.X;
                this.Width = _cbo.Width;
                this.Top = cboPos.Y + _cbo.Height;
                this.Height = _cbo.Width;
                this.Hide();
                _cbo.DropDownHeight = 1;  // We are replacing the current dropdown so make the cbo dropdown as small as posible
            }
            this.BringToFront();

            //Update style
            if (Native.SetWindowTheme(this.treeView1.Handle, "explorer"))
                treeView1.ShowLines = false;

            tocSync = new TocSync(treeView1);
        }

        public Panel_Toc(Control parentControl, ComboBox comboBox): this(parentControl, comboBox, null) { }
        public Panel_Toc(Control parentControl) : this(parentControl, null, null) { }


        private Point CalcCboPos()
        {
            Point point = new Point(_cbo.Left, _cbo.Top);
            Control c = _cbo;
            while (c.Parent != null)
            {
                c = c.Parent;
                point.X = point.X + c.Left;
                point.Y = point.Y + c.Top;
            }
            return point;
        }



        public Panel_Toc(Control parentControl, Catalog helpCatalog): this(parentControl, null, helpCatalog) { }


        public void AddSelectCallback(SelectChangeDelegate callback)
        {
            selectChangeHandler = callback;
        }


        public Catalog HelpCatalog
        {
            get { return _catalog; }
            set
            {
               _catalog = value;
               InitToc();
            }
        }

        public void Clear()
        {
            this.HelpCatalog = null;
        }


        
        const String VIRTUAL_NODE = "+";

        private void InitToc()
        {
            if (_catalog == null || !_catalog.IsOpen)
            {
                treeView1.Nodes.Clear();
                _lastContentPath = null;
                return;
            }

            // No need to update if nothing has changed
            if (_catalog.ContentPath == _lastContentPath)
                return;
            _lastContentPath = _catalog.ContentPath;

            treeView1.Nodes.Clear();
            try
            {
                //Is there a -1 super toc node? Added it (RWC at time of writing not sure if we can show the -1 linkNode)

                TreeNode tnHome = null;
                TreeNodeCollection tnCollection = null;

                ITopic topic = _catalogRead.GetIndexedTopicDetails(_catalog, "-1", null);
                if (topic != null)
                {
                    String sTitle = topic.Title;       // -1 linkNode can be an empty title
                    if (String.IsNullOrEmpty(sTitle))
                        sTitle = "Help Viewer Home";

                    tnHome = new TreeNode(sTitle);
                    tnHome.Tag = topic;   //Keep tabs on the Topic obj 
                    treeView1.Nodes.Add(tnHome);
                }

                //Add next level list
                ITopicCollection topics = _catalogRead.GetTableOfContents(_catalog, "-1", null, TocReturnDetail.TocRootNodes);
                if (topics.Count > 0)
                {
                    if (tnHome == null)
                        tnCollection = treeView1.Nodes;
                    else
                        tnCollection = tnHome.Nodes;

                    treeView1.BeginUpdate();
                    for (int i = 0; i < topics.Count; i++)
                    {
                        topics.MoveTo(i);
                        //Topic 
                        topic = (Topic)topics.Current;

                        TreeNode tn = new TreeNode(topic.Title);
                        tn.Tag = topic;   //Keep tabs on the Topic obj 
                        tnCollection.Add(tn);
                    }
                    treeView1.EndUpdate();

                    if (tnHome != null)
                        tnHome.Expand();

                    //Add one more level of list
                    foreach (TreeNode node in tnCollection)
                    {
                        AddChildNodes(node);
                    }
                }

                //nice to expand level 0 list if only one node
                if (treeView1.Nodes.Count == 1)  
                    treeView1.Nodes[0].Expand();

                //Hook up events
                treeView1.BeforeExpand += new TreeViewCancelEventHandler(this.treeView1_BeforeExpand);
                treeView1.BeforeCollapse += new TreeViewCancelEventHandler(this.treeView1_BeforeCollapse);
            }
            catch
            {
                MessageBox.Show("Unknown exception building TOC");
            }
        }

        public void SelectFirstNode() 
        {
            if (treeView1.Nodes.Count > 0)
            {
                treeView1.SelectedNode = treeView1.Nodes[0];
            }
        }

        private void AddChildNodes(TreeNode treeNode)
        {
            Topic topic = treeNode.Tag as Topic;
            if (topic == null) //unlikely
                return;

            treeNode.Nodes.Clear();  //wipe out the fake node we added

            Cursor.Current = Cursors.WaitCursor;
            treeView1.BeginUpdate();
            try
            {
                //Get Child list
                ITopicCollection topics = _catalogRead.GetTableOfContents(_catalog, topic.Id, null, TocReturnDetail.TocChildren);
                for (int i = 0; i < topics.Count; i++)
                {
                    topics.MoveTo(i);
                    Topic childTopic = (Topic)topics.Current;
                    TreeNode tn = new TreeNode(childTopic.Title);
                    tn.Tag = childTopic;
                    treeNode.Nodes.Add(tn);
                }

                //Add virtual node.. add temporary child so the +\- button show. Add junk text for now and add correct text when the node expands
                foreach (TreeNode node in treeNode.Nodes)
                {
                    topic = node.Tag as Topic;
                    if (topic != null && topic.TableOfContentsHasChildren)
                        node.Nodes.Add(VIRTUAL_NODE);
                }
            }
            finally
            {
                treeView1.EndUpdate();
                Cursor.Current = Cursors.Default;
            }
        }


        //Avoid expand contract on a double click (if we are Cbo dropdown)

        Boolean _clickedOnLabel = false;

        private void treeView1_MouseDown(object sender, MouseEventArgs e)
        {
            TreeNode Node = treeView1.GetNodeAt(e.Location);
            _clickedOnLabel = (Node != null) && Node.Bounds.Contains(e.Location);
        }

        private void treeView1_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            e.Cancel = _cbo != null && _clickedOnLabel;  //avoid double clicks for CBO dropdown panels
            if (!e.Cancel && e.Node.Nodes.Count == 1 && e.Node.Nodes[0].Text == VIRTUAL_NODE)
            {
                AddChildNodes(e.Node);
            }
        }

        private void treeView1_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            e.Cancel = _cbo != null && _clickedOnLabel;  //avoid double clicks for CBO dropdown panels
        }

        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            _clickedOnLabel = false; //reset flag
        }


        //Select a Node - Return the value to the dropdown control

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            Topic topic = e.Node.Tag as Topic;
            if (_cbo != null)
                _cbo.Text = topic.Id;
            if (selectChangeHandler != null)
                selectChangeHandler(topic.Id);   // ping main form
        }


        #region Popup Code (when used with ComboBox)

        // On shot timer sets focus to ourself when going visible

        private void TocPanel_VisibleChanged(object sender, EventArgs e)
        {
            //kick off the oneshot-timer
            oneshotTimer.Enabled = true;
        }

        private void oneshotTimer_Tick(object sender, EventArgs e)
        {
            oneshotTimer.Enabled = false;  //done with timer
            if (_cbo != null)  //working with a ComboBox
            {
                if (this.Visible)   //We were just shown... Give ourselves thefocus
                {
                    if (!this.Focused)
                    {
                        this.Focus();
                    }
                }
                else  //We were just hidden ... make sure the drop down button is disengaged & now has focus
                {
                    if (_cbo != null)
                    {
                        _cbo.DroppedDown = false;
                        _cbo.Focus();
                    }
                }
            }
        }

        public Boolean SmartShow()  //Don't show if in limbo (we need some settling time) 
        {
            Boolean fOkToShow = !oneshotTimer.Enabled && !this.Visible;
            if (fOkToShow)
                this.Show();
            return fOkToShow;
        }

        #endregion


        // We close ourself on lose focus / double click / Enter or Escape

        private void TocPanel_Leave(object sender, EventArgs e)
        {
            if (_cbo != null)  //working with a ComboBox
                Hide();
        }

        private void treeView1_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (_cbo != null)
                Hide();
        }

        private void treeView1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return || e.KeyCode == Keys.Escape)
                if (_cbo != null)
                {
                    Hide();
                    return;
                }
            if (e.KeyCode == Keys.Multiply)  //override the [*] button -- It could fetch 10000's of items as it expands everything
            {
                e.SuppressKeyPress = true;
                return;
            }
            //override Alt+Left / Alt+Right --  We are going to use these for Back/Foward shortcuts on the main form - suppress normal expand actions
            if ((e.KeyData == (Keys.Alt | Keys.Left)) || (e.KeyData == (Keys.Alt | Keys.Right)))  
            {
                e.SuppressKeyPress = true;
                return;
            }
        }

        public void DoFocus()
        {
            if (treeView1.CanFocus && !treeView1.Focused) 
                treeView1.Focus(); 
        }


        // --------------------------------------------------------------------------------------------
        // TOC Sync
        // --------------------------------------------------------------------------------------------

        public void Sync(String topicID)
        {
            TreeNode node;
            TocSync.SyncReturn syncRet = tocSync.Sync(_catalog, topicID, out node);

            IfStart:

            if (syncRet == TocSync.SyncReturn.ExactMatch && node != null)  // found the node matching the topic id
            {
                node.EnsureVisible();
                treeView1.SelectedNode = node;
                return;
            }
            else if (syncRet == TocSync.SyncReturn.ParialMatch && node != null)   // found an ancestor node for the topic id
            {
                // add child nodes until we find the topic ID

                AddChildNodes(node);
                while (true)
                {
                    TreeNode oldNode = node;
                    syncRet = tocSync.Sync(_catalog, topicID, out node);
                    if (syncRet == TocSync.SyncReturn.ExactMatch)
                        goto IfStart;
                    else if (syncRet == TocSync.SyncReturn.ParialMatch && node != oldNode)
                    {
                        node.EnsureVisible();
                        treeView1.SelectedNode = node;
                        goto IfStart;
                    }
                }
            }
        }



    }



}
