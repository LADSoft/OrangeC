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
    public partial class DebugForm : Form
    {
        public static DebugForm self = null;
        public static RichTextBoxFuncs rtf;       //rtf text formatter
        private static Boolean mOkToClose = false;

        public static void Create()
        {
            if (self == null)
            {
                self = new DebugForm();
            }
        }

        //constructor
        public DebugForm()
        {
            InitializeComponent();
            rtf = new RichTextBoxFuncs(richTextBox1);
        }

        public static Boolean OkToClose
        {
            get { return mOkToClose; }
            set { mOkToClose = value; }
        }

        private void DebugForm_Shown(object sender, EventArgs e)
        {
            using (UserData userData = new UserData(Globals.DataRegKey))
            { userData.RestoreFormPos(this); }
        }

        private void DebugForm_FormClosing(object sender, FormClosingEventArgs e)  
        {
            using (UserData userData = new UserData(Globals.DataRegKey))
            { userData.SaveFormPos(this); }

            this.Hide();
            e.Cancel = !OkToClose;
        }

        public static void Clear()
        {
            if (self != null)
                self.richTextBox1.Clear();
        }

        private void DebugForm_Resize(object sender, EventArgs e)  
        {
            richTextBox1.Width = this.ClientSize.Width;    //Shouldn't need this on a dock=filled component. A VS 11 preview bug?
            richTextBox1.Height = this.ClientSize.Height;
        }

        private void DebugForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (!e.Control && !e.Alt && !e.Shift && e.KeyCode == Keys.Escape)
            {
                this.Close();
            }
        }

    }
}
