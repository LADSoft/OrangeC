#ifndef UTF8_h
#define UTF8_h

#include <string>

class UTF8
{
public:
    static bool IsAlpha(int);
    static bool IsAlnum(int);
    static bool IsAlpha(const char *);
    static bool IsAlnum(const char *);
    static int Decode(const char *);
    static int Encode(char *, int);
    static int CharSpan(const char *);
    static int Span(const char *);
    static int ToUpper(int val);
    static std::string ToUpper(std::string val);
private:
    static int alpha[];
    static int alnum[];
    static int upper[][2];
};
#endif