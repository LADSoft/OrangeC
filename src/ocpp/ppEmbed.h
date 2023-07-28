#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ppInclude.h"
#include <limits.h>
using embeder_size = uint64_t;
struct embeder_info
{
    std::string filename;
    int limit = INT_MAX;
    std::vector<embeder_size> prefix = {};
    std::vector<embeder_size> postfix = {};
    bool is_system;
    int bytes = 0;
};
class embeder
{
  public:
    embeder(ppInclude& includer) : includer(includer) {}
    std::tuple<std::vector<embeder_size>, bool> EmbedFile(std::string& input, embeder_info info);
    // Deal with the possibility of having more than 1 byte (AKA optoinal extensions) now so that it is easier to deal with later
    bool has_embed(const std::string& file, bool is_system, int bytes = 1);
    void set_embed_function(std::function<void(std::vector<embeder_size>)> embed_func) { embed_elements = embed_func; }
    bool Check(kw token, std::string& args);
    bool GetLine(std::string& line, int& lineno);
    embeder_info GetEmbedFromLine(const std::string& line);

  private:
    ppInclude& includer;
    std::string nextLine;
    static std::function<void(std::vector<embeder_size>)> embed_elements;
};
