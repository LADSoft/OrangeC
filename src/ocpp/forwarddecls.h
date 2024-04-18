#pragma once
enum class kw;
template <typename T>
using KeywordTable = std::unordered_map<std::string, T>;
using KeywordHash = KeywordTable<kw>;
class Token;
class ppDefine;
class ppInclude;
class ppCond;
template <typename T = kw>
class Tokenizer;