/* Protogen Version 3.0.1.6Thursday November 27, 2014  01:32:16 */

                              /* Altcomb.c */

LRESULT CALLBACK historyEditComboProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK historyComboProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void SubClassHistoryCombo(HWND combo);
void SubClassNECombo(HWND combo);
void RegisterHistoryComboWindow(void);

                            /* Autodepend.c */

void CalculateProjectDepends(PROJECTITEM *pj);
void CalculateFileAutoDepends(char *fileName);
PROJECTITEM *RetrieveInternalDepend(char *fileName);
void ResetInternalAutoDepends(void);

                              /* Brfold.c */

void GetDefaultProjectsPath(LPSTR pszPath);
DWORD BrowseForFile(HWND hwnd, LPSTR pszDisplayName, LPSTR pszPath, UINT cchPath);

                              /* Brkcode.c */

void SetTempBreakPoint(int procid, int threadid, int *addresses);
void ClearTempBreakPoint(int procid);
int SittingOnBreakPoint(DEBUG_EVENT *dbe);
void WriteBreakPoint(HANDLE hProcess, int address, int value);
void allocBreakPoint(HANDLE hProcess, BREAKPOINT *pt);
void freeBreakPoint(HANDLE hProcess, BREAKPOINT *pt);
void SetBreakPoints(int procid);
void ClearBreakPoints(int procid);
int inTempBreakPoint(int addr);
int isBreakPoint(int addr);
int isLocalBreakPoint(int addr);
int dbgSetBreakPoint(char *name, int linenum, char *extra);
void dbgClearBreakPoint(char *name, int linenum);
void SetBP(DEBUG_EVENT *dbe);
void functionbp(void);
                              /* Brkdata.c */

void databpInit(void);
BOOL databpAnyBreakpoints(void);
BOOL databpAnyDisabledBreakpoints(void);
void databpEnableAllBreakpoints(BOOL enableState);
void databpDelete(DATABREAK *data);
void databpRemove(void);
void databpSetBP(HANDLE hProcess);
void databpResetBP(HANDLE hProcess);
void databpEnd(void);
int databpCheck(DEBUG_EVENT *stDE);
int LoadDataBreakpoints(HWND hwnd);
void RemoveDataBp(HWND hwnd);
INT_PTR CALLBACK DataBpAddProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void AddDataBp(HWND hwnd);
LRESULT CALLBACK DataBpProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void databp(HWND hwnd);

                              /* Brkhdwe.c */

void hbpInit(void);
BOOL hbpAnyBreakpoints(void);
BOOL hbpAnyDisabledBreakpoints(void);
void hbpEnableAllBreakpoints(BOOL enableState);
void hbpDisable(void);
void hbpSetBP(void);
void hbpResetBP(void);
void hbpEnd(void);
int hbpCheck(THREAD *tThread);
void hbpDialog(void);

                              /* Brkstep.c */

void SingleStep(DWORD procID, DWORD threadID);
void ClearTraceFlag(DWORD procID, DWORD threadID);
int StepOverIncrement(DEBUG_EVENT *dbe);
void DoStepOver(DEBUG_EVENT *dbe);
void DoStepIn(DEBUG_EVENT *dbe);
int IsStepping(DEBUG_EVENT *dbe);
void StepOver(DEBUG_EVENT *dbe);
void StepOut(DEBUG_EVENT *dbe);
void StepIn(DEBUG_EVENT *dbe);
int RunTo(DEBUG_EVENT *dbe);
int isSteppingOut(DEBUG_EVENT *dbe);

                              /* Brkwnd.c */

void RegisterBreakWindow(HINSTANCE hInstance);
HWND CreateBreakWindow(void);

                              /* Browse.c */

void BrowseBack(void);
sqlite3 *BrowseOpenDBByHWND(HWND hwnd, PROJECTITEM **returnProj);
void DBClose(sqlite3* db);
int LookupSymbolBrowse(sqlite3 *db, char *name);
BROWSELIST *LookupCPPNamesBrowse(sqlite3 *db, char *name, BROWSELIST *in);
void InsertBrowse(char *filname, int curline);
void BrowseTo(HWND hwnd, char *msg, BOOL toDeclaration);
void RegisterBrowseWindow(HINSTANCE hInstance);
HWND CreateBrowseWindow(void);

                             /* Buildrul.c */

void LoadDefaultRules(void);
void RestoreBuildRules(struct xmlNode *node, int version);
void SaveBuildRules(FILE *out);
void BuildRulesCustomize(void);

                             /* Codecomp.c */

int ccGetColorizeData(char* file, COLORIZE_HASH_ENTRY* entries[]);
void ReloadLineData(char *name);
void DoParse(char *name);
void deleteFileData(char *name);
char ** ccGetMatchingNames(char *name, char **names, int *size, int *max);
int ccLookupType(char *buffer, char *name, char *module, int line, int *rflags, sqlite_int64 *rtype);
int ccLookupMemberType(char *name, char *module, int line, sqlite3_int64 *structId, int *indir);
int ccLookupStructId(char *name, char *module, int line, sqlite3_int64 *structId);
BYTE * ccGetLineData(char *file, int *max);
void InsertColorizeEntry(COLORIZE_HASH_ENTRY *entries[], char *name, int start, int end, int type);
void StartCodeCompletionServer(void);

                             /* Ctlbound.c */

void DrawBoundingRect(RECT *r1);
void HideBoundingRect(void);
void CreateBoundingRect(HINSTANCE hInstance, HWND hwndFrame);

                              /* Ctlcont.c */

void RegisterControlWindow(HINSTANCE hInstance);
HWND CreateControlWindow(HWND parent);

                             /* Ctlframe.c */

void RegisterFrameWindow(HINSTANCE hInstance);
HWND CreateFrameWindow(HWND parent);

                              /* Ctltab.c */

struct _singleTab *CreateItem(char *name, LPARAM lParam);
void RemoveFromActive(struct _tabStruct *ptr, struct _singleTab *tabs);
void RemoveFromSelected(struct _tabStruct *ptr, struct _singleTab *tabs);
void AddToActive(struct _tabStruct *ptr, int pos, struct _singleTab *tabs);
void AddToSelected(struct _tabStruct *ptr, int pos, struct _singleTab *tabs);
void MyDrawEdge(HDC hDc, struct _tabStruct *ptr, int xStart, int yStart, int xEnd, int yEnd, int inc);
void MyDrawArc(HDC hDc, struct _tabStruct *ptr, int x, int y, int inc);
void DrawWindowBmp(HWND hwnd, struct _tabStruct *ptr, HDC hDc, int bordertype);
void DrawTabs(HWND hwnd, HDC hDc, RECT *r, struct _tabStruct *ptr);
int GetItem(struct _tabStruct *ptr , int index);
struct _singleTab *AddTab(struct _tabStruct *ptr, int pos, char *name, LPARAM lParam);
struct _singleTab *RemoveTab(struct _tabStruct *ptr, LPARAM lParam);
BOOL ModifyTab(struct _tabStruct *ptr, WPARAM modified, LPARAM lParam);
void SelectTab(struct _tabStruct *ptr, LPARAM lParam);
void RemoveAllTabs(struct _tabStruct *ptr);
struct _singleTab * FindTabByPos(HWND hwnd, struct _tabStruct *ptr, LPARAM lParam);
BOOL IsCloseButton(HWND hwnd, struct _tabStruct *ptr, struct _singleTab *tab, LPARAM lParam);
void ShowCloseButton(HWND hwnd, struct _tabStruct *ptr, struct _singleTab *tab, BOOL open, LPARAM lParam);
BOOL IsWindowButton(HWND hwnd, struct _tabStruct *ptr, LPARAM lParam);
void ShowWindowButton(HWND hwnd, struct _tabStruct *ptr, int flags);
BOOL ChangeHighLight(HWND hwnd, struct _tabStruct *ptr, struct _singleTab *tab);
void SetFonts(HWND hwnd, struct _tabStruct *ptr, HFONT font);
int Notify(HWND hwnd, struct _singleTab *tab, int notifyCode);
void RegisterLsTabWindow(HINSTANCE hInstance);
HWND CreateLsTabWindow(HWND parent, int style);

                               /* Ctltb.c */

