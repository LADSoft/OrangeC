using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Reflection;

namespace HV2Viewer
{

    static class Program
    {
        private static Form1 mainWindow;
        private static void OnApplicationExit(object sender, EventArgs e)
        {
                PipeServer.Stop();
        }
        private static void PipeServerCallback(string token)
        {
                mainWindow.DoNavigateByF1Keyword(token);
                mainWindow.BringToFront();
        }
		private static string GetMSHelpDirectory()
		{
			string pf = System.Environment.GetFolderPath(System.Environment.SpecialFolder.ProgramFiles);
			string pf1 = pf + @"\Microsoft Help Viewer\v2.0";
			if (Directory.Exists(pf1))
				return pf1;

			pf1 = pf + @"\Microsoft Help Viewer\v2.1";
			if (Directory.Exists(pf1))
				return pf1;
			pf1 = pf + @"\Microsoft Help Viewer\v2.2";
			if (Directory.Exists(pf1))
				return pf1;
			return "";
		}
		/// when not on win8 or higher, we need to utilize a VS dll that gets installed with help viewer 2.0
		/// this accesses it from its native location
		private static Assembly ResolveAssembly(object sender, ResolveEventArgs args)
		{
			//This handler is called only when the common language runtime tries to bind to the assembly and fails.

			Assembly executingAssembly = Assembly.GetExecutingAssembly();

			string applicationDirectory = GetMSHelpDirectory();

			if (applicationDirectory == "")
			{
				MessageBox.Show("Please install VS 2012 or higher");
				return null;
			}
			string[] fields = args.Name.Split(',');
			string assemblyName = fields[0];
			string assemblyCulture;
			if (fields.Length < 2)
				assemblyCulture = null;
			else
				assemblyCulture = fields[2].Substring(fields[2].IndexOf('=') + 1);


			string assemblyFileName = assemblyName + ".dll";
			string assemblyPath;

			if (assemblyName.EndsWith(".resources"))
			{
				// Specific resources are located in app subdirectories
				string resourceDirectory = Path.Combine(applicationDirectory, assemblyCulture);

				assemblyPath = Path.Combine(resourceDirectory, assemblyFileName);
			}
			else
			{
				assemblyPath = Path.Combine(applicationDirectory, assemblyFileName);
			}



			if (File.Exists(assemblyPath))
			{
				//Load the assembly from the specified path.                    
				Assembly loadingAssembly = Assembly.LoadFrom(assemblyPath);

				//Return the loaded assembly.
				return loadingAssembly;
			}
			else
			{
				return null;
			}

		}
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
		
        [STAThread]
        static void Main(string[] args)
        {
			bool ok;
			System.Threading.Mutex m = new System.Threading.Mutex(true, "__MSDN_HV2VIEWER_LADSOFT__", out ok);

			if (! ok)
			{
				if (args.Length == 1)
					PipeClient.Write(args[0]);
			}
			else
			{
                AppDomain currentDomain = AppDomain.CurrentDomain;
                currentDomain.AssemblyResolve += new ResolveEventHandler(ResolveAssembly);

				Application.ApplicationExit += OnApplicationExit;

				PipeServer.Start(PipeServerCallback);
				Application.EnableVisualStyles();
				Application.SetCompatibleTextRenderingDefault(false);
				mainWindow = new Form1(args);
				Application.Run(mainWindow);
				GC.KeepAlive(m);                // important!
			}
        }
    }
}
