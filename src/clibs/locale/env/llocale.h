typedef struct {
    char *qualname;
    unsigned char data[2000];
} COLLATE_DATA;
typedef struct {
    char *qualname;
    short flags ; // 1 = has upper/lower tables
    unsigned char ctypetab[256];
    unsigned char keys1[256];
    unsigned char keys2[256];
    
} CTYPE_DATA;
typedef struct {
    char *qualname;
    char shortdays[7][10];
    char longdays[7][20];
    char shortmonths[12][10];
    char longmonths[12][20];
    char am[10];
    char pm[10];
    char dates[2][25] ; // extended, short
    char times[2][25]; // military, 
    unsigned char ths[1000]; // sequence of null-terminated strings
} TIME_DATA;
typedef struct {
    char *qualname;
   char int_curr_symbol[5];
   char currency_symbol[5];
   char mon_decimal_point[2];
   char mon_thousands_sep[5];
   char mon_grouping[5];
   char positive_sign[5];
   char negative_sign[5];
   char int_frac_digits;
   char frac_digits;
   char p_cs_precedes;
   char p_sep_by_space;
   char n_cs_precedes;
   char n_sep_by_space;
   char p_sign_posn;
   char n_sign_posn;    
   // borland didn't have the following... not presently initialized
   char int_p_cs_precedes;
   char int_p_sep_by_space;
   char int_n_cs_precedes;
   char int_n_sep_by_space;
   char int_p_sign_posn;
   char int_n_sign_posn;    
} MONETARY_DATA;
typedef struct {
    char *qualname;
   char decimal_point[2];
   char thousands_sep[10];
   char grouping[20];
    
} NUMERIC_DATA ;
typedef struct {
    char *qualname;
    char ychar[5];
    char nchar[5];    
} MESSAGE_DATA;

typedef struct {
    char *unqual_name;
    COLLATE_DATA *cd;
    CTYPE_DATA *ct;
    MONETARY_DATA *md;
    NUMERIC_DATA *nd;
    TIME_DATA *td;
    MESSAGE_DATA *msgd;
    int *userd;
} LOCALE_HEADER ;
