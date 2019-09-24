/* line.h   */

{
    char *  file = __FILE__;
    file += strlen( file) - 6;
    assert( __LINE__ == 6 && strcmp( file, "line.h") == 0);
}
