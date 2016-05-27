#include "Resource.h"
#include  <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include "xml.h"

char *szAppName   =   "ListView";
char *stitle       =  "List View";
char szSourceFilter[] = 
            "XML files (*.xml)\0*.xml\0"
            "All Files (*.*)\0*.*\0" ;
   TBBUTTON tbrButtons[3];

HINSTANCE hInstance ;
HBITMAP hbmpNode, hbmpAttrib ;
HIMAGELIST lvIml ;
int ilNode, ilAttrib ;
int lvItems ;
/* Centers a window.  Yes we can do this with dialog styles, this is an example tho... */
VOID WINAPI CenterWindow(HWND hWnd)
{
    RECT rect;
    WORD wWidth,
         wHeight;
         
    // find out how big we are
    GetWindowRect(hWnd,&rect);

    // get screen size
    wWidth =GetSystemMetrics(SM_CXSCREEN);
    wHeight=GetSystemMetrics(SM_CYSCREEN);

    // move to center
    MoveWindow(hWnd,(wWidth/2)   - ((rect.right -  rect.left)/2),
                    (wHeight/2)  - ((rect.bottom - rect.top) /2),
                     rect.right  -   rect.left,
                     rect.bottom -   rect.top, 
                     FALSE);
}

/* Handle the about box */
int PASCAL AboutBoxProc (HWND hWnd, int wmsg, int wParam, int lParam)
{
    switch(wmsg) {
        case WM_INITDIALOG:
            CenterWindow(hWnd) ;
            return 0;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                    EndDialog(hWnd,1) ;
                    break ;
            } 
    }
    return 0 ;
}
/* Put upt an open file dialog to get the name of the XML file to display */
int GetFileName(HWND handle, char *buf) 
{
    OPENFILENAME ofn ;
    char title[260] ;
    char dir[260] ;
    GetCurrentDirectory(sizeof(dir)-2,dir) ;
    memset(&ofn,0,sizeof(ofn)) ;
    ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400 ;
    ofn.hwndOwner = handle ;
    ofn.hInstance = hInstance ;
    ofn.lpstrFilter = szSourceFilter ;
    ofn.nFilterIndex = 1 ;
    ofn.lpstrFile = buf ;
    ofn.nMaxFile = 260 ;
    ofn.lpstrFileTitle = title ;
    ofn.nMaxFileTitle = 260 ;
    ofn.lpstrInitialDir = dir ;
    ofn.Flags = OFN_EXPLORER ;
    return GetOpenFileName(&ofn) ;
}
void PopulateAttribs(HWND hwndListView, struct xmlAttr *attribs, int level) 
{
    while (attribs) {
        char buf[512] ;
        LV_ITEM t ;
        sprintf(buf,"%d",level) ;
        memset(&t,0,sizeof(t)) ;
         t.iItem = lvItems++ ;
         t.iSubItem = 0 ;
         t.mask = LVIF_TEXT  | LVIF_IMAGE;
         t.pszText = buf ; 
         t.cchTextMax = strlen(buf) ;
         t.iImage = ilAttrib ;
         ListView_InsertItem(hwndListView, &t) ;
        sprintf(buf,"%s=\"%s\"",attribs->name,attribs->value) ;
        attribs->userdata = strdup(buf) ;
         t.iSubItem = 2 ;
         t.mask = LVIF_TEXT ;
         t.pszText = attribs->userdata ;
         t.cchTextMax = strlen(attribs->userdata) ;
         ListView_SetItem(hwndListView, &t) ;
        attribs = attribs->next ;
    }
}
/* populate the tree view with our XML nodes */
void PopulateTree(HWND hwndListView, struct xmlNode *nodes, int level)
{
    while (nodes) {
        char buf[20] ;
        LV_ITEM t ;
        sprintf(buf,"%d",level) ;
        memset(&t,0,sizeof(t)) ;
         t.iItem = lvItems++ ;
         t.iSubItem = 0 ;
         t.mask = LVIF_TEXT | LVIF_IMAGE;
         t.pszText = buf ; 
         t.cchTextMax = strlen(buf) ;
         t.iImage = ilNode ;
         ListView_InsertItem(hwndListView, &t) ;
         t.iSubItem =1 ;
         t.mask = LVIF_TEXT ;
         t.pszText = nodes->userdata ;
         t.cchTextMax = strlen(nodes->userdata) ;
         ListView_SetItem(hwndListView, &t) ;
        PopulateAttribs(hwndListView,nodes->attribs,level) ;
        PopulateTree(hwndListView,nodes->children,level + 1) ;
        nodes = nodes->next ;
    }
}
void CreateNames(struct xmlNode *nodes)
{
    char buf[10000] ;
    while (nodes) {
        struct xmlAttr *attribs = nodes->attribs ;
        CreateNames(nodes->children) ;
        sprintf(buf,"<%s> ",nodes->elementType) ;
//		while (attribs) {
//			sprintf(buf+strlen(buf),"%s=\"%s\" ",attribs->name,attribs->value) ;
//			attribs = attribs->next ;
//		}
//		sprintf(buf+strlen(buf),"'%s'",nodes->textData) ;
        nodes->userdata = strdup(buf) ;
        nodes = nodes->next ;
    }
}
void FreeNames(struct xmlNode *nodes)
{
    while (nodes) {
        struct xmlAttr *attribs = nodes->attribs ;
        while (attribs) {
            free(attribs->userdata) ;
            attribs = attribs->next ;
        }
        FreeNames(nodes->children) ;
        free(nodes->userdata) ;
        nodes = nodes->next ;
    }
}
/* Window procedure */
int PASCAL WndProc (HWND hWnd, int wmsg, int wParam, int lParam)
{
    static HWND hwndListView ; // static so it won't go away when we exit WndProc
    static struct xmlNode *nodes ;
    static HWND hWndToolbar ;
   HDC dc ;
   RECT r,tr ;
   LV_COLUMN lvC ;

   char buf[260] ;
    switch(wmsg) {
        case WM_CREATE:
            nodes = 0 ;
            /*create toolbar */
            tbrButtons[0].iBitmap = STD_FILENEW;
            tbrButtons[0].fsState = TBSTATE_ENABLED;
            tbrButtons[0].fsStyle = TBSTYLE_BUTTON;
            tbrButtons[0].idCommand = IDM_NEW;
        
            tbrButtons[1].iBitmap = STD_FILEOPEN;
            tbrButtons[1].fsState = TBSTATE_ENABLED;
            tbrButtons[1].fsStyle = TBSTYLE_BUTTON;
            tbrButtons[1].idCommand = IDM_OPEN;
        
            tbrButtons[2].iBitmap = STD_FILESAVE;
            tbrButtons[2].fsState = TBSTATE_ENABLED;
            tbrButtons[2].fsStyle = TBSTYLE_BUTTON;
            tbrButtons[2].idCommand = IDM_SAVEAS;
        
            hWndToolbar = CreateToolbarEx(hWnd,WS_CHILD|WS_VISIBLE,1,15,HINST_COMMCTRL,IDB_STD_SMALL_COLOR,tbrButtons,3,16,16,16,16,sizeof(TBBUTTON));
            /* Create list view */
            GetClientRect(hWnd,&r) ;
            GetClientRect(hWndToolbar,&tr) ;
            hwndListView = CreateWindowEx(0,WC_LISTVIEW,"",
               WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
                    0,tr.bottom,r.right,r.bottom-tr.bottom,hWnd, (HMENU) 100, hInstance, NULL) ;
            hbmpNode = LoadBitmap(hInstance, MAKEINTRESOURCE(ID_NODE)) ;
            hbmpAttrib = LoadBitmap(hInstance, MAKEINTRESOURCE(ID_ATTRIB)) ;
            lvIml = ImageList_Create(16,16,FALSE,2,0) ;
            ilNode = ImageList_Add(lvIml, hbmpNode, 0) ;
            ilAttrib = ImageList_Add(lvIml, hbmpAttrib, 0) ;
            DeleteObject(hbmpNode) ;
            DeleteObject(hbmpAttrib) ;
            ListView_SetImageList(hwndListView,lvIml,LVSIL_SMALL) ;
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = 100 ;
            lvC.iSubItem = 0 ;
            lvC.pszText = "Level" ;
            ListView_InsertColumn(hwndListView, 0, &lvC) ;
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = (r.right - r.left - 100) /2 ;
            lvC.iSubItem = 1 ;
            lvC.pszText = "Node" ;
            ListView_InsertColumn(hwndListView, 1, &lvC) ;
            lvC.mask = LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
            lvC.cx = (r.right - r.left - 100) /2 ;
            lvC.iSubItem = 2 ;
            lvC.pszText = "Attrib" ;
            ListView_InsertColumn(hwndListView, 2, &lvC) ;
            break ;
        case WM_PAINT:
            /* Do basically nothing in the paint.  I do do the begin and end paint because
             * I've noticed that sometimes when empty windows don't do this strange 
             * behavior results 
             */
            {
                PAINTSTRUCT ps ;
                dc = BeginPaint(hWnd,&ps);
                EndPaint(hWnd,&ps) ;
            }
            break ;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDM_OPEN:
                    /* Get the name of the file and load it in */
                    buf[0] = 0 ;
                    if (GetFileName(hWnd,buf)) {
                        FILE *fil = fopen(buf,"r") ;
                        if (!fil)
                            MessageBox(0,"Could not open file","Error",0) ;
                        else {
                            if (nodes) {
                                ListView_DeleteAllItems(hwndListView) ;
                                lvItems = 0 ;
                                FreeNames(nodes) ;
                                xmlFree(nodes) ;
                            }
                            nodes = xmlReadFile(fil) ;
                            fclose(fil) ;
                            if (!nodes)
                                MessageBox(0,"Could not parse XML file","Error",0) ;
                            else {
                                CreateNames(nodes) ;
                                PopulateTree(hwndListView,nodes,1) ;
                            }
                        }
                    }
                    break ;
                case IDM_SAVEAS:
                    MessageBox(0,"You pressed the Save As button","Information",MB_ICONEXCLAMATION) ;
                    break ;
                case IDM_NEW:
                    MessageBox(0,"You pressed the New button","Information",MB_ICONEXCLAMATION) ;
                    break ;
                case IDM_ABOUT:
                    /* Put up the about box */
                    DialogBox(hInstance,MAKEINTRESOURCE(DLG_ABOUT),hWnd,(DLGPROC)AboutBoxProc) ;
                    break ;
                case IDM_EXIT:
                    /* Graceful exit of the program */
                    PostMessage(hWnd,WM_CLOSE,0,0) ;
                    break ;
            }
            break ;
        case WM_SIZE:
            {
                RECT rcTool;
                int iToolHeight;


                   int iListviewHeight;
                RECT rcClient;

                // Size toolbar and get height
                SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);

                GetClientRect(hWndToolbar, &rcTool);
                iToolHeight = rcTool.bottom ;


                // Calculate remaining height and size edit

                GetClientRect(hWnd, &rcClient);

                iListviewHeight = rcClient.bottom - iToolHeight;

                MoveWindow(hwndListView, 0,iToolHeight, rcClient.right,iListviewHeight, TRUE) ;
            
            }
            break ;
        case WM_DESTROY:
            /* At the destroy window, free resources and post a message to make
             * the message loop stop
             */
            if (nodes) {
                FreeNames(nodes) ;
                xmlFree(nodes) ;
            }
            PostQuitMessage(0) ;
            break ;		
    }
    return DefWindowProc(hWnd,wmsg,wParam,lParam);	
}