int GetToolBarData2(HWND hwnd, char *horiz);
void SetToolBarData2(HWND hwnd, char *horiz);
void RegisterToolBarWindow(HINSTANCE hInstance);
HWND CreateToolBarWindow(int id, HWND notify, HWND parent, int bmp, int bmpcount, TBBUTTON *buttons, char **hints,
    char *title, int helpItem, BOOL visible);

                              /* Ctltree.c */

int LoadItemText(HWND hwnd, HTREEITEM htreeItem, char *buf, int indent);
int GetClipboardText(HWND hwnd, HTREEITEM item, char *buf, int indent);
LRESULT CALLBACK extTreeWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK ColumnTreeWndProc(HWND hwnd, UINT iMessage, WPARAM
    wParam, LPARAM lParam);
void RegisterextTreeWindow(HINSTANCE hInstance);
HWND CreateextTreeWindow(HWND parent, int style, RECT *r, TCHeader *h);

                               /* Ctltt.c */

void HookMouseMovement(MSG *msg);
void RegisterTTIPWindow(HINSTANCE hInstance);
HWND CreateTTIPWindow(HWND hwndParent, int style);
                              /* Dbgasm.c */

void GetCodeLine(char *s);
void CalculateDisassembly(int moving);
void DoDisassembly(HDC dc, RECT *r);
LRESULT CALLBACK gotoProc2(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void asmDoPaint(HWND hwnd);
LRESULT CALLBACK ASMProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterASMWindow(HINSTANCE hInstance);
void StopASMWindow(void);
HWND CreateASMWindow(void);

                               /* Dbgdb.c */

DEBUG_INFO *DebugDBOpen(char *name);
int GetSymbolAddress(DEBUG_INFO *dbg_info, char *name);
int GetEqualsBreakpoint(DEBUG_INFO *dbg_info, DWORD Address, char *module, int *linenum);
int GetHigherBreakpoint(DEBUG_INFO *dbg_info, DWORD Address, char *module, int *linenum);
void GetBreakpointAddressesInternal(DEBUG_INFO *dbg_info, char *module, int *linenum, int **array, int *max, int *count);
int GetHigherBreakpointLineByModule(DEBUG_INFO *dbg_info, char *module, int linenum);
int *GetLineTableInternal(DEBUG_INFO *dbg_info, char *name, int lineno, int *lc);
int GetGlobalName(DEBUG_INFO *dbg_info, char *name, int *type, int Address, int equals);
int GetFuncId(DEBUG_INFO *dbg_info, int Address);
NAMELIST *GetEnclosedAutos(DEBUG_INFO *dbg_info, int funcId, int line);
int GetLocalSymbolAddress(DEBUG_INFO *dbg_info, int funcId, char *name, char *filename, int line, int *address, int *type);
int GetGlobalSymbolAddress(DEBUG_INFO *dbg_info, char *name, char *filename, int line, int *address, int *type);
int GetType(DEBUG_INFO *dbg_info, char *name);
int GetTypeName(DEBUG_INFO *dbg_info, int type, char *name);
int GetReturnTypeQualifier(DEBUG_INFO *dbg_info, int type);
int GetTypeQualifier(DEBUG_INFO *dbg_info, int type);
FUNCTIONLIST *LookupCPPNames(DEBUG_INFO *dbg_info, char *name, FUNCTIONLIST *in);
int LookupTypedefValues(DEBUG_INFO *dbg_info, int type);
int LookupPointerSubtype(DEBUG_INFO *dbg_info, int type);
int LookupBitfieldInfo(DEBUG_INFO *dbg_info, int type, int *start, int *end);
VARINFO *LookupStructInfo(DEBUG_INFO *dbg_info, int type, int Address, 
                          char *structtag, int *size);
int LookupArrayInfo(DEBUG_INFO *dbg_info, int type, int *size);
int LookupEnumValues(DEBUG_INFO *dbg_info, int type, char *structtag);
int LookupEnumValue(DEBUG_INFO *dbg_info, int type, char *name);
void LookupEnumName(DEBUG_INFO *dbg_info, int type, char *name, int ord);

                             /* Dbghints.c */

int ReadValue(int address, void *val, int size, VARINFO *var);
int WriteValue(int address, void *value, int size, CONTEXT *regs);
int GetEnumValue(DEBUG_INFO *dbg_info, VARINFO *info, char *text);
int HintBasicValue(VARINFO *info, int *signedtype, char *data);
void HintEnum(DEBUG_INFO *dbg_info, VARINFO *info, char *buf, int toenum, int onevalue);
int HintBf(VARINFO *info, int *signedtype);
void GetStringValue(VARINFO *info, char *buf, int len, int address);
void HintValue(DEBUG_INFO *dbg_info, VARINFO *info, char *buf);
void SimpleTypeName(char *name, int type);
char *SymTypeName(char *buf, DEBUG_INFO *info, VARINFO *v);

                              /* Dbglocals.c */
void RegisterLocalsWindow(HINSTANCE hInstance);
HWND CreateLocalsWindow(void);
                              /* Dbgmain.c */

void RunProgram(PROJECTITEM *plist);
int initiateDebug(PROJECTITEM *pj, int stopimmediately);
BOOL CALLBACK topenumfunc(HWND wnd, LPARAM value);
void ProcessToTop(DWORD processid);
BOOL CALLBACK consoleMenuFunc(HWND wnd, LPARAM value);
void DisableConsoleCloseButton(char *cmd);
THREAD *GetThread(DWORD procId, DWORD threadId);
PROCESS *GetProcess(DWORD procId);
DLL_INFO *GetDLLInfo(DWORD procId, DWORD addr);
void BlastExitProcFunc(DWORD procID, DWORD threadID, DWORD address);
void GetRegs(DWORD procID);
void SetRegs(DWORD procID);
void StopRunning(int newState);
void abortDebug(void);
void TranslateFilename(char * szFilename);
BOOL GetFileNameOfDLL(HMODULE hpsapiLib, HANDLE hProcess, DWORD base, DWORD nameBase, BOOL fUnicode, char *outName);
void StartDebug(char *cmd);

                              /* Dbgmem.c */

void RegisterMemWindow(HINSTANCE hInstance);

HWND CreateMem1Window(void);
HWND CreateMem2Window(void);
HWND CreateMem3Window(void);
HWND CreateMem4Window(void);

                              /* Dbgreg.c */

void SaveRegisterContext(void);
LRESULT CALLBACK RegisterProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterRegisterWindow(HINSTANCE hInstance);
HWND CreateRegisterWindow(void);

                             /* Dbgstack.c */

void ClearStackArea(HWND hwnd);
int eipReal(int eip);
int readStackedData(int inebp, int *outebp);
void SetStackArea(void);
LRESULT CALLBACK StackProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterStackWindow(HINSTANCE hInstance);
HWND CreateStackWindow(void);

                              /* Dbgsyms.c */

void unmanglename(char *buf);
DEBUG_INFO *GetDebugInfo(HANDLE hProcess, DWORD base);
DEBUG_INFO *findDebug(int Address);
void FreeDebugInfo(DEBUG_INFO *dbg);
DWORD GetMainAddress(DEBUG_INFO *dbg);
int * GetLineTable(char *name, int line, int *lc);
int GetBreakpointLine(int Address, char *module, int *linenum, BOOL next);
int *GetBreakpointAddresses(char *module, int *linenum);
int GetBreakpointNearestLine(char *module, int linenum, int inmodule);
FUNCTIONLIST *GetFunctionList(DEBUG_INFO *dbg_info, SCOPE *scope, char *name);
int FindFunctionName(char *buf, int Address, DEBUG_INFO **dbg, int *type);
int FindGlobalSymbol(DEBUG_INFO **dbg_info, int Address, char *buf, int *type);
NAMELIST *FindEnclosedAutos(SCOPE *scope);
SCOPE * FindSymbol(DEBUG_INFO **dbg_info, SCOPE *scope, char *name, int *address, int *type);
int FindTypeSymbol(DEBUG_INFO **dbg_info, SCOPE *scope, char *name);
int basictypesize(int type);
void ExpandPointerInfo(DEBUG_INFO *dbg_info, VARINFO *v);
int GetDbgPointerInfo(DEBUG_INFO *dbg_info, VARINFO *v);
int GetBitFieldInfo(DEBUG_INFO *dbg_info, VARINFO *v);
int GetStructInfo(DEBUG_INFO *dbg_info, VARINFO *v, int qual);
int GetArrayInfo(DEBUG_INFO *dbg_info, VARINFO *v);
int GetVLAInfo(DEBUG_INFO *dbg_info, VARINFO *v);
int GetEnumInfo(DEBUG_INFO *dbg_info, VARINFO *v);
int GetTypedefInfo(DEBUG_INFO *dbg_info, VARINFO *v);
int DeclType(DEBUG_INFO *dbg_info, VARINFO *v);
VARINFO *GetVarInfo(DEBUG_INFO *dbg, char *name, int address, int type, SCOPE *scope, THREAD *thread);
void FreeVarInfo(VARINFO *info);

                             /* Dbgthread.c */

LRESULT CALLBACK ThreadProc(HWND hwnd, UINT iMessage, WPARAM
    wParam, LPARAM lParam);
void RegisterThreadWindow(HINSTANCE hInstance);
HWND CreateThreadWindow(void);

                             /* Dbgwatch.c */

void RegisterWatchWindow(HINSTANCE hInstance);
HWND CreateWatch1Window(void);
HWND CreateWatch2Window(void);
HWND CreateWatch3Window(void);
HWND CreateWatch4Window(void);
HWND CreateLocalsWindow(void);
int SendToLastWatch(unsigned iMessage, WPARAM wParam, LPARAM lParam);

                             /* Dlgctlwnd.c */

LRESULT CALLBACK CtlTbProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterCtlTbWindow(HINSTANCE hInstance);
HWND CreateCtlTbWindow(void);


                              /* Doswind.c */

char *GetCmd(void);
LPTCH GetEnv(void);
void FreeEnv(LPTCH env);
DWORD DosWindowThread(void *xx);
void DosWindow(char *path, char *exec, char *args, char *rvTitle, char *rvBody);

                              /* Drawacc.c */

void GetAccPropText(char *buf, HWND lv, struct resRes *data, int row);
HWND AccPropStartEdit(HWND lv, int row, struct resRes *data);
void AccPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data);
LRESULT CALLBACK EditorSubclassProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void AccSubclassEditWnd(HWND parent, HWND hwnd);
LRESULT CALLBACK AccSetKeyProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
char *GetVKName(int val);
EXPRESSION *GetVKExpression(int val);
LRESULT CALLBACK AcceleratorDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterAcceleratorDrawWindow(HINSTANCE hInstance);
void CreateAcceleratorDrawWindow(struct resRes *info);

                              /* Drawdlg.c */

