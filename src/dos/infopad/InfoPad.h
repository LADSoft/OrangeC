#define Uses_TApplication

#include "tv.h"

class DWINFO;
class TFileDialog;

class InfoPad :public TApplication
{
public:
    InfoPad();
    ~InfoPad();

    DWINFO *openfile(DWINFO *newInfo, int newwindow, int visible);
    DWINFO *CreateDrawWindow(DWINFO *baseinfo, int visible);

    DWINFO *current;
    virtual DWORD WinMessage(DWORD iMessage, DWORD wParam, DWORD lParam);
protected:
    virtual void idle();
    virtual void handleEvent(TEvent& event);
    void AboutBox();
    void WindowMenuBox();
    static TStatusLine *initStatusLine( TRect );
    static TMenuBar *initMenuBar( TRect );
    static TDeskTop *initDeskTop( TRect );

private:
    TFileDialog *fileDlg;
};

#define ID_BASE 1000
#define ID_NEW (ID_BASE + 0)
#define ID_OPEN (ID_BASE + 1)
#define ID_SAVE (ID_BASE + 2)
#define ID_SAVEAS (ID_BASE + 3)
#define ID_CLOSE (ID_BASE + 4)
#define ID_PRINT (ID_BASE + 5)
#define ID_PRINTSETUP (ID_BASE + 6)
#define ID_DOS (ID_BASE + 7)
#define ID_EXIT (ID_BASE + 8)
#define ID_REOPEN (ID_BASE + 9)

#define ID_UNDO (ID_BASE + 20)
#define ID_CUT (ID_BASE + 21)
#define ID_COPY (ID_BASE + 22)
#define ID_PASTE (ID_BASE + 23)
#define ID_TOUPPER (ID_BASE + 24)
#define ID_TOLOWER (ID_BASE + 25)
#define ID_INDENT (ID_BASE + 26)
#define ID_OUTDENT (ID_BASE + 27)
#define ID_COMMENT (ID_BASE + 28)
#define ID_UNCOMMENT (ID_BASE + 29)

#define ID_SEARCH (ID_BASE + 30)
#define ID_REPLACE (ID_BASE + 31)
#define ID_SEARCHNEXT (ID_BASE + 32)
#define ID_GOTO (ID_BASE + 33)

#define ID_OPTIONS (ID_BASE + 40)

#define ID_CLOSEALL (ID_BASE + 50)
#define ID_WINDOW (ID_BASE +51)
#define ID_MOREWINDOWS (ID_BASE + 52)

#define ID_AUTOINDENT (ID_BASE + 80)
#define ID_AUTOFORMAT (ID_BASE + 80)

#define ID_ABOUT (ID_BASE + 60)

#define FILE_MRU_BASE 400
#define WINDOW_MRU_BASE 500
