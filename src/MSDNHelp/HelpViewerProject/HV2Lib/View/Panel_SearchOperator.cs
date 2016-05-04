using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace HVProject.HV2Lib.View
{
    public partial class Panel_SearchOperator : UserControl
    {
        private ComboBox _cbo = null;
        private Timer oneshotTimer = null;

        public Panel_SearchOperator(Control parentControl, ComboBox comboBox)
        {
            InitializeComponent();

            _cbo = comboBox;

            // our oneshot timer
            oneshotTimer = new Timer();
            oneshotTimer.Interval = 10;
            oneshotTimer.Tick += new EventHandler(oneshotTimer_Tick);

            //parent, size, hide ourself
            Point cboPos = CalcCboPos();
            this.Parent = parentControl;
            this.Left = cboPos.X;
            this.Top = cboPos.Y + _cbo.Height;
            this.Hide();
            this.BringToFront();
        }

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


        #region Popup Code

        // On shot timer sets focus to ourself when going visible

        private void SearchOperatorPanel_VisibleChanged(object sender, EventArgs e)
        {
            //kick off the oneshot-timer
            oneshotTimer.Enabled = true;
        }

        private void oneshotTimer_Tick(object sender, EventArgs e)
        {
            oneshotTimer.Enabled = false;  //done with timer
            if (this.Visible)   //We were just shown... Give ourselves thefocus
            {
                if (!this.Focused)
                {
                    this.Focus();
                }
            }
            else  //We were just hidden ... make sure the drop down button is disengaged
            {
                if (_cbo != null)
                    _cbo.DroppedDown = false;
            }
        }

        public void SmartShow()  //Don't show if in limbo (we need some settling time) 
        {
            if (!oneshotTimer.Enabled && !this.Visible)
                this.Show();
        }

        // We close ourself when we lose focus

        private void SearchOperatorPanel_Leave(object sender, EventArgs e)
        {
            Hide();
        }

        #endregion


        // Click a link

        private void linkTitle_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            if (_cbo != null)
            {
                LinkLabel link = sender as LinkLabel;
                String text = _cbo.Text;
                String[] ss = text.Trim().Split(new Char[] { ':' });   //We want everything after the last ':'
                if (ss.Length > 0)
                    text = link.Text + ss[ss.Length - 1];
                else
                    text = link.Text;
                _cbo.Text = text;

                this.Hide();
                _cbo.Focus();
            }
        }

    }
}
