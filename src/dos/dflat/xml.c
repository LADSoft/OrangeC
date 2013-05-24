#ifdef INCLUDE_CONFIG_XML
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xml.h"

/* define this to strip leading and trailing spaces from text data on input */
#define STRIPTEXT

#define TRUE 1
#define FALSE 0

/* is it one of the special characters that have to be quoted? */
static int isspecial(int t)
{
    return t == '>' || t == '<' || t == '=' || t == '&' || t == '\'' || t == '\"' ;
}
/* skip spaces, and return FALSE if it end of text */
static int skipspace(char **s)
{
    while (isspace(**s)) (*s)++ ;

    if (**s)
        return TRUE ;
    return FALSE ;
}
/* when reading a string or attribute, replace &xxx; quoted sequences
 * with the appropriate character
 */
static int convertchar(char **s)
{
    if (**s != '&')
        return *(*s)++ ;
    if (!strncmp((*s)+1,"amp;",4)) {
          (*s)+=5 ;
          return '&' ;
    } else if (!strncmp((*s)+1,"quot;",5)) {
          (*s)+=6 ;
          return '"' ;
    } else if (!strncmp((*s)+1,"apos;",5)) {
          (*s)+=6 ;
          return '\'' ;
    } else if (!strncmp((*s)+1,"lt;",3)) {
          (*s)+=4 ;
          return '<' ;
    } else if (!strncmp((*s)+1,"gt;",3)) {
          (*s)+=4 ;
          return '>' ;
    }
   return 0 ;

}
/* read a tag or attribute name.  Must start with an alpha and
 * consist only of alphanumeric characters
 */
static void GetName(char *name, char ** s)
{
  if (!isalpha(**s)) {
      *name = 0 ;
      return ;
  }
  while(*name++=*(*s)++)    
    if (!isalnum(**s))
        break ;

  *name = 0 ;
}
/* insert an attribute at the end of a nodes attribute list */
static int InsertAttr(struct xmlNode *Node, char *name, char *attr)
{
    struct xmlAttrib *map = calloc(sizeof(struct xmlAttrib),1),**p=&Node->attribs ;
    if (!map)
        return FALSE ;
    map->name = strdup(name) ;
    map->value = strdup(attr) ;
    if (!map->name || !map->value) {
        free(map) ;
        return FALSE ;
    }
    while (*p)
        p = &(*p)->next ;
    *p = map ;
    return TRUE ;
}
/* when reading the text associated with a tag, we are going to strip
 * leading and trailing spaces.  Probably not XML kosher, but it helps
 * with some of the text for this particular app
 */
