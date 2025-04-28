#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ppInclude.h"
#include "Token.h"
#include <limits.h>
#include <variant>
using embed_token_type = std::shared_ptr<Token>;
using embed_value_type = uintmax_t;
using embeder_type = std::variant<embed_token_type, embed_value_type>;
// This is used exclusively for __has_embed, which returns one of three values
enum EmbedReturnValue
{
    EMBED_NOT_FOUND,
    EMBED_FOUND,
    EMBED_EMPTY,
    EMBED_IO_ERROR
};

struct embeder_info
{
    std::string filename;
    int limit = -1;
    std::vector<embeder_type> prefix = {};
    std::vector<embeder_type> suffix = {};
    bool is_system;
    unsigned long long offset = 0;
    int bytes = 1;
    EmbedReturnValue ret_value = EMBED_EMPTY;
    std::unordered_map<std::string, std::vector<embed_token_type>> mapped_values;
};
class embeder
{
  public:
    using embeder_func = void(std::vector<embeder_type>&);
    embeder(ppInclude& includer, ppDefine& definer) : includer(includer), definer(definer) {}
    std::tuple<std::vector<embeder_type>, EmbedReturnValue> EmbedFile(std::string& input, embeder_info info);
    // Deal with the possibility of having more than 1 byte (AKA optoinal extensions) now so that it is easier to deal with later
    EmbedReturnValue has_embed(embeder_info info, bool throw_error = false);
    void set_embed_function(std::function<embeder_func> embed_func) { embed_elements = embed_func; }
    bool Check(kw token, std::string& args);
    bool GetLine(std::string& line, int& lineno);
    embeder_info GetEmbedFromLine(const std::string& line);

  private:
    ppInclude& includer;
    ppDefine& definer;
    std::string nextLine;
    static std::function<embeder_func> embed_elements;
};
