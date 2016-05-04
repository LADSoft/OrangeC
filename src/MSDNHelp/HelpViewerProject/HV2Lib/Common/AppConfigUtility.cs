namespace HVProject.HV2Lib.Common
{
    using Microsoft.Win32;
    using System;
    using System.Collections.Generic;
    using System.Configuration;
    using System.Globalization;
    using System.IO;
    using System.Reflection;

    /// <summary>Access .config file setting</summary>
    /// <example>
    /// if (AppConfigUtility.GetSetting("IPV6", "HlpViewer.exe.config") == "1")
    /// </example>

    internal static class AppConfigUtility
    {
        private static Dictionary<string, string> _settings = new Dictionary<string, string>();

        public static void ClearAllSettings()
        {
            _settings = new Dictionary<string, string>();
        }

        private static string GetFullSettingName(string settingName, string configFileName)
        {
            return (configFileName.ToUpperInvariant() + ":" + settingName);
        }

        public static string GetSetting(string settingName, string configFileName)
        {
            if (string.IsNullOrEmpty(settingName))
            {
                throw new ArgumentNullException("settingName");
            }
            if (string.IsNullOrEmpty(configFileName))
            {
                throw new ArgumentNullException("configFileName");
            }
            string fullSettingName = GetFullSettingName(settingName, configFileName);
            string str2 = string.Empty;
            lock (_settings)
            {
                if (_settings.ContainsKey(fullSettingName))
                {
                    return _settings[fullSettingName];
                }

                string path = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), configFileName);
                if (!File.Exists(path))
                {
                    path = Path.Combine(ReadRegistryValue(Registry.LocalMachine, @"SOFTWARE\Microsoft\Help\v2.0", "AppRoot"), configFileName);
                }
                if (File.Exists(path))
                {
                    AppSettingsSection appSettings = ConfigurationManager.OpenMappedExeConfiguration(new ExeConfigurationFileMap { ExeConfigFilename = path }, ConfigurationUserLevel.None).AppSettings;
                    if (appSettings.Settings[settingName] != null)
                    {
                        str2 = appSettings.Settings[settingName].Value;
                    }
                }
                if (string.IsNullOrEmpty(str2))
                {
                    str2 = ReadRegistryValue(Registry.LocalMachine, @"SOFTWARE\Microsoft\Help\v2.0", settingName);
                }
                _settings.Add(fullSettingName, str2);
            }
            return str2;
        }

        private static string ReadRegistryValue(RegistryKey baseKeyName, string subKey, string KeyName)
        {
            RegistryKey key2 = baseKeyName.OpenSubKey(subKey);
            string str = "";
            if (key2 != null)
            {
                object obj2 = key2.GetValue(KeyName);
                if (obj2 == null)
                {
                    return "";
                }
                if (obj2.GetType() == typeof(int))
                {
                    str = ((int) obj2).ToString(CultureInfo.InvariantCulture);
                }
                else
                {
                    str = (string) obj2;
                }
                if (str != null)
                {
                    return str;
                }
            }
            return "";
        }

        public static void SetSetting(string settingName, string settingValue, string configFileName)
        {
            if (string.IsNullOrEmpty(settingName))
            {
                throw new ArgumentNullException("settingName");
            }
            if (string.IsNullOrEmpty(configFileName))
            {
                throw new ArgumentNullException("configFileName");
            }
            string fullSettingName = GetFullSettingName(settingName, configFileName);
            lock (_settings)
            {
                if (_settings.ContainsKey(fullSettingName))
                {
                    _settings[fullSettingName] = settingValue;
                }
                else
                {
                    _settings.Add(fullSettingName, settingValue);
                }
                string path = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), configFileName);
                if (!File.Exists(path))
                {
                    path = Path.Combine(ReadRegistryValue(Registry.LocalMachine, @"SOFTWARE\Microsoft\Help\v2.0", "AppRoot"), fullSettingName);
                }
                if (File.Exists(path))
                {
                    System.Configuration.Configuration configuration = ConfigurationManager.OpenMappedExeConfiguration(new ExeConfigurationFileMap { ExeConfigFilename = path }, ConfigurationUserLevel.None);
                    configuration.AppSettings.Settings[settingName].Value = settingValue;
                    configuration.Save(ConfigurationSaveMode.Modified);
                }
            }
        }
    }
}

