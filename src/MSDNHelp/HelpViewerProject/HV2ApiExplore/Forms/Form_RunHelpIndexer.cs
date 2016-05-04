using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Helpware.Misc;

namespace HV2ApiExplore.Forms
{
    public partial class Form_RunHelpIndexer : Form
    {
        public static Form_RunHelpIndexer self = null;

        internal static Form_RunHelpIndexer Create()
        {
            self = new Form_RunHelpIndexer();
            return self;
        }


        public readonly Panel_HelpIndexer helpIndexer;

        public Form_RunHelpIndexer()
        {
            InitializeComponent();

            //fill the window with Help Indexer Panel
            helpIndexer = Panel_HelpIndexer.Create();
            helpIndexer.Parent = this;
            helpIndexer.Dock = DockStyle.Fill;
            helpIndexer.BringToFront();
        }

        private void Form_RunHelpIndexer_Shown(object sender, EventArgs e)
        {
            //Restore form size/pos
            using (UserData userData = new UserData(Globals.DataRegKey))
            { userData.RestoreFormPos(this); }
        }

        private void Form_RunHelpIndexer_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!e.Cancel)
            {
                //Save form size/pos
                using (UserData userData = new UserData(Globals.DataRegKey))
                   { userData.SaveFormPos(this); }
            }
        }

        private void Form_RunHelpIndexer_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
                this.Close();
        }


    }
}
