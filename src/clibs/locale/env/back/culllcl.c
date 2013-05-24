#include <stdio.h>
#define FALSE 0
#define TRUE 1

static char qualified_name[256];
struct lconv {

   char *decimal_point;
   char *thousands_sep;
   char *grouping;
   char *int_curr_symbol;
   char *currency_symbol;
   char *mon_decimal_point;
   char *mon_thousands_sep;
   char *mon_grouping;
   char *positive_sign;
   char *negative_sign;
   char int_frac_digits;
   char frac_digits;
   char p_cs_precedes;
   char p_sep_by_space;
   char n_cs_precedes;
   char n_sep_by_space;
   char p_sign_posn;
   char n_sign_posn;
};
#define _IS_SP     1           /* space */
#define _IS_DIG    2           /* digit */
#define _IS_UPP    4           /* upper case */
#define _IS_LOW    8           /* lower case */
#define _IS_HEX   16           /* [0..9] or [A-F] or [a-f] */
#define _IS_CTL   32           /* control */
#define _IS_PUN   64           /* punctuation */
#define _IS_BLK  128           /* blank */

char *cts[8]= {"_IS_SP ","_IS_DIG","_IS_UPP","_IS_LOW","_IS_HEX","_IS_CTL","_IS_PUN","_IS_BLK"};
struct nmstr {
    struct nmstr *next ;
    struct nmdata {
        char name[0x20];
        short unk;
        int inofs;
    } data ;
} *catlist;

