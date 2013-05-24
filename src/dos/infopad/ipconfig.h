#define CFGFILE "infopad.cfg"
#include <string>

class TMenuItem;

class MRUItem
{
public:
    MRUItem(char *Name, char *Title)
    {
        strcpy(name, Name);
        strcpy(title, Title);
    }
    char title[256];  
    char name[MAX_PATH];
};
class MRU
{
public:
    enum { MAX_MRU = 5, MAX_WINDOW_MRU=100 };

    MRU() : limit(MAX_MRU), count(0), insertAt(NULL), trailer(NULL) { }
    ~MRU();
    void Add(char *name, char *title, bool atEnd = false);
    void Remove(char *name);
    MRUItem *Get(int index);
    void MoveToTop(int index);
    void ToMenu(TMenuItem *menu, TMenuItem *Trailer = NULL);
          
private:
    MRUItem *items[MAX_WINDOW_MRU];
    int limit;
    int count;    
    TMenuItem *insertAt;
    TMenuItem *trailer;
};
class InfoPadCfg
{
public:
    InfoPadCfg() : toColorize(true), toAutoIndent(true), tabsAsSpaces(true),
        toAutoFormat(true), tabIndent(4), toMatchParenthesis(true), toBackupFiles(true),
        toShowLineNumbers(true)
        { Load(); }

    ~InfoPadCfg() { if (changed) Save(); }
    
    MRU &FileMRU() { return fileMRU; }
    bool IsToColorize() { return toColorize; }
    void SetToColorize(bool value) { toColorize = value; changed = true; }
    bool IsToAutoIndent() { return toAutoIndent; }
    void SetToAutoIndent(bool value) { toAutoIndent = value; changed = true; }
    bool IsTabsAsSpaces() { return tabsAsSpaces; }
    void SetTabsAsSpaces(bool value) { tabsAsSpaces = value; changed = true; }
    bool IsToAutoFormat() { return toAutoFormat; }
    void SetToAutoFormat(bool value) { toAutoFormat = value; changed = true; }
    int  TabIndent()      { return tabIndent; }
    void SetTabIndent(int value)      { tabIndent = value; changed = true; }
    bool IsToMatchParenthesis() { return toMatchParenthesis; }
    void SetToMatchParenthesis(bool value) { toMatchParenthesis = value; changed = true; }
    bool IsToBackupFiles() { return toBackupFiles; }
    void SetToBackupFiles(bool value) { toBackupFiles = value; changed = true; }
    bool IsToShowLineNumbers() { return toShowLineNumbers; }
    void SetToShowLineNumbers(bool value) { toShowLineNumbers = value; changed = true; }

    void SetChanged() { changed = true; }    
protected:
    void Load();
    void Save();
    std::string GetFileName();
    
private:

    bool toColorize;
    bool toAutoIndent;
    bool tabsAsSpaces;
    bool toAutoFormat;
    int tabIndent;
    bool toMatchParenthesis;
    bool toBackupFiles;
    bool toShowLineNumbers;
    
    bool changed;
    MRU fileMRU;
} ;