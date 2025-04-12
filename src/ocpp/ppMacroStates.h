#pragma once
enum MacroStates
{
    MACRO_REPLACE_SIZE = 65536,
    REPLACED_TOKENIZING = 1,
    /* left or right-hand size of a ## when an arg has been replaced by an empty string */
    TOKENIZING_PLACEHOLDER = 2,
    STRINGIZING_PLACEHOLDER = 3,
    REPLACED_ALREADY = 4,
    MACRO_PLACEHOLDER = 5
};