#include <string.h>
#ifndef SHADER_TARGET
#    define SHADER_TARGET "edith-impl.glsl"
#endif
extern char* null_term_shader_data;
void fill_in_data()
{
    const char internal_data[] = {
#embed SHADER_TARGET suffix(,)
        0};
    strcpy(null_term_shader_data, internal_data);
}