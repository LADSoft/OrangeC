#pragma once
#include <unordered_map>
#include <string>
enum class kw;
class ppDefine;
class ppInclude;
class ppExpr;
class ppMacro;
template <typename T>
using KeywordTable = std::unordered_map<std::string, T>;
using KeywordHash = KeywordTable<kw>;
class Tokenizer;
class Token;
class embeder;