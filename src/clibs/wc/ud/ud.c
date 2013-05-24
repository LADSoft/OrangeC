#define _IS_UPP     1           /* upper case */
#define _IS_LOW     2           /* lower case */
#define _IS_DIG     4           /* digit */
#define _IS_SP      8           /* space */
#define _IS_PUN    16           /* punctuation */
#define _IS_CTL    32           /* control */
#define _IS_BLK    64           /* blank */
#define _IS_HEX   128           /* [0..9] or [A-F] or [a-f] */
#define _IS_GPH   512
#include <stdio.h>

int main()
{
    FILE *in = fopen("unicodedata.txt","r"), *out;
    unsigned short *mem = calloc(0x10000,15*2);
    int i;
    mem[8] |= _IS_SP;
    mem[9] |= _IS_SP;
    mem[0xa] |= _IS_SP;
    mem[0xb] |= _IS_SP;
    mem[0xc] |= _IS_SP;
    mem[0xd] |= _IS_SP;
    mem[0x85] |= _IS_SP;
    for (i=0x30; i <= 0x39; i++)
        mem[i] |= _IS_HEX;
    for (i=0x41; i <= 0x46; i++)
        mem[i] |= _IS_HEX;
    for (i=0x61; i <= 0x66; i++)
        mem[i] |= _IS_HEX;
    for (i=0xff10; i <= 0xff19; i++)
        mem[i] |= _IS_HEX;
    for (i=0xff21; i <= 0xff26; i++)
        mem[i] |= _IS_HEX;
    for (i=0xff41; i <= 0xff46; i++)
        mem[i] |= _IS_HEX;
    mem[0x96F6          ] |= _IS_DIG ; // 0.0 # Lo       CJK UNIFIED IDEOGRAPH-96F6
    mem[0x4E00          ] |= _IS_DIG ; // 1.0 # Lo       CJK UNIFIED IDEOGRAPH-4E00
    mem[0x58F1          ] |= _IS_DIG ; // 1.0 # Lo       CJK UNIFIED IDEOGRAPH-58F1
    mem[0x58F9          ] |= _IS_DIG ; // 1.0 # Lo       CJK UNIFIED IDEOGRAPH-58F9
    mem[0x5F0C          ] |= _IS_DIG ; // 1.0 # Lo       CJK UNIFIED IDEOGRAPH-5F0C
    mem[0x4E8C          ] |= _IS_DIG ; // 2.0 # Lo       CJK UNIFIED IDEOGRAPH-4E8C
    mem[0x5169          ] |= _IS_DIG ; // 2.0 # Lo       CJK UNIFIED IDEOGRAPH-5169
    mem[0x5F0D          ] |= _IS_DIG ; // 2.0 # Lo       CJK UNIFIED IDEOGRAPH-5F0D
    mem[0x5F10          ] |= _IS_DIG ; // 2.0 # Lo       CJK UNIFIED IDEOGRAPH-5F10
    mem[0x8CAE          ] |= _IS_DIG ; // 2.0 # Lo       CJK UNIFIED IDEOGRAPH-8CAE
    mem[0x8D30          ] |= _IS_DIG ; // 2.0 # Lo       CJK UNIFIED IDEOGRAPH-8D30
    mem[0x4E09          ] |= _IS_DIG ; // 3.0 # Lo       CJK UNIFIED IDEOGRAPH-4E09
    mem[0x53C1          ] |= _IS_DIG ; // 3.0 # Lo   [3] CJK UNIFIED IDEOGRAPH-53C1..CJK UNIFIED IDEOGRAPH-53C3
    mem[0x53C2          ] |= _IS_DIG ; // 3.0 # Lo   [3] CJK UNIFIED IDEOGRAPH-53C1..CJK UNIFIED IDEOGRAPH-53C3
    mem[0x53C3          ] |= _IS_DIG ; // 3.0 # Lo   [3] CJK UNIFIED IDEOGRAPH-53C1..CJK UNIFIED IDEOGRAPH-53C3
    mem[0x5F0E          ] |= _IS_DIG ; // 3.0 # Lo       CJK UNIFIED IDEOGRAPH-5F0E
    mem[0x56DB          ] |= _IS_DIG ; // 4.0 # Lo       CJK UNIFIED IDEOGRAPH-56DB
    mem[0x8086          ] |= _IS_DIG ; // 4.0 # Lo       CJK UNIFIED IDEOGRAPH-8086
    mem[0x4E94          ] |= _IS_DIG ; // 5.0 # Lo       CJK UNIFIED IDEOGRAPH-4E94
    mem[0x4F0D          ] |= _IS_DIG ; // 5.0 # Lo       CJK UNIFIED IDEOGRAPH-4F0D
    mem[0x516D          ] |= _IS_DIG ; // 6.0 # Lo       CJK UNIFIED IDEOGRAPH-516D
    mem[0x9646          ] |= _IS_DIG ; // 6.0 # Lo       CJK UNIFIED IDEOGRAPH-9646
    mem[0x9678          ] |= _IS_DIG ; // 6.0 # Lo       CJK UNIFIED IDEOGRAPH-9678
    mem[0x4E03          ] |= _IS_DIG ; // 7.0 # Lo       CJK UNIFIED IDEOGRAPH-4E03
    mem[0x67D2          ] |= _IS_DIG ; // 7.0 # Lo       CJK UNIFIED IDEOGRAPH-67D2
    mem[0x516B          ] |= _IS_DIG ; // 8.0 # Lo       CJK UNIFIED IDEOGRAPH-516B
    mem[0x634C          ] |= _IS_DIG ; // 8.0 # Lo       CJK UNIFIED IDEOGRAPH-634C
    mem[0x4E5D          ] |= _IS_DIG ; // 9.0 # Lo       CJK UNIFIED IDEOGRAPH-4E5D
    mem[0x7396          ] |= _IS_DIG ; // 9.0 # Lo       CJK UNIFIED IDEOGRAPH-7396
    mem[0x5341          ] |= _IS_DIG ; // 10.0 # Lo       CJK UNIFIED IDEOGRAPH-5341
    mem[0x62FE          ] |= _IS_DIG ; // 10.0 # Lo       CJK UNIFIED IDEOGRAPH-62FE
    mem[0x5EFF          ] |= _IS_DIG ; // 20.0 # Lo       CJK UNIFIED IDEOGRAPH-5EFF
    mem[0x5345          ] |= _IS_DIG ; // 30.0 # Lo       CJK UNIFIED IDEOGRAPH-5345
    mem[0x4F70          ] |= _IS_DIG ; // 100.0 # Lo       CJK UNIFIED IDEOGRAPH-4F70
    mem[0x767E          ] |= _IS_DIG ; // 100.0 # Lo       CJK UNIFIED IDEOGRAPH-767E
    mem[0x964C          ] |= _IS_DIG ; // 100.0 # Lo       CJK UNIFIED IDEOGRAPH-964C
    mem[0x4EDF          ] |= _IS_DIG ; // 1000.0 # Lo       CJK UNIFIED IDEOGRAPH-4EDF
    mem[0x5343          ] |= _IS_DIG ; // 1000.0 # Lo       CJK UNIFIED IDEOGRAPH-5343
    mem[0x4E07          ] |= _IS_DIG ; // 10000.0 # Lo       CJK UNIFIED IDEOGRAPH-4E07
    mem[0x842C          ] |= _IS_DIG ; // 10000.0 # Lo       CJK UNIFIED IDEOGRAPH-842C
    mem[0x4EBF          ] |= _IS_DIG ; // 100000000.0 # Lo       CJK UNIFIED IDEOGRAPH-4EBF
    mem[0x5104          ] |= _IS_DIG ; // 100000000.0 # Lo       CJK UNIFIED IDEOGRAPH-5104
    mem[0x5146          ] |= _IS_DIG ; // 1000000000000.0 # Lo       CJK UNIFIED IDEOGRAPH-5146
    while (!feof(in)) {
        char buf[256] ;
        int end;
        buf[0] = 0;
        fgets(buf,256,in);
        if (buf[0]) {
            int id = strtol(buf, &end, 16);
            if (id < 0xf0000) {
                char *p = strchr(buf,';');
                p = strchr(p+1,';');
                p++;
                if (p[0] == 'Z') {
                    mem[id] |= _IS_SP;
                    if (p[1] == 's')
                        mem[id] |= _IS_BLK;
                } else if (p[0] == 'C') {
                    if (p[1] != 'o')
                        mem[id] |= _IS_CTL;
                } else if (p[0] == 'S' || p[0] == 'P')
                    mem[id] |= _IS_PUN ;
                else if (p[0] == 'N')
                    mem[id] |= _IS_DIG ;
                else if (p[0] == 'L') {
                    if (p[1] == 'l')
                        mem[id] |= _IS_LOW ;
                    else if (p[1] == 'u')
                        mem[id] |= _IS_UPP ;
                }
            }            
        }
    }
    fclose (in);
    in = fopen("ud.dat","r");
    while (!feof(in)) {
        char buf[256];
        int high,low;
        buf[0] = 0;
        fgets(buf,256,in);
        if (buf[0]) {
            int n = sscanf(buf,"%x..%x",&low, &high);
            if (n) {
                if (n == 1)
                    high = low;
                for(i=low; i <= high; i++)
                    if (!(mem[i] & _IS_SP))
                        mem[i] |= _IS_GPH ;
            }
        }
    }
    fclose(in);
    memset(mem+0x10000, 0, 0x40000);
    in = fopen("casefolding.txt","r");
    while (!feof(in)) {
        char buf[256];
        buf[0] = 0;
        fgets(buf,256,in);
        if (buf[0]) {
            char select;
            int upper,lower;
            int n = sscanf(buf,"%x; %c; %x",&upper,&select, &lower);
            if (n == 3 && upper < 0x10000 && lower < 0x10000)
                if (select == 'C' || select == 'S') {
                    if ((mem[upper] && _IS_UPP) && (mem[lower] && _IS_LOW)) {
                        if (upper < 256 && lower < 256 || upper >= 256 && lower >= 256) {
                            mem[0x10000 + upper] = lower;
                            mem[0x20000 + lower] = upper;
                        }
                    }
                }
        }
    }
    out = fopen("ud.dt2","wb");
    fwrite(mem,1,6 * 0x10000,out);
    fclose(out);
}
