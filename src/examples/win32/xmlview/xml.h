#define MAX_XML_TEXT_LEN 0x800
#define MAX_XML_NAME_LEN 0x80
struct xmlAttr {
  struct xmlAttr *next ;
  char *name ;
  char *value ;
  void *userdata ;
} ;

struct xmlNode
{
  struct xmlNode *next ;
  char * elementType ;
  char * textData;
  struct xmlAttr * attribs ;
  struct xmlNode  *children;
  struct xmlNode*   parent;
  void *userdata ;
};

void xmlFree(struct xmlNode *Node) ;
struct xmlNode *xmlReadFile(FILE *in) ;
void xmlWriteFile(FILE *out, struct xmlNode *root) ;
char *xmlConvertString(char *s, int attr) ;