int PASCAL WinMain( HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
                    int nCmdShow )
{
    HWND hWnd ;
    MSG msgx;
    WNDCLASS  wc ;
    HANDLE hMenuMain ;
    hInstance = hInst ;
    /* Create the window classe */
    if (!FindWindow(szAppName,stitle)) {
        memset(&wc,0,sizeof(wc)) ;
        wc.style = CS_HREDRAW + CS_VREDRAW ;
        wc.lpfnWndProc = WndProc ;
        wc.cbClsExtra = 0 ;
        wc.cbWndExtra = 0 ;
        wc.hInstance = hInstance ;
        wc.hIcon = 0 ; // LoadIcon(hInstance, ID_ICON) ;
        wc.hCursor = LoadCursor(0,IDC_ARROW) ;
        wc.hbrBackground = GetStockObject(WHITE_BRUSH) ;
        wc.lpszMenuName = 0 ; // menuname ;
        wc.lpszClassName = szAppName ;
        if (!RegisterClass(&wc))
            return 0 ;
    }
    /* Initialize common controls DLL */
    InitCommonControls() ;
    
    /* Create The Window */
    hMenuMain = LoadMenu(hInstance,MAKEINTRESOURCE(MAINMENU)) ;
    hWnd= CreateWindow(szAppName,stitle,WS_OVERLAPPEDWINDOW + WS_VISIBLE,
            CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
            0,hMenuMain,hInstance,0) ;
    UpdateWindow(hWnd) ;
    
    /* Message loop */
    while (GetMessage(&msgx,0,0,0)) {
        TranslateMessage(&msgx);
        DispatchMessage(&msgx) ;
    }
    return msgx.wParam ;
}