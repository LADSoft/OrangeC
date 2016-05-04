namespace VisualStudioHelpDownloader2012
{
	using System;
	using System.Diagnostics;
	using System.Globalization;
	using System.Windows.Forms;

	/// <summary>
	/// The program.
	/// </summary>
	internal static class Program
	{
		/// <summary>
		/// Log exception to event log.
		/// </summary>
		/// <param name="ex">
		/// The exeception to log
		/// </param>
		public static void LogException( Exception ex )
		{
			if ( null != ex )
			{
				EventLog.WriteEntry(
					"Application", 
					string.Format( CultureInfo.InvariantCulture, "{1}{0}{0}{2}", Environment.NewLine, ex.Message, ex.StackTrace ), 
					EventLogEntryType.Error );
			}
		}

		/// <summary>
		/// The main.
		/// </summary>
		[STAThread]
		private static void Main(string [] args)
		{
			AppDomain.CurrentDomain.UnhandledException += ( sender, e ) => LogException( e.ExceptionObject as Exception );
			Application.ThreadException += ( sender, e ) => LogException( e.Exception );
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault( false );
			Application.Run( new MainForm(args) );
		}
	}
}
