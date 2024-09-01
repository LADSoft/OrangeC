#ifndef _WINCON_H
#define _WINCON_H

/* Windows Console subsystem definitions */

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef NOGDI
#include <wingdi.h>
#endif

typedef struct _COORD {
    SHORT X;
    SHORT Y;
} COORD, *PCOORD;

typedef struct _SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
} SMALL_RECT, *PSMALL_RECT;

typedef struct _KEY_EVENT_RECORD {
    BOOL bKeyDown;
    WORD wRepeatCount;
    WORD wVirtualKeyCode;
    WORD wVirtualScanCode;
    union {
        WCHAR UnicodeChar;
        CHAR AsciiChar;
    } uChar;
    DWORD dwControlKeyState;
} KEY_EVENT_RECORD, *PKEY_EVENT_RECORD;

#define RIGHT_ALT_PRESSED  0x0001
#define LEFT_ALT_PRESSED  0x0002
#define RIGHT_CTRL_PRESSED  0x0004
#define LEFT_CTRL_PRESSED  0x0008
#define SHIFT_PRESSED  0x0010
#define NUMLOCK_ON  0x0020
#define SCROLLLOCK_ON  0x0040
#define CAPSLOCK_ON  0x0080
#define ENHANCED_KEY  0x0100
#define NLS_DBCSCHAR  0x00010000
#define NLS_ALPHANUMERIC  0x00000000
#define NLS_KATAKANA  0x00020000
#define NLS_HIRAGANA  0x00040000
#define NLS_ROMAN  0x00400000
#define NLS_IME_CONVERSION  0x00800000
#define NLS_IME_DISABLE  0x20000000

typedef struct _MOUSE_EVENT_RECORD {
    COORD dwMousePosition;
    DWORD dwButtonState;
    DWORD dwControlKeyState;
    DWORD dwEventFlags;
} MOUSE_EVENT_RECORD, *PMOUSE_EVENT_RECORD;

#define FROM_LEFT_1ST_BUTTON_PRESSED  0x0001
#define RIGHTMOST_BUTTON_PRESSED  0x0002
#define FROM_LEFT_2ND_BUTTON_PRESSED  0x0004
#define FROM_LEFT_3RD_BUTTON_PRESSED  0x0008
#define FROM_LEFT_4TH_BUTTON_PRESSED  0x0010

#define MOUSE_MOVED  0x0001
#define DOUBLE_CLICK  0x0002
#define MOUSE_WHEELED 0x0004
#if(_WIN32_WINNT >= 0x0600)
#define MOUSE_HWHEELED 0x0008
#endif /* _WIN32_WINNT >= 0x0600 */

typedef struct _WINDOW_BUFFER_SIZE_RECORD {
    COORD dwSize;
} WINDOW_BUFFER_SIZE_RECORD, *PWINDOW_BUFFER_SIZE_RECORD;

typedef struct _MENU_EVENT_RECORD {
    UINT dwCommandId;
} MENU_EVENT_RECORD, *PMENU_EVENT_RECORD;

typedef struct _FOCUS_EVENT_RECORD {
    BOOL bSetFocus;
} FOCUS_EVENT_RECORD, *PFOCUS_EVENT_RECORD;

typedef struct _INPUT_RECORD {
    WORD EventType;
    union {
        KEY_EVENT_RECORD KeyEvent;
        MOUSE_EVENT_RECORD MouseEvent;
        WINDOW_BUFFER_SIZE_RECORD WindowBufferSizeEvent;
        MENU_EVENT_RECORD MenuEvent;
        FOCUS_EVENT_RECORD FocusEvent;
    } Event;
} INPUT_RECORD, *PINPUT_RECORD;

#define KEY_EVENT  0x0001
#define MOUSE_EVENT  0x0002
#define WINDOW_BUFFER_SIZE_EVENT  0x0004
#define MENU_EVENT  0x0008
#define FOCUS_EVENT  0x0010

typedef struct _CHAR_INFO {
    union {
        WCHAR UnicodeChar;
        CHAR AsciiChar;
    } Char;
    WORD Attributes;
} CHAR_INFO, *PCHAR_INFO;

