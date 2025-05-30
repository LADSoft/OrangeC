#ifndef _STIREG_H
#define _STIREG_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Still Image API registry entries */

#define REGSTR_VAL_TYPE_W  L"Type"
#define REGSTR_VAL_VENDOR_NAME_W  L"Vendor"
#define REGSTR_VAL_DEVICETYPE_W  L"DeviceType"
#define REGSTR_VAL_DEVICESUBTYPE_W  L"DeviceSubType"
#define REGSTR_VAL_DEV_NAME_W  L"DeviceName"
#define REGSTR_VAL_DRIVER_DESC_W  L"DriverDesc"
#define REGSTR_VAL_FRIENDLY_NAME_W  L"FriendlyName"
#define REGSTR_VAL_GENERIC_CAPS_W  L"Capabilities"
#define REGSTR_VAL_HARDWARE_W  L"HardwareConfig"
#define REGSTR_VAL_HARDWARE  TEXT("HardwareConfig")
#define REGSTR_VAL_DEVICE_NAME_W  L"DriverDesc"
#define REGSTR_VAL_DATA_W  L"DeviceData"
#define REGSTR_VAL_GUID_W  L"GUID"
#define REGSTR_VAL_GUID  TEXT("GUID")
#define REGSTR_VAL_LAUNCH_APPS_W  L"LaunchApplications"
#define REGSTR_VAL_LAUNCH_APPS  TEXT("LaunchApplications")
#define REGSTR_VAL_LAUNCHABLE_W  L"Launchable"
#define REGSTR_VAL_LAUNCHABLE  TEXT("Launchable")

#define STI_DEVICE_VALUE_TWAIN_NAME  L"TwainDS"
#define STI_DEVICE_VALUE_ISIS_NAME  L"ISISDriverName"
#define STI_DEVICE_VALUE_ICM_PROFILE  L"ICMProfile"
#define STI_DEVICE_VALUE_DEFAULT_LAUNCHAPP  L"DefaultLaunchApp"
#define STI_DEVICE_VALUE_TIMEOUT  L"PollTimeout"
#define STI_DEVICE_VALUE_DISABLE_NOTIFICATIONS  L"DisableNotifications"
#define REGSTR_VAL_BAUDRATE  L"BaudRate"

#define STI_DEVICE_VALUE_TWAIN_NAME_A  "TwainDS"
#define STI_DEVICE_VALUE_ISIS_NAME_A  "ISISDriverName"
#define STI_DEVICE_VALUE_ICM_PROFILE_A  "ICMProfile"
#define STI_DEVICE_VALUE_DEFAULT_LAUNCHAPP_A  "DefaultLaunchApp"
#define STI_DEVICE_VALUE_TIMEOUT_A  "PollTimeout"
#define STI_DEVICE_VALUE_DISABLE_NOTIFICATIONS_A  "DisableNotifications"
#define REGSTR_VAL_BAUDRATE_A  "BaudRate"


#endif /* _STIREG_H */