void drawdlginit(void);
char *GetCtlName(CONTROL *c);
void ChangePosition(struct resRes *dlgData, EXPRESSION **pos, int offset);
LRESULT CALLBACK DlgControlInputRedirProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK DlgControlInputProc(HWND hwnd, UINT iMessage, WPARAM wParam,
     LPARAM lParam);
BOOL RemoveFromStyle(EXPRESSION **style, int val);
void AddToStyle(EXPRESSION **style, char *text, int val);
void GetDlgPropText(char *buf, HWND lv, struct resRes *data, int row);
HWND DlgPropStartEdit(HWND lv, int row, struct resRes *data);
void DlgPropEndEdit(HWND lv, int row, HWND editWnd, struct resRes *data);
LRESULT CALLBACK DlgDlgProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK DlgDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterDlgDrawWindow(HINSTANCE hInstance);
void CreateDlgDrawWindow(struct resRes *info);

                             /* Drawedit.c */

void EditorRundown(void);
void backupFile(char *name);
int xstricmpz(const char *str1, const char *str2);
int xstricmp(const char *str1, const char *str2);
char *stristr(char *str1, char *str2);
DWINFO *DeQueueEditWindow(void);
void EnQueueEditWindow(DWINFO *ptr);
void ResetEditTitles(void );
void rehighlight(char *text, int whole, int casesensitive);
BOOL CALLBACK doSysMenu(HWND hwnd, LPARAM hide);
void doMaximize(void);
void ApplyEditorSettings(void);
void InvalidateByName(char *name);
DWINFO *GetFileInfo(char *name);
void EditRenameFile(char *oldName, char *newName);
void ApplyBreakAddress(char *module, int linenum);
void SaveDrawAll(void);
int AnyModified(void);
void CloseAll(void);
void RedrawAllBreakpoints(void);
char *GetEditData(HWND hwnd);
void FreeEditData(char *buf);
int SetEditData(HWND hwnd, char *buf, BOOL savepos);
int WriteFileData(char *name, char *buf, int flags);
char* ReadFileData(char *name, int*flags);
int SaveFile(HWND hwnd, DWINFO *info);
int LoadFile(HWND hwnd, DWINFO *info, BOOL savepos);
void SetTitle(HWND hwnd);
void drawParams(DWINFO *info, HWND hwnd);
void eraseParams(HWND hwnd);
int PaintBreakpoints(HWND hwnd, HDC dc, PAINTSTRUCT *paint, RECT *rcl);
LRESULT CALLBACK gotoProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void recolorize(DWINFO *ptr);
DWORD MsgWait(HANDLE event, DWORD timeout);
void InstallForParse(HWND hwnd);
void InstallAllForParse(void);
void RemoveAllParse();
LRESULT CALLBACK DrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterDrawWindow(HINSTANCE hInstance);
HWND openfile(DWINFO *newInfo, int newwindow, int visible);
HWND CreateDrawWindow(DWINFO *baseinfo, int visible);

                             /* Drawimage.c */

LRESULT CALLBACK ImageDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterImageDrawWindow(HINSTANCE hInstance);
void CreateImageDrawWindow(struct resRes *info);

                             /* Drawmenu.c */

void RecurseSetSeparator(MENU *menu, MENUITEM *item, BOOL issep);
int menuHitTest(HWND hwnd, struct resRes *menuData, POINT mouse);
void InsertDelete(HWND hwnd, struct resRes * menuData, int code);
LRESULT CALLBACK MenuDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterMenuDrawWindow(HINSTANCE hInstance);
void CreateMenuDrawWindow(struct resRes *info);

                            /* Drawrcdata.c */

void FormatAsciiString(char *buf, char *string, int len);
LRESULT CALLBACK rcDataDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterRCDataDrawWindow(HINSTANCE hInstance);
void CreateRCDataDrawWindow(struct resRes *info);

                            /* Drawstrings.c */

LRESULT CALLBACK StringTableDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterStringTableDrawWindow(HINSTANCE hInstance);
void CreateStringTableDrawWindow(struct resRes *info);

                             /* Drawutil.c */

void InitDrawUtil(void);
int FileAttributes(char *name);
void GetFileList(HWND hwndLV, PROJECTITEM *pj, int *items, BOOL changed);
void GetSecondaryFileList(HWND hwndLV, int *items, BOOL changed);
long APIENTRY FileSaveProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam);
long APIENTRY FileChangeProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam);
void CheckEditWindowChangedThread(void *aa);
void CheckEditWindowChanged(void);
int QuerySaveAll(void);
int PaintMDITitleBar(HWND hwnd, int iMessage, WPARAM wParam, LPARAM lParam);

                            /* Drawversion.c */

char *ParseVersionString(char *string, int *returnedLength);void FormatVersionString(char *buf, WCHAR *string, int len);
LRESULT CALLBACK VersionDrawProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterVersionDrawWindow(HINSTANCE hInstance);
void CreateVersionDrawWindow(struct resRes *info);

                              /* Effect.c */

void InitTheme(HWND hwnd);
void DestructTheme(void);
DWORD RetrieveSysColor(DWORD iColorId);
HBRUSH RetrieveSysBrush(DWORD iColorId);
VOID ChangeBitmapColor(HBITMAP hbmSrc, COLORREF rgbOld, COLORREF rgbNew);
VOID FAR PASCAL ChangeBitmapColorDC(HDC hdcBM, LPBITMAP lpBM, COLORREF rgbOld,
    COLORREF rgbNew);
void Tint(HBITMAP hBmpSrc, COLORREF dest);
void ConvertToGray(HBITMAP hBmpSrc);
HBITMAP ConvertToTransparent(HBITMAP hBmpSrc, DWORD color);
HBITMAP CopyBitmap(HWND wnd, HBITMAP src, int x, int y, int width, int height);

                              /* Extools.c */

