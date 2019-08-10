/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

unsigned CRCtab[256];
unsigned currentcrc;

unsigned char masktab[9] = {0, 1, 3, 7, 0xf, 0x1f, 0x3f, 0x7f, 0xff};
unsigned char tab7_rearrange[0x13] = {0x10, 0x11, 0x12, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
unsigned accum, accum_count;
int inflatepos = 0;

jmp_buf inflate_sync, inflate_badtab;

char inflate_tab_1[0x140];
unsigned char inflate_tab_2[32];
short inflate_tab_3[256];
short inflate_tab_4[256];
unsigned short inflate_tab_5[0x240];
unsigned short inflate_tab_6[64];
unsigned char inflate_tab_7[0x13];
unsigned short inflate_tab_8[0x116];
unsigned short intermed_tab[17];
unsigned short intermed_tab_2[17];
unsigned short intermed_tab_3[0x140];
int if1_count, if2_count;
int outputPos = 0;
unsigned char *inflatebuf, *outputQueue;
void CreateCRCTab(void)
{
    int i;
    for (i = 0; i < 256; i++)
    {
        unsigned k = i, j;
        for (j = 0; j < 8; j++)
        {
            if (k & 1)
            {
                k >>= 1;
                k ^= 0xEdB88320;
            }
            else
                k >>= 1;
        }
        CRCtab[i] = k;
    }
}

//-------------------------------------------------------------------------

void increment_crc(unsigned char* data, int len)
{
    int i;
    for (i = 0; i < len; i++)
        currentcrc = (currentcrc >> 8) ^ CRCtab[*data++ ^ (unsigned char)currentcrc];
}

//-------------------------------------------------------------------------

void fetchbyte(void)
{
    accum |= (inflatebuf[inflatepos++]) << 8;
    accum_count = 8;
}

//-------------------------------------------------------------------------

void shr_n_bits(int count)
{
    if (count >= accum_count)
    {
        accum >>= accum_count;
        count = count - accum_count;
        fetchbyte();
    }
    accum >>= count;
    accum_count -= count;
}

//-------------------------------------------------------------------------

unsigned get_n_bits_b(int count)
{
    int rv = accum & masktab[count];
    shr_n_bits(count);
    return rv;
}

//-------------------------------------------------------------------------

unsigned get_n_bits_w(int count)
{
    int rv = get_n_bits_b(8);
    rv += (get_n_bits_b(count - 8) << 8);
    return rv;
}

//-------------------------------------------------------------------------

unsigned get_n_bits(int count)
{
    if (count > 8)
        return get_n_bits_w(count);
    else
        return get_n_bits_b(count);
}

//-------------------------------------------------------------------------

void BadTable(void) { longjmp(inflate_badtab, 1); }

//-------------------------------------------------------------------------

void ExpandTables(unsigned short* dest2, unsigned short* dest, unsigned char* source, int len)
{
    int i;
    short sum = 0, xlen = 0;
    unsigned short* idest;
    if (!len)
        return;
    memset(intermed_tab, 0, sizeof(intermed_tab));
    for (i = 0; i < len; i++)
        intermed_tab[source[i]]++;
    intermed_tab_2[1] = 0;
    for (i = 1; i < 16; i++)
    {
        sum += intermed_tab[i];
        sum <<= 1;
        intermed_tab_2[i + 1] = sum;
    }
    if (sum)
    {
        sum = 0;
        for (i = 1; i < 16; i++)
            sum += intermed_tab[i];
        if (sum > 1)
        {
            BadTable();
        }
    }
    for (i = 0; i < len; i++)
        if (source[i] == 0)
            intermed_tab_3[i] = 0;
        else
            intermed_tab_3[i] = intermed_tab_2[source[i]]++;

    idest = intermed_tab_3;
    for (i = 0; i < len; i++)
    {
        sum = source[i] - 1;
        if (sum <= 0)
            idest++;
        else
        {
            short shift = *idest;
            short shift1 = 0;
            do
            {
                shift1 <<= 1;
                if (shift & 1)
                    shift1 |= 1;
                shift >>= 1;
            } while (--sum && shift != 0);
            shift1 <<= 1;
            if (shift & 1)
                shift1 |= 1;
            shift1 <<= sum;
            *(idest)++ = shift1;
        }
    }
    memset(dest, 0, 256 * 2);
    idest = intermed_tab_3;
    source += len - 1;
    idest += len - 1;
    for (i = len - 1; i >= 0; i--)
    {
        if ((sum = *source--))
        {
            if (sum > 8)
            {
                short* xdest = (short*)(dest + (*idest & 0xff));
                int j, shift = 1, mask = *idest >> 8;
                sum -= 8;
                for (j = 0; j < sum; j++)
                {
                    if (*xdest == 0)
                    {
                        *xdest = ~xlen;
                        dest2[xlen] = dest2[xlen + 1] = 0;
                        xlen += 2;
                    }
                    xdest = (short*)(~*xdest + dest2);
                    if (mask & shift)
                        xdest++;
                    shift <<= 1;
                }
                *xdest = i;
            }
            else
            {
                int pos = *idest;
                sum = 1 << sum;
                do
                {
                    dest[pos] = i;
                    pos += sum;
                } while (pos < 0x100);
            }
        }
        idest--;
    }
}

//-------------------------------------------------------------------------

void GetInflateTables(void)
{
    int v = get_n_bits(2);
    int count, scount, i, len, val, shift;
    unsigned char* xdest;
    switch (v)
    {
        case 0:
            /* copy data in the clear */
            if (accum_count != 8)
                shr_n_bits(accum_count);
            count = accum;
            scount = inflatebuf[inflatepos++];
            scount |= inflatebuf[inflatepos++] << 8;
            if ((((count ^ scount) + 1) & 0xffff) == 0)
            {
                do
                {

                    outputQueue[outputPos++] = inflatebuf[inflatepos++];
                } while (--count);
                longjmp(inflate_sync, 1);
            }
            else
            {
                BadTable();
            }

            break;
        case 1:
            /* default tabs */
            memset(inflate_tab_1, 8, 0x90);
            memset(inflate_tab_1 + 0x90, 9, 0x70);
            memset(inflate_tab_1 + 0x100, 7, 0x18);
            memset(inflate_tab_1 + 0x118, 8, 0x8);
            if2_count = 32;
            memset(inflate_tab_2, 5, 32);
            if1_count = 0x120;
            break;
        case 2:
            /* compressed tabs */
            if1_count = get_n_bits(5) + 0x101;
            if2_count = get_n_bits(5) + 1;
            memset(inflate_tab_7, 0, 0x13);
            len = get_n_bits(4) + 4;
            for (i = 0; i < len; i++)
                inflate_tab_7[tab7_rearrange[i]] = get_n_bits(3);
            ExpandTables((unsigned short*)tab7_rearrange + len, inflate_tab_8, inflate_tab_7, 0x13);
            xdest = (unsigned char*)inflate_tab_1;
            for (i = if1_count + if2_count; i > 0;)
            {
                val = inflate_tab_8[accum & 0xff];
                shr_n_bits(inflate_tab_7[val]);
                if (val < 0x10)
                {
                    *xdest++ = val;
                    i--;
                }
                else
                {
                    switch (val)
                    {
                        case 0x10:
                            val = get_n_bits(2) + 3;
                            shift = *(xdest - 1);
                            break;
                        case 0x11:
                            val = get_n_bits(3) + 3;
                            shift = 0;
                            break;
                        default:
                            val = get_n_bits(7) + 11;
                            shift = 0;
                            break;
                    }
                    if ((i -= val) < 0)
                    {
                        BadTable();
                    }
                    memset(xdest, shift, val);
                    xdest += val;
                }
            }
            memcpy(inflate_tab_2, inflate_tab_1 + if1_count, if2_count);
            break;
        case 3:
            BadTable();
            break;
    }
    ExpandTables(inflate_tab_5, (unsigned short*)inflate_tab_3, (unsigned char*)inflate_tab_1, if1_count);
    ExpandTables(inflate_tab_6, (unsigned short*)inflate_tab_4, (unsigned char*)inflate_tab_2, if2_count);
}

//-------------------------------------------------------------------------

unsigned fancymove(short b, short* tab, int* count)
{
    short c;
    *count = 0;
    shr_n_bits(8);
    c = accum & 0xff;
    do
    {
        b = ~b;
        (*count)++;
        b &= ~1;
        if (c & 1)
            b |= 1;
        c >>= 1;
    } while ((b = tab[b]) < 0);
    return b;
}

//-------------------------------------------------------------------------

unsigned consulttabs1(void)
{
    short b = (accum & 0xff);
    int count, shift;
    if ((b = inflate_tab_3[b]) >= 0)
        shr_n_bits(count = inflate_tab_1[b]);
    else
    {
        b = fancymove(b, (short*)inflate_tab_5, &count);
        shr_n_bits(count);
    }
    if (b < 0x109)
        return b;
    if (b == 0x11d)
    {
        return 0x200;
    }
    b -= 0x101;
    shift = (b >> 2) - 1;
    b = (((b & 3) + 4) << shift) + 0x101;
    b += get_n_bits(shift);
    return b;
}

//-------------------------------------------------------------------------

unsigned consulttabs2(void)
{
    short b = (accum & 0xff);
    int count, shift;
    if ((b = inflate_tab_4[b]) >= 0)
        shr_n_bits(count = inflate_tab_2[b]);
    else
    {
        b = fancymove(b, (short*)inflate_tab_6, &count);
        shr_n_bits(count);
    }
    if ((b & 0xff) >= 4)
    {
        shift = (b & 0xff) / 2 - 1;
        b = ((b & 1) + 2) << shift;
        b += get_n_bits(shift);
    }
    return b;
}

//-------------------------------------------------------------------------

int Inflate(unsigned char* idata, unsigned char* odata)
{
    int i;

    unsigned matchword = 0;

    inflatebuf = idata;
    outputQueue = odata;
    inflatepos = outputPos = 0;
    CreateCRCTab();

    if (setjmp(inflate_badtab))
    {
        printf("Bad Table");
        return 0;
    }
    setjmp(inflate_sync);

    accum = inflatebuf[inflatepos++];
    accum |= inflatebuf[inflatepos++] << 8;
    accum_count = 8;

    while (matchword == 0)
    {
        matchword <<= 1;
        matchword |= accum & 1;
        shr_n_bits(1);
        GetInflateTables();
        while (1)
        {
            int val = consulttabs1(), b;
            if (!(val & 0xff00))
            {
                outputQueue[outputPos++] = val & 0xff;
            }
            else
            {
                char* src;
                if (val == 0x100)
                    break;
                val -= 0xfe;
                b = consulttabs2();
                src = (char*)(outputQueue + outputPos - 1 - b);
                for (i = 0; i < val; i++)
                {
                    outputQueue[outputPos++] = *src++;
                }
            }
        }
    }
    return 1;
}

//-------------------------------------------------------------------------

unsigned char* InflateWrap(unsigned char* data)
{
    int outsize = *(int*)(data + 22);
    unsigned char* buf = calloc(outsize, 1);
    if (!buf)
        return 0;
    if (Inflate(data + 30 + *(int*)(data + 26), buf))
        return buf;
    return 0;
}
