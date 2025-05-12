#include <iostream>
namespace OrangeC
{
namespace Utils
{
enum class VerbosityLevels
{
    ERROR,
    WARNING,
    DEBUG,
    INFO,
    EXTREMEDEBUG,
};
class BasicLogger
{
    static int verbosity;

  public:
    static void SetVerbosity(int iverbosity) { verbosity = iverbosity; }
    static std::string stringify(std::string str) { return str; }
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
            std::cout << stringify(args...) << std::endl;
        }
    }
    template <typename... Args>
    static void log(VerbosityLevels verbositylevel, Args... args)
    {
        log((int)verbositylevel, args...);
    }
};
};  // namespace Utils
};  // namespace OrangeC