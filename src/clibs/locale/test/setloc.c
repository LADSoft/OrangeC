#include <locale.h>

void putch(unsigned char c)
{
        if (c < 32 || c > 126 || c == '"')
            printf("\\%03o",c);
        else
            printf("%c",c);
}
void putstr(unsigned char *s)
{
    printf("\t\"");
    while (*s) {
        putch(*s++);
    }
    printf("\",\n");
}
void putnum(char n)
{
    printf("%d, ",n);
}
void showlocale(void)
{
    struct lconv *conv = localeconv();
    printf("LOCALE\n");
    putstr(conv->decimal_point);
    putstr(conv->thousands_sep);
    putstr(conv->grouping);
    putstr(conv->int_curr_symbol);
    putstr(conv->currency_symbol);
    putstr(conv->mon_decimal_point);
    putstr(conv->mon_thousands_sep);
    putstr(conv->mon_grouping);
    putstr(conv->positive_sign);
    putstr(conv->negative_sign);
    
    putnum(conv->int_frac_digits);
    putnum(conv->frac_digits);
    putnum(conv->p_cs_precedes);
    putnum(conv->p_sep_by_space);
    putnum(conv->n_cs_precedes);
    putnum(conv->n_sep_by_space);
    putnum(conv->p_sign_posn);
    putnum(conv->n_sign_posn);
    putnum(conv->int_p_cs_precedes); // CHAR_MAX
    putnum(conv->int_n_cs_precedes); // CHAR_MAX
    putnum(conv->int_p_sep_by_space); // CHAR_MAX
    putnum(conv->int_n_sep_by_space); // CHAR_MAX
    putnum(conv->int_p_sign_posn); // CHAR_MAX
    putnum(conv->int_n_sign_posn); // CHAR_MAX};
}
main()
{
    char data[256];
    char *bf = setlocale(LC_NUMERIC,"en_US");
    printf("%s\n",bf);
    bf = setlocale(LC_CTYPE,"de_DE.WIN1252");
    printf("%s\n",bf);
    bf = setlocale(LC_ALL,0);
    printf("%s\n",bf);
    strcpy(data,bf);
    bf = setlocale(LC_ALL,"C");
    printf("%s\n",bf);
    bf = setlocale(LC_ALL,data);
    printf("%s\n",bf);
    bf = setlocale(LC_ALL,0);
    printf("%s\n",bf);
    bf = setlocale(LC_ALL,"fr_FR");
    printf("%s\n",bf);
    bf = setlocale(LC_NUMERIC,data);
    printf("%s\n",bf);
    bf = setlocale(LC_ALL,0);
    printf("%s\n",bf);
    bf = setlocale(LC_NUMERIC,"C;C");
    printf("%s\n",bf);
    bf = setlocale(LC_ALL,"en_GB;en_GB");
    printf("%s\n",bf);
    bf = setlocale(LC_ALL,"C");
    bf = setlocale(LC_ALL,"");
    bf = setlocale(LC_NUMERIC,"");
    bf = setlocale(LC_COLLATE,"");
    bf = setlocale(LC_ALL,0);
    printf("%s\n",bf);
    setlocale(LC_ALL,"de_DE");
    showlocale    ();
}