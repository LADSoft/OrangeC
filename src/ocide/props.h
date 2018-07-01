#define PR_END 0
#define PR_TEXT 1
#define PR_COMBO 2
#define PR_CHECKBOX 3
#define PR_FONT 4

typedef struct propctl
{
    HWND hwndCtl;
} PROPCTL;
typedef struct proprow
{
    int rowtype;
    char* rowName;
    void* rowData;
    PROPCTL* controlInfo;
} PROPROW;

typedef struct propdesc
{
    PROPROW* rows;
    void* userData;
} PROPDESC;