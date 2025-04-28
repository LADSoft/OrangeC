#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING 1
#include "forwarddecls.h"
#include "ppEmbed.h"
#include "Utils.h"
#include "TemplatedToken.h"
#include "ppkw.h"
#include <string>
#include <iostream>
#include <initializer_list>
#include <typeinfo>
#include <array>
#include <functional>
#include <strstream>
#include <cstdio>
#include "ppExpr.h"
std::function<embeder::embeder_func> embeder::embed_elements;
struct visitor_struct
{
    std::string operator()(embed_token_type token) { return token->GetChars(); }
    std::string operator()(embed_value_type token) { return std::to_string(token); }
};
KeywordHash hasher = {{"(", kw::openpa},      {")", kw::closepa}, {"+", kw::plus},   {"-", kw::minus}, {"!", kw::lnot},
                      {"~", kw::bcompl},      {"*", kw::star},    {"/", kw::divide}, {"%", kw::mod},   {"<<", kw::leftshift},
                      {">>", kw::rightshift}, {">", kw::gt},      {"<", kw::lt},     {">=", kw::geq},  {"<=", kw::leq},
                      {"==", kw::eq},         {"!=", kw::ne},     {"|", kw::bor},    {"&", kw::band},  {"^", kw::bxor},
                      {"||", kw::lor},        {"&&", kw::land},   {"?", kw::hook},   {":", kw::colon}, {",", kw::comma},
                      {"::", kw::coloncolon}};

std::array<std::string, 5> ourKnownValues = {"prefix", "suffix", "limit", "if_empty", "offset"};
static void push_back_values(std::vector<embeder_type>& embed_vec,
                             const std::unordered_map<std::string, std::vector<embed_token_type>>& args,
                             const std::string& map_vals_string)
{
    auto prefix_val = args.find(map_vals_string);
    if (prefix_val != args.end())
    {
        for (auto&& val : prefix_val->second)
        {
            embed_vec.push_back(val);
        }
    }
}
static void push_back_values(std::vector<embeder_type>& embed_vec, const std::vector<embed_value_type>& embed_val_vector)
{
    for (auto&& val : embed_val_vector)
    {
        embed_vec.push_back(val);
    }
}
std::string reconstruct_string(const std::vector<embed_token_type>& embed_vec)
{
    std::string start = "";

    for (auto&& val : embed_vec)
    {
        start += val->GetChars() + " ";
    }
    return start;
}
struct FileDeleter {
    void operator()(FILE* file) const {
        if (file) {
            if (fclose(file) != 0) {

            }
        }
    }
};
std::tuple<std::vector<embeder_type>, EmbedReturnValue> embeder::EmbedFile(std::string& input, embeder_info info)
{
    std::vector<uintmax_t> next_thing = {};
    std::vector<embeder_type> builder = {};
    EmbedReturnValue ret_val = has_embed(info, true);
    if (ret_val == EmbedReturnValue::EMBED_NOT_FOUND)
    {
        return std::tuple<std::vector<embeder_type>, EmbedReturnValue>{builder, EmbedReturnValue::EMBED_NOT_FOUND};
    }
    bool found_system = false;
    int discard = 0;
    bool found = false;
    std::string fil = includer.FindFile(info.is_system, info.filename, false, discard, found_system, found);
    if (!Utils::FileExists(fil))
    {
        return {builder, EmbedReturnValue::EMBED_NOT_FOUND};
    }
    size_t size = Utils::file_size(fil);
    ppExpr evaluator(false, Dialect::c2x);
    // I would love if the above evaluator could run on tokens
    if (info.mapped_values.find("limit") != info.mapped_values.end())
    {
        auto& vals = info.mapped_values["limit"];
        std::string line = reconstruct_string(vals);
        info.limit = evaluator.Eval(line);
    }
    if (info.mapped_values.find("offset") != info.mapped_values.end())
    {
        auto& vals = info.mapped_values["offset"];
        std::string line = reconstruct_string(vals);
        info.offset = evaluator.Eval(line);
    }
    if (fil != "" && info.limit != 0 && size != 0)
    {
        push_back_values(info.prefix, info.mapped_values, "prefix");
        push_back_values(info.suffix, info.mapped_values, "suffix");
        builder.insert(builder.end(), info.prefix.begin(), info.prefix.end());

        std::unique_ptr<FILE,FileDeleter> file(fopen(fil.c_str(), "rb"));
        // we need to actually get the real file in here
        size_t resval = info.limit < 0 ? size : info.limit;
        next_thing.resize(resval);
        if (fseek(file.get(), info.offset, SEEK_SET) < 0)
        {
            return std::tuple<std::vector<embeder_type>, EmbedReturnValue>{builder, EmbedReturnValue::EMBED_IO_ERROR};
        }
        // We will always correctly get the number of bytes based on the size given for us...
        auto data_loc = next_thing.data();
        for (int i = 0; i < resval; i++)
        {
            size_t numread = fread(&data_loc[i], info.bytes, 1, file.get());
            if (numread != info.bytes)
            {
                return std::tuple<std::vector<embeder_type>, EmbedReturnValue>{builder, EmbedReturnValue::EMBED_IO_ERROR};
            }
        }

        push_back_values(builder, next_thing);
        builder.insert(builder.end(), info.suffix.begin(), info.suffix.end());
        if (embed_elements)
        {
            embed_elements(builder);
        }
        return std::tuple<std::vector<embeder_type>, EmbedReturnValue>{builder, has_embed(std::move(info))};
    }
    else if (fil != "" && (info.limit == 0 || size == 0))
    {

        // We're placing if_empty values into here where possible
        std::vector<embeder_type> builder = {};
        push_back_values(builder, info.mapped_values, "if_empty");
        builder.insert(builder.end(), next_thing.begin(), next_thing.end());
        if (embed_elements)
        {
            embed_elements(builder);
        }
        return std::tuple<std::vector<embeder_type>, EmbedReturnValue>{builder, has_embed(std::move(info))};
    }
    return std::tuple<std::vector<embeder_type>, EmbedReturnValue>{builder, has_embed(std::move(info))};
}

EmbedReturnValue embeder::has_embed(embeder_info info, bool throw_error)
{
    int discard;
    bool is_system = info.is_system;
    auto& file = info.filename;
    bool found_system = false;
    bool found = false;
    std::string fil = includer.FindFile(is_system, file, false, discard, found_system, found);
    bool FoundInvalid = false;
    std::strstream string_stream;
    if (throw_error)
    {
        for (auto&& value : info.mapped_values)
        {
            auto val = std::find(ourKnownValues.begin(), ourKnownValues.end(), value.first);
            if (val == ourKnownValues.end())
            {
                // std::format C++23 my god, better than a strstream here
                string_stream << value.first;
                Errors::ErrorWithLine(string_stream.str(), Errors::GetFileName(), Errors::GetErrorLine());
                FoundInvalid = true;
                string_stream.clear();
            }
        }
    }
    if (fil != "" && !FoundInvalid)
    {
        if (info.limit == 0)
        {
            return EmbedReturnValue::EMBED_EMPTY;
        }
        auto fil_size = Utils::file_size(fil);
        if (info.offset > fil_size || (info.limit != -1 && (info.offset + info.limit > fil_size)))
        {
            return EmbedReturnValue::EMBED_EMPTY;
        }
        return (Utils::file_size(fil) % info.bytes == 0) ? EmbedReturnValue::EMBED_FOUND : EmbedReturnValue::EMBED_EMPTY;
    }
    return EmbedReturnValue::EMBED_NOT_FOUND;
}

std::string tokens_to_inject(std::vector<embeder_type> type)
{
    std::string total = "";
    size_t type_size = type.size();
    size_t minus_one = type_size - 1;
    auto visitor = visitor_struct{};
    for (size_t i = 0; i < type_size; i++)
    {
        // Technically, here, it would make a lot of sense to kick the next "type" into cache, too bad there's no easy way to pull
        // this off portibly.
        auto&& val = type[i];
        total += std::visit(visitor, val);
        if (std::holds_alternative<embed_value_type>(val) && i < minus_one && std::holds_alternative<embed_value_type>(type[i + 1]))
        {
            total += ',';
        }
    }
    return total;
}

bool embeder::Check(kw token, std::string& args)
{
    if (token == kw::EMBED)
    {
        // comment to check line info
        embeder_info info = GetEmbedFromLine(args);
        auto return_val = EmbedFile(args, std::move(info));
        auto& vec = std::get<0>(return_val);
        auto ret = std::get<1>(return_val);
        if (ret == EmbedReturnValue::EMBED_NOT_FOUND)
        {
            Errors::ErrorWithLine("The embedded file was not found", Errors::GetFileName(), Errors::GetErrorLine());
            return true;
        }
        if (ret == EmbedReturnValue::EMBED_IO_ERROR)
        {
            Errors::ErrorWithLine("File I/O error while reading the embedded file", Errors::GetFileName(), Errors::GetErrorLine());
            return true;
        }
        std::string thing = tokens_to_inject(std::move(vec));
        this->nextLine = std::move(thing);
        return true;
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
std::array<char, 3> leftTokens = {'(', '{', '['};
std::array<char, 3> rightTokens = {']', '}', ')'};
// copied from a newer libcxx because this only gets added in C++20 because the C++ standards committee never thought to add it in
// over 20 years
bool ends_with(const std::string& str, const std::string& ending)
{
    return str.size() >= ending.size() && str.compare(str.size() - ending.size(), std::string::npos, ending) == 0;
}
bool starts_with(const std::string& str, const std::string& ending)
{
    return str.size() >= ending.size() && str.compare(0, ending.size(), ending) == 0;
}
std::string strip_underscores(const std::string& str)
{
    if (starts_with(str, "__") && ends_with(str, "__"))
    {
        return str.substr(2, str.size() - 4);
    }
    return str;
}

embeder_info embeder::GetEmbedFromLine(const std::string& line)
{
    embeder_info info;
    embeder_info blank_info;
    // Our token state machine!
    // Default unless we support an extension, don't know what that extension will look like yet though...
    info.bytes = 1;
    info.is_system = false;
    bool system_file = false;
    info.suffix = std::vector<embeder_type>();
    info.prefix = std::vector<embeder_type>();
    std::string int_line = line;
    definer.Process(int_line);
    info.filename = this->includer.ParseName(int_line, info.is_system);
    if (info.filename == "")
    {
        return info;
    }
    std::string temp_string_for_tokenizer(std::move(int_line));
    auto loc = temp_string_for_tokenizer.find(info.filename);
    // Grabs the < or " characters,
    char to_find = info.is_system ? '<' : '"';
    auto first_char = temp_string_for_tokenizer.rfind(to_find, loc);
    auto last_char = temp_string_for_tokenizer.find_first_of("\">", loc);
    temp_string_for_tokenizer.erase(first_char, last_char);
    //
    //
    TemplatedTokenizer<kw> tokenizer(temp_string_for_tokenizer, &hasher);
    enum ParsingState
    {
        IdentifierParsing,
        AdditionalIdentifierParsing,
        BalancedParsing
    } state = IdentifierParsing;
    std::stack<char> TokenBalancer;
    std::string identifierString;
    bool join_next_identifier = false;
    std::vector<embed_token_type> items;
    for (embed_token_type tk = tokenizer.NextShared(); tk && !tk->IsEnd(); tk = tokenizer.NextShared())
    {
        // Constructions we don't error on
        // identifier identifier(list, list1, list2...) identifier::identifier identifier::identifier(list, list1, list2...)
        // Don't forget, each identifier can also look like: __identifier__ because reasons
        // Deal with this

        // Basic way of joining identifiers where possible, theoretically we should deal with this the same way we deal with C23
        // attributes, but for now since we don't do extensions I just want to throw them into an unordered_map and mass reject it
        // if we're not doing a has_embed
        if (state == IdentifierParsing)
        {
            // Gotta love state machines!
            // We get kicked back here from AdditionalIdentifierParsing so we reduce the logic,
            // this just is recursive depth search (kind of) in a "stackless" manner to generate the total value
            if (!identifierString.empty() && tk->IsKeyword())
            {
                switch (tk->GetKeyword())
                {
                    case kw::coloncolon:
                        identifierString += tk->GetChars();
                        state = AdditionalIdentifierParsing;
                        break;
                    case kw::openpa:
                        TokenBalancer.push('(');
                        state = BalancedParsing;
                        break;
                    default:
                        // Wheeeeeeeeee, would be nice if tokens also included position information so I could print the exact
                        // keyword here
                        Errors::ErrorWithLine("Syntax error: Incorrect value for a keyword in this context", Errors::GetFileName(),
                                              Errors::GetErrorLine());
                        break;
                }
            }
            else if (tk->IsIdentifier())
            {
                if (!identifierString.empty())
                {
                    // Initialize this where possible
                    // C++20 adds contains, which will be *so* nice.
                    auto val = info.mapped_values.find(identifierString);
                    if (val == info.mapped_values.end())
                    {
                        info.mapped_values[identifierString] = std::vector<embed_token_type>();
                        identifierString = "";
                    }
                    else
                    {
                        Errors::ErrorWithLine("Attempted to set the same identifier twice", Errors::GetFileName(),
                                              Errors::GetErrorLine());
                    }
                }
                std::string my_copy = tk->GetId();
                identifierString = strip_underscores(my_copy);
                // Don't special-case our own identifiers here, what we want to do is just add ourselves to the unknown-values map
                // if possible
            }
        }
        else if (state == BalancedParsing)
        {
            if (tk->IsKeyword())
            {
                if (tk->GetKeyword() == kw::closepa)
                {
                    info.mapped_values[identifierString] = items;
                    identifierString = "";
                    items = std::vector<embed_token_type>();
                    state = IdentifierParsing;
                }
                else
                {
                    // It would be *awesome* to be able to have
                    items.push_back(tk);
                }
            }
            else
            {
                // Theoretically a lot can be here, if it ain't a comma, just munch
                items.push_back(tk);
            }
        }
        else if (state == AdditionalIdentifierParsing)
        {
            state = IdentifierParsing;
            if (!tk->IsIdentifier() && !tk->IsKeyword())
            {
                Errors::ErrorWithLine("Expected a new identifier or more :: extensions", Errors::GetFileName(),
                                      Errors::GetErrorLine());
            }
            if (tk->IsKeyword())
            {
                if (tk->GetKeyword() == kw::coloncolon)
                {
                    identifierString += tk->GetChars();
                    state = AdditionalIdentifierParsing;
                }
                else
                {
                    Errors::ErrorWithLine("Unexpected Keyword after identifier extension", Errors::GetFileName(),
                                          Errors::GetErrorLine());
                }
            }
            if (tk->IsIdentifier())
            {
                identifierString += tk->GetChars();
            }
        }
    }
    return info;
}