DWORD RunExternalToolThread(void *p);
void RunExternalTool(int id);
void ExternalToolsToMenu(void);
void ExternalToolsToProfile(void);
void ProfileToExternalTools(void);
LRESULT CALLBACK ExToolsEditProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK ExToolsCustomizeProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void EditExternalTools(void);

                            /* Filebrowse.c */

void FileBrowseLeft(void);
void FileBrowseRight(void);
void FileBrowseLineChange(DWINFO *info, int lineno, int delta);
void FileBrowseClose(DWINFO *info);
void FileBrowseCloseAll(void);
void FileBrowseMenu(int x, int y, HWND hwndToolbar);
void FileBrowseTrigger(int i);

                              /* Filedlg.c */

void ProfileToMRD(void);
void MRDToProfile(void);
void setofndata(OPENFILENAME *ofn, char *name, HWND handle, char *filter);
void savedir(char *name);
void AppendExtension(OPENFILENAME *ofn);
int OpenFileDialogWithCancel(OPENFILENAME *ofn, char *name, HWND handle, int new, int
    multiple, char *filter, char *title);
int OpenFileDialog(OPENFILENAME *ofn, char *name, HWND handle, int new, int
    multiple, char *filter, char *title);
int SaveFileDialog(OPENFILENAME *ofn, char *name, HWND handle, int saveas, char
    *filter, char *title);

                               /* Find.c */

void findInit(void);
void AddFileToArray(DWINFO ***files, int *count, int *max, char *name);
void EndFind(void);
void FindResetWindows(void);
void FindSetDirty(void);
void FindStringFromToolbar(char *search);
void DoFindNext(void *p);
void DoReplaceNext(void *p);
void SetFlags(HWND hwndDlg, DLGHDR *pHdr);
void LoadBrowsePath(HWND hwndDlg, int id);
void GetBrowsePath(HWND hwndDlg, int id);
void xGetFileType(HWND hwndDlg, int id);
DLGTEMPLATE * WINAPI DoLockDlgRes(LPCSTR lpszResName);
LRESULT CALLBACK FindChildDlgProc(HWND hwndDlg, UINT iMessage, WPARAM wParam, LPARAM
                                  lParam);
VOID WINAPI OnSelChanged(HWND hwndDlg);
LRESULT CALLBACK FindDlgProc(HWND hwndDlg, UINT iMessage, WPARAM wParam, LPARAM
    lParam);
void OpenFindDialog(void);
void OpenReplaceDialog(void);
void OpenFindInFilesDialog(void);

                                /* find.c */

void RegisterFindWindow(HINSTANCE hInstance);
HWND CreateFind1Window(void);
HWND CreateFind2Window(void);
void SendFindMessage(BOOL second, char *buf);

                             /* Idedialog.c */

void DisableControl(HWND hwnd, int control, int disable);
void SetEditFieldRO(HWND hwnd, int control);
void SetEditField(HWND hWnd, int control, char *text);
void SetEditFieldValue(HWND hWnd, int control, long value);
void GetEditField(HWND hWnd, int control, char *text);
long GetEditFieldValue(HWND hWnd, int control);
void SetCBField(HWND hWnd, int control, int value);
int GetCBField(HWND hWnd, int control);
void NewFocus(HWND hWnd, int control);
void AddComboString(HWND hwnd, int control, char *string);
void SetComboSel(HWND hwnd, int control, int sel);
int GetComboSel(HWND hwnd, int control);
void AddListString(HWND hwnd, int control, char *string);
void SetListSel(HWND hwnd, int control, int sel);
int GetListSel(HWND hwnd, int control);

                              /* Ideeval.c */

void GetQualifiedName(char *dest, char **src, BOOL type, BOOL towarn);
VARINFO *EvalExpr(DEBUG_INFO **dbg, SCOPE *sc,
    char *text, int towarn);

                              /* Idehelp.c */

int ConfigMSDNHelp(void);
int InitHelp(void);
void RundownHelp(void);
int WebHelp(char *string);
int SpecifiedHelp(char *string);
int RTLHelp(char *string);
int LanguageHelp(char *string);
void ContextHelp(int id);
void GenericHelp(char *str, int id);
int MSDNHelp(char* string);

                              /* Idemenu.c */

BOOL CALLBACK WindowChangeEnumProc(HWND window, LPARAM param);
long APIENTRY WindowShowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam);
void ShowWindowList(void);
BOOL CALLBACK winmenEnumProc(HWND window, LPARAM param);
void SetWindowMenu(void);
void CreateMenuBitmaps(void);
void InsertBitmapsInMenu(HMENU hMenu);
UINT GetMenuCheckedState(HMENU menu, int Id);
void SetMenuCheckedState(HMENU menu, int did, int id);
void InitMenuPopup(HMENU menu);
void MenuActivateWinmenu(int wParam);
HMENU LoadMenuGeneric(HANDLE inst, char *name);

                             /* Ideregexp.c */

                              /* Idexml.c */

char *xmlConvertString(char *s, int attr);
void xmlFree(struct xmlNode *Node);
struct xmlNode *xmlReadFile(FILE *in);
void xmlWriteFile(FILE *out, struct xmlNode *root);
int main(int argc, char **argv);

                              /* Imgdraw.c */

void  imageInit(void);
HBITMAP LocalCreateBitmap(HDC dc, int width, int height, int colors);
int Pastable(HWND hwnd);
void DrawLine(HDC hDC, POINT start, POINT end);

                               /* Imgui.c */

LRESULT CALLBACK toolProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK toolScreenProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK  ScreenColorWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK  Color2WndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void PaintOneCurrentColor(HDC dc, RECT *r, DWORD color);
LRESULT CALLBACK  ControlWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK  AuxWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK  DrawAreaWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK  WorkspaceWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK  ImageWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterImageWindows(HINSTANCE hInstance);

                              /* Infownd.c */

void SimpleSearchInTextBox(HWND hwndEdit);
int getfile(char *start, char *buffer, char end, DWINFO *info);
void TextToClipBoard(HWND hwnd, char *text);
void EditCopyText(HWND hwnd);
LRESULT CALLBACK buildEditProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK errorProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterInfoWindow(HINSTANCE hInstance);
HWND CreateInfoWindow(void);
void SetInfoColor(int window, DWORD color);
void SelectInfoWindow(int window);
void SendInfoMessage(int window, char *buf);

void RegisterErrorWindow(HINSTANCE hInstance);
HWND CreateErrorWindow(void);

                             /* Instance.c */

int SendFileName(char *msg);
void PassFilesToInstance(void);
int RetrieveInstanceFile(DWINFO *info);
int msThread(void *aa);
int StartInstanceComms(void);
void StopInstanceComms(void);

                             /* Jumplist.c */

void LoadJumpSymbols(void);
void FreeJumpSymbols(void);
void SetJumplistPos(HWND hwnd, int linepos);
LRESULT CALLBACK ValueEditProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK ValueComboProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK JumpListProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterJumpListWindow(HINSTANCE hInstance);
HWND CreateJumpListWindow(void);


                               /* Make.c */

void InitMake(void);
void SetOutputNames(PROJECTITEM *pj, BOOL first);
void SetOutputExtensions(PROJECTITEM *pj, BOOL first);
void StopBuild(void);
void Maker(PROJECTITEM *pj, BOOL clean);
void dbgRebuildMainThread(void *v);
void dbgRebuildMain(int cmd, PROJECTITEM *pt);


                            /* Makecommon.c */

