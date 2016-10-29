//
// this header describes a windows control that is a combination of a treeview and a listview
// while not a standard common control, it is available on most windows systems
// in the RASDLG.DLL.  It is similar to both a tree view and a list view.
// the returned HWND from CreateWindow is a TreeListView handle, but it has a child which
// is a standard list view, so for example you can do standard list view functions with it.
// When inserting items, the handle returned is an HTREEITEM.  You can use this handle
// to do a setitem on successive columns through the TreeListView interface.
//
// If you want events, the right-click comes from the list view.  Which means the iItem in this
// case is the row number, not an HTREEITEM.  If you need to index it back to the TreeListView HTREEITEM
// handle associated with the line, just fetch the LPARAM.  As far as I can tell the LPARAMs
// are protected and you cannot overwrite them, so, you have to cache instance data locally.
//
// You will get a left click from both the list view and the TreeListView.  I usually filter
// on the ListView and fetch the HTREEITEM from the LPARAM.
//
// You can get other types of events from the ListView but I haven't explored that much.
//
// In preliminary testing, I had problems with TLM_DELETEALLITEMS.  I had to call it twice
// in a row to keep the RASDLG.DLL from crashing when I added more top level tree elements
// after I called it.  Your mileage may vary.
//
// Some of the TreeView constants are similar and others seem to differ.
//
// to use this simply do a LoadLibrary on rasdlg.dll, it will register the window
// and then you can do a create window with the TreeList class name
//
// oh and the source I got this from indicated that RASDLG.DLL only supports a UNICODE
// version of this functionality.
//
// BTW this TreeListView stops the multiselect on the list view from working, but, if
// you want to get it back modify the window styles after creating the window (With GetWindowLong)
// and get rid of the LVS_SINGLESEL style.
//
#ifndef TREELISTVIEW_H
#define TREELISTVIEW_H

#define TLM_INSERTITEM     WM_USER + 1 
#define TLM_DELETEITEM     WM_USER + 2 
#define TLM_DELETEALLITEMS WM_USER + 3 
#define TLM_GETITEM        WM_USER + 4 
#define TLM_SETITEM        WM_USER + 5 
#define TLM_GETITEMCOUNT   WM_USER + 6 
#define TLM_GETNEXTITEM    WM_USER + 7 
#define TLM_EXPAND         WM_USER + 8 
#define TLM_SETIMAGELIST   WM_USER + 9 
#define TLM_GETIMAGELIST   WM_USER + 10 
#define TLM_INSERTCOLUMN   WM_USER + 11 
#define TLM_DELETECOLUMN   WM_USER + 12 
#define TLM_SELECTITEM     WM_USER + 13 
#define TLM_REDRAWWINDOW   WM_USER + 14 
#define TLM_ISEXPANDED     WM_USER + 15 
#define TLM_GETCOLUMNWIDTH WM_USER + 16 
#define TLM_SETCOLUMNWIDTH WM_USER + 17 

// this differ slightly from the TVGN_ variants
// I haven't fleshed them out fully.
#define TLGN_ROOT TVGN_ROOT
#define TLGN_CHILD 2
#define TLGN_NEXT 4

// kinda a a TV_INSERTSTRUCT with an LVITEM instead of a TVITEM
typedef struct tagTL_INSERTSTRUCT 
{ 
        PVOID hParent; 
        PVOID hInsertAfter; 
        LVITEM *plvitem; 
} TL_INSERTSTRUCT; 

#define TREELIST_DLL L"RASDLG.DLL"
#define TREELIST_CONTROL L"TreeList"

#endif