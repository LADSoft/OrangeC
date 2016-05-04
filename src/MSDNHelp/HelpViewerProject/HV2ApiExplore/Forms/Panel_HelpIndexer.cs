using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Reflection;
using System.Diagnostics;
using Helpware.Misc;

namespace HV2ApiExplore.Forms
{
    public partial class Panel_HelpIndexer : UserControl
    {
        public static Panel_HelpIndexer self = null;

        internal static Panel_HelpIndexer Create()
        {
            self = new Panel_HelpIndexer();
            return self;
        }


        public readonly String _helpIndexerPath;

        public Panel_HelpIndexer()
        {
            InitializeComponent();
            self = this;

            Assembly thisAssembly = System.Reflection.Assembly.GetExecutingAssembly();
            _helpIndexerPath = Path.Combine(Path.GetDirectoryName(thisAssembly.Location), "HelpIndexer\\helpIndexer.exe");

            new SuperTip(RunBtnTip, "HelpIndexer.exe",
                "HelpIndexer.exe currently ships with Windows 8 ADK (Assessment and Deployment Kit),",
                "You can download Win 8 ADK from MSDN Subscription Downloads. This page expects",
                "HelpIndexer.exe to be in the following location: ",
                _helpIndexerPath);

            //Persistence: Load settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            {
                InputFilename = userData.ReadStr("Mshx.InputFilename", "");
                OutputDir = userData.ReadStr("Mshx.OutputDir", "");
                OutputName = userData.ReadStr("Mshx.OutputName", "");
                VendorName = userData.ReadStr("Mshx.VendorName", "");
                CombineMode = userData.ReadBool("Mshx.CombineMode", true);
            }
            EnableDisable();
            UpdateCommandLine();
        }

        public void Cleanup()  //called at shutdown
        {
            //Persistence: Save settings
            using (UserData userData = new UserData(Globals.DataRegKey))
            {
                userData.WriteStr("Mshx.InputFilename", InputFilename);
                userData.WriteStr("Mshx.OutputDir", OutputDir);
                userData.WriteStr("Mshx.OutputName", OutputName);
                userData.WriteStr("Mshx.VendorName", VendorName);
                userData.WriteBool("Mshx.CombineMode", CombineMode);
            }
        }
        
        public String InputFilename
        {
            get
            {
                return textBoxInput.Text.Trim();
            }
            set
            {
                textBoxInput.Text = value.Trim();
            }
        }

        public String OutputDir
        {
            get
            {
                return textBoxOutput.Text.Trim();
            }
            set
            {
                textBoxOutput.Text = value.Trim();
            }
        }

        public Boolean CombineMode  //false for IndexMode
        {
            get
            {
                return rdoCombine.Checked;
            }
            set
            {
                rdoCombine.Checked = value;
                rdoIndex.Checked = !value;
                EnableDisable();
            }
        }

        private void EnableDisable()
        {
            textBoxVendorName.Enabled = rdoIndex.Checked;
            labelVendorName.Enabled = rdoIndex.Checked;
        }

        private String CorrectNameExt()
        {
            String name = textBoxOutputFilename.Text.Trim();
            if (!String.IsNullOrEmpty(name))
            {
                if (CombineMode)
                    name = Path.ChangeExtension(name, ".mshx");
                else
                    name = Path.ChangeExtension(name, ".mshi");
            }
            textBoxOutputFilename.Text = name;
            return name;
        }

        public String OutputName
        {
            get
            {
                return CorrectNameExt();
            }
            set
            {
                textBoxOutputFilename.Text = value;
                CorrectNameExt();
            }
        }

        public String VendorName
        {
            get
            {
                return textBoxVendorName.Text.Trim();
            }
            set
            {
                textBoxVendorName.Text = value.Trim();
            }
        }

        private String IfSpacesWrapInQuotes(String text)
        {
            if (text.IndexOfAny(new Char[] { ' ' }) >= 0)
                text = "\"" + text + "\"";
            return text;
        }

        public void UpdateCommandLine()
        {
            String cmdLine;
            if (CombineMode)
                cmdLine = "/t E";
            else
                cmdLine = "/t I";

            String inFile = IfSpacesWrapInQuotes(InputFilename);
            String outDir = IfSpacesWrapInQuotes(OutputDir);
            String outName = IfSpacesWrapInQuotes(OutputName);
            String vendor = IfSpacesWrapInQuotes(VendorName);

            cmdLine += " /i " + inFile;
            cmdLine += " /o " + outDir;
            if (outName != "")
                cmdLine += " /f " + outName;
            if (vendor != "")
                cmdLine += " /v " + vendor;
            richTextBox1.Text = cmdLine;
        }

