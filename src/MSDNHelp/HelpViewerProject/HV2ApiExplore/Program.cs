using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using HV2ApiExplore.Forms;

namespace HV2ApiExplore
{
    static class Program
    {
        /// <summary>
        /// The main zipEntry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form_H2Main());
        }
    }
}
