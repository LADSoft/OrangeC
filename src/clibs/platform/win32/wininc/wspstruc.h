/* 
   wspstruc.h

   Base definitions

   Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   If you are interested in a warranty or support for this source code,
   contact Scott Christley <scottc@net-community.com> for more information.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 
#pragma pack(8)

typedef struct _PRINTER_INFO_1 {
    DWORD   Flags;
    LPTSTR   pDescription;
    LPTSTR   pName;
    LPTSTR   pComment;
} PRINTER_INFO_1, *PPRINTER_INFO_1, *LPPRINTER_INFO_1;

typedef struct _PRINTER_INFO_2 {
    LPSTR     pServerName;
    LPSTR     pPrinterName;
    LPSTR     pShareName;
    LPSTR     pPortName;
    LPSTR     pDriverName;
    LPSTR     pComment;
    LPSTR     pLocation;
    LPDEVMODE pDevMode;
    LPSTR     pSepFile;
    LPSTR     pPrintProcessor;
    LPSTR     pDatatype;
    LPSTR     pParameters;
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
    DWORD   Attributes;
    DWORD   Priority;
    DWORD   DefaultPriority;
    DWORD   StartTime;
    DWORD   UntilTime;
    DWORD   Status;
    DWORD   cJobs;
    DWORD   AveragePPM;
} PRINTER_INFO_2, *PPRINTER_INFO_2, *LPPRINTER_INFO_2;

typedef struct _PRINTER_INFO_3 {
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
} PRINTER_INFO_3, *PPRINTER_INFO_3, *LPPRINTER_INFO_3;

typedef struct _PRINTER_INFO_4 {
    LPTSTR   pPrinterName;
    LPTSTR   pServerName;
    DWORD   Attributes;
} PRINTER_INFO_4, *PPRINTER_INFO_4, *LPPRINTER_INFO_4;

typedef struct _PRINTER_INFO_5 {
    LPTSTR   pPrinterName;
    LPTSTR   pPortName;
    DWORD   Attributes;
    DWORD   DeviceNotSelectedTimeout;
    DWORD   TransmissionRetryTimeout;
} PRINTER_INFO_5, *PPRINTER_INFO_5, *LPPRINTER_INFO_5;

typedef struct _PRINTER_INFO_6 {
    DWORD   dwStatus;
} PRINTER_INFO_6, *PPRINTER_INFO_6, *LPPRINTER_INFO_6;


typedef struct _PRINTER_INFO_7 {
  LPTSTR    pszObjectGUID;
  DWORD    dwAction;
} PRINTER_INFO_7, *PPRINTER_INFO_7, *LPPRINTER_INFO_7;

typedef struct _PRINTER_INFO_8 {
    LPDEVMODE pDevMode;
} PRINTER_INFO_8, *PPRINTER_INFO_8, *LPPRINTER_INFO_8;

typedef struct _PRINTER_INFO_9 {
    LPDEVMODE pDevMode;
} PRINTER_INFO_9, *PPRINTER_INFO_9, *LPPRINTER_INFO_9;

typedef struct _JOB_INFO_1 {
   DWORD    JobId;
   LPTSTR      pPrinterName;
   LPTSTR      pMachineName;
   LPTSTR      pUserName;
   LPTSTR      pDocument;
   LPTSTR      pDatatype;
   LPTSTR      pStatus;
   DWORD    Status;
   DWORD    Priority;
   DWORD    Position;
   DWORD    TotalPages;
   DWORD    PagesPrinted;
   SYSTEMTIME Submitted;
} JOB_INFO_1, *PJOB_INFO_1, *LPJOB_INFO_1;

typedef struct _JOB_INFO_2 {
   DWORD    JobId;
   LPSTR      pPrinterName;
   LPSTR      pMachineName;
   LPSTR      pUserName;
   LPSTR      pDocument;
   LPSTR      pNotifyName;
   LPSTR      pDatatype;
   LPSTR      pPrintProcessor;
   LPSTR      pParameters;
   LPSTR      pDriverName;
   LPDEVMODE  pDevMode;
   LPSTR      pStatus;
   PSECURITY_DESCRIPTOR pSecurityDescriptor;
   DWORD    Status;
   DWORD    Priority;
   DWORD    Position;
   DWORD    StartTime;
   DWORD    UntilTime;
   DWORD    TotalPages;
   DWORD    Size;
   SYSTEMTIME Submitted;    
   DWORD    Time;           
   DWORD    PagesPrinted;
} JOB_INFO_2, *PJOB_INFO_2, *LPJOB_INFO_2;

typedef struct _JOB_INFO_3 {
    DWORD   JobId;
    DWORD   NextJobId;
    DWORD   Reserved;
} JOB_INFO_3, *PJOB_INFO_3, *LPJOB_INFO_3;

typedef struct _ADDJOB_INFO_1 {
    LPTSTR     Path;
    DWORD   JobId;
} ADDJOB_INFO_1, *PADDJOB_INFO_1, *LPADDJOB_INFO_1;

typedef struct _DRIVER_INFO_1 {
    LPTSTR     pName;              
} DRIVER_INFO_1, *PDRIVER_INFO_1, *LPDRIVER_INFO_1;

typedef struct _DRIVER_INFO_2 {
    DWORD   cVersion;
    LPTSTR     pName;              
    LPTSTR     pEnvironment;       
    LPTSTR     pDriverPath;        
    LPTSTR     pDataFile;          
    LPTSTR     pConfigFile;        
} DRIVER_INFO_2, *PDRIVER_INFO_2, *LPDRIVER_INFO_2;

typedef struct _DRIVER_INFO_3 {
    DWORD   cVersion;
    LPTSTR     pName;                    
    LPTSTR     pEnvironment;             
    LPTSTR     pDriverPath;              
    LPTSTR     pDataFile;                
    LPTSTR     pConfigFile;              
    LPTSTR     pHelpFile;                
    LPTSTR     pDependentFiles;          
    LPTSTR     pMonitorName;             
    LPTSTR     pDefaultDataType;         
} DRIVER_INFO_3, *PDRIVER_INFO_3, *LPDRIVER_INFO_3;

typedef struct _DRIVER_INFO_4 {
    DWORD   cVersion;
    LPTSTR     pName;                    
    LPTSTR     pEnvironment;             
    LPTSTR     pDriverPath;              
    LPTSTR     pDataFile;                
    LPTSTR     pConfigFile;              
    LPTSTR     pHelpFile;                
    LPTSTR     pDependentFiles;          
    LPTSTR     pMonitorName;             
    LPTSTR     pDefaultDataType;         
    LPTSTR     pszzPreviousNames;        
} DRIVER_INFO_4, *PDRIVER_INFO_4, *LPDRIVER_INFO_4;

typedef struct _DRIVER_INFO_5 {
    DWORD   cVersion;
    LPTSTR     pName;                    
    LPTSTR     pEnvironment;             
    LPTSTR     pDriverPath;              
    LPTSTR     pDataFile;                
    LPTSTR     pConfigFile;              
    DWORD     dwDriverAttributes;       
    DWORD     dwConfigVersion;          
    DWORD     dwDriverVersion;          
} DRIVER_INFO_5, *PDRIVER_INFO_5, *LPDRIVER_INFO_5;

typedef struct _DRIVER_INFO_6 {
    DWORD     cVersion;
    LPTSTR     pName;                    
    LPTSTR     pEnvironment;             
    LPTSTR     pDriverPath;              
    LPTSTR     pDataFile;                
    LPTSTR     pConfigFile;              
    LPTSTR     pHelpFile;                
    LPTSTR     pDependentFiles;          
    LPTSTR     pMonitorName;             
    LPTSTR     pDefaultDataType;         
    LPTSTR     pszzPreviousNames;        
    FILETIME  ftDriverDate;
    DWORDLONG dwlDriverVersion;
    LPTSTR      pszMfgName;
    LPTSTR      pszOEMUrl;
    LPTSTR      pszHardwareID;
    LPTSTR      pszProvider;
} DRIVER_INFO_6, *PDRIVER_INFO_6, *LPDRIVER_INFO_6;

typedef struct _DOC_INFO_1 {
    LPTSTR     pDocName;
    LPTSTR     pOutputFile;
    LPTSTR     pDatatype;
} DOC_INFO_1, *PDOC_INFO_1, *LPDOC_INFO_1;

typedef struct _FORM_INFO_1 {
    DWORD   Flags;
    LPTSTR     pName;
    SIZEL   Size;
    RECTL   ImageableArea;
} FORM_INFO_1, *PFORM_INFO_1, *LPFORM_INFO_1;

typedef struct _DOC_INFO_2 {
    LPTSTR     pDocName;
    LPTSTR     pOutputFile;
    LPTSTR     pDatatype;
    DWORD   dwMode;
    DWORD   JobId;
} DOC_INFO_2, *PDOC_INFO_2, *LPDOC_INFO_2;

typedef struct _DOC_INFO_3 {
    LPTSTR     pDocName;
    LPTSTR     pOutputFile;
    LPTSTR     pDatatype;
    DWORD     dwFlags;
} DOC_INFO_3, *PDOC_INFO_3, *LPDOC_INFO_3;

typedef struct _PRINTPROCESSOR_INFO_1 {
    LPTSTR     pName;
} PRINTPROCESSOR_INFO_1, *PPRINTPROCESSOR_INFO_1, *LPPRINTPROCESSOR_INFO_1;

typedef struct _PORT_INFO_1 {
    LPTSTR     pName;
} PORT_INFO_1, *PPORT_INFO_1, *LPPORT_INFO_1;

typedef struct _PORT_INFO_2 {
    LPTSTR     pPortName;
    LPTSTR     pMonitorName;
    LPTSTR     pDescription;
    DWORD     fPortType;
    DWORD     Reserved;
} PORT_INFO_2, *PPORT_INFO_2, *LPPORT_INFO_2;

typedef struct _PORT_INFO_3 {
    DWORD   dwStatus;
    LPTSTR   pszStatus;
    DWORD   dwSeverity;
} PORT_INFO_3, *PPORT_INFO_3, *LPPORT_INFO_3;

typedef struct _MONITOR_INFO_1{
    LPTSTR     pName;
} MONITOR_INFO_1, *PMONITOR_INFO_1, *LPMONITOR_INFO_1;

typedef struct _MONITOR_INFO_2{
    LPTSTR     pName;
    LPTSTR     pEnvironment;
    LPTSTR     pDLLName;
} MONITOR_INFO_2, *PMONITOR_INFO_2, *LPMONITOR_INFO_2;

typedef struct _DATATYPES_INFO_1{
    LPTSTR     pName;
} DATATYPES_INFO_1, *PDATATYPES_INFO_1, *LPDATATYPES_INFO_1;

typedef struct _PRINTER_DEFAULTS{
    LPSTR         pDatatype;
    PDEVMODE pDevMode;
    ACCESS_MASK DesiredAccess;
} PRINTER_DEFAULTS, *PPRINTER_DEFAULTS, *LPPRINTER_DEFAULTS;

typedef struct _PRINTER_ENUM_VALUES {
    LPTSTR   pValueName;
    DWORD   cbValueName;
    DWORD   dwType;
    LPBYTE  pData;
    DWORD   cbData;
} PRINTER_ENUM_VALUES, *PPRINTER_ENUM_VALUES, *LPPRINTER_ENUM_VALUES;

typedef struct _PRINTER_NOTIFY_OPTIONS_TYPE {
    WORD Type;
    WORD Reserved0;
    DWORD Reserved1;
    DWORD Reserved2;
    DWORD Count;
    PWORD pFields;
} PRINTER_NOTIFY_OPTIONS_TYPE, *PPRINTER_NOTIFY_OPTIONS_TYPE, *LPPRINTER_NOTIFY_OPTIONS_TYPE;

typedef struct _PRINTER_NOTIFY_OPTIONS {
    DWORD Version;
    DWORD Flags;
    DWORD Count;
    PPRINTER_NOTIFY_OPTIONS_TYPE pTypes;
} PRINTER_NOTIFY_OPTIONS, *PPRINTER_NOTIFY_OPTIONS, *LPPRINTER_NOTIFY_OPTIONS;

typedef struct _PRINTER_NOTIFY_INFO_DATA {
    WORD Type;
    WORD Field;
    DWORD Reserved;
    DWORD Id;
    union {
        DWORD adwData[2];
        struct {
            DWORD  cbBuf;
            LPVOID pBuf;
        } Data;
    } NotifyData;
} PRINTER_NOTIFY_INFO_DATA, *PPRINTER_NOTIFY_INFO_DATA, *LPPRINTER_NOTIFY_INFO_DATA;

typedef struct _PRINTER_NOTIFY_INFO {
    DWORD Version;
    DWORD Flags;
    DWORD Count;
    PRINTER_NOTIFY_INFO_DATA aData[1];
} PRINTER_NOTIFY_INFO, *PPRINTER_NOTIFY_INFO, *LPPRINTER_NOTIFY_INFO;

typedef struct _PROVIDOR_INFO_1{
    LPTSTR     pName;
    LPTSTR     pEnvironment;
    LPTSTR     pDLLName;
} PROVIDOR_INFO_1, *PPROVIDOR_INFO_1, *LPPROVIDOR_INFO_1;

typedef struct _PROVIDOR_INFO_2{
    LPTSTR     pOrder;
} PROVIDOR_INFO_2, *PPROVIDOR_INFO_2, *LPPROVIDOR_INFO_2;

#ifdef __cplusplus
}
#endif

#pragma pack()