        public Boolean CheckPathsValid()
        {
            String inFile = InputFilename;
            String outDir = OutputDir;
            String vendorName = VendorName;
            if (inFile == "" || !File.Exists(inFile))
                MessageBox.Show("Input file not found!");
            else if (outDir == "" || !Directory.Exists(outDir))
                MessageBox.Show("Output Directory not found!");
            else if (!CombineMode && vendorName == "")
                MessageBox.Show("Vendor name required when creating an .mshi index file!");
            else
                return true;
            return false;
        }

        private Boolean CheckOKtoOverwrite()  //Return TRUE if OK to continue
        {
            String inFile = InputFilename;
            String outFile = Path.Combine(OutputDir, Path.GetFileNameWithoutExtension(InputFilename));
            if (CombineMode)
                outFile += ".mshx";
            else
                outFile += ".mshi";
            Boolean fRes = true;
            if (File.Exists(outFile))
            {
                fRes = MessageBox.Show("OK to overwrite output file?\r\r" + outFile, "Overwrite File", MessageBoxButtons.YesNo) == DialogResult.Yes;
            }
            return fRes;
        }

        public void Run()
        {
            if (!CheckPathsValid())
                return;

            if (!File.Exists(_helpIndexerPath))
            {
                dosBox.Text = "File not found: " + _helpIndexerPath + Environment.NewLine
                     + "Note: HelpIndexer.exe can be downloaded with the Windows 8 ADK.";
                return;
            }
            UpdateCommandLine();
            String cmdLine = richTextBox1.Text;

            if (!CheckOKtoOverwrite())
                return;

            Cursor.Current = Cursors.WaitCursor;
            try
            {
                StreamWriter stdIn;
                Process p = new Process();
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.FileName = _helpIndexerPath;

                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.RedirectStandardInput = true;
                p.StartInfo.RedirectStandardError = true;
                //p.StartInfo.CreateNoWindow = true;

                p.StartInfo.Arguments = cmdLine;
                p.Start();
                stdIn = p.StandardInput;
                stdIn.WriteLine(_helpIndexerPath + " " + cmdLine);
                p.WaitForExit();
                dosBox.Text = p.StandardOutput.ReadToEnd() + "\r" + p.StandardError.ReadToEnd() + "\rDone!";
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }
        }

        public void ShowHelpIndexerHelp()
        {
            if (!File.Exists(_helpIndexerPath))
            {
                MessageBox.Show("File not found: " + _helpIndexerPath);
                return;
            }
            Cursor.Current = Cursors.WaitCursor;
            try
            {
                Process p = new Process();
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.FileName = _helpIndexerPath;
                p.StartInfo.Arguments = "/?";
                p.Start();
                dosBox.Text = p.StandardOutput.ReadToEnd();
                p.WaitForExit();
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }
        }

        private void HelpIndexerHelpBtn_Click(object sender, EventArgs e)
        {
            ShowHelpIndexerHelp();
        }


        private void RunBtn_Click(object sender, EventArgs e)
        {
            Run();
        }

        private void buttonInput_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            openFileDialog1.DefaultExt = ".mshc";
            openFileDialog1.Filter = "MS Help Container (*.mshc)|*.mshc";
            openFileDialog1.FilterIndex = 0;
            if (File.Exists(InputFilename))
                openFileDialog1.InitialDirectory = Path.GetDirectoryName(InputFilename);
            openFileDialog1.CheckFileExists = true;
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBoxInput.Text = openFileDialog1.FileName;
            }
        }

        String lastOutputDir = "";

        private void buttonOutput_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowser = new FolderBrowserDialog();
            folderBrowser.Description = "Select the folder to create the output file.";
            if (lastOutputDir != "" && Directory.Exists(lastOutputDir))
                folderBrowser.SelectedPath = lastOutputDir;
            else if (File.Exists(InputFilename))
                folderBrowser.SelectedPath = Path.GetDirectoryName(InputFilename);
            if (folderBrowser.ShowDialog() == DialogResult.OK)
            {
                lastOutputDir = folderBrowser.SelectedPath;
                textBoxOutput.Text = lastOutputDir;
            }
        }


        // Text Changes reflect in Command line


        private void rdoCombine_CheckedChanged(object sender, EventArgs e)
        {
            if (rdoCombine.Checked)
                UpdateCommandLine();
            EnableDisable();
        }

        private void rdoIndex_CheckedChanged(object sender, EventArgs e)
        {
            if (rdoIndex.Checked)
                UpdateCommandLine();
            EnableDisable();
        }

        private void textBoxInput_TextChanged(object sender, EventArgs e)
        {
            UpdateCommandLine();
        }

        private void textBoxOutput_TextChanged(object sender, EventArgs e)
        {
            UpdateCommandLine();
        }

        private void textBoxOutputFilename_TextChanged(object sender, EventArgs e)
        {
            UpdateCommandLine();
        }

        private void textBoxVendorName_TextChanged(object sender, EventArgs e)
        {
            UpdateCommandLine();
        }



    }
}