static char *stripdup(char *p)
{
#ifdef STRIPTEXT
    char buf[MAX_XML_TEXT_LEN],*q=buf ;

    while (isspace(*p))
        p++ ;

    while (*p)
        *q++ = *p++ ;

    while (q != buf && isspace(*(q-1)))
        q-- ;
    *q = 0 ;

    return strdup(buf) ;
#else
    return strdup(p) ;
#endif
}
/* parse a tag and all its sub tags */
static int ParseNode(struct xmlNode* Node, char **s)
{
  char szText[MAX_XML_TEXT_LEN];
  char attr[MAX_XML_NAME_LEN];
  int j ;

  /* look for opening < */
  if (!skipspace(s))
    return FALSE ;

  if (**s != '<')
    return FALSE ;
  (*s)++ ;
  
  if (!skipspace(s))
    return FALSE ;

  /* get the tag name and store it */
  GetName(attr,s);
  Node->elementType = strdup(attr) ;
  if (!Node->elementType)
    return FALSE ;

  /* is it immediately followed by /? 
   * if so we are going to define it as a null tag, I don't know
   * if this is XML kosher or not
   */
  if (!skipspace(s))
    return FALSE ;

  if (**s == '/') {
    (*s)++ ;
    if (!skipspace(s))
        return FALSE ;

    /* we always have Node->textData point to something */
    Node->textData = strdup("") ;
    if (!Node->textData)
        return FALSE ;
    return *(*s)++ == '>' ;
  }

  /* are there any attributes? */
  if(**s!='>')
  {
    /* while there are attributes */
    while(**s)
    {
      char attr[MAX_XML_NAME_LEN];
      j = 0 ;

      /* get the attribute name */
      GetName(attr, s) ;
      if (!**s)
        return FALSE ;

      if (!skipspace(s))
         return FALSE ;

      /* need an equal sign */
      if (**s != '=')
        return FALSE ;
      (*s)++ ;

      if (!skipspace(s))
         return FALSE ;

      /* need an opening quote */
      if (**s != '"')
        return FALSE ;
      (*s)++ ;

      /* now read the attribute value */
      while(**s && **s!='"'&&j<MAX_XML_TEXT_LEN) {
        szText[j] = convertchar(s);
        if (szText[j++] == 0)
            return FALSE ;
      }
      szText[j] = '\0';
      if (**s)
        (*s)++;

      /* insert it into the node */
      if (!InsertAttr(Node,attr,szText))
        return FALSE ;

      if (!skipspace(s))
         return FALSE ;

      /* auto close out the tag? */
      if(**s=='/')
      {
        
        (*s)++ ;
        if (!skipspace(s))
           return FALSE ;
    
        /* we always have Node->textData point to something */

        Node->textData = strdup("") ;
        if (!Node->textData)
            return 0 ;

        /* make sure we really have the end of the tag */
        return (*(*s)++ == '>') ;
      }

      /* other wise if it is the end of a normal opening tag
       * break out of the attribute loop and go get text
       */
      if(**s=='>')  {
        (*s)++ ;
        break;
      }
    }
  } else
    (*s)++ ;

  j = 0 ;
  /* now start processing child nodes */
  while(**s)
  {
    struct xmlNode *child ;
    struct xmlNode **link  = &Node->children;
  
    /* get any text available now
     */
    while(**s && **s!='<'&&j<MAX_XML_TEXT_LEN) {
      szText[j] = convertchar(s) ;
      if (szText[j++] == 0)
        return FALSE ;
    }
    szText[j] = '\0';

    /* MUST have an opening < at this point */
    if (**s != '<')
        return FALSE ;
    (*s)++ ;

    /* is this the beginning of a closing tag? */
    if(**s=='/') 
    {
      /* if so get its name */
      (*s)++ ;
      GetName(attr,s) ;

      if (!skipspace(s))
        return FALSE ;

      /* make sure we end with > */
      if (*(*s)++ != '>')
        return FALSE ;

      /* save text, textdata always points to something even if it is null text */
      Node->textData = stripdup(szText) ;
      if (!Node->textData)
        return FALSE ;

      /* and only return TRUE if it matched the opening tag for this node */
      return !strcmp(Node->elementType,attr) ;
    }

    /* otherwise back up to < as we are going to call this routine
     * recursively to get the node data */
    while (*(--*s) != '<') ;

    /* make a new child */
    child = calloc(sizeof(struct xmlNode),1) ;
    if (!child)
        return FALSE ;

    /* find the end of the node's children and insert the new child */
    while (*link)
        link = &((*link)->next) ;
    *link = child ;

    /* now parse the child */
    if (!ParseNode(child,s))
        return FALSE ;

  }
  return FALSE ;
}
/* write out a node and all its subnodes, using the currrent indent */
static void WriteNode(FILE *out, struct xmlNode *node, int indent) 
{
  int i ;
  struct xmlNode *x = node->children ;
  struct xmlAttrib *z = node->attribs ;

  /* select indent */
  for (i=0; i < indent; i++)
    fprintf(out,"\t") ;

  /* out element type */
  fprintf(out,"<%s",node->elementType) ;

  /* print attributes */
    while (z) {
        fprintf(out," %s=\"%s\"",z->name,xmlConvertString(z->value,TRUE)) ;
        z= z->next ;
    }

  if (x || node->textData[0]) {

    /* has subnodes or text */
    fprintf(out,">\n") ;

#ifdef STRIPTEXT
    /* select indent */
    for (i=0; i < indent+1; i++)
      fprintf(out,"\t") ;
#endif

    /* put the text out first */
    if (node->textData[0])
        fprintf(out,"%s",xmlConvertString(node->textData,FALSE)) ;
    fprintf(out,"\n") ;

    /* now write the subnodes */
    while (x) {
        WriteNode(out,x,indent + 1) ;
        x = x->next ;
    }

    /* write the closing tag */
    for (i=0; i < indent; i++)
        fprintf(out,"\t") ;
    fprintf(out,"</%s>\n",node->elementType) ;

  } else
    /* simple non-data non-subelement tag */
    fprintf(out,"/>\n") ;
}
/* function removes the document tags */
static void RemoveDocTags(char * s,const char * szleft,const char * szright)
{
  char * s1,*s2;
  while(1)
  {
    s1 = strstr(s,szleft);
    if(s1==NULL)
      break;
    s2 = strstr(s1,szright);
    if (s2)
        while (s1 != s2 + strlen(szright))
            *s1++ = ' ' ;
  }
  return;
}
/* aux function to convert a string into a quoted string.  use 'attr'
 * as true if it is an attribute string
 */
