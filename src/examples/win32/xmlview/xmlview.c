#include "Resource.h"
#include  <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "xml.h"

char *szAppName   =   "XMLView";
char *stitle       =  "XML View";
char szSourceFilter[] = 
            "XML files (*.xml)\0*.xml\0"
            "All Files (*.*)\0*.*\0" ;

HINSTANCE hInstance ;
HBITMAP hbmpNode, hbmpAttrib ;
HIMAGELIST treeIml ;
int ilNode, ilAttrib ;
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
void PopulateAttribs(HWND hwndTree, HWND below, struct xmlAttr *attribs) 
{
    while (attribs) {
        HTREEITEM current ;
        TV_INSERTSTRUCT t;
        char buf[512] ;
        sprintf(buf,"%s=\"%s\"",attribs->name,attribs->value) ;
        attribs->userdata = strdup(buf) ;
        memset(&t,0,sizeof(t)) ;
        t.hParent = below ;
        t.hInsertAfter = TVI_LAST ;
        t.item.mask = TVIF_TEXT  | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        t.item.hItem = 0;	
        t.item.pszText = attribs->userdata ;
        t.item.cchTextMax = strlen(attribs->userdata) ;
        t.item.lParam = attribs ;
        t.item.iImage = ilAttrib ;
        t.item.iSelectedImage = ilAttrib ;
        TreeView_InsertItem(hwndTree,&t) ;
        attribs = attribs->next ;
    }
}
/* populate the tree view with our XML nodes */
void PopulateTree(HWND hwndTree, HWND below, struct xmlNode *nodes)
{
    while (nodes) {
        HTREEITEM current ;
        TV_INSERTSTRUCT t;
        memset(&t,0,sizeof(t)) ;
        t.hParent = below ;
        t.hInsertAfter = TVI_LAST ;
        t.item.mask = TVIF_TEXT  | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        t.item.hItem = 0;	
        t.item.pszText = nodes->userdata ;
        t.item.cchTextMax = strlen(nodes->userdata) ;
        t.item.lParam = nodes ;
        t.item.iImage = ilNode ;
        t.item.iSelectedImage = ilNode ;
        current = TreeView_InsertItem(hwndTree,&t) ;
        PopulateAttribs(hwndTree,current,nodes->attribs) ;
        PopulateTree(hwndTree,current,nodes->children) ;
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
    static HWND hwndTree ; // static so it won't go away when we exit WndProc
    static struct xmlNode *nodes ;
   HDC dc ;
   RECT r ;
   NMTREEVIEW *nmhdr ;
   struct xmlNode *selNode ;
   char buf[260] ;
    switch(wmsg) {
        case WM_CREATE:
            nodes = 0 ;
            GetClientRect(hWnd,&r) ;
            hwndTree = CreateWindowEx(0,WC_TREEVIEW,"",
               WS_VISIBLE | WS_CHILD | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_BORDER,
                    0,0,r.right,r.bottom,hWnd, (HMENU) 100, hInstance, NULL) ;
            hbmpNode = LoadBitmap(hInstance, MAKEINTRESOURCE(ID_NODE)) ;
            hbmpAttrib = LoadBitmap(hInstance, MAKEINTRESOURCE(ID_ATTRIB)) ;
            treeIml = ImageList_Create(16,16,FALSE,2,0) ;
            ilNode = ImageList_Add(treeIml, hbmpNode, 0) ;
            ilAttrib = ImageList_Add(treeIml, hbmpAttrib, 0) ;
            DeleteObject(hbmpNode) ;
            DeleteObject(hbmpAttrib) ;
            TreeView_SetImageList(hwndTree,treeIml,TVSIL_NORMAL) ;
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
        case WM_NOTIFY:
            nmhdr = (NMTREEVIEW *)lParam ;
            switch(nmhdr->hdr.code) {
                case NM_RCLICK:
                    selNode = (struct xmlNode *)nmhdr->itemNew.lParam ;
                    
                    break ;
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
                                TreeView_DeleteItem(hwndTree, TVI_ROOT) ;
                                FreeNames(nodes) ;
                                xmlFree(nodes) ;
                            }                            nodes = xmlReadFile(fil) ;
                            fclose(fil) ;
                            if (!nodes)
                                MessageBox(0,"Could not parse XML file","Error",0) ;
                            else {
                                CreateNames(nodes) ;
                                PopulateTree(hwndTree,TVI_ROOT,nodes) ;
                            }
                        }
                    }
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
        default:
            return DefWindowProc(hWnd,wmsg,wParam,lParam);	
    }
    return 0 ;
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