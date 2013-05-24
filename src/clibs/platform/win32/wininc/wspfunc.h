/* 
   wspfunc.h

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
#ifdef __cplusplus
extern "C" {
#endif

HANDLE PASCAL WINBASEAPI DeletePrinter(
   HANDLE   hPrinter
);
BOOL PASCAL WINBASEAPI StartPagePrinter(
    HANDLE  hPrinter
);
BOOL PASCAL WINBASEAPI WritePrinter(
    HANDLE  hPrinter,
    LPVOID  pBuf,
    DWORD   cbBuf,
    LPDWORD pcWritten
);
BOOL PASCAL WINBASEAPI FlushPrinter(
    HANDLE   hPrinter,
    LPVOID   pBuf,
    DWORD    cbBuf,
    LPDWORD pcWritten,
    DWORD    cSleep
);
BOOL PASCAL WINBASEAPI EndPagePrinter(
   HANDLE   hPrinter
);
BOOL PASCAL WINBASEAPI AbortPrinter(
   HANDLE   hPrinter
);
BOOL PASCAL WINBASEAPI ReadPrinter(
    HANDLE  hPrinter,
    LPVOID  pBuf,
    DWORD   cbBuf,
    LPDWORD pNoBytesRead
);
BOOL PASCAL WINBASEAPI EndDocPrinter(
   HANDLE   hPrinter
);
BOOL PASCAL WINBASEAPI ScheduleJob(
    HANDLE  hPrinter,
    DWORD   JobId
);
BOOL PASCAL WINBASEAPI PrinterProperties(
    HWND    hWnd,
    HANDLE  hPrinter
);
DWORD PASCAL WINBASEAPI WaitForPrinterChange(
    HANDLE  hPrinter,
    DWORD   Flags
);
HANDLE PASCAL WINBASEAPI FindFirstPrinterChangeNotification(
    HANDLE  hPrinter,
    DWORD   fdwFlags,
    DWORD   fdwOptions,
    LPVOID  pPrinterNotifyOptions
);
BOOL PASCAL WINBASEAPI FindNextPrinterChangeNotification(
    HANDLE hChange,
    PDWORD pdwChange,
    LPVOID pvReserved,
    LPVOID *ppPrinterNotifyInfo
);
BOOL PASCAL WINBASEAPI FreePrinterNotifyInfo(
    PPRINTER_NOTIFY_INFO pPrinterNotifyInfo
);
BOOL PASCAL WINBASEAPI FindClosePrinterChangeNotification(
    HANDLE hChange
);
BOOL PASCAL WINBASEAPI ClosePrinter(
    HANDLE hPrinter
);
HANDLE PASCAL WINBASEAPI ConnectToPrinterDlg(
    HWND    hwnd,
    DWORD   Flags
);

#ifdef UNICODE
BOOL PASCAL WINBASEAPI EnumPrintersW(
    DWORD   Flags,
    LPWSTR Name,
    DWORD   Level,
    LPBYTE  pPrinterEnum,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI OpenPrinterW(
   LPWSTR    pPrinterName,
   LPHANDLE phPrinter,
   LPPRINTER_DEFAULTS pDefault
);
BOOL PASCAL WINBASEAPI ResetPrinterW(
   HANDLE   hPrinter,
   LPPRINTER_DEFAULTS pDefault
);
BOOL PASCAL WINBASEAPI SetJobW(
    HANDLE  hPrinter,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   Command
);
BOOL PASCAL WINBASEAPI GetJobW(
   HANDLE   hPrinter,
   DWORD    JobId,
   DWORD    Level,
   LPBYTE   pJob,
   DWORD    cbBuf,
   LPDWORD  pcbNeeded
);
BOOL PASCAL WINBASEAPI EnumJobsW(
    HANDLE  hPrinter,
    DWORD   FirstJob,
    DWORD   NoJobs,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
HANDLE PASCAL WINBASEAPI AddPrinterW(
    LPWSTR   pName,
    DWORD   Level,
    LPBYTE  pPrinter
);
BOOL PASCAL WINBASEAPI SetPrinterW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   Command
);
BOOL PASCAL WINBASEAPI GetPrinterW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI AddPrinterDriverW(
    LPWSTR   pName,
     DWORD   Level,
    LPBYTE  pDriverInfo
);
BOOL PASCAL WINBASEAPI AddPrinterDriverExW(
    LPWSTR   pName,
    DWORD     Level,
    LPBYTE pDriverInfo,
    DWORD     dwFileCopyFlags
);
BOOL PASCAL WINBASEAPI EnumPrinterDriversW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI GetPrinterDriverW(
    HANDLE  hPrinter,
    LPWSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI GetPrinterDriverDirectoryW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverDirectory,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI DeletePrinterDriverW(
   LPWSTR    pName,
   LPWSTR    pEnvironment,
   LPWSTR    pDriverName
);
BOOL PASCAL WINBASEAPI DeletePrinterDriverExW(
   LPWSTR    pName,
   LPWSTR    pEnvironment,
   LPWSTR    pDriverName,
   DWORD      dwDeleteFlag,
   DWORD      dwVersionFlag
);
BOOL PASCAL WINBASEAPI AddPrintProcessorW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    LPWSTR   pPathName,
    LPWSTR   pPrintProcessorName
);
BOOL PASCAL WINBASEAPI EnumPrintProcessorsW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI GetPrintProcessorDirectoryW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI EnumPrintProcessorDatatypesW(
    LPWSTR   pName,
    LPWSTR   pPrintProcessorName,
    DWORD   Level,
    LPBYTE  pDatatypes,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI DeletePrintProcessorW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    LPWSTR   pPrintProcessorName
);
DWORD PASCAL WINBASEAPI StartDocPrinterW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pDocInfo
);
BOOL PASCAL WINBASEAPI AddJobW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pData,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
LONG PASCAL WINBASEAPI DocumentPropertiesW(
    HWND      hWnd,
    HANDLE    hPrinter,
    LPWSTR   pDeviceName,
    PDEVMODE pDevModeOutput,
    PDEVMODE pDevModeInput,
    DWORD     fMode
);
LONG PASCAL WINBASEAPI AdvancedDocumentPropertiesW(
    HWND    hWnd,
    HANDLE  hPrinter,
    LPWSTR   pDeviceName,
    PDEVMODE pDevModeOutput,
    PDEVMODE pDevModeInput
);
DWORD PASCAL WINBASEAPI GetPrinterDataW(
    HANDLE   hPrinter,
    LPWSTR  pValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
);
DWORD PASCAL WINBASEAPI GetPrinterDataExW(
    HANDLE   hPrinter,
    LPCWSTR pKeyName,
    LPCWSTR pValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
);
DWORD PASCAL WINBASEAPI EnumPrinterDataW(
    HANDLE   hPrinter,
    DWORD    dwIndex,
    LPWSTR  pValueName,
    DWORD    cbValueName,
    LPDWORD  pcbValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    cbData,
    LPDWORD  pcbData
);
DWORD PASCAL WINBASEAPI EnumPrinterDataExW(
    HANDLE   hPrinter,
    LPCWSTR pKeyName,
    LPBYTE   pEnumValues,
    DWORD    cbEnumValues,
    LPDWORD  pcbEnumValues,
    LPDWORD  pnEnumValues
);
DWORD PASCAL WINBASEAPI EnumPrinterKeyW(
    HANDLE   hPrinter,
    LPCWSTR pKeyName,
    LPWSTR  pSubkey,
    DWORD    cbSubkey,
    LPDWORD  pcbSubkey
);
DWORD PASCAL WINBASEAPI SetPrinterDataW(
    HANDLE  hPrinter,
    LPWSTR pValueName,
    DWORD   Type,
    LPBYTE  pData,
    DWORD   cbData
);
DWORD PASCAL WINBASEAPI SetPrinterDataExW(
    HANDLE   hPrinter,
    LPCWSTR pKeyName,
    LPCWSTR pValueName,
    DWORD    Type,
    LPBYTE   pData,
    DWORD    cbData
);
DWORD PASCAL WINBASEAPI DeletePrinterDataW(
    HANDLE  hPrinter,
    LPWSTR pValueName
);
DWORD PASCAL WINBASEAPI DeletePrinterDataExW(
    HANDLE   hPrinter,
    LPCWSTR pKeyName,
    LPCWSTR pValueName
);
DWORD PASCAL WINBASEAPI DeletePrinterKeyW(
    HANDLE   hPrinter,
    LPCWSTR pKeyName
);
DWORD PASCAL WINBASEAPI PrinterMessageBoxW(
    HANDLE  hPrinter,
    DWORD   Error,
    HWND    hWnd,
    LPWSTR   pText,
    LPWSTR   pCaption,
    DWORD   dwType
);
BOOL PASCAL WINBASEAPI AddFormW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm
);
BOOL PASCAL WINBASEAPI DeleteFormW(
    HANDLE  hPrinter,
    LPWSTR   pFormName
);
BOOL PASCAL WINBASEAPI GetFormW(
    HANDLE  hPrinter,
    LPWSTR   pFormName,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI SetFormW(
    HANDLE  hPrinter,
    LPWSTR   pFormName,
    DWORD   Level,
    LPBYTE  pForm
);
BOOL PASCAL WINBASEAPI EnumFormsW(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI EnumMonitorsW(
    LPWSTR   pName,
    DWORD   Level,
    LPBYTE  pMonitors,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI AddMonitorW(
    LPWSTR   pName,
    DWORD   Level,
    LPBYTE  pMonitors
);
BOOL PASCAL WINBASEAPI DeleteMonitorW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    LPWSTR   pMonitorName
);
BOOL PASCAL WINBASEAPI EnumPortsW(
    LPWSTR   pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI AddPortW(
    LPWSTR   pName,
    HWND    hWnd,
    LPWSTR   pMonitorName
);
BOOL PASCAL WINBASEAPI ConfigurePortW(
    LPWSTR   pName,
    HWND    hWnd,
    LPWSTR   pPortName
);
BOOL PASCAL WINBASEAPI DeletePortW(
    LPWSTR pName,
    HWND    hWnd,
    LPWSTR pPortName
);
BOOL PASCAL WINBASEAPI XcvDataW(
    HANDLE  hXcv,
    PCWSTR  pszDataName,
    PBYTE   pInputData,
    DWORD   cbInputData,
    PBYTE   pOutputData,
    DWORD   cbOutputData,
    PDWORD  pcbOutputNeeded,
    PDWORD  pdwStatus
);
BOOL PASCAL WINBASEAPI GetDefaultPrinterW(
    LPWSTR   pszBuffer,
    LPDWORD  pcchBuffer
    );
BOOL PASCAL WINBASEAPI SetDefaultPrinterW(
    LPCWSTR pszPrinter
    );
BOOL PASCAL WINBASEAPI SetPortW(
    LPWSTR     pName,
    LPWSTR     pPortName,
    DWORD       dwLevel,
    LPBYTE      pPortInfo
);
BOOL PASCAL WINBASEAPI AddPrinterConnectionW(
    LPWSTR   pName
);
BOOL PASCAL WINBASEAPI DeletePrinterConnectionW(
    LPWSTR   pName
);
BOOL PASCAL WINBASEAPI AddPrintProvidorW(
    LPWSTR  pName,
    DWORD    level,
    LPBYTE   pProvidorInfo
);
BOOL PASCAL WINBASEAPI DeletePrintProvidorW(
    LPWSTR   pName,
    LPWSTR   pEnvironment,
    LPWSTR   pPrintProvidorName
);

#define AddForm AddFormW
#define AddJob AddJobW
#define AddMonitor AddMonitorW
#define AddPort AddPortW
#define AddPrintProcessor AddPrintProcessorW
#define AddPrintProvidor AddPrintProvidorW
#define AddPrinter AddPrinterW
#define AddPrinterConnection AddPrinterConnectionW
#define AddPrinterDriver AddPrinterDriverW
#define AddPrinterDriverEx AddPrinterDriverExW
#define AdvancedDocumentProperties AdvancedDocumentPropertiesW
#define ConfigurePort ConfigurePortW
#define DeleteForm DeleteFormW
#define DeleteMonitor DeleteMonitorW
#define DeletePort DeletePortW
#define DeletePrintProcessor DeletePrintProcessorW
#define DeletePrintProvidor DeletePrintProvidorW
#define DeletePrinterConnection DeletePrinterConnectionW
#define DeletePrinterData DeletePrinterDataW
#define DeletePrinterDataEx DeletePrinterDataExW
#define DeletePrinterDriver DeletePrinterDriverW
#define DeletePrinterDriverEx DeletePrinterDriverExW
#define DeletePrinterKey DeletePrinterKeyW
#define DocumentProperties DocumentPropertiesW
#define EnumForms EnumFormsW
#define EnumJobs EnumJobsW
#define EnumMonitors EnumMonitorsW
#define EnumPorts EnumPortsW
#define EnumPrintProcessorDatatypes EnumPrintProcessorDatatypesW
#define EnumPrintProcessors EnumPrintProcessorsW
#define EnumPrinterData EnumPrinterDataW
#define EnumPrinterDataEx EnumPrinterDataExW
#define EnumPrinterDrivers EnumPrinterDriversW
#define EnumPrinterKey EnumPrinterKeyW
#define EnumPrinters EnumPrintersW
#define GetDefaultPrinter GetDefaultPrinterW
#define GetForm GetFormW
#define GetJob GetJobW
#define GetPrintProcessorDirectory GetPrintProcessorDirectoryW
#define GetPrinter GetPrinterW
#define GetPrinterData GetPrinterDataW
#define GetPrinterDataEx GetPrinterDataExW
#define GetPrinterDriver GetPrinterDriverW
#define GetPrinterDriverDirectory GetPrinterDriverDirectoryW
#define OpenPrinter OpenPrinterW
#define PrinterMessageBox PrinterMessageBoxW
#define ResetPrinter ResetPrinterW
#define SetDefaultPrinter SetDefaultPrinterW
#define SetForm SetFormW
#define SetJob SetJobW
#define SetPort SetPortW
#define SetPrinter SetPrinterW
#define SetPrinterData SetPrinterDataW
#define SetPrinterDataEx SetPrinterDataExW
#define StartDocPrinter StartDocPrinterW

#else
BOOL PASCAL WINBASEAPI EnumPrintersA(
    DWORD   Flags,
    LPSTR Name,
    DWORD   Level,
    LPBYTE  pPrinterEnum,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI OpenPrinterA(
   LPSTR    pPrinterName,
   LPHANDLE phPrinter,
   LPPRINTER_DEFAULTS pDefault
);
BOOL PASCAL WINBASEAPI ResetPrinterA(
   HANDLE   hPrinter,
   LPPRINTER_DEFAULTS pDefault
);
BOOL PASCAL WINBASEAPI SetJobA(
    HANDLE  hPrinter,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   Command
);
BOOL PASCAL WINBASEAPI GetJobA(
   HANDLE   hPrinter,
   DWORD    JobId,
   DWORD    Level,
   LPBYTE   pJob,
   DWORD    cbBuf,
   LPDWORD  pcbNeeded
);
BOOL PASCAL WINBASEAPI EnumJobsA(
    HANDLE  hPrinter,
    DWORD   FirstJob,
    DWORD   NoJobs,
    DWORD   Level,
    LPBYTE  pJob,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
HANDLE PASCAL WINBASEAPI AddPrinterA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pPrinter
);
BOOL PASCAL WINBASEAPI SetPrinterA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   Command
);
BOOL PASCAL WINBASEAPI GetPrinterA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pPrinter,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI AddPrinterDriverA(
    LPSTR   pName,
     DWORD   Level,
    LPBYTE  pDriverInfo
);
BOOL PASCAL WINBASEAPI AddPrinterDriverExA(
    LPSTR   pName,
    DWORD     Level,
    LPBYTE pDriverInfo,
    DWORD     dwFileCopyFlags
);
BOOL PASCAL WINBASEAPI EnumPrinterDriversA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI GetPrinterDriverA(
    HANDLE  hPrinter,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI GetPrinterDriverDirectoryA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pDriverDirectory,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI DeletePrinterDriverA(
   LPSTR    pName,
   LPSTR    pEnvironment,
   LPSTR    pDriverName
);
BOOL PASCAL WINBASEAPI DeletePrinterDriverExA(
   LPSTR    pName,
   LPSTR    pEnvironment,
   LPSTR    pDriverName,
   DWORD      dwDeleteFlag,
   DWORD      dwVersionFlag
);
BOOL PASCAL WINBASEAPI AddPrintProcessorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pPathName,
    LPSTR   pPrintProcessorName
);
BOOL PASCAL WINBASEAPI EnumPrintProcessorsA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI GetPrintProcessorDirectoryA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    DWORD   Level,
    LPBYTE  pPrintProcessorInfo,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI EnumPrintProcessorDatatypesA(
    LPSTR   pName,
    LPSTR   pPrintProcessorName,
    DWORD   Level,
    LPBYTE  pDatatypes,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI DeletePrintProcessorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pPrintProcessorName
);
DWORD PASCAL WINBASEAPI StartDocPrinterA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pDocInfo
);
BOOL PASCAL WINBASEAPI AddJobA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pData,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
LONG PASCAL WINBASEAPI DocumentPropertiesA(
    HWND      hWnd,
    HANDLE    hPrinter,
    LPSTR   pDeviceName,
    PDEVMODE pDevModeOutput,
    PDEVMODE pDevModeInput,
    DWORD     fMode
);
LONG PASCAL WINBASEAPI AdvancedDocumentPropertiesA(
    HWND    hWnd,
    HANDLE  hPrinter,
    LPSTR   pDeviceName,
    PDEVMODE pDevModeOutput,
    PDEVMODE pDevModeInput
);
DWORD PASCAL WINBASEAPI GetPrinterDataA(
    HANDLE   hPrinter,
    LPSTR  pValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
);
DWORD PASCAL WINBASEAPI GetPrinterDataExA(
    HANDLE   hPrinter,
    LPCSTR pKeyName,
    LPCSTR pValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
);
DWORD PASCAL WINBASEAPI EnumPrinterDataA(
    HANDLE   hPrinter,
    DWORD    dwIndex,
    LPSTR  pValueName,
    DWORD    cbValueName,
    LPDWORD  pcbValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    cbData,
    LPDWORD  pcbData
);
DWORD PASCAL WINBASEAPI EnumPrinterDataExA(
    HANDLE   hPrinter,
    LPCSTR pKeyName,
    LPBYTE   pEnumValues,
    DWORD    cbEnumValues,
    LPDWORD  pcbEnumValues,
    LPDWORD  pnEnumValues
);
DWORD PASCAL WINBASEAPI EnumPrinterKeyA(
    HANDLE   hPrinter,
    LPCSTR pKeyName,
    LPSTR  pSubkey,
    DWORD    cbSubkey,
    LPDWORD  pcbSubkey
);
DWORD PASCAL WINBASEAPI SetPrinterDataA(
    HANDLE  hPrinter,
    LPSTR pValueName,
    DWORD   Type,
    LPBYTE  pData,
    DWORD   cbData
);
DWORD PASCAL WINBASEAPI SetPrinterDataExA(
    HANDLE   hPrinter,
    LPCSTR pKeyName,
    LPCSTR pValueName,
    DWORD    Type,
    LPBYTE   pData,
    DWORD    cbData
);
DWORD PASCAL WINBASEAPI DeletePrinterDataA(
    HANDLE  hPrinter,
    LPSTR pValueName
);
DWORD PASCAL WINBASEAPI DeletePrinterDataExA(
    HANDLE   hPrinter,
    LPCSTR pKeyName,
    LPCSTR pValueName
);
DWORD PASCAL WINBASEAPI DeletePrinterKeyA(
    HANDLE   hPrinter,
    LPCSTR pKeyName
);
DWORD PASCAL WINBASEAPI PrinterMessageBoxA(
    HANDLE  hPrinter,
    DWORD   Error,
    HWND    hWnd,
    LPSTR   pText,
    LPSTR   pCaption,
    DWORD   dwType
);
BOOL PASCAL WINBASEAPI AddFormA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm
);
BOOL PASCAL WINBASEAPI DeleteFormA(
    HANDLE  hPrinter,
    LPSTR   pFormName
);
BOOL PASCAL WINBASEAPI GetFormA(
    HANDLE  hPrinter,
    LPSTR   pFormName,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
);
BOOL PASCAL WINBASEAPI SetFormA(
    HANDLE  hPrinter,
    LPSTR   pFormName,
    DWORD   Level,
    LPBYTE  pForm
);
BOOL PASCAL WINBASEAPI EnumFormsA(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI EnumMonitorsA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pMonitors,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI AddMonitorA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pMonitors
);
BOOL PASCAL WINBASEAPI DeleteMonitorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pMonitorName
);
BOOL PASCAL WINBASEAPI EnumPortsA(
    LPSTR   pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
);
BOOL PASCAL WINBASEAPI AddPortA(
    LPSTR   pName,
    HWND    hWnd,
    LPSTR   pMonitorName
);
BOOL PASCAL WINBASEAPI ConfigurePortA(
    LPSTR   pName,
    HWND    hWnd,
    LPSTR   pPortName
);
BOOL PASCAL WINBASEAPI DeletePortA(
    LPSTR pName,
    HWND    hWnd,
    LPSTR pPortName
);
BOOL PASCAL WINBASEAPI GetDefaultPrinterA(
    LPSTR   pszBuffer,
    LPDWORD  pcchBuffer
    );
BOOL PASCAL WINBASEAPI SetDefaultPrinterA(
    LPCSTR pszPrinter
    );
BOOL PASCAL WINBASEAPI SetPortA(
    LPSTR     pName,
    LPSTR     pPortName,
    DWORD       dwLevel,
    LPBYTE      pPortInfo
);
BOOL PASCAL WINBASEAPI AddPrinterConnectionA(
    LPSTR   pName
);
BOOL PASCAL WINBASEAPI DeletePrinterConnectionA(
    LPSTR   pName
);
BOOL PASCAL WINBASEAPI AddPrintProvidorA(
    LPSTR  pName,
    DWORD    level,
    LPBYTE   pProvidorInfo
);
BOOL PASCAL WINBASEAPI DeletePrintProvidorA(
    LPSTR   pName,
    LPSTR   pEnvironment,
    LPSTR   pPrintProvidorName
);
#define AddForm AddFormA
#define AddJob AddJobA
#define AddMonitor AddMonitorA
#define AddPort AddPortA
#define AddPrintProcessor AddPrintProcessorA
#define AddPrintProvidor AddPrintProvidorA
#define AddPrinter AddPrinterA
#define AddPrinterConnection AddPrinterConnectionA
#define AddPrinterDriver AddPrinterDriverA
#define AddPrinterDriverEx AddPrinterDriverExA
#define AdvancedDocumentProperties AdvancedDocumentPropertiesA
#define ConfigurePort ConfigurePortA
#define DeleteForm DeleteFormA
#define DeleteMonitor DeleteMonitorA
#define DeletePort DeletePortA
#define DeletePrintProcessor DeletePrintProcessorA
#define DeletePrintProvidor DeletePrintProvidorA
#define DeletePrinterConnection DeletePrinterConnectionA
#define DeletePrinterData DeletePrinterDataA
#define DeletePrinterDataEx DeletePrinterDataExA
#define DeletePrinterDriver DeletePrinterDriverA
#define DeletePrinterDriverEx DeletePrinterDriverExA
#define DeletePrinterKey DeletePrinterKeyA
#define DocumentProperties DocumentPropertiesA
#define EnumForms EnumFormsA
#define EnumJobs EnumJobsA
#define EnumMonitors EnumMonitorsA
#define EnumPorts EnumPortsA
#define EnumPrintProcessorDatatypes EnumPrintProcessorDatatypesA
#define EnumPrintProcessors EnumPrintProcessorsA
#define EnumPrinterData EnumPrinterDataA
#define EnumPrinterDataEx EnumPrinterDataExA
#define EnumPrinterDrivers EnumPrinterDriversA
#define EnumPrinterKey EnumPrinterKeyA
#define EnumPrinters EnumPrintersA
#define GetDefaultPrinter GetDefaultPrinterA
#define GetForm GetFormA
#define GetJob GetJobA
#define GetPrintProcessorDirectory GetPrintProcessorDirectoryA
#define GetPrinter GetPrinterA
#define GetPrinterData GetPrinterDataA
#define GetPrinterDataEx GetPrinterDataExA
#define GetPrinterDriver GetPrinterDriverA
#define GetPrinterDriverDirectory GetPrinterDriverDirectoryA
#define OpenPrinter OpenPrinterA
#define PrinterMessageBox PrinterMessageBoxA
#define ResetPrinter ResetPrinterA
#define SetDefaultPrinter SetDefaultPrinterA
#define SetForm SetFormA
#define SetJob SetJobA
#define SetPort SetPortA
#define SetPrinter SetPrinterA
#define SetPrinterData SetPrinterDataA
#define SetPrinterDataEx SetPrinterDataExA
#define StartDocPrinter StartDocPrinterA

#define XcvData XcvDataW
#endif

#ifdef __cplusplus
}
#endif
