#include <ctype.h>
#include <locale.h>
#include "_locale.h"
extern LOCALE_HEADER de_DE_DOS437_data;
extern LOCALE_HEADER en_GB_DOS437_data;
extern LOCALE_HEADER en_US_DOS437_data;
extern LOCALE_HEADER fr_FR_DOS437_data;
extern LOCALE_HEADER de_DE_DOS850_data;
extern LOCALE_HEADER en_GB_DOS850_data;
extern LOCALE_HEADER en_US_DOS850_data;
extern LOCALE_HEADER fr_FR_DOS850_data;
extern LOCALE_HEADER de_DE_WIN1252_data;
extern LOCALE_HEADER en_GB_WIN1252_data;
extern LOCALE_HEADER en_US_WIN1252_data;
extern LOCALE_HEADER fr_FR_WIN1252_data;

int len = 12;
void *data[12] = {
    &de_DE_DOS437_data,
    &en_GB_DOS437_data,
    &en_US_DOS437_data,
    &fr_FR_DOS437_data,
    &de_DE_DOS850_data,
    &en_GB_DOS850_data,
    &en_US_DOS850_data,
    &fr_FR_DOS850_data,
    &de_DE_WIN1252_data,
    &en_GB_WIN1252_data,
    &en_US_WIN1252_data,
    &fr_FR_WIN1252_data,
};
#include "de_DE_DOS437.env"
#include "en_GB_DOS437.env"
#include "en_US_DOS437.env"
#include "fr_FR_DOS437.env"
#include "de_DE_DOS850.env"
#include "en_GB_DOS850.env"
#include "en_US_DOS850.env"
#include "fr_FR_DOS850.env"
#include "de_DE_WIN1252.env"
#include "en_GB_WIN1252.env"
#include "en_US_WIN1252.env"
#include "fr_FR_WIN1252.env"
