/* libcxx support */

void freelocale(void *);

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
