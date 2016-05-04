using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Helpware.Misc
{
    /// <summary>Classes encapsulates the ToolTip control and sets the hover time to the maximum allowable time of 32.77 seconds.</summary>
    /// <example>new SuperTip(control, TitleText, textLine1, textLine2, textLine3, ...)</example>
    public class SuperTip
    {
        public ToolTip toolTip;
        public Control control;
        private String title;
        private String text;

        //constructor 

        public SuperTip(Control control, ToolTipIcon icon, String title, String text)
        {
            this.control = control;
            this.toolTip = new ToolTip();
            this.title = title;
            this.text = text;

            toolTip.ToolTipIcon = icon;
            toolTip.IsBalloon = false;
            toolTip.ShowAlways = true;

            if (control is PictureBox)
                control.Cursor = Cursors.Help;

            toolTip.InitialDelay = 50;
            toolTip.AutoPopDelay = Int16.MaxValue;   // passed as a 16bit param in native code means that 32767 is the max delay
            toolTip.ReshowDelay = 10;

            toolTip.ToolTipTitle = title;
            toolTip.SetToolTip(this.control, text);
        }

        public SuperTip(Control control, String title, String text)
            : this(control, ToolTipIcon.None, title, text) { }

        public SuperTip(Control control, String title, params String[] lines)
            : this(control, ToolTipIcon.None, title, String.Join("\n", lines)) { }

        public SuperTip(Control control, ToolTipIcon icon, String title, params String[] lines)
            : this(control, icon, title, String.Join("\n", lines)) { }

        
        public void Add(Control xtraControl)  
        {
            toolTip.SetToolTip(xtraControl, this.text);
            if (xtraControl is PictureBox)
                xtraControl.Cursor = Cursors.Help;
        }



    }
}