int GetFilenamePaths(PROJECTITEM *pj, char **dest, char *ext, int len, BOOL first, int escaped, int spacing, PROJECTITEM **lcd);
void EvalMacros(PROJECTITEM *pj, char *parsedCmd, char *rawCmd, PROJECTITEM **lcd);
unsigned MakeHash(char *name);
PROJECTITEM *LCD(PROJECTITEM *proj, PROJECTITEM *lcd);
char *Lookup(char *id, PROJECTITEM *proj, PROJECTITEM **lcd);
void AddMakeSymbol(char *name, char *value, BOOL dup, BOOL assign);
void RecursiveAddSymbols(SETTING *set, BOOL assign);
void AddAssigns(SETTING *set);
void AddRuleSymbols(PROJECTITEM *fi);
void AddRuleCommands(PROJECTITEM *fi);
void CreateEnvironmentMacros(void);
void CreateBuiltinWorkspaceMacros(PROJECTITEM *wa);
void CreateBuiltinProjectMacros(PROJECTITEM *pj, BOOL rel);
void CreateBuiltinFileMacros(PROJECTITEM *pj);
BOOL CreateFullPath(char *path);
void AddSymbolTable(PROJECTITEM *fi, BOOL rel);
void AddRootTables(PROJECTITEM *pj, BOOL rel);
void RemoveSymbols(HashTable *table);
void RemoveSymbolTable(void);
void ReleaseSymbolTables(void);
void SetIsMaking(int makeRunning);
void CreateTempFileName(void);
FILE *FindOnPath(char *string, char *searchPath);
void LoadPath(char *root, char *dest, char *item);
char *GetNextFile(char *out, char *in);
void countErrors(char *buf);
int FileTime(FILETIME *timex, char *name);
int CompareTimes(FILETIME *target, FILETIME *source);
int ParsePipeData(HANDLE handle, int window, HANDLE hProcess);
int Execute(char *cmd, char *wdp, int window);
void MakeMessage(char *title, char *name);
void ErrWarnCounts(void);
void ResetErrorCounts(void);

                              /* Makegen.c */

char *relpathmake(char *name, char *path);
void genMakeFile(PROJECTITEM *l);

                                /* Mru.c */

void MRUToMenu(int which);
void MRUToProfile(int which);
void ProfileToMRU(int which);
void InsertMRU(DWINFO *data, int which);
void MoveMRUUp(int index, int which);

                               /* Ocide.c */

void SetStatusMessage(char *str, int highlight);
void SetBusy(int state);
VOID WINAPI CenterWindow(HWND hWnd);
int ExtendedMessageBox(char *title, int flag, char *fmt, ...);
void ProjSetup(char select, char *string);
void WorkAreaSetup(char select, char *string);
int IsEditWindow(HWND hwnd);
int IsResourceWindow(HWND hwnd);
LRESULT CALLBACK BrowseToProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK WaitingProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
LRESULT CALLBACK WatchAddProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
char *exceptval(int num);
int GetHelpID(void );
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void doSplash(void);
void doAbout(void);
LRESULT CALLBACK CursorHookProc(int code, int wParam, int lParam);
int IsBusyMessage(MSG *msg);
void ProcessMessage(MSG *msg);
void ApplyDialogFont(HWND hwnd);
void InitFont(BOOL up);
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
    int nCmdShow);

                              /* Opcodes.c */

                             /* Operands.c */

void FormatDissassembly(char *buffer);
void RegisterSymbols(void);

                              /* Pefile.c */

int IsPEFile(char *filename);
int GetEntryPoint(void);
int FindExitProcessAddress(HANDLE hProcess, int imagebase);

                               /* Popup.c */

void doPopup(HWND hwnd, char *res);

                               /* Print.c */

void split (char *fmt, char *left, char *right, char *center);
int subs(HDC hDC, char *out, const char *fmt, const char *filename, int page);
void setTimeFormats(void);
int Print(HWND win);

                              /* Printwm.c */

void printwmsg(HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

                             /* PrjDepends.c */

void CreateProjectDependenciesList(void);
void EditProjectDependencies(PROJECTITEM *pj);

                              /* Prjfile.c */

int VerifyPath(char *path);
PROJECTITEM *HasFile(PROJECTITEM *data, char *name);
PROJECTITEM *AddFile(PROJECTITEM *data, char *name, BOOL automatic);
long APIENTRY NewFileProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam);
void ProjectNewFile(void);
void ProjectExistingFile(void);

                             /* Prjfolder.c */

PROJECTITEM *CreateFolder(PROJECTITEM *p, char *name, BOOL always );
void ProjectNewFolder(void);

                              /* Prjprj.c */

long APIENTRY NewProjectProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam);
void SetProjectType(PROJECTITEM *pj, int newMode);
void PropagateAllProjectTypes(void);
void ProjectNewProject(void);
void ProjectExistingProject(void);
void SaveAllProjects(PROJECTITEM *workArea, BOOL always);
void LoadProject(char *name);
void IndirectProjectWindow(DWINFO *info);

                              /* Prjutil.c */

void MoveChildrenUp(PROJECTITEM *data);
void FreeSubTree(PROJECTITEM *data, BOOL save);
void ProjectRemoveAll(void);
void ProjectRemove(void);
void ProjectRename(void);
void DoneRenaming(void);
void ImportProject(BOOL ctg);
LRESULT CALLBACK prjEditWndProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);

                               /* Prjwa.c */

void GetDefaultWorkAreaName(char *buf);
void SetWorkAreaMRU(PROJECTITEM *workArea);
void LoadWorkArea(char *name, BOOL existing);
void CloseWorkArea(void);
void ProjectNewWorkArea(void);
void ProjectExistingWorkArea(void);

                              /* Prjwnd.c */

int imageof(PROJECTITEM *data, char *name);
void ResyncProjectIcons(void);
void MarkChanged(PROJECTITEM *item, BOOL ws);
void TVInsertItem(HWND hTree, HTREEITEM hParent, HTREEITEM after, 
                       PROJECTITEM *data);
void ExpandParents(PROJECTITEM *p);
void RecursiveCreateTree(HTREEITEM parent, HTREEITEM pos, PROJECTITEM *proj);
void ProjectSetActive(void);
HTREEITEM FindItemRecursive(PROJECTITEM *l, DWINFO *info);
HTREEITEM FindItemByWind(HWND hwnd);
void CreateProjectMenu(void);
LRESULT CALLBACK ProjectProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterProjectWindow(HINSTANCE hInstance);
HWND CreateProjectWindow(void);

                              /* Profile.c */

void GetUserDataPath(char *buf);
char *getprofilestring(char *name, char *def, HKEY rootkey);
void StringToProfile(char *name, char *string);
char *ProfileToString(char *name, char *def);
void IntToProfile(char *key, int value);
int ProfileToInt(char *key, int def);

                             /* Progress.c */

int FAR PASCAL ProgressProc(HWND hWndDlg, UINT wmsg, WPARAM wparam, LPARAM
    lparam);
void MakeProgress(HWND hWnd, HINSTANCE hInst, LPCSTR title, long value);
void DeleteProgress(void);
void SetProgress(long value, LPCSTR fmt, ...);

                               /* Props.c */
BOOL MatchesExt(char *name, char *exts);

char *LookupProfileName(char *name);
void ClearProfileNames(void);
void InsertSetting(PROJECTITEM *pj, SETTING *set);
SETTING **GetSettingsAddress(PROJECTITEM *pj);
SETTING *GetSettings(PROFILE *pf);
LRESULT CALLBACK SelectProfileDlgProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void SelectProfileDialog(void);
void InitProps(void);
void EvalDependentRules(SETTING *depends, PROJECTITEM *item, struct _propsData *data);
void SelectRules(PROJECTITEM *item, struct _propsData *data);
void GetActiveRules(PROJECTITEM *item, struct _propsData *data);
void SetupCurrentValues(SETTING *setting, PROJECTITEM *saveTo);
void ApplyCurrentValues(SETTING *setting, PROJECTITEM *saveTo);
void RemoveCurrentValues(SETTING *setting);
long APIENTRY NewProfileProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam);
struct buttonWindow *CreateButtonWnd(HWND parent, BOOL staticText, BOOL combo);
void RegisterPropWindows(HINSTANCE hInstance);
void ShowGeneralProperties(void);
void ShowBuildProperties(PROJECTITEM *projectItem);
SETTING *PropFind(SETTING *props, char *id);
SETTING *PropSearchProtos(PROJECTITEM *item, char *id, SETTING **value);
void PropGetFont(PROJECTITEM *item, char *id, LPLOGFONT lplf);
BOOL PropGetBool(PROJECTITEM *item, char *id);
int PropGetInt(PROJECTITEM *item, char *id);
void PropGetString(PROJECTITEM *item, char *id, char *string, int len);
COLORREF PropGetColor(PROJECTITEM *item, char *id);

                             /* Propswnd.c */

HWND PropGetHWNDCombobox(HWND parent, BOOL vscroll);
HWND PropGetHWNDNumeric(HWND parent);
HWND PropGetHWNDText(HWND parent);
void PropSetExp(struct resRes *data, char *buf, EXPRESSION **exp);
EXPRESSION *GetBaseId(EXPRESSION *exp);
void PropSetIdName(struct resRes *data, char *buf, EXPRESSION **exp, CONTROL *ctls, BOOL changeTree);
void PropSetItem(HWND lv, int index, int group, char *label);
void PropSetGroup(HWND lv, int group, WCHAR *label);
void SetResourceProperties(void *data, struct propertyFuncs *funcs);
void PropsWndRedraw(void);
void PropsWndClearEditBox(MSG *msg);
LRESULT CALLBACK PropsProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterPropsWindow(HINSTANCE hInstance);
HWND CreatePropsWindow(void);

                               /* Rcerr.c */

void initerr(void);
void fatal(char *fmt, ...);
int printerr(char *buf, int errnum, void *data);
void basicerror(int n, void *data);
void Error(char *string);
void generror(int n, int data);
void gensymerror(int n, char *data);
void expecttoken(int n);
void generrorexp(int n, int data);
void gensymerrorexp(int n, char *data);
void expecttokenexp(int n);

                               /* Rcgs.c */

BOOL iswhitespacechar(short x) ;
BOOL issymchar(short x) ;
BOOL isstartchar(short x) ;
void initsym(void);
int getstring(char *s, int len);
LIST *GetCachedLines(void);
int getch(void);
void getid(void);
int getsch(void) ;
int getsym2(void);
void getsym(void);
void skip_comma(void);
void need_eol(void);
void need_begin(void);
void need_end(void);
int needpunc(enum e_sym p);
int needpuncexp(enum e_sym p);
int is_number(void);

                               /* Rcie.c */

void freeExpr(EXPRESSION *r);
EXPRESSION *ReadExpFromString(WCHAR *id);
EXPRESSION *InternalLookup(char *id, int translate);
EXPRESSION *LookupWithTranslation(char *id);
EXPRESSION *ReadExp(void);
EXPRESSION *ReadExpOr(EXPRESSION *p);
int Eval(EXPRESSION *p);
int intexpr(void);

                             /* Rcinterf.c */

void InsertRCWindow(HWND hwnd);
void RemoveRCWindow(HWND hwnd);
void CloseAllResourceWindows(void);
void CreateNewResourceFile(PROJECTITEM *data, char *name, int flag);
void FormatExp(char *buf, EXPRESSION *exp);
void FormatResId(char *buf, IDENT *id);
void ResShowSubTree(PROJECTITEM *file);
struct resDir *FindResDir(struct resData *data, int type);
void ResCreateSubTree(struct resData *data);
void ResFreeSubTree(struct resData *data);
struct resData *ResLoad(char *name);
void ResRewriteTitle(struct resRes *res, BOOL dirty);
void ResSetDirty(struct resRes *res);
void ResSetClean(struct resRes *res);
void ResSave(char *name, struct resData *data);
HANDLE ResGetHeap(PROJECTITEM *pj, struct resRes *data);
BOOL ResSaveCurrent(PROJECTITEM *pj, struct resRes *res);
PROJECTITEM *GetResData(PROJECTITEM *cur);
void ResSaveAll(void);
void ResOpen(PROJECTITEM *file);
void HandleDblClick(HTREEITEM item, BOOL err);
LRESULT CALLBACK NewResourceProc(HWND hWnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void HandleRightClick(HWND hWnd, HTREEITEM item);
BOOL ResCheckChanged(RESOURCE *resources);
void ResFree(PROJECTITEM *file, BOOL toSave);
void ResReload(PROJECTITEM *pj);
void NavigateToResource(PROJECTITEM *file);
void ResAddItem(PROJECTITEM *file);
void ResDeleteItem(PROJECTITEM *data);
void ResDeleteAllItems(void);
void ResGetMenuItemName(EXPRESSION *id, char *text);
void ResGetStringItemName(EXPRESSION *id, char *text);
int ResChangeId(char *name, int id);
char *ResAddNewDef(char *name, int id);
EXPRESSION *ResAllocateMenuId(void);
EXPRESSION *ResAllocateStringId(void);
int ResNextCtlId(CONTROL *ctls);
EXPRESSION *ResAllocateControlId(struct resRes *dlgData, char *sel);
EXPRESSION *ResAllocateResourceId(int type);
EXPRESSION *ResReadExp(struct resRes *data, char *buf);
WCHAR *WStrDup(WCHAR *s);
void ResSetTreeName(struct resRes *data, char *buf);
PROJECTITEM *GetResProjectFile(HTREEITEM sel);
void ResRename(void);
void ResDoneRenaming(void);
LRESULT CALLBACK ResourceProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void RegisterResourceWindow(HINSTANCE hInstance);
HWND CreateResourceWindow(void);

                               /* Rckw.c */

char *namefromkw(int st);
void kwini(void);
int searchkw(void);

                               /* Rcpp.c */

void preprocini(void);
int preprocess(void);
int doerror(void);
int dopragma(void);
int doline(void);
void ReadFileName(void);
int doinclude(int unquoted);
WCHAR *prcStrdup(const WCHAR *string);
void glbdefine(char *name, char *value);
void getdefsym(void);
int dodefine(void);
int doundef(void);
void undef2(void);
int defid(WCHAR *name, WCHAR **p, char *q);
int definsert(WCHAR *end, WCHAR *begin, WCHAR *text, int len, int replen);
int defreplace(WCHAR *macro, int count, WCHAR **oldargs, WCHAR **newargs);
void cnvt(WCHAR *out, char *in);
void filemac(WCHAR *string);
void datemac(WCHAR *string);
void timemac(WCHAR *string);
void linemac(WCHAR *string);
void defmacroreplace(WCHAR *macro, WCHAR *name);
void nodefines(void);
int indefine(SYM *sp);
void enterdefine(SYM *sp);
void exitdefine(void);
int replacesegment(WCHAR *start, WCHAR *end, int *inbuffer, int totallen, int
    *changed);
int defcheck(WCHAR *line);
void pushif(void);
void popif(void);
int doifdef(int flag);
int doif(int flag);
int doelif(void);
int doelse(void);
int doendif(void);

                              /* Rcread.c */

void *rcAlloc(int v);
char *rcStrdup(const char *s);
void rcFree(void *p);
BITMAP_ *RCLoadBitmap(char *fileName);
CURSOR *RCLoadCursor(char *fileName);
int StringAsciiToWChar(WCHAR **text, char *string, int len);
FONT *RCLoadFont(char *fileName);
ICON *RCLoadIcon(char *fileName);
BYTE *RCLoadMessageTable(char *fileName, int *size);
int StrToClass(char *str);
void FreeResources(RESOURCE_DATA *r);
void SetRCMallocScope(RESOURCE_DATA *r);
RESOURCE_DATA *ReadResources(char *fileName);

                               /* Rcsym.c */

void symini(void);
HASHREC *AddHash(HASHREC *item, HASHREC **table, int size);
HASHREC **LookupHash(char *key, HASHREC **table, int size);
SYM *basesearch(char *na, TABLE *table, int checkaccess);
SYM *search(char *na, TABLE *table);
void insert(SYM *sp, TABLE *table);

                              /* Rcutil.c */

FILE *MySearchPath(char *string, char *searchpath, char *mode);
int pstrncmp(const WCHAR *str1, const WCHAR *str2, int n);
int pstrcmp(const WCHAR *str1, const WCHAR *str2);
void pstrcpy(WCHAR *str1, const WCHAR *str2);
void pstrncpy(WCHAR *str1, const WCHAR *str2, int len);
void pstrcat(WCHAR *str1, const WCHAR *str2);
WCHAR *pstrchr(WCHAR *str, WCHAR ch);
WCHAR *pstrrchr(WCHAR *str, WCHAR ch);
int pstrlen(const WCHAR *s);
WCHAR *pstrstr(WCHAR *str1,const WCHAR *str2);

                              /* Rcwrite.c */

void StringWToA(char *a, WCHAR *w, int l);
void WriteResources(char *fileName, RESOURCE_DATA *select);

                              /* Simops.c */

void SegmentDisplay(char *buffer, long value);

                              /* Slprefs.c */

char *nocr(char *s);
int IsNode(struct xmlNode *node, char *name);
int IsAttrib(struct xmlAttr *attr, char *name);
void addcr(char *buf);
void RestorePlacement(struct xmlNode *node, int version);
void RestoreMemoryWindowSettings(struct xmlNode *node, int version);
void RestoreFindflags(struct xmlNode *node, int version);
void RestoreCustomColors(struct xmlNode *node, int version);
void RestoreGeneralProps(struct xmlNode *node, int version);
int RestorePreferences(void);
void SaveProps(FILE *out, SETTING *settings, int indent);
void SavePreferences(void);

                              /* Slproj.c */

void RestoreProps(struct xmlNode *input, PROJECTITEM *root, PROJECTITEM *parent);
void RestoreProject(PROJECTITEM *project, BOOL loadWA);
BOOL HasProperties(PROJECTITEM *pj);
void SaveProfiles(FILE *out, PROJECTITEM *pj, int indent);
void SaveFiles(FILE *out, PROJECTITEM *proj, PROJECTITEM *children, int indent);
void SaveProject(PROJECTITEM *project);

                              /* Slrule.c */

SETTINGCOMBO *LoadCombo(struct xmlNode *node, int version);
SETTING *LoadItem(struct xmlNode *node, int version, BOOL debug);
SETTING *LoadPropItems(struct xmlNode *node, int version, BOOL debug);
SETTING *LoadAssignmentItems(struct xmlNode *node, int version);
SETTING *LoadCommandItems(struct xmlNode *node, int version);
SETTING *LoadDependsItems(struct xmlNode *node, int version);
BOOL iscleanable(char* id);
void FreeCombo(SETTINGCOMBO *c);
void FreeRule(SETTING *rule);
void LoadChildren(struct xmlNode *node, int version, SETTING *rvp, BOOL debug);
PROFILE *LoadRule(char *fileName);

                               /* Slwa.c */

void abspath(char *name, char *path);
char *relpath(char *name, char *path);
char *relpath2(char *name, char *path);
void absincludepath(char *name, char *path);
char *relincludepath(char *name, char *path);
PROJECTITEM *LoadErr(struct xmlNode *root, char *name);
PROJECTITEM *NoMemory(struct xmlNode *root);
void NoMemoryWS(void);
void RestoreDocks(struct xmlNode *node, int version);
void RestoreHistory(struct xmlNode *node, int version);
void RestoreWindows(struct xmlNode *node, int version, PROJECTITEM *wa);
void RestoreChangeLn(struct xmlNode *node, int version, PROJECTITEM *wa);
void RestoreFileBrowse(struct xmlNode *node, int version, PROJECTITEM *wa);
void RestoreTags(struct xmlNode *node, int version, PROJECTITEM *wa);
void RestoreFind(struct xmlNode *node, int version);
void RestoreWatch(struct xmlNode *node, int version);
void RestoreDataBreakpoints(struct xmlNode *node, int version);
void RestoreToolBars(struct xmlNode *node, int version);
void RestoreProjectNames(struct xmlNode *node, int version, PROJECTITEM *wa);
void RestoreProfileNames(struct xmlNode *node, int version, PROJECTITEM *wa);
PROJECTITEM *RestoreWorkArea(char *name);
void onehistsave(FILE *out, char **hist, char *name);
void SaveHistory(FILE *out);
void SaveChangeLn(FILE *out, PROJECTITEM *wa);
void SaveFileBrowse(FILE *out, PROJECTITEM *wa);
void saveonetag(FILE *out, int tag, PROJECTITEM *wa);
void SaveTags(FILE *out, PROJECTITEM *wa);
void SaveDocks(FILE *out);
void SaveWindows(FILE *out, PROJECTITEM *wa);
void SaveToolBarA(FILE *out, HWND hwnd);
void SaveToolBars(FILE *out);
void SaveWatchpoints(FILE *out);
void SaveDataBreakpoints(FILE *out);
void SaveProjectNames(FILE *out, PROJECTITEM *wa);
void SaveWorkArea(PROJECTITEM *wa);

                              /* Splash.c */

                             /* Srchpath.c */

FILE *searchPathI(char *string, char *searchPath, char *mode);

                               /* Tags.c */

int IsTagged(char *module, int line);
int TagAnyBreakpoints(void);
BOOL TagAnyDisabledBreakpoints(void);
int TagEnableAllBreakpoints(BOOL enableState);
int Tag(int type, char *name, int drawnLineno, int charpos, void *extra, int
    freeextra, int alwaysadd);
void TagRemoveAll(int type);
void TagRegenBreakPoints(BOOL toerror);
int TagOldLine(char *name, int lineno);
int TagNewLine(char *name, int lineno);
void TagRemoveOld(struct tagfile *l);
void TagLineChange(char *name, int drawnLineno, int delta);
void TagLinesAdjust(char *name, int mode);
void TagBreakpoint(char *module, int line);
int BPLine(char *module);
void TagEraseAllEntries(void);
BOOL AnyBookmarks(void);
void ToggleBookMark(int linenum);
int findbmpos(struct tagfile **l, struct tag **t);
void NextBookMark(void);
void NextBookMarkFile(void);
void PreviousBookMark(void);
void PreviousBookMarkFile(void);
LRESULT CALLBACK BMProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam);
void ShowBookMarks(void);

                              /* Toolbar.c */

