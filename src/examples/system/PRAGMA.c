/*
 * This is an example of how to use #pragma to generate errors and warnings. 
 * If you compile this with ERROR and WARNING defined (as is set in the make
 * and batch files) the program will not compile because of the error
 */
#ifdef ERROR
#pragma error This error was defined with a #pragma
#endif

#ifdef WARNING
#pragma warning This warning was defined with a #pragma
#endif

void printf(char *format,...);
main()
{
    printf("This is a compile-time example");
    return 0 ;
}