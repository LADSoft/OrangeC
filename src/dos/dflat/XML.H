#ifdef INCLUDE_CONFIG_XML
#define MAX_XML_TEXT_LEN 0x800
#define MAX_XML_NAME_LEN 0x80
struct xmlAttrib
{
    struct xmlAttrib *next;
    char *name;
    char *value;
};

struct xmlNode
{
    struct xmlNode *next;
    char *elementType;
    char *textData;
    struct xmlAttrib *attribs;
    struct xmlNode *children;
    struct xmlNode *parent;
};

void xmlFree(struct xmlNode *Node);
struct xmlNode *xmlReadFile(FILE *in);
void xmlWriteFile(FILE *out, struct xmlNode *root);
char *xmlConvertString(char *s, int attr);
#endif