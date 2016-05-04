using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HV2ApiExplore.Forms
{
    public partial class Page_CreateMSHX : UserControl
    {
        public static Page_CreateMSHX self = null;

        internal static Page_CreateMSHX Create()
        {
            self = new Page_CreateMSHX();
            return self;
        }


        public readonly Panel_HelpIndexer helpIndexer;

        public Page_CreateMSHX()
        {
            InitializeComponent();
            self = this;

            //fill the window with Help Indexer Panel
            helpIndexer = Panel_HelpIndexer.Create();
            helpIndexer.Parent = this.dockPanel;
            helpIndexer.Dock = DockStyle.Fill;
            helpIndexer.BringToFront();
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(-1);
        }
    }
}
