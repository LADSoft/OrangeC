/* this program does huffman encoding and decoding, with the symbol type being BYTE.
 * the algorithms are the basic algorithms as given on e.g. wikipedia, but for simplicity
 * it stores the file length in the output file instead of having a 257th 'end of file'
 * symbol.   Although it wouldn't be hard to add the eof symbol if desired...
 */

#include "huff.h"
static HUFF freqs[256];
static BHEAP *heap;
static BYTE buf;
static int bitCount;
static int createHeap()
{
    int rv = 0;
    heap = BHeapCreate(256);
    for (int i=0; i < 256; i++)
    {
        if (freqs[i].freq)
        {
            rv++;
            BHeapInsert(heap, &freqs[i]);
        }
    }
    return rv;
}
void encodeBit(FILE *fil, int bit)
{
    buf <<= 1;
    buf |= !!bit;
    if (++bitCount >= 8)
    {
        bitCount = 0;
        fputc(buf, fil);
        buf = 0;
    }
}
void flush(FILE *fil)
{
    while (bitCount != 0)
        encodeBit(fil, 0);
}
void encodeSymbol(FILE *fil, HUFF *ch)
{
//    printf("Symbol '%c': ", ch->ch);
    if (!ch->parent) // only one symbol in file
    {
//        printf("X");
        encodeBit(fil, 1);
    }
    else 
    {
        BYTE buf[257], count = 0;
        while (ch->parent)
        {
            if (ch->parent->children[0] == ch)
            {
                buf[count++] = 0;
//                printf("0");
            }
            else
            {
                buf[count++] = 1;
//                printf("1");
            }
            ch = ch->parent;
        }
        while (count > 0)
            encodeBit(fil, buf[--count]);
    }
//    printf("\n");
}
void encodeByte(FILE *fil, BYTE byte)
{
    for (int i=7; i >=0; i--)
        encodeBit(fil, byte & (1 << i));
}
void encodeTree(FILE *fil, HUFF *tree)
{
    if (tree->ch == -1)
    {
        encodeBit(fil, 0);
        encodeTree(fil, tree->children[0]);
        encodeTree(fil, tree->children[1]);
    }
    else
    {
        encodeBit(fil, 1);
        encodeByte (fil, tree->ch);
    }
}
int decodeBit(BYTE **data)
{
    int bit = !!(**data & (1 << (7 - bitCount)));
    if (++bitCount == 8)
    {
        (*data)++;
        bitCount = 0;
    }
    return bit;
}
int decodeSymbol (BYTE **data, HUFF *tree )
{
    while (tree->ch == -1)
    {
        switch (decodeBit(data))
        {
            case 0:
                tree = tree->children[0];
                break;
            case 1:
                tree = tree->children[1];
                break;
        }
    }
    return tree->ch;
}
int decodeByte(BYTE **data)
{
    int rv = 0;
    for (int i=0; i < 8; i++)
    {
        rv <<= 1;
        rv |= decodeBit(data);
    }
    return rv;
}
HUFF *decodeTree(BYTE **data)
{
    HUFF *next;
    switch(decodeBit(data))
    {
        case 0:
            next = calloc(sizeof(HUFF),1);
            next->ch = -1;
            next->children[0] = decodeTree(data);
            next->children[1] = decodeTree(data);
            break;
        case 1:
            next = &freqs[decodeByte(data)];
            break;
    }
    return next;
}
void encode (char *name, BYTE *data, int len)
{
    strcat(name, ".hf");
    
    FILE *fil = fopen(name, "wb");
    fwrite(&len, 1, 4, fil);
    if (len)
    {
        BYTE *p = data;
        for (int i=0; i < len; i++)
        {
            freqs[*p++].freq ++;
        }
        int count = createHeap();
        HUFF * result = NULL;
        while (1)
        {
            result = BHeapRemove(heap);
//            printf("removing '%c'",result->ch);
            HUFF *result1 = BHeapRemove(heap);
            if (!result1)
            {
//                printf("\n");
                break;
            }
//            printf(":'%c'\n",result1->ch);
            HUFF * nw = calloc(sizeof(HUFF), 1);
            nw->ch = -1;
            nw->freq = result->freq + result1->freq;
            nw->children[0] = result;
            nw->children[1] = result1;
            result->parent = nw;
            result1->parent = nw;
            BHeapInsert(heap, nw);
        }
        encodeTree(fil, result);
        p = data;
        for (int i=0 ; i < len; i++)
            encodeSymbol(fil, &freqs[*p++]);
        flush(fil);
    }
    fclose(fil);
}
void decode (char *name, BYTE *data)
{
    if (strstr(name, ".hf") == name + strlen(name) - 3)
    {
        int val = 1;
        int len = *(int *) data;
        data += 4;
        HUFF *result = decodeTree(&data);
        name[strlen(name) - 3] = 0;
        strcat(name, ".hfd");
        FILE *fil = fopen(name, "wb");
        for (int i=0; i < len; i++)
        {
            fputc(decodeSymbol(&data, result), fil);
        }
        return;
    }
    printf("Not an hf file");
}
int main(int argc, char **argv)
{
    int dodecode = 0;
    if (argc > 1)
    {
        if (!strcmp(argv[1],"-d"))
        {
            dodecode = 1;
            argv[1] = argv[2];
            argc--;
        }
    }
    if (argc > 1)
    {
        FILE *fil = fopen(argv[1], "rb");
        if (fil)
        {
            fseek(fil, 0, SEEK_END);
            int filepos = ftell(fil);
            fseek(fil, 0, SEEK_SET);
            BYTE *data = calloc(1, filepos+1);
            fread(data, 1, filepos, fil);
            for (int i=0; i < 256; i++)
                freqs[i].ch = i;      
            if (!dodecode)
                encode(argv[1], data, filepos);
            else
                decode(argv[1], data);
            return 0;
        }
    }
    printf("no file");
    return 1;
}
