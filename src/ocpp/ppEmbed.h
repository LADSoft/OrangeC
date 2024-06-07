#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ppInclude.h"
#include <limits.h>
using embeder_size = uint64_t;
// This is used exclusively for __has_embed, which returns one of three values
enum EmbedReturnValue
{
    EMBED_NOT_FOUND,
    EMBED_FOUND,
    EMBED_EMPTY
};

struct embeder_info
{
    std::string filename;
    int limit = -1;
    std::vector<embeder_size> prefix = {};
    std::vector<embeder_size> postfix = {};
    bool is_system;
    int bytes = 1;
    EmbedReturnValue ret_value;
    std::unordered_map<std::string, std::vector<std::string>> mapped_values;
};
class embeder
{
  public:
    embeder(ppInclude& includer, ppDefine& definer) : includer(includer), definer(definer) {}
    std::tuple<std::vector<embeder_size>, EmbedReturnValue> EmbedFile(std::string& input, embeder_info info);
    // Deal with the possibility of having more than 1 byte (AKA optoinal extensions) now so that it is easier to deal with later
    EmbedReturnValue has_embed(embeder_info info, bool throw_error = false);
    void set_embed_function(std::function<void(std::vector<embeder_size>)> embed_func) { embed_elements = embed_func; }
    bool Check(kw token, std::string& args);
    bool GetLine(std::string& line, int& lineno);
    embeder_info GetEmbedFromLine(const std::string& line);

  private:
    ppInclude& includer;
    ppDefine& definer;
    std::string nextLine;
    static std::function<void(std::vector<embeder_size>)> embed_elements;
};
