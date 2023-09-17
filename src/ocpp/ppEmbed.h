#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "ppInclude.h"
#include <limits.h>
#include "Utils.h"
#include <string>
#include <stdio.h>
#include "ppkw.h"
#include <iostream>
struct embeder_info
{
    std::string filename;
    int limit = INT_MAX;
    std::vector<embeder_size> prefix = {};
    std::vector<embeder_size> postfix = {};
    bool is_system;
    int bytes = 0;
};
template <typename T, bool(isSymbolChar)(const char*, bool), void(embed_elements)(std::vector<embeder_size>) = nullptr>
class embeder
{
  public:
    embeder(ppInclude& includer) : includer(includer) {}

    std::tuple<std::vector<embeder_size>, bool> EmbedFile(std::string& input, embeder_info info)
    {
        if (!has_embed(info.filename, info.is_system, info.bytes))
        {
            return {std::vector<embeder_size>(), false};
        }
        bool found_system = false;
        int discard = 0;
        std::string fil = includer.FindFile(info.is_system, info.filename, false, discard, found_system);
        if (fil != "")
        {
            size_t size = Utils::file_size(fil);
            std::vector<embeder_size> builder = {};
            builder.insert(builder.end(), info.prefix.begin(), info.prefix.end());

            FILE* file = fopen(fil.c_str(), "rb");
            // we need to actually get the real file in here
            std::vector<embeder_size> next_thing = {};
            size_t resval = info.limit == 0 ? size : info.limit;
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
            return {builder, size % info.bytes == 0};
        }
        return {std::vector<embeder_size>(), false};
    }
    // Deal with the possibility of having more than 1 byte (AKA optional extensions) now so that it is easier to deal with later
    bool has_embed(const std::string& file, bool is_system, int bytes = 1)
    {
        int discard;
        bool found_system = false;
        std::string fil = includer.FindFile(is_system, file, false, discard, found_system);
        if (fil != "")
        {
            return Utils::file_size(fil) % bytes == 0;
        }
        return false;
    }
    bool Check(kw token, std::string& args)
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
            return std::get<1>(ret);
        }
        return false;
    }
    bool GetLine(std::string& line, int& lineno)
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
    embeder_info GetEmbedFromLine(const std::string& line)
    {
        embeder_info info;
        // Default unless we support an extension, don't know what that extension will look like yet though...
        info.bytes = 1;
        info.is_system = false;
        bool system_file = false;
        /**
         * We need to support the following:
         * <filename> limit(number) prefix(list of values to prepend) postfix(list of values to append)
         * limit(number) prefix(list of values to prepend) postfix(list of values to append) <filename>
         * and all of those iterations. This means that we need to get rid of everything related to *our* reference and actually
         * parse this thing. Parsing is hard, modify the tokenizer
         */
        info.limit = 0;
        info.postfix = {};
        info.prefix = {};
        info.filename = this->includer.ParseName(line, info.is_system);
        return info;
    }


  private:
    ppInclude& includer;
    std::string nextLine;
};
