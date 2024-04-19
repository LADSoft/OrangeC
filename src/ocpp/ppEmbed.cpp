#include "forwarddecls.h"
#include "ppEmbed.h"
#include "Utils.h"
#include "Token.h"
#include "ppkw.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <initializer_list>
std::function<void(std::vector<embeder_size>)> embeder::embed_elements;

enum embed_parser_kw
{
    // (
    lparen,
    // )
    rparen,
    // ""
    doublequote,
    // '
    singlequote,
};
KeywordTable<embed_parser_kw> hasher = {{"(", embed_parser_kw::lparen},
                                        {")", embed_parser_kw::rparen},
                                        {"\"", embed_parser_kw::doublequote},
                                        {"'", embed_parser_kw::singlequote}};
enum handler_tokens
{
    prefix,
    postfix,
    limit,
    unknown
};

std::tuple<std::vector<embeder_size>, EmbedReturnValue> embeder::EmbedFile(std::string& input, embeder_info info)
{
    std::vector<embeder_size> next_thing = {};
    if (!has_embed(info))
    {
        return {next_thing, EmbedReturnValue::EMBED_NOT_FOUND};
    }
    bool found_system = false;
    int discard = 0;
    std::string fil = includer.FindFile(info.is_system, info.filename, false, discard, found_system);
    if (fil != "" && info.limit != 0)
    {
        size_t size = Utils::file_size(fil);
        std::vector<embeder_size> builder = {};
        builder.insert(builder.end(), info.prefix.begin(), info.prefix.end());

        FILE* file = fopen(fil.c_str(), "rb");
        // we need to actually get the real file in here
        size_t resval = info.limit < 0 ? size : info.limit;
        next_thing.resize(resval);
        // We will always correctly get the number of bytes based on the size given for us...
        auto data_loc = next_thing.data();
        for (int i = 0; i < resval; i++)
        {
            size_t numread = fread(&data_loc[i], info.bytes, 1, file);
        }
        builder.insert(builder.end(), next_thing.begin(), next_thing.end());
        builder.insert(builder.end(), info.postfix.begin(), info.postfix.end());
        if (embed_elements)
        {
            embed_elements(builder);
        }
        return {builder, has_embed(info)};
    }
    return {next_thing, has_embed(info)};
}

EmbedReturnValue embeder::has_embed(embeder_info info)
{
    int discard;
    EmbedReturnValue found = EmbedReturnValue::EMBED_NOT_FOUND;
    bool is_system = info.is_system;
    auto file = info.filename;
    bool found_system = false;
    std::string fil = includer.FindFile(is_system, file, false, discard, found_system);
    if (fil != "")
    {
        if (info.limit <= 0)
        {
            return EmbedReturnValue::EMBED_EMPTY;
        }
        return (Utils::file_size(fil) % info.bytes == 0) ? EmbedReturnValue::EMBED_EMPTY : EmbedReturnValue::EMBED_FOUND;
    }
    return EmbedReturnValue::EMBED_NOT_FOUND;
}
bool embeder::Check(kw token, std::string& args)
{
    if (token == kw::EMBED)
    {
        // comment to check line info
        embeder_info info = GetEmbedFromLine(args);
        auto ret = EmbedFile(args, info);
        auto vec = std::get<0>(ret);
        std::string thing = "";
        for (auto&& item : vec)
        {
            thing += std::to_string(item);
            thing += ',';
        }
        if (thing.length() > 0)
        {
            thing.resize(thing.length() - 1);
        }
        this->nextLine = thing;
        return std::get<1>(ret) != 0;
    }
    return false;
}
bool embeder::GetLine(std::string& line, int& lineno)
{
    if (embed_elements == nullptr)
    {
        if (nextLine != "")
        {

            // Don't allow for parsing to happen if we've automatically embedded elements into the declaration, should provide
            // SIGNIFICANT compiler speedup
            line = this->nextLine;
            this->nextLine = "";
            return true;
        }
    }
    if (nextLine != "")
    {
        this->nextLine = "";
        return true;
    }
    return false;
}

embeder_info embeder::GetEmbedFromLine(const std::string& line)
{
    embeder_info info;
    Tokenizer<embed_parser_kw> tokenizer(line, &hasher);
    // Our token state machine!
    // Default unless we support an extension, don't know what that extension will look like yet though...
    info.bytes = 1;
    info.is_system = false;
    bool system_file = false;
    info.postfix = {};
    info.prefix = {};
    info.filename = this->includer.ParseName(line, info.is_system);

    for (const Token* tk = tokenizer.Next(); tk; tk = tokenizer.Next())
    {
    }
    return info;
}
