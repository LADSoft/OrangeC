#include <windows.h>
#include <stdio.h>
#include "resource.h"

char fruit[256] = "orange";
char veggi[256] = "lima beans";

LRESULT CALLBACK DlgProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage)
    {
        case WM_INITDIALOG:
            SendDlgItemMessage(hwnd, IDC_FRUIT, CB_ADDSTRING, 0, (LPARAM)"apple");
            SendDlgItemMessage(hwnd, IDC_FRUIT, CB_ADDSTRING, 0, (LPARAM)"orange");
            SendDlgItemMessage(hwnd, IDC_FRUIT, CB_ADDSTRING, 0, (LPARAM)"pear");
            SendDlgItemMessage(hwnd, IDC_FRUIT, CB_ADDSTRING, 0, (LPARAM)"banana");
            SendDlgItemMessage(hwnd, IDC_FRUIT, CB_SELECTSTRING, 0, (LPARAM)fruit);
            SendDlgItemMessage(hwnd, IDC_VEGGI, WM_SETTEXT, 0, (LPARAM)veggi);
            SendDlgItemMessage(hwnd, IDC_VEGGI, EM_SETLIMITTEXT, sizeof(veggi), 0);
            break;
        case WM_COMMAND:
            switch (wParam)
            {
                case IDCANCEL:
                    EndDialog(hwnd, 0);
                    break;
                case IDOK:
                {
                    int v = SendDlgItemMessage(hwnd, IDC_FRUIT, CB_GETCURSEL, 0, 0);
                    if (v != CB_ERR)
                    {
                        SendDlgItemMessage(hwnd, IDC_FRUIT, CB_GETLBTEXT, v, (LPARAM)fruit);
                        SendDlgItemMessage(hwnd, IDC_VEGGI, WM_GETTEXT, sizeof(veggi), (LPARAM)veggi);
                        EndDialog(hwnd, 1);
                    }
                    else
                    {
                        MessageBeep(MB_ICONASTERISK);
                    }
                    break;
                }
            }
            break;
    }
    return 0;
}
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszCmdLine,
    int nCmdShow)
{
    FILE *fil = fopen("aa.dat","r");
    if (fil)
    {
        fgets(fruit, 256, fil);
        if (strlen(fruit) && fruit[strlen(fruit)-1] < ' ')
            fruit[strlen(fruit)-1] = 0;
        fgets(veggi, 256, fil);
        if (strlen(veggi) && veggi[strlen(veggi)-1] < ' ')
            veggi[strlen(veggi)-1] = 0;
        fclose(fil);
    }
    if (DialogBox(hInst,MAKEINTRESOURCE(IDD_TEST),NULL, (DLGPROC)DlgProc))
    {
        fil = fopen("aa.dat","w");
        if (fil)
        {
            fprintf(fil, "%s\n%s\n", fruit, veggi);
            fclose(fil);
            MessageBox(0, "Data file written", "ok", MB_OK);
        }
    }
    return 0;
}
