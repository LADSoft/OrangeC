#pragma once
#include <string>
#include <iostream>
#include <utility>
#include <functional>
#include <type_traits>

namespace OrangeC
{
namespace Utils
{
enum class VerbosityLevels
{
    VERB_ERROR,
    VERB_WARNING,
    VERB_DEBUG,
    VERB_INFO,
    VERB_EXTREMEDEBUG,
};
class BasicLogger
{
    static int verbosity;
    static std::string prologue;

  public:
    static void SetVerbosity(int iverbosity) { verbosity = iverbosity; }
    static void SetPrologue(std::string str) { prologue = str; }
    static std::string stringify(const std::string&& str) { return str; }
    static std::string stringify(const std::string& str) { return str; }
    static std::string stringify(const char* str) { return str; }
    template <typename T>
    static std::string stringify(T item)
    {
        return std::to_string(item);
    }
    template <typename... Args>
    static void log(int verbositylevel, Args... args)
    {
        if (verbosity >= (int)verbositylevel)
        {
            // I'd really love something like fmt::format here, or std::format, but alas, c++14 vs c++20
            std::cout << prologue;
            auto arr = {stringify(args)...};
            for (auto&& val : arr)
            {
                std::cout << val;
            }
            std::cout << std::endl;
        }
    }
    template <typename... Args>
    static void log(VerbosityLevels verbositylevel, Args... args)
    {
        log((int)verbositylevel, args...);
    }
    template <typename... Args>
    static void extremedebug(Args... args)
    {
        log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, args...);
    }
    template <typename... Args>
    static void debug(Args... args)
    {
        log(OrangeC::Utils::VerbosityLevels::VERB_DEBUG, args...);
    }
};
class LoggerEnterExit
{
    std::string message;

  public:
    LoggerEnterExit(std::string generic_message)
    {
        message = generic_message;
        BasicLogger::extremedebug("Entering: " + message);
    }
    ~LoggerEnterExit() { BasicLogger::extremedebug("Exiting: " + message); }
};
};  // namespace Utils
};  // namespace OrangeC