struct collatedata {
    short unk;
    short len;
    char name[32];
    short unk1;
    unsigned char data[20000];
} ;
struct ctypehdr {
    short unk0;
    short reclen;
    char name[0x20]; // subclass/char set
    short unk; // 1 = all data, 202 = duplicate char set;
    char unk1;
    unsigned char ctypetab[256];
    unsigned char keys1[256];
    unsigned char keys2[256];
    
};
struct names {
    short unk ;
    short len;
    char name[0x20];
    short unk1 ;
    char shortdays[7][10];
    char longdays[7][20];
    char shortmonths[12][10];
    char longmonths[12][20];
    char am[10];
    char pm[10];
    char dates[2][25] ; // extended, short
    char times[2][25]; // military, 
    char reserved[75]; // unknown   
    unsigned char ths[1000]; // sequence of null-terminated strings
} ;
struct monetarylocaledata {
    short unk;
    short len;
    char name[32];
    short unk1;
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
   // borland didn't have the following...
#ifdef XXXXX
   char int_p_cs_precedes;
   char int_p_sep_by_space;
   char int_n_cs_precedes;
   char int_n_sep_by_space;
   char int_p_sign_posn;
   char int_n_sign_posn;    
#endif
} ;
struct numericlocaledata {
    short unk;
    short len;
    char name[32];
    short unk1;
   char decimal_point[2];
   char thousands_sep[10];
   char grouping[20];
    
};
struct messagedata {
    short unk;
    short len;
    char name[32];
    short unk1;
    char ychar[5];
    char nchar[5];    
};
#define LC_ALL      0
#define LC_COLLATE  1
#define LC_CTYPE    2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME     5
#define LC_MESSAGES 6
#define LC_userdef  7
#define LC_LAST     LC_userdef
struct classhdr {
    char name[0x20]; // just the clas, not the char set
    char unk[0x12];
    struct ctypehdr *ct; // ctype
    void *collate; // collate
    struct names *nmstr; // time
    struct monetarylocaledata *ld1; // monetary
    struct numericlocaledata *ld2; // numeric
    struct messagedata *md; // messages
    void *next; // userdef
    short unk2;    
};
void putch(FILE *out, unsigned char c)
{
        if (c < 32 || c > 126 || c == '"')
            fprintf(out,"\\%03o",c);
        else
            fprintf(out,"%c",c);
}
void putstr(FILE *out, unsigned char *s)
{
    fprintf(out,"\t\"");
    while (*s) {
        putch(out,*s++);
    }
    fprintf(out,"\",\n");
}
void putstr2(FILE *out, unsigned char *s)
{
    fprintf(out,"\t\"");
    while (*s) {
        putch(out,*s++);
    }
    putch(out,0);
    fprintf(out,"\"\n");
}
void dumpcollate(FILE *in, FILE *out, struct collatedata *cd)
{
    int len = cd->len - 36,i,j;
    fprintf(out,"static COLLATE_DATA %s_collate_data= {\n\t&%s_qualified_name,\n\t{\n\t",qualified_name,qualified_name);
    for (i=0; i < len; i+=8) {
        for (j=0; j<8; j++) {
            if (i + j >= len)
                break;
            fprintf(out,"0x%02X, ",cd->data[i+j]);
        }
        fprintf(out,"\n\t");
    }
    fprintf(out,"\n\t}\n};\n");
}
void dumpnames(FILE *in, FILE *out, struct names *nm)
{
    int i,j;
    char *p;
    fprintf(out,"static TIME_DATA %s_time_data= {\n\t&%s_qualified_name,\n",qualified_name,qualified_name);
    for (i=0; i < 7; i++)
            putstr(out,nm->shortdays[i]);
    for (i=0; i < 7; i++)
            putstr(out,nm->longdays[i]);
    for (i=0; i < 12; i++)
            putstr(out,nm->shortmonths[i]);
    for (i=0; i < 12; i++)
            putstr(out,nm->longmonths[i]);
    putstr(out,nm->am);
    putstr(out,nm->pm);
    for (i=0; i < 4; i++)
            putstr(out,nm->dates[i]);
    p = nm->ths;
    fprintf(out,"\n\t{\n");
    while (*p) {    
        putstr2(out,p);
        p += strlen(p)+1;
    }
    fprintf(out,"\t}\n");
    fprintf(out,"};\n");
}
void putct(FILE *out, int val)
{
    int i;
    int started = FALSE ;
    for (i=0; i <8;i++) {
        if (val & (1 << i)) {
            if (started)
                fprintf(out,"|");
            started = TRUE ;
            fprintf(out,"%s",cts[i]);
        }
    }
    if (val == 0)
        fprintf(out,"%d",0);
}
void dumpct(FILE *in, FILE *out, struct ctypehdr *ct)
{
    int i,j;
    fprintf(out,"static CTYPE_DATA %s_ctype_data= {\n\t&%s_qualified_name,1,\n\t{\n\t",qualified_name,qualified_name);
    for (i=0; i < 256; i+=8) {
        for (j=0; j < 8; j++) {
            putct(out, ct->ctypetab[i+j]);
            fprintf(out,", ");
        }
        fprintf(out,"\n\t");
    }
    fprintf(out,"},\n\t");
    fprintf(out,"{\n\t");
    fprintf(out,"//tolower\n\t");
    for (i=0; i < 256; i+=16) {
        fprintf(out,"\"");
        for (j=0; j < 16; j++) {
            putch(out,ct->ctypetab[i+j+256]);
        }
        fprintf(out,"\"\n\t");
    }
    fprintf(out,"},\n\t");
    fprintf(out,"{\n\t");
    fprintf(out,"//toupper\n\t");
    for (i=0; i < 256; i+=16) {
        fprintf(out,"\"");
        for (j=0; j < 16; j++) {
            putch(out,ct->ctypetab[i+j+512]);
        }
        fprintf(out,"\"\n\t");
    }
    fprintf(out,"},\n\t");
    fprintf(out,"\n};\n");
}
void dumpld1(FILE *in, FILE *out, struct monetarylocaledata *ld1)
{
    fprintf(out,"static MONETARY_DATA %s_monetary_data= {\n\t&%s_qualified_name,\n",qualified_name,qualified_name);
    putstr(out,ld1->int_curr_symbol);
    putstr(out,ld1->currency_symbol);
    putstr(out,ld1->mon_decimal_point);
    putstr(out,ld1->mon_thousands_sep);
    putstr(out,ld1->mon_grouping);
    putstr(out,ld1->positive_sign);
        putstr(out,ld1->negative_sign);
       fprintf(out,"\t0x%02X,", ld1->int_frac_digits);
       fprintf(out,"0x%02X,", ld1->frac_digits);
       fprintf(out,"0x%02X,", ld1->p_cs_precedes);
       fprintf(out,"0x%02X,", ld1->p_sep_by_space);
       fprintf(out,"0x%02X,", ld1->n_cs_precedes);
       fprintf(out,"0x%02X,", ld1->n_sep_by_space);
       fprintf(out,"0x%02X,", ld1->p_sign_posn);
       fprintf(out,"0x%02X,\n", ld1->n_sign_posn);
    fprintf(out,"};\n");
}
void dumpld2(FILE *in, FILE *out, struct numericlocaledata *ld2)
{
    int i,j;
    fprintf(out,"static NUMERIC_DATA %s_numeric_data= {\n\t&%s_qualified_name\n,",qualified_name,qualified_name);
    putstr(out,ld2->decimal_point);
    putstr(out,ld2->thousands_sep);
    putstr(out,ld2->grouping);
    fprintf(out,"};\n");
}
void dumpmd(FILE *in, FILE *out, struct messagedata *md)
{
    int i,j;
    fprintf(out,"static MESSAGE_DATA %s_message_data = {\n\t&%s_qualified_name,\n",qualified_name,qualified_name);
    putstr(out,md->ychar);
    putstr(out,md->nchar);
    fprintf(out,"};\n");
}
void dumpud(FILE *in, FILE *out)
{
    int i,j;
    fprintf(out,"static USERDEF_DATA %s_userdef_data = {\n\t&%s_qualified_name, 0,\n",qualified_name,qualified_name);
    fprintf(out,"};\n");
}
void dumphdr(FILE *in, FILE *out, struct classhdr *hdr)
{

    struct ctypehdr ct;
    struct names nm;
    struct monetarylocaledata ld1;
    struct numericlocaledata ld2;
    struct messagedata md;
    struct collatedata collate;
    char name[256],*p;
        strcpy(name,hdr->name);
        while (p = strchr(name,'.'))
            *p = '_';
    fprintf(out,"static char %s_unqualified_name[] = \"%s\";\n\n",qualified_name,hdr->name);
    fseek(in,(int)hdr->collate,SEEK_SET); //
    fread(&collate,1,sizeof(struct collatedata),in);
    dumpcollate(in,out,&collate);
    fseek(in,(int)hdr->ct,SEEK_SET); //
    fread(&ct,1,sizeof(ct),in);
    dumpct(in,out,&ct);
    fseek(in,(int)hdr->nmstr,SEEK_SET);
    fread(&nm,1,sizeof(nm),in);
    dumpnames(in,out,&nm);
    fseek(in,(int)hdr->ld1,SEEK_SET);
    fread(&ld1,1,sizeof(ld1),in);
    dumpld1(in,out,&ld1);
    fseek(in,(int)hdr->ld2,SEEK_SET);
    fread(&ld2,1,sizeof(ld2),in);
    dumpld2(in,out,&ld2);
    fseek(in,(int)hdr->md,SEEK_SET);
    fread(&md,1,sizeof(md),in);
    dumpmd(in,out,&md);
    dumpud(in,out);
    fprintf(out,"LOCALE_HEADER %s_data = {\n",qualified_name);
    fprintf(out,"\t&%s_unqualified_name,\n",qualified_name);
    fprintf(out,"\t&%s_qualified_name,\n",qualified_name);
    fprintf(out,"\t&%s_collate_data,\n",qualified_name);
    fprintf(out,"\t&%s_ctype_data,\n",qualified_name);
    fprintf(out,"\t&%s_monetary_data,\n",qualified_name);
    fprintf(out,"\t&%s_numeric_data,\n",qualified_name);
    fprintf(out,"\t&%s_time_data,\n",qualified_name);
    fprintf(out,"\t&%s_message_data,\n",qualified_name);
    fprintf(out,"\t&%s_userdef_data\n};\n",qualified_name);
}           
void dump(FILE *in, FILE *out, struct nmdata *data)
{
    struct classhdr header;
    fseek(in,data->inofs,SEEK_SET);
    fread(&header,1,sizeof(header),in);
    dumphdr(in,out,&header);
}
main()
{
    struct nmstr *look;
    int i;
    short sh;
    FILE *in = fopen("LOCALE.BLL","rb");
    fseek(in,0x24,SEEK_SET);
    fread(&sh,1,2,in);
    for (i=0;i < sh; i++) {
    
        struct nmstr *p = calloc(sizeof(struct nmstr),1),**find = &catlist;
        fread(&p->data,1,sizeof(p->data),in);
        while (*find)
            find = &(*find)->next;
        *find = p;
    }
    look = catlist;
    while (look) {
        char *p,name[256];
        FILE *out;
        strcpy(qualified_name,look->data.name);
        while (p = strchr(qualified_name,'.'))
            *p = '_';
        strcpy(name,qualified_name);
        strcat(name,".env");
        out = fopen(name,"w");
        fprintf(out,"static char %s_qualified_name[] = \"%s\";\n",qualified_name,look->data.name);
        dump(in,out,&look->data);
        fclose(out);
        look = look->next;
    }
    fclose(in) ;
}