#define FOREGROUND_BLUE  0x0001
#define FOREGROUND_GREEN  0x0002
#define FOREGROUND_RED  0x0004
#define FOREGROUND_INTENSITY  0x0008
#define BACKGROUND_BLUE  0x0010
#define BACKGROUND_GREEN  0x0020
#define BACKGROUND_RED  0x0040
#define BACKGROUND_INTENSITY  0x0080
#define COMMON_LVB_LEADING_BYTE  0x0100
#define COMMON_LVB_TRAILING_BYTE  0x0200
#define COMMON_LVB_GRID_HORIZONTAL 0x0400
#define COMMON_LVB_GRID_LVERTICAL  0x0800
#define COMMON_LVB_GRID_RVERTICAL  0x1000
#define COMMON_LVB_REVERSE_VIDEO  0x4000
#define COMMON_LVB_UNDERSCORE  0x8000

#define COMMON_LVB_SBCSDBCS  0x0300

typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
    COORD dwSize;
    COORD dwCursorPosition;
    WORD wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO, *PCONSOLE_SCREEN_BUFFER_INFO;

typedef struct _CONSOLE_SCREEN_BUFFER_INFOEX {
    ULONG cbSize;
    COORD dwSize;
    COORD dwCursorPosition;
    WORD wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
    WORD wPopupAttributes;
    BOOL bFullscreenSupported;
    COLORREF ColorTable[16];
} CONSOLE_SCREEN_BUFFER_INFOEX, *PCONSOLE_SCREEN_BUFFER_INFOEX;

typedef struct _CONSOLE_CURSOR_INFO {
    DWORD dwSize;
    BOOL bVisible;
} CONSOLE_CURSOR_INFO, *PCONSOLE_CURSOR_INFO;

typedef struct _CONSOLE_FONT_INFO {
    DWORD nFont;
    COORD dwFontSize;
} CONSOLE_FONT_INFO, *PCONSOLE_FONT_INFO;

#ifndef NOGDI
typedef struct _CONSOLE_FONT_INFOEX {
    ULONG cbSize;
    DWORD nFont;
    COORD dwFontSize;
    UINT FontFamily;
    UINT FontWeight;
    WCHAR FaceName[LF_FACESIZE];
} CONSOLE_FONT_INFOEX, *PCONSOLE_FONT_INFOEX;
#endif /* NOGDI */

#define HISTORY_NO_DUP_FLAG  0x1

typedef struct _CONSOLE_HISTORY_INFO {
    UINT cbSize;
    UINT HistoryBufferSize;
    UINT NumberOfHistoryBuffers;
    DWORD dwFlags;
} CONSOLE_HISTORY_INFO, *PCONSOLE_HISTORY_INFO;

#if(_WIN32_WINNT >= 0x0500)

typedef struct _CONSOLE_SELECTION_INFO {
    DWORD dwFlags;
    COORD dwSelectionAnchor;
    SMALL_RECT srSelection;
} CONSOLE_SELECTION_INFO, *PCONSOLE_SELECTION_INFO;

#define CONSOLE_NO_SELECTION  0x0000
#define CONSOLE_SELECTION_IN_PROGRESS  0x0001
#define CONSOLE_SELECTION_NOT_EMPTY  0x0002
#define CONSOLE_MOUSE_SELECTION  0x0004
#define CONSOLE_MOUSE_DOWN  0x0008
#endif /* _WIN32_WINNT >= 0x0500 */

typedef BOOL (WINAPI *PHANDLER_ROUTINE)(DWORD);

#define CTRL_C_EVENT  0
#define CTRL_BREAK_EVENT  1
#define CTRL_CLOSE_EVENT  2
#define CTRL_LOGOFF_EVENT  5
#define CTRL_SHUTDOWN_EVENT 6

#define ENABLE_PROCESSED_INPUT  0x0001
#define ENABLE_LINE_INPUT  0x0002
#define ENABLE_ECHO_INPUT  0x0004
#define ENABLE_WINDOW_INPUT  0x0008
#define ENABLE_MOUSE_INPUT  0x0010
#define ENABLE_INSERT_MODE  0x0020
#define ENABLE_QUICK_EDIT_MODE  0x0040
#define ENABLE_EXTENDED_FLAGS  0x0080
#define ENABLE_AUTO_POSITION  0x0100
#define ENABLE_VIRTUAL_TERMINAL_INPUT  0x0200

#define ENABLE_PROCESSED_OUTPUT  0x0001
#define ENABLE_WRAP_AT_EOL_OUTPUT  0x0002
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#define DISABLE_NEWLINE_AUTO_RETURN  0x0008
#define ENABLE_LVB_GRID_WORLDWIDE  0x0010

#define CONSOLE_REAL_OUTPUT_HANDLE  (LongToHandle(-2))
#define CONSOLE_REAL_INPUT_HANDLE  (LongToHandle(-3))

WINBASEAPI BOOL WINAPI PeekConsoleInputW(HANDLE, PINPUT_RECORD, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI WriteConsoleInputA(HANDLE, CONST INPUT_RECORD *, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI WriteConsoleInputW(HANDLE, CONST INPUT_RECORD *, DWORD, LPDWORD);
WINBASEAPI BOOL WINAPI ReadConsoleOutputA(HANDLE, PCHAR_INFO, COORD, COORD, PSMALL_RECT);
WINBASEAPI BOOL WINAPI ReadConsoleOutputW(HANDLE, PCHAR_INFO, COORD, COORD, PSMALL_RECT);
WINBASEAPI BOOL WINAPI WriteConsoleOutputA(HANDLE, CONST CHAR_INFO *, COORD, COORD, PSMALL_RECT);
WINBASEAPI BOOL WINAPI WriteConsoleOutputW(HANDLE, CONST CHAR_INFO *, COORD, COORD, PSMALL_RECT);
WINBASEAPI BOOL WINAPI ReadConsoleOutputCharacterA(HANDLE, LPSTR, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI ReadConsoleOutputCharacterW(HANDLE, LPWSTR, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI ReadConsoleOutputAttribute(HANDLE, LPWORD, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI WriteConsoleOutputCharacterA(HANDLE, LPCSTR, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI WriteConsoleOutputCharacterW(HANDLE, LPCWSTR, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI WriteConsoleOutputAttribute(HANDLE, CONST WORD *, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI FillConsoleOutputCharacterA(HANDLE, CHAR, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI FillConsoleOutputCharacterW(HANDLE, WCHAR, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI FillConsoleOutputAttribute(HANDLE, WORD, DWORD, COORD, LPDWORD);
WINBASEAPI BOOL WINAPI GetConsoleScreenBufferInfo(HANDLE, PCONSOLE_SCREEN_BUFFER_INFO);
WINBASEAPI BOOL WINAPI GetConsoleScreenBufferInfoEx(HANDLE, PCONSOLE_SCREEN_BUFFER_INFOEX);
WINBASEAPI BOOL WINAPI SetConsoleScreenBufferInfoEx(HANDLE, PCONSOLE_SCREEN_BUFFER_INFOEX);
WINBASEAPI COORD WINAPI GetLargestConsoleWindowSize(HANDLE);
WINBASEAPI BOOL WINAPI GetConsoleCursorInfo(HANDLE, PCONSOLE_CURSOR_INFO);

#ifdef UNICODE
#define PeekConsoleInput  PeekConsoleInputW
#define WriteConsoleInput  WriteConsoleInputW
#define ReadConsoleOutput  ReadConsoleOutputW
#define WriteConsoleOutput  WriteConsoleOutputW
#define ReadConsoleOutputCharacter  ReadConsoleOutputCharacterW
#define WriteConsoleOutputCharacter  WriteConsoleOutputCharacterW
#define FillConsoleOutputCharacter  FillConsoleOutputCharacterW
#else /* !UNICODE */
#define WriteConsoleInput  WriteConsoleInputA
#define ReadConsoleOutput  ReadConsoleOutputA
#define WriteConsoleOutput  WriteConsoleOutputA
#define ReadConsoleOutputCharacter  ReadConsoleOutputCharacterA
#define WriteConsoleOutputCharacter  WriteConsoleOutputCharacterA
#define FillConsoleOutputCharacter  FillConsoleOutputCharacterA
#endif /* !UNICODE */

#if(_WIN32_WINNT >= 0x0500)
WINBASEAPI BOOL WINAPI GetCurrentConsoleFont(HANDLE, BOOL, PCONSOLE_FONT_INFO);
#ifndef NOGDI
WINBASEAPI BOOL WINAPI GetCurrentConsoleFontEx(HANDLE, BOOL, PCONSOLE_FONT_INFOEX);
WINBASEAPI BOOL WINAPI SetCurrentConsoleFontEx(HANDLE, BOOL, PCONSOLE_FONT_INFOEX);
#endif /* NOGDI */
WINBASEAPI BOOL WINAPI GetConsoleHistoryInfo(PCONSOLE_HISTORY_INFO);
WINBASEAPI BOOL WINAPI SetConsoleHistoryInfo(PCONSOLE_HISTORY_INFO);
WINBASEAPI COORD WINAPI GetConsoleFontSize(HANDLE, DWORD);
WINBASEAPI BOOL WINAPI GetConsoleSelectionInfo(PCONSOLE_SELECTION_INFO);
#endif /* _WIN32_WINNT >= 0x0500 */

WINBASEAPI BOOL WINAPI GetNumberOfConsoleMouseButtons(LPDWORD);
WINBASEAPI BOOL WINAPI SetConsoleActiveScreenBuffer(HANDLE);
WINBASEAPI BOOL WINAPI FlushConsoleInputBuffer(HANDLE);
WINBASEAPI BOOL WINAPI SetConsoleScreenBufferSize(HANDLE, COORD);
WINBASEAPI BOOL WINAPI SetConsoleCursorPosition(HANDLE, COORD);
WINBASEAPI BOOL WINAPI SetConsoleCursorInfo(HANDLE, CONST CONSOLE_CURSOR_INFO *);
WINBASEAPI BOOL WINAPI ScrollConsoleScreenBufferA(HANDLE, CONST SMALL_RECT *, CONST SMALL_RECT *, COORD, CONST CHAR_INFO *);
WINBASEAPI BOOL WINAPI ScrollConsoleScreenBufferW(HANDLE, CONST SMALL_RECT *, CONST SMALL_RECT *, COORD, CONST CHAR_INFO *);

#ifdef UNICODE
#define ScrollConsoleScreenBuffer  ScrollConsoleScreenBufferW
#else /* !UNICODE */
#define ScrollConsoleScreenBuffer  ScrollConsoleScreenBufferA
#endif /* !UNICODE */

WINBASEAPI BOOL WINAPI SetConsoleWindowInfo(HANDLE, BOOL, CONST SMALL_RECT *);
WINBASEAPI BOOL WINAPI SetConsoleTextAttribute(HANDLE, WORD);
WINBASEAPI BOOL WINAPI GenerateConsoleCtrlEvent(DWORD, DWORD);
WINBASEAPI BOOL WINAPI FreeConsole(VOID);

#if(_WIN32_WINNT >= 0x0500)
WINBASEAPI BOOL WINAPI AttachConsole(DWORD);
#define ATTACH_PARENT_PROCESS  ((DWORD)-1)
#endif /* _WIN32_WINNT >= 0x0500 */

WINBASEAPI DWORD WINAPI GetConsoleTitleA(LPSTR, DWORD);
WINBASEAPI DWORD WINAPI GetConsoleTitleW(LPWSTR, DWORD);

#ifdef UNICODE
#define GetConsoleTitle  GetConsoleTitleW
#else /* !UNICODE */
#define GetConsoleTitle  GetConsoleTitleA
#endif /* !UNICODE */

#if(_WIN32_WINNT >= 0x0600)
WINBASEAPI DWORD WINAPI GetConsoleOriginalTitleA(LPSTR, DWORD);
WINBASEAPI DWORD WINAPI GetConsoleOriginalTitleW(LPWSTR, DWORD);
#ifdef UNICODE
#define GetConsoleOriginalTitle  GetConsoleOriginalTitleW
#else /* !UNICODE */
#define GetConsoleOriginalTitle  GetConsoleOriginalTitleA
#endif /* !UNICODE */
#endif /* _WIN32_WINNT >= 0x0600 */

WINBASEAPI BOOL WINAPI SetConsoleTitleA(LPCSTR);
WINBASEAPI BOOL WINAPI SetConsoleTitleW(LPCWSTR);

#ifdef UNICODE
#define SetConsoleTitle  SetConsoleTitleW
#else /* !UNICODE */
#define SetConsoleTitle  SetConsoleTitleA
#endif /* !UNICODE */

typedef struct _CONSOLE_READCONSOLE_CONTROL {
    ULONG nLength;
    ULONG nInitialChars;
    ULONG dwCtrlWakeupMask;
    ULONG dwControlKeyState;
} CONSOLE_READCONSOLE_CONTROL, *PCONSOLE_READCONSOLE_CONTROL;

#define CONSOLE_TEXTMODE_BUFFER  1

#ifndef NOAPISET
#include <consoleapi.h>
#endif /* !NOAPISET */

WINBASEAPI HANDLE WINAPI CreateConsoleScreenBuffer(DWORD, DWORD, CONST SECURITY_ATTRIBUTES *, DWORD, LPVOID);
WINBASEAPI BOOL WINAPI SetConsoleCP(UINT);
WINBASEAPI BOOL WINAPI SetConsoleOutputCP(UINT);

#if(_WIN32_WINNT >= 0x0500)

#define CONSOLE_FULLSCREEN  1
#define CONSOLE_FULLSCREEN_HARDWARE  2

WINBASEAPI BOOL APIENTRY GetConsoleDisplayMode(LPDWORD);

#define CONSOLE_FULLSCREEN_MODE  1
#define CONSOLE_WINDOWED_MODE  2

BOOL APIENTRY SetConsoleDisplayMode(HANDLE, DWORD, PCOORD);
WINBASEAPI HWND APIENTRY GetConsoleWindow(VOID);

#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)

WINBASEAPI DWORD APIENTRY GetConsoleProcessList(LPDWORD, DWORD);
WINBASEAPI BOOL APIENTRY AddConsoleAliasA(LPSTR, LPSTR, LPSTR);
WINBASEAPI BOOL APIENTRY AddConsoleAliasW(LPWSTR, LPWSTR, LPWSTR);
WINBASEAPI DWORD APIENTRY GetConsoleAliasA(LPSTR, LPSTR, DWORD, LPSTR);
WINBASEAPI DWORD APIENTRY GetConsoleAliasW(LPWSTR, LPWSTR, DWORD, LPWSTR);
WINBASEAPI DWORD APIENTRY GetConsoleAliasesLengthA(LPSTR);
WINBASEAPI DWORD APIENTRY GetConsoleAliasesLengthW(LPWSTR);
WINBASEAPI DWORD APIENTRY GetConsoleAliasExesLengthA(VOID);
WINBASEAPI DWORD APIENTRY GetConsoleAliasExesLengthW(VOID);
WINBASEAPI DWORD APIENTRY GetConsoleAliasesA(LPSTR, DWORD, LPSTR);
WINBASEAPI DWORD APIENTRY GetConsoleAliasesW(LPWSTR, DWORD, LPWSTR);
WINBASEAPI DWORD APIENTRY GetConsoleAliasExesA(LPSTR, DWORD);
WINBASEAPI DWORD APIENTRY GetConsoleAliasExesW(LPWSTR, DWORD);

#ifdef UNICODE
#define AddConsoleAlias  AddConsoleAliasW
#define GetConsoleAlias  GetConsoleAliasW
#define GetConsoleAliasesLength  GetConsoleAliasesLengthW
#define GetConsoleAliasExesLength  GetConsoleAliasExesLengthW
#define GetConsoleAliases  GetConsoleAliasesW
#define GetConsoleAliasExes  GetConsoleAliasExesW
#else /* !UNICODE */
#define AddConsoleAlias  AddConsoleAliasA
#define GetConsoleAlias  GetConsoleAliasA
#define GetConsoleAliasesLength  GetConsoleAliasesLengthA
#define GetConsoleAliasExesLength  GetConsoleAliasExesLengthA
#define GetConsoleAliases  GetConsoleAliasesA
#define GetConsoleAliasExes  GetConsoleAliasExesA
#endif /* !UNICODE */

#endif /* _WIN32_WINNT >= 0x0501 */

#endif /* _WINCON_H */