LRESULT CALLBACK CustomizeProc(HWND hwnd, UINT iMessage, WPARAM wParam,
    LPARAM lParam);
void TBCustomize(void);
void RedrawToolBar(void);
int ToolBarDropDown(int id, int lParam);
LRESULT CALLBACK tbStatProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam);
HWND GetToolWindow(int DID);
void MakeToolBar(HWND hwnd);

                                /* unmangle.c */

char *unmangle(char *val, const char *name);

                                /* Uz.c */

void CreateCRCTab(void);
void increment_crc(unsigned char *data, int len);
void fetchbyte(void);
void shr_n_bits(int count);
unsigned get_n_bits_b(int count);
unsigned get_n_bits_w(int count);
unsigned get_n_bits(int count);
void BadTable(void);
void ExpandTables(unsigned short *dest2, unsigned short *dest, unsigned char
    *source, int len);
void GetInflateTables(void);
unsigned fancymove(short b, short *tab, int *count);
unsigned consulttabs1(void);
unsigned consulttabs2(void);
int Inflate(unsigned char *idata, unsigned char *odata);
unsigned char *InflateWrap(unsigned char *data);

                               /* Wargs.c */

int parse_args(int *argc, char *argv[], int case_sensitive);

                              /* Window.c */

void GetFrameWindowRect(RECT* r);
void SelectDebugWindows(BOOL dbgMode);
void Select(int flag);
void RegisterAllWindows(void);
void CreateAllWindows(void);
HWND GetWindowHandle(int);
LRESULT PostDIDMessage(int DID, unsigned iMessage, WPARAM wParam, LPARAM lParam);
LRESULT SendDIDMessage(int DID, unsigned iMessage, WPARAM wParam, LPARAM lParam);
int GetWindowIndex(int DID);
void RemoveWindow(int DID);
void SelectWindow(int DID);
void SelectDebugWindows(BOOL dbgMode);
void ResizeLayout(RECT *rect);
void ResizeDockWindow(HWND hwnd, int ox, int oy, int x, int y);
void DockEndDrag(HWND hwnd, int ox, int oy, int x, int y);
BOOL DockDrag(HWND hwnd, int ox, int oy, int x, int y);
void DockRemoveWindow(HWND hwnd);
HWND CreateInternalWindow(int DID, char *clsName, char *title);
void CreateDocks(void);


                             /* Xclipbrd.c */

void SelToClipboard(HWND hwnd, EDITDATA *p);
void ClipboardToSel(HWND hwnd, EDITDATA *ptr);

                             /* Xcodecomp.c */

BOOL GetContainerData(int lineno, char *file, char *ns, char *func);
CCSTRUCTDATA * GetStructType(char **name, int lineno, char *file, CCSTRUCTDATA *structData, int * indir_in);
LRESULT CALLBACK codecompProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam);
void GetCompletionPos(HWND hwnd, EDITDATA *p, LPPOINT pt, int width, int height);
int showStruct(HWND hwnd, EDITDATA *p, int ch);
KEYLIST *partialmatchkeyword(KEYLIST *table, int tabsize, char *name);
char **GetCodeCompKeywords(char *name, char **names, int *size, int *max, HWND hwnd, EDITDATA *p);
char **GetCodeCompVariables(char *name, char **names, int *size, int *max, HWND hwnd, EDITDATA *p);
void showVariableOrKeyword(HWND hwnd, EDITDATA *p);
LRESULT CALLBACK funcProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam);
void showFunction(HWND hwnd, EDITDATA *p, int ch);

                             /* Xcolorize.c */

void SendUpdate(HWND hwnd); /* PROTOTYPE */
void LoadColors(void);
void Colorize(INTERNAL_CHAR *buf, int start, int len, int color, int
    italic);                                                      
int keysym(char x);
INTERNAL_CHAR *strpstr(INTERNAL_CHAR *t, char *text, int len);
int strplen(INTERNAL_CHAR *t);
int pcmp(INTERNAL_CHAR *s, char *t, int preproc, int *retlen, int
    caseinsensitive, int bykey);
KEYLIST *matchkeyword(KEYLIST *table, int tabsize, int preproc, INTERNAL_CHAR
    *t, int *retlen, int insensitive);
int instring(INTERNAL_CHAR *buf, INTERNAL_CHAR *t1);              
void FormatBuffer(COLORIZE_HASH_ENTRY *entries[], INTERNAL_CHAR *buf, int start, int end, int type, int bkColor);
void FormatBufferFromScratch(COLORIZE_HASH_ENTRY *entries[], INTERNAL_CHAR *buf, int start, int end, int
    type, int bkColor);                                           
void FormatLine(HWND hwnd, INTERNAL_CHAR *buf, int type, int bkColor);
void FullColorize(HWND hwnd, EDITDATA *p, int creation);
void InsertColorizeEntry(COLORIZE_HASH_ENTRY *entries[], char *name, int start, int end, int type);
void FreeColorizeEntries(COLORIZE_HASH_ENTRY *entries[]);
void SyntaxCheck(HWND hWnd, EDITDATA *p);

                               /* Xedit.c */

void getPageSize(void);
int freemem(EDITDATA *p);
int allocmem(EDITDATA *p, int size);
int commitmem(EDITDATA *p, int size);
void reparse(HWND hwnd, EDITDATA *p);
int posfromchar(HWND hwnd, EDITDATA *p, POINTL *point, int pos);
int charfrompos(HWND hwnd, EDITDATA *p, POINTL *point);
void VScrollLen(HWND hwnd, int count, int set);
void VScrollPos(HWND hwnd, int count, int set);
int curcol(EDITDATA *p, INTERNAL_CHAR *text, int pos);
void setcurcol(EDITDATA *p);
void MoveCaret(HWND hwnd, EDITDATA *p);
void scrollleft(HWND hwnd, EDITDATA *p, int cols);
void scrollup(HWND hwnd, EDITDATA *p, int lines);
void ScrollCaretIntoView(HWND hwnd, EDITDATA *p, BOOL middle);
void TrackVScroll(HWND hwnd, EDITDATA *p, int end);
void TrackHScroll(HWND hwnd, EDITDATA *p, int end);
int lfchars(INTERNAL_CHAR *c, int start, int end);
int LineFromChar(EDITDATA *p, int pos);
void Replace(HWND hwnd, EDITDATA *p, char *s, int lens);
int GetLineOffset(HWND hwnd, EDITDATA *p, int chpos);
void drawline(HWND hwnd, EDITDATA *p, int chpos);                 
void insertchar(HWND hwnd, EDITDATA *p, int ch);
void insertcrtabs(HWND hwnd, EDITDATA *p); /* PROTOTYPE */
void insertcr(HWND hwnd, EDITDATA *p, BOOL tabs);
void inserttab(HWND hwnd, EDITDATA *p);
int firstword(INTERNAL_CHAR *pos, char *name);
void backtab(HWND hwnd, EDITDATA *p);
void removechar(HWND hwnd, EDITDATA *p, int utype);
void upline(HWND hwnd, EDITDATA *p, int lines);
int lineParsed(EDITDATA *p, int line);
int getfragment(EDITDATA *p, int pos, int autoDecoration, int colorizing, char *buf, int bufsz, COLORREF *fcolor, COLORREF *bcolor,
    HFONT *font, int *col, int line);
void EditPaint(HWND hwnd, EDITDATA *p);
void endofline(HWND hwnd, EDITDATA *p);
void sol(HWND hwnd, EDITDATA *p);
void leftword(HWND hwnd, EDITDATA *p);
void rightword(HWND hwnd, EDITDATA *p);
void CALLBACK AutoTimeProc(HWND hwnd, UINT message, UINT event, DWORD timex);
void StartAutoScroll(HWND hwnd, EDITDATA *p, int dir);
void EndAutoScroll(HWND hwnd, EDITDATA *p);
void HilightWord(HWND hwnd, EDITDATA *p);
void verticalCenter(HWND hwnd, EDITDATA *p);
LRESULT CALLBACK exeditProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM
    lParam);
void RegisterXeditWindow(void);

                              /* Xformat.c */

void insertcrtabs(HWND hwnd, EDITDATA *p);
int preprocline(EDITDATA *p, int pos);
void InsertBeginTabs(HWND hwnd, EDITDATA *p);
void InsertEndTabs(HWND hwnd, EDITDATA *p, int newend);
void SelectIndent(HWND hwnd, EDITDATA *p, int insert);
void SelectComment(HWND hwnd, EDITDATA *p, int insert);
void DeletePound(HWND hwnd, EDITDATA *p);
void DeletePercent(HWND hwnd, EDITDATA *p);
void upperlowercase(HWND hwnd, EDITDATA *p, int ucase);
int DeleteColonSpaces(HWND hwnd, EDITDATA *p);

                              /* Xmatch.c */

void CancelParenMatch(HWND hwnd, EDITDATA *p);
int FindParenMatchBackward(HWND hwnd, EDITDATA *p, int dec);
int FindParenMatchForward(HWND hwnd, EDITDATA *p, int dec);

                               /* Xundo.c */

UNDO *getundo(HWND hwnd, EDITDATA *p, int type);
void insertautoundo(HWND hwnd, EDITDATA *p, int type);
UNDO *undo_deletesel(HWND hwnd, EDITDATA *p);
UNDO *undo_casechange(HWND hwnd, EDITDATA *p);
UNDO *undo_insertsel(HWND hwnd, EDITDATA *p, char *s);
UNDO *undo_deletechar(HWND hwnd, EDITDATA *p, int ch, int type);
UNDO *undo_modifychar(HWND hwnd, EDITDATA *p);
UNDO *undo_insertchar(HWND hwnd, EDITDATA *p, int ch);
void undo_pchar(HWND hwnd, EDITDATA *p, int ch);
int doundo(HWND hwnd, EDITDATA *p);
int doredo(HWND hwnd, EDITDATA *p);

                               /* Xutil.c */

BOOL GetWordSpan(EDITDATA* ptr, CHARRANGE* range);
int CPPScanForward(EDITDATA* p, int pos, BOOL asExpression);
int CPPScanBackward(EDITDATA* p, int pos, BOOL asExpression);
void GetCPPWordSpan(EDITDATA* ptr, CHARRANGE* range, BOOL asExpression);
int GetWordFromPos(HWND hwnd, char *outputbuf, int fullcpp, int charpos, int *linenum, int
    *startoffs, int *endoffs);
void DoHelp(HWND edwin, int speced);                              
void ClientArea(HWND hwnd, EDITDATA *p, RECT *r);
void GetEditPopupFrame(RECT *rect);
void PopupFullScreen (HWND hwnd, EDITDATA *p);
void ReleaseFullScreen(HWND hwnd, EDITDATA *p);
void SendUpdate(HWND hwnd);
int KeyboardToAscii(int vk, int scan, BOOL fullstate);

void xdrawline(HWND hwnd, EDITDATA* p, int chpos);
void CancelParenMatch(HWND hwnd, EDITDATA* p);
int FindParenMatchBackward(HWND hwnd, EDITDATA* p, int dec);
int FindParenMatchForward(HWND hwnd, EDITDATA* p, int dec);
void FindParenMatch(HWND hwnd, EDITDATA* p);
void FindBraceMatchForward(HWND hwnd, EDITDATA* p);
void FindBraceMatchBackward(HWND hwnd, EDITDATA* p);
void FindBraceMatch(HWND hwnd, EDITDATA* p, int ch);
