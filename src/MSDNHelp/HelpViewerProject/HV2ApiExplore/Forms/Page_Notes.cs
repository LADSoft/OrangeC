using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Helpware.Misc;

namespace HV2ApiExplore.Forms
{
    public partial class Page_Notes : UserControl
    {
        public static Page_Notes self = null;

        internal static Page_Notes Create()
        {
            self = new Page_Notes();
            return self;
        }

        public Page_Notes()
        {
            InitializeComponent();
            self = this;
            ShowNotes();
        }

        private void helpLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Globals.ShowAppHelp(Globals.ID_PAGE_NOTES);
        }

        private void ShowNotes()
        {
            richTextBox1.Clear();
            RichTextBoxFuncs rtf = new RichTextBoxFuncs(richTextBox1);

            Color headerColor = Color.Chocolate;
            Color textColor = Color.DarkSlateGray;
            Font boldFont = new Font(this.Font.FontFamily, 12, FontStyle.Regular);
            int textIndent = 16;

            rtf.WriteLine("ICatalog & ICatalogRead", headerColor, boldFont);
            rtf.Write("Once you import the Windows.Help.Runtime.DLL COM interface, Open & Close a Help Catalog using ", textColor);
            rtf.Write("ICatalog", headerColor);
            rtf.Write(", then use ", textColor);
            rtf.Write("ICatalogRead", headerColor);
            rtf.WriteLine(" to access catalog data (Search/Index/TOC/etc).", textColor);
            rtf.WriteLine("Most other API classes (see below) are used by these two interfaces.", textColor);
            
            rtf.Write("MemoryIStream", headerColor);
            rtf.Write(", ", textColor);
            rtf.Write("FileIStream", headerColor);
            rtf.Write(" and ", textColor);
            rtf.Write("HelpReader", headerColor);
            rtf.WriteLine(" are used internally and exposed via COM. Most developers won't use these.", textColor);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.IHelpFilter", headerColor, boldFont);
            rtf.Write("This is a Dictionary<string, string> list of key/value pairs used by the various ", textColor);
            rtf.Write("ICatalogRead", headerColor);
            rtf.WriteLine(" calls to get filtered results.", textColor, textIndent);
            rtf.WriteLine("The Key is not case sensitive. A key/value pair may look something like this: \"topiclocale\",\"en-US\".", textColor, textIndent);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.IHelpKeyValuePair", headerColor, boldFont);
            rtf.Write("Represents a single key/value items of a ", textColor);
            rtf.Write("IHelpFilter", headerColor);
            rtf.WriteLine(" list.", textColor, textIndent);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.ICatalogReadWriteLock", headerColor, boldFont);
            rtf.Write("Object returned by calling ", textColor);
            rtf.Write("ICatalog.GetReadWriteLock(CatalogLocation)", headerColor);
            rtf.WriteLine(".", textColor, textIndent);
            rtf.WriteLine("Use the object to get or set a catalogs read/write locks.", textColor, textIndent);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.IKeywordCollection", headerColor, boldFont);
            rtf.Write("Used to return topicID data by ", textColor);
            rtf.Write("ICatalogRead.GetKeywords()", headerColor);
            rtf.WriteLine(" calls.", textColor, textIndent);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.IKeyword", headerColor, boldFont);
            rtf.Write("Access keywords in ", textColor);
            rtf.Write("IKeywordCollection", headerColor);
            rtf.WriteLine(".", textColor, textIndent);
            rtf.WriteLine("");
            
            rtf.WriteLine("Help.Runtime.ITopicCollection", headerColor, boldFont);
            rtf.Write("Used to return topic results by ", textColor);
            rtf.Write("ICatalogRead::GetTableOfContents()", headerColor);
            rtf.Write(" & ", textColor, textIndent);
            rtf.Write("ICatalogRead::GetSearchResults()", headerColor);
            rtf.WriteLine(".", textColor, textIndent);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.ITopic", headerColor, boldFont);
            rtf.Write("Access topic info in ", textColor);
            rtf.Write("ITopicCollection", headerColor);
            rtf.WriteLine(".", textColor, textIndent);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.SearchOptions", headerColor, boldFont);
            rtf.Write("This is an enumeration used in ", textColor);
            rtf.Write("ICatalogRead.GetSearchResults()", headerColor);
            rtf.WriteLine(".", textColor, textIndent);
            rtf.WriteLine("none=0; -- Default processing.", textColor, textIndent);
            rtf.WriteLine("SearchTermHighlight=1; -- Apply highlighting to search hits.", textColor, textIndent);
            rtf.WriteLine("OrSearchOverride=2; -- \"OR\" search terms instead of \"AND\" (default).", textColor, textIndent);
            rtf.WriteLine("");

            rtf.WriteLine("Help.Runtime.TocReturnDetail", headerColor, boldFont);
            rtf.Write("This is an enumeration used in calls to ", textColor);
            rtf.Write("ICatalogRead.GetTableOfContents()", headerColor);
            rtf.WriteLine(".", textColor, textIndent);
            rtf.WriteLine("TocChildren=0; -- Get only children.", textColor, textIndent);
            rtf.WriteLine("TocSiblings=1; -- Get only siblings.", textColor, textIndent);
            rtf.WriteLine("TocAncestors=2; -- Get only ancestors.", textColor, textIndent);
            rtf.WriteLine("TocRootNodes=3; -- Get only root list.", textColor, textIndent);
            rtf.WriteLine("TocDescendants=4; -- Get only descendants.", textColor, textIndent);
            rtf.WriteLine("");

            

            
        }



    }
}