char *xmlConvertString(char *s, int attr)
{
    static char buf[MAX_XML_TEXT_LEN] ;
    char *p = buf ;
    int len = 0 ;
    while (*s && len < MAX_XML_TEXT_LEN-1) {

        /* while we have a non-quotable character */
        while (*s && !isspecial(*s) && len < MAX_XML_TEXT_LEN-1)
            *p++ = *s++,len++ ;

        /* do we have plenty of space? */
        if (len + 7 >= MAX_XML_TEXT_LEN) {
            *p = 0 ;
            return buf ;
        }

        /* now put in the quoted text for the char */
        switch(*s) {
            case '"' :
                if (attr)
                    strcpy(p,"&quot;") ;
                else {
                    *p++ = *s++,len++ ;
                    *p = 0 ;
                }
                break ;
            case '\'':
                if (attr)
                    strcpy(p,"&apos;") ;
                else {
                    *p++ = *s++,len++ ;
                    *p = 0 ;
                }
                break ;
            case '&':
                strcpy(p,"&amp;") ;
                break ;
            case '<':
                strcpy(p,"&lt;") ;
                break ;
            case '>':
                strcpy(p,"&gt;") ;
                break ;
            default:
                /* hm, must have hit max text len */
                *p = 0 ;
                break ;
        }
        /* bump indexes */
        if (*s)
            s++ ;
        len += strlen(p) ;
        p += strlen(p) ;
    }
    *p = 0 ;
    return buf ;

}
/* free an xml node and all its subnodes */
void xmlFree(struct xmlNode *Node)
{
  struct xmlNode *x ;
  struct xmlAttrib *z ;
  if (!Node)
    return ;
  x = Node->children ;
  z = Node->attribs ;

  /* free the children */
  while (x) {
    struct xmlNode *y = x->next ;
    xmlFree(x) ;
    free(x) ;
    x = y ;
  }

  /* free the attributes */
  while (z) {
    struct xmlAttrib *y = z->next ;
    free(z->name) ;
    free(z->value) ;
    free(z) ;
    z = y ;
  }

  /* free the node name */
  free(Node->elementType) ;

  /* free the text */
  free(Node->textData) ;
}
/* read an xml file */
struct xmlNode *xmlReadFile(FILE *in)
{
    int size ;
    char *data,*p ;
    struct xmlNode *root ;

    if (!in)
        return 0 ;

    /* find len */
    fseek(in,0,SEEK_END) ;
    size = ftell(in) ;

    /* read the data into the buffer */
    data = calloc(size+1,1) ;
    fseek(in,0,SEEK_SET) ;
    fread(data,1,size,in) ;

    /* ignore doc tags */
    RemoveDocTags(data,"<?","?>");
    RemoveDocTags(data,"<!--","-->");

    /* make a root node */
    root = calloc(sizeof(struct xmlNode),1) ;
    if (!root) {
        free(data) ;
        return 0 ;
    }

    /* parse the data */
    p = data ;
    if (!ParseNode(root,&p)) {
        free(data) ;
        xmlFree(root) ;
        return 0 ;
    }

    /* free the data and return the root */
    free(data) ;
    return root ;

}
/* entry point to write a file
 * same as Write Node except it initializes the indent
 */
void xmlWriteFile(FILE *out, struct xmlNode *root)
{
    WriteNode(out,root,0) ;
}
/* test routine */
#ifdef XXXXX
int main(int argc, char **argv)
{
    FILE *in = fopen(argv[1],"r"),*out ;
    struct xmlNode *root = xmlReadFile(in) ;
    fclose(in) ;
    if (root) {
        out = fopen(argv[2],"w") ;
        xmlWriteFile(out,root) ;
        fclose(out) ;
    } else
        printf("could not parse %s",argv[1]) ;
    xmlFree(root) ;
}
#endif
#endif