/* libcxx support */
#include <string.h>
#include <stdlib.h>
void * newlocale(int category_mask, const char *locale,
                          void *base) { return 0; }
void freelocale(void *) { }

inline int snprintf_l(char * __buffer, size_t n, locale_t, 
                                const char * __format, ...)
{
    int rv;
    va_list arg;
    va_start(arg, __format);
    rv = vsnprintf(__buffer, n, __format, arg);
    va_end(arg);
    return rv;
}
inline int asprintf_l(char **str, locale_t, const char *__format, ...)
{
    char __buffer[10000];
    int rv;
    va_list arg;
    va_start(arg, __format);
    rv = vsnprintf(__buffer, sizeof(__buffer), __format, arg);
    va_end(arg);
    *str = (char *)malloc(strlen(__buffer) + 1);
    strcpy(*str, __buffer);
    return rv;
}
inline int isdigit_l(char c, locale_t)
{
    return c >= '0' && c <= '9';
}
inline int isxdigit_l(char c, locale_t)
{
    return c >= '0' && c <= '9' || c>='a' && c <= 'f' || c >= 'A' && c <= 'F';
}
inline long long strtoll_l(const char *restrict __s, char **restrict __endptr,
                                    int __radix, locale_t)
{
    return strtoll(__s, __endptr, __radix);
}
inline long long strtoull_l(const char *restrict __s, char **restrict __endptr,
                                    int __radix, locale_t)
{
    return strtoull(__s, __endptr, __radix);
}
inline long double strtold_l(const char *restrict __s, char ** restrict __endptr, locale_t)
{
    return strtold(__s, __endptr);
}
inline int sscanf_l(const char *restrict __buffer, locale_t, 
                                  const char *restrict __format, ...)
{
    va_list arg;
    va_start(arg, __format);
    int rv = vsscanf(__buffer, __format, arg);
    va_end(arg);
    return rv;
}
