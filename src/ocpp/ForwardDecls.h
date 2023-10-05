#pragma once
#include <vector>
using embeder_size = uint64_t;
template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppDefine;
template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppCtx;
template <typename T, bool(isSymbolChar)(const char*, bool)>
class ppInclude;
template <typename T, bool(isSymbolChar)(const char*, bool), void(embed_elements)(std::vector<embeder_size>)>

class ppMacro;
template <typename T, bool(isSymbolChar)(const char*, bool), void(embed_elements)(std::vector<embeder_size>)>
class PreProcessor;