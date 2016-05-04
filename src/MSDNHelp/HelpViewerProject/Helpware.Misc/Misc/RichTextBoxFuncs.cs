using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;
using System.Text;

namespace Helpware.Misc
{
    /// <summary>Class to output formatted text to a RichTextBox.</summary>
    /// <example>
    /// RichTextBoxFuncs rt = new RichTextBoxFuncs(richTextBox1);
    /// rt.WriteLine("blue text",  Color.Blue);
    /// </example>
    public class RichTextBoxFuncs
    {
        readonly private System.Windows.Forms.RichTextBox mRichTextBox = null;
        private Color mTextColor;
        private Color mAltColor;

        //Constructor
        public RichTextBoxFuncs(RichTextBox aRichTextBox)
        {
            mRichTextBox = aRichTextBox;
            mTextColor = mRichTextBox.ForeColor;
            mAltColor = Color.BlueViolet;
        }

        public Color TextColor
        {
            get { return mTextColor; }
            set { mTextColor = value; }
        }

        public Color AltColor
        {
            get { return mAltColor; }
            set { mAltColor = value; }
        }

        // Write() WriteLine()

        public void Write(String text)
        {
            Write(text, mTextColor);
        }

        public void WriteLine(String text)
        {
            WriteLine(text, mTextColor);
        }

        public void Write(String text, Color color, int indent, Font font) 
        {
            if (text == null)
                text = "";
            int xLen = mRichTextBox.TextLength;
            mRichTextBox.AppendText(text);
            mRichTextBox.Select(xLen, text.Length);
            mRichTextBox.SelectionColor = color;
            mRichTextBox.SelectionIndent = indent;
            if (font != null)
                mRichTextBox.SelectionFont = font; 
            //Restore Selection and color for next
            mRichTextBox.SelectionStart = mRichTextBox.TextLength;
            mRichTextBox.SelectionColor = this.mTextColor;
        }

        public void Write(String text, Color color, int indent)
        {
            this.Write(text, color, indent, null);
        }

        public void Write(String text, Color color, Font font)
        {
            this.Write(text, color, 0, font);
        }

        public void Write(String text, Color color)
        {
            this.Write(text, color, 0, null);
        }


        public void WriteLine(String text, Color color, int indent)
        {
            this.Write(text, color, indent, null);
            mRichTextBox.AppendText(Environment.NewLine);
        }

        public void WriteLine(String text, Color color, Font font)
        {
            this.Write(text, color, 0, font);
            mRichTextBox.AppendText(Environment.NewLine);
        }

        public void WriteLine(String text, Color color)
        {
            this.Write(text, color);
            mRichTextBox.AppendText(Environment.NewLine);
        }



        public void WriteLine(String[] atext, Color color, int indent)
        {
            foreach (String text in atext)
            {
                this.Write(text, color, indent, null);
                mRichTextBox.AppendText(Environment.NewLine);
            }
        }

        public void WriteLine(String[] atext, Color color, Font font)
        {
            foreach (String text in atext)
            {
                this.Write(text, color, 0, font);
                mRichTextBox.AppendText(Environment.NewLine);
            }
        }

        public void WriteLine(String[] atext, Color color)
        {
            foreach (String text in atext)
            {
                this.Write(text, color);
                mRichTextBox.AppendText(Environment.NewLine);
            }
        }



        public void Write(String text1, String text2)
        {
            Write(text1, text2, 30);
        }

        public void WriteLine(String text1, String text2)
        {
            WriteLine(text1, text2, 30);
        }


        public void Write(String text1, String text2, int prePadChars)
        {
            text1 = text1.PadRight(prePadChars);
            Write(text1, mAltColor);           //text1 is colored
            mRichTextBox.AppendText("\t");     // tab
            if (!String.IsNullOrEmpty(text2))
                Write(text2, Color.Black);
        }

        public void WriteLine(String text1, String text2, int prePadChars)
        {
            Write(text1, text2, prePadChars);
            mRichTextBox.AppendText(Environment.NewLine);
        }

    }
}
