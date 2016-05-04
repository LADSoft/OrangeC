using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;

namespace Helpware.Misc
{

    /// <summary>
    /// Class to read/write persistent settings from/to the registry HK_CURRENTUSER area.
    /// </summary>
    /// <example>
    /// // The using() statement takes care of closing the registry location.
    /// using (UserData userData = new UserData(@"software\helpware\AppName\userdata\")
    /// {
    ///     String fishType = userData.ReadStr("LastFishType", "SwordFish");
    ///     RestoreFormPos(this);   
    /// }
    /// </example>

    public class UserData : IDisposable
    {
        private RegistryKey regKey;
        private bool disposed = false;  // Track whether Dispose has been called.

        //Constructor - passin 
        public UserData(String aSubKey)
        {
            if (aSubKey == null) throw new ArgumentException("Programmer error: UserData() called without parameter");
            String subKey = aSubKey.Replace('/', '\\');  //UnixToDos: replace / with \
            subKey = subKey.Trim('\\');                  //Trim leading/trailing slash 
            if (subKey == "") throw new ArgumentException("Programmer error: UserData() called without parameter");
            regKey = Registry.CurrentUser.CreateSubKey(subKey); //Open or create key in HKEY_CURRENT_USER    
        }

        ~UserData() //Destructor
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            // This object will be cleaned up by the Dispose method.
            // Therefore, we call GC.SupressFinalize to take this object off the finalization queue 
            // and prevent finalization code for this object from executing a second time.
            GC.SuppressFinalize(this);
        }

        // Dispose(bool disposing) executes in two distinct scenarios.
        // If disposing equals true, the method has been called directly
        // or indirectly by a user's code. Managed and unmanaged resources
        // can be disposed. If disposing equals false, the method has been called by the 
        // runtime from inside the finalizer and you should not reference other objects. 
        // Only unmanaged resources can be disposed.
        private void Dispose(bool disposing)
        {
            if (!this.disposed)   // Check to see if Dispose has already been called.
            {
                if (disposing)    // If disposing equals true, dispose all managed and unmanaged resources.
                {
                    // Dispose managed resources.
                }

                // Call the appropriate methods to clean up unmanaged resources here.
                // If disposing is false, only the following code is executed.
                if (regKey != null)
                    regKey.Close();     //Flush any changes out to disk
                regKey = null;
            }
            disposed = true;
        }

        public void Flush() //Flush data to disk - Note that Close() also does a flush
        {
            if (regKey != null)
                regKey.Flush();
        }

        #region === Write to Registry

        public void WriteStr(String valueName, String value)
        {
            if (regKey != null)
                regKey.SetValue(valueName, value);    //Creates the key if does not exist
        }

        public void WriteBool(String valueName, Boolean value)
        {
            if (value)
                this.WriteStr(valueName, "1");
            else
                this.WriteStr(valueName, "0");
        }

        public void WriteInt(String valueName, int value)
        {
            this.WriteStr(valueName, value.ToString());
        }

        public void WriteStrParams(String valueName, String delimitor, params String[] paramList)
        {
            String s = Strg.PackString(delimitor, paramList);
            this.WriteStr(valueName, s);
        }

        #endregion

        #region === Read from Registry

        public String ReadStr(String valueName, String defaultValue)
        {
            String s = null;
            Object obj = regKey.GetValue(valueName, null);

            //Important to check data type before reading
            if ((obj != null) && (regKey.GetValueKind(valueName) == RegistryValueKind.String))
                s = (String)obj;

            if (s == null)
                s = defaultValue;
            return s;
        }

        public int ReadInt(String valueName, int defaultValue)
        {
            int iNum;
            String num = this.ReadStr(valueName, "");
            Boolean ok = int.TryParse(num, NumberStyles.Integer, null, out iNum);
            if (ok)
                return iNum;
            else
                return defaultValue;
        }

        public Boolean ReadBool(String valueName, Boolean defaultValue)
        {
            String value;
            if (defaultValue)
                value = this.ReadStr(valueName, "1");
            else
                value = this.ReadStr(valueName, "0");
            value = value.ToLower();
            return ((value == "1") || (value == "y") || (value == "yes") || (value == "true"));
        }

        public String[] ReadStrParams(String valueName, int count, String delimitor)
        {
            String s = this.ReadStr(valueName, "");
            return Strg.UnpackString(delimitor, s, count);
        }


        #endregion

        #region SaveScreenPos()/RestoreScreenPos()

        /// <summary>
        /// SaveFormPos(this, KeyName) - Saves a forms size and location to the registry
        /// Use RestoreFormPos(this, KeyName) to restore a screens position
        /// Note: Assume user has already done a SubKey() call to set the destination registry key
        /// </summary>
        /// <param name="form">Typically 'this' where this is a Form</param>
        /// 
        public void SaveFormPos(Form form)
        {
            SaveFormPos(form, "");
        }

        public void SaveFormPos(Form form, String sKeyName)
        {
            if (String.IsNullOrEmpty(sKeyName))
                sKeyName = form.Name;
            sKeyName = sKeyName + ".";

            //save form pos to registry
            WriteStr(sKeyName + "Left", form.Left.ToString());
            WriteStr(sKeyName + "Top", form.Top.ToString());
            WriteStr(sKeyName + "Width", form.Width.ToString());
            WriteStr(sKeyName + "Height", form.Height.ToString());

            //save window state
            if (form.WindowState == FormWindowState.Maximized)
                WriteStr(sKeyName + "state", "Maximized");
            else if (form.WindowState == FormWindowState.Minimized)
                WriteStr(sKeyName + "state", "Minimized");
            else
                WriteStr(sKeyName + "state", "");
        }

        public void RestoreFormPos(Form form)
        {
            RestoreFormPos(form, "");
        }

        /// <summary>
        /// RestoreFormPos(this, KeyName) - Restores a forms size and location from the registry
        /// Use SaveFormPos(this, KeyName) to save a screens position
        /// Note: Assume user has already done a SubKey() call to set the destination registry key
        /// </summary>
        /// <param name="form">Typically 'this' where this is a Form</param>
        /// 

        public void RestoreFormPos(Form form, String sKeyName)
        {
            int iLeft, iTop, iWidth, iHeight;
            string sLeft, sTop, sWidth, sHeight;

            if (String.IsNullOrEmpty(sKeyName))
                sKeyName = form.Name;
            sKeyName = sKeyName + ".";

            //Maximized?
            String state = this.ReadStr(sKeyName + "state", "");
            if (state == "Maximized")
                form.WindowState = FormWindowState.Maximized;
            else if (state == "Minimized")
                form.WindowState = FormWindowState.Minimized;
            else
                form.WindowState = FormWindowState.Normal;

            //Read settings from registry
            sLeft = this.ReadStr(sKeyName + "Left", "");
            sTop = this.ReadStr(sKeyName + "Top", "");
            sWidth = this.ReadStr(sKeyName + "Width", "");
            sHeight = this.ReadStr(sKeyName + "Height", "");

            //Str -> Int
            iLeft = iTop = iWidth = iHeight = 0;
            Boolean ok = int.TryParse(sLeft, NumberStyles.Integer, null, out iLeft);
            ok = ok && int.TryParse(sTop, NumberStyles.Integer, null, out iTop);
            ok = ok && int.TryParse(sWidth, NumberStyles.Integer, null, out iWidth);
            ok = ok && int.TryParse(sHeight, NumberStyles.Integer, null, out iHeight);

            if (ok)
                ok = AreaOnScreen(iLeft, iTop, iWidth, iHeight); 

            //Restore to position read (since all read ok)
            if (ok)
                form.SetBounds(iLeft, iTop, iWidth, iHeight);
            else // Center the Form on the user's screen
                form.SetBounds((Screen.GetBounds(form).Width / 2) - (form.Width / 2),
                    (Screen.GetBounds(form).Height / 2) - (form.Height / 2),
                    form.Width, form.Height, BoundsSpecified.Location);
        }

        public Boolean AreaOnScreen(int X, int Y, int CX, int CY)
        {
            Screen[] screens = Screen.AllScreens;  //AllScreens uses an unmanaged API to get the screens on the first access, then stores the result in a static variable for later use.
            int upperBound = screens.GetUpperBound(0);
            Boolean ok = false;
            for (int i = 0; i <= upperBound; i++)
            {
                ok = (X < screens[i].Bounds.Right) && (X + CX > screens[i].Bounds.Left)
                  && (Y < screens[i].Bounds.Bottom) && (Y + CY > screens[i].Bounds.Top);
                if (ok)  //This monitor is a match
                    break;
            }
            return ok;
        }        



        #endregion

    }


}
