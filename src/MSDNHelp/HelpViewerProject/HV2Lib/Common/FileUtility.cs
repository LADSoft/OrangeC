namespace HVProject.HV2Lib.Common
{
    using System;
    using System.Globalization;
    using System.IO;
    using System.Linq;
    using System.Resources;
    using System.Security;
    using System.Security.AccessControl;
    using System.Security.Principal;
    using System.Text.RegularExpressions;
    using System.Threading;
    using System.Reflection;

    /// <summary>
    /// General file utilities.
    /// </summary>

    public static class FileUtility
    {
        private static readonly Regex deletedPattern = new Regex(@"\.deleted\.");
        
        public static bool CanGetFullPath(string path)
        {
            bool flag = false;
            if (!string.IsNullOrEmpty(path))
            {
                try
                {
                    Path.GetFullPath(path);
                    flag = true;
                }
                catch (ArgumentException)
                {
                }
                catch (PathTooLongException)
                {
                }
                catch (NotSupportedException)
                {
                }
                catch (SecurityException)
                {
                }
            }
            return flag;
        }

        public static bool CheckDirectoryAccessRights(string path, FileSystemRights rightsToCheck)   // Test a directory access rights
        {
            if (string.IsNullOrEmpty(path))
            {
                throw new ArgumentException();
            }
            if (!Directory.Exists(path))
            {
                throw new DirectoryNotFoundException(path);
            }
            WindowsIdentity current = WindowsIdentity.GetCurrent();
            IdentityReferenceCollection groups = current.Groups;
            AuthorizationRuleCollection rules = new DirectoryInfo(path).GetAccessControl().GetAccessRules(true, true, typeof(SecurityIdentifier));
            bool flag = false;
            bool flag2 = false;
            foreach (FileSystemAccessRule rule in rules)
            {
                if (groups.Contains(rule.IdentityReference) || current.User.Equals(rule.IdentityReference))
                {
                    if (rule.AccessControlType.Equals(AccessControlType.Deny))
                    {
                        if ((rule.FileSystemRights & rightsToCheck) == 0)
                        {
                            continue;
                        }
                        flag2 = true;
                        break;
                    }
                    if (rule.AccessControlType.Equals(AccessControlType.Allow) && ((rule.FileSystemRights & rightsToCheck) == rightsToCheck))
                    {
                        flag = true;
                    }
                }
            }
            return (flag & !flag2);
        }

        public static bool CheckFileAccessRights(string path, FileSystemRights rightsToCheck)
        {
            if (string.IsNullOrEmpty(path))
            {
                throw new ArgumentException();
            }
            if (!File.Exists(path))
            {
                throw new FileNotFoundException(path);
            }
            WindowsIdentity current = WindowsIdentity.GetCurrent();
            IdentityReferenceCollection groups = current.Groups;
            AuthorizationRuleCollection rules = new FileInfo(path).GetAccessControl().GetAccessRules(true, true, typeof(SecurityIdentifier));
            bool flag = false;
            bool flag2 = false;
            foreach (FileSystemAccessRule rule in rules)
            {
                if (groups.Contains(rule.IdentityReference) || current.User.Equals(rule.IdentityReference))
                {
                    if (rule.AccessControlType.Equals(AccessControlType.Deny))
                    {
                        if ((rule.FileSystemRights & rightsToCheck) == 0)
                        {
                            continue;
                        }
                        flag2 = true;
                        break;
                    }
                    if (rule.AccessControlType.Equals(AccessControlType.Allow) && ((rule.FileSystemRights & rightsToCheck) == rightsToCheck))
                    {
                        flag = true;
                    }
                }
            }
            return (flag & !flag2);
        }

        public static void CleanupDirectory(string path)   //Given dir delete all files and subdirs
        {
            if (!string.IsNullOrEmpty(path) && Directory.Exists(path))
            {
                foreach (string str in Directory.GetFiles(path))
                {
                    File.Delete(str);
                }
                foreach (string str2 in Directory.GetDirectories(path))
                {
                    CleanupDirectory(str2);
                }
                Directory.Delete(path);
            }
        }

        public static void CopyFileWithRetry(string source, string destination, params TimeSpan[] retryIntervals)
        {
            CopyFileWithRetry(source, destination, null, retryIntervals);
        }

        public static void CopyFileWithRetry(string source, string destination, Action<string, string> failureCallback, params TimeSpan[] retryIntervals)
        {
            int index = 0;
        Label_0002:
            try
            {
                File.Copy(source, destination);
            }
            catch (IOException)
            {
                if (index >= retryIntervals.Length)
                {
                    if (failureCallback != null)
                    {
                        failureCallback(source, destination);
                    }
                    throw;
                }
                Thread.Sleep(retryIntervals[index]);
                index++;
                goto Label_0002;
            }
        }


        /// <summary>
        /// if a version file exists return that, otherwise return the non-versioned file passed
        /// </summary>
        /// <example>
        /// String realPath = FileUtility.GetFileName("c:\store\package.mshc");  //Returns actual filename - eg. "c:\store\package.2.mshc"
        /// </example>
        /// <param name="file">Path to version file without any version number.</param>
        /// <returns></returns>
        public static string GetFileName(string file)  
        {
            if (string.IsNullOrEmpty(file))
            {
                throw new ArgumentException();
            }
            string versionFileName = GetVersionFileName(file);
            if (string.IsNullOrEmpty(versionFileName) && File.Exists(file))
            {
                versionFileName = file;
            }
            return versionFileName;
        }

        /// <summary>
        /// Reduce a version file to a non-version file. 
        /// dir\xxx.VersionNum.xml or dir\xxx.xml --> dir\xxx.xml
        /// </summary>
        /// <param name="file">File path to file name with or without a version number</param>
        /// <returns></returns>
        public static string GetUnversionedFileName(string file)   
        {
            if (string.IsNullOrEmpty(file))
            {
                throw new ArgumentException();
            }
            string fileNameWithoutExtension = Path.GetFileNameWithoutExtension(file);
            if (fileNameWithoutExtension.Length == Path.GetFileNameWithoutExtension(fileNameWithoutExtension).Length)
            {
                return file;   // file has no version number
            }
            fileNameWithoutExtension = Path.GetFileNameWithoutExtension(fileNameWithoutExtension);   //clip off .VersionNum
            string str3 = Path.GetExtension(file).Substring(1);
            string directoryName = Path.GetDirectoryName(file);
            return string.Format(CultureInfo.InvariantCulture, "{0}.{1}", new object[] { Path.Combine(directoryName, fileNameWithoutExtension), str3 });
        }

        public static int GetVersion(string file)  // return 30 from dir\name.30.xml If no version number return -1
        {
            if (string.IsNullOrEmpty(file))
            {
                throw new ArgumentException();
            }
            string fileNameWithoutExtension = Path.GetFileNameWithoutExtension(file);    //  dir\name.v.xml --> dir\name.v
            if (string.IsNullOrEmpty(fileNameWithoutExtension))
            {
                throw new ArgumentException();
            }
            string extension = Path.GetExtension(fileNameWithoutExtension);       //  dir\name.v --> .v
            if (string.IsNullOrEmpty(extension))
            {
                return 0;   // no .v num found
            }
            string str3 = extension.Substring(1);   // .v --> v
            if (string.IsNullOrEmpty(str3))
            {
                throw new ArgumentException();
            }
            int result = 0;
            if (!int.TryParse(str3, NumberStyles.Integer, CultureInfo.InvariantCulture, out result))   // v -> int
            {
                result = 0;
            }
            return result;
        }

        public static string GetVersionFileName(string file)  //given non-versioned filename, return the largest versioned filename found in the folder
        {
            if (string.IsNullOrEmpty(file))
            {
                throw new ArgumentException();
            }
            string fileNameWithoutExtension = Path.GetFileNameWithoutExtension(file);
            string str2 = Path.GetExtension(file).Substring(1);
            string directoryName = Path.GetDirectoryName(file);
            string str4 = null;
            if (!Directory.Exists(directoryName))
            {
                return str4;
            }
            string filespec = string.Format(CultureInfo.InvariantCulture, "{0}.{1}.{2}", new object[] { fileNameWithoutExtension, '*', str2 });
            string[] source = Directory.GetFiles(directoryName, filespec, SearchOption.TopDirectoryOnly);
            if (source.Length <= 0)
            {
                return str4;
            }

            //return biggest versioned filename
            string input = source.Aggregate<string>(delegate (string first, string next) {
                if (GetVersion(first) <= GetVersion(next))
                {
                    return next;
                }
                return first;
            });

            //if ".deleted." found in highest filename return null
            return (deletedPattern.Match(input).Success ? null : input);
        }

        public static bool IsPathRooted(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                return false;
            }
            try
            {
                if (!Path.IsPathRooted(path))
                {
                    return false;
                }
                if ((path.Length == 2) && (path[1] == ':'))
                {
                    return false;
                }
                if (((path.Length >= 3) && (path[1] == ':')) && ((path[2] != '\\') && (path[2] != '/')))
                {
                    return false;
                }
                return true;
            }
            catch (ArgumentException)
            {
                return false;
            }
        }

        public static FileStream OpenFileStreamWithRetry(string fileName, params TimeSpan[] retryIntervals)
        {
            return OpenFileStreamWithRetry(fileName, null, retryIntervals);
        }

        public static FileStream OpenFileStreamWithRetry(string fileName, Action<string> failureCallback, params TimeSpan[] retryIntervals)
        {
            FileStream stream = null;
            int index = 0;
        Label_0004:
            try
            {
                stream = new FileStream(fileName, FileMode.Open, FileAccess.Read);
            }
            catch (IOException)
            {
                if (index >= retryIntervals.Length)
                {
                    if (failureCallback != null)
                    {
                        failureCallback(fileName);
                    }
                    throw;
                }
                Thread.Sleep(retryIntervals[index]);
                index++;
                goto Label_0004;
            }
            return stream;
        }
    }
}


