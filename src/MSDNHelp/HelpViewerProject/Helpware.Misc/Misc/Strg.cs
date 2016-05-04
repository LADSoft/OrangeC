using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;

namespace Helpware.Misc
{

    /// <summary>
    /// Strg - Various static string functions
    /// </summary>

    public static class Strg
    {
        static public readonly Char[] anySlash = new Char[2] { '\\', '/' };
        static public readonly String[] CRLF = new String[1] { Environment.NewLine};

        internal static String PathEnd(String path)
        {
            path = path.TrimEnd(anySlash);
            int i = path.LastIndexOfAny(anySlash);
            if (i >= 0)
                path = path.Substring(i + 1);
            return path;
        }

        internal static String PathEnd(String path, String newExt)
        {
            path = Path.ChangeExtension(path, newExt);
            return PathEnd(path);
        }

        internal static String PackString(String delimStr, params String[] list)
        {
            String s = "";
            for (int i = 0; i < list.Length; i++)
                s += list[i] + delimStr;
            return s;
        }

        internal static String[] UnpackString(String delimStr, String s, int count)  //Always returns atleast count params
        {
            String[] delims = new String[1] { delimStr };
            for (int i = 0; i < count; i++)  //Pad out so we can provide count results
                s += delimStr;
            return s.Split(delims, count + 1, StringSplitOptions.None);  //return +1 (this last item will be junk from padding)
        }

        internal static String TrimEndStr(String s, String find)
        {
            if (s.EndsWith(find))
                return s.Substring(0, s.Length - find.Length);
            else
                return s;
        }

        internal static String HardCRLF(String s, String breakString)
        {
            return s.Replace(breakString, Environment.NewLine); //replace breakString with Hard CR LF
        }

        internal static String HardCRLF(String s)
        {
            return HardCRLF(s, "|");
        }

        internal static int StrToInt(String sNum, int defaultValue)   //Safe StrToInt function
        {
            int iNum;
            Boolean ok = int.TryParse(sNum, NumberStyles.Integer, null, out iNum);
            if (ok)
                return iNum;
            else
                return defaultValue;
        }

        internal static String BoolTo10(Boolean f)
        {
            if (f)
                return "1";
            else
                return "0";
        }

        //return list of files in a dir according to the ext list provided - eg. String[] ss = Dir("c:\temp", ".pas", ".def")
        internal static ArrayList Dir(String dirPath, params String[] fileExt)
        {
            ArrayList list = new ArrayList();
            DirectoryInfo dir = new DirectoryInfo(dirPath);
            if (dir.Exists)
            {
                for (int i = 0; i < fileExt.Length; i++)
                {
                    String fileSpec = "*" + fileExt[i];
                    FileInfo[] files = dir.GetFiles(fileSpec, SearchOption.TopDirectoryOnly);
                    list.Capacity = list.Count + files.Length;
                    for (int k = 0; k < files.Length; k++)
                    {
                        //Looking for an exact match. The stupid C# function will returm *.clmxxx for *.clm spec
                        String s = files[k].Name;
                        int idx = s.LastIndexOf(fileExt[i], StringComparison.CurrentCultureIgnoreCase);
                        if (idx == s.Length - fileExt[i].Length)
                            list.Add(files[k].Name);                       //return just the filename (no path)
                    }
                }
            }
            return list;
        }

        internal static String[] StrArray(String[] sa1, String[] sa2)  //join 2 String arrays
        {
            String[] ss = new String[sa1.Length + sa2.Length];
            sa1.CopyTo(ss, 0);
            sa2.CopyTo(ss, sa1.Length);
            return ss;
        }

        internal static int CompareStringArray(String[] ssList1, String[] ssList2, Boolean ignoreCase)  //Return true if String lists identical
        {
            int change = 0;
            if (ssList1.Length < ssList2.Length) change = -1;
            else if (ssList1.Length > ssList2.Length) change = 1;
            if (change == 0)
            {
                for (int i = 0; i < ssList1.Length; i++)
                {
                    change = String.Compare(ssList1[i], ssList2[i], ignoreCase);  // 0 if equal
                    if (change != 0)
                        break;
                }
            }
            return change;
        }

        internal static int CompareStringArray(String[] ssList1, String[] ssList2)
        {
            return CompareStringArray(ssList1, ssList2, false); //default is 'false' Case Sensitive
        }
    }
}
