#include "BasicLogging.h"
namespace OrangeC
{
namespace Utils
{
int BasicLogger::verbosity = 0;
std::string BasicLogger::prologue = "";
std::mutex BasicLogger::display_mutex;
};  // namespace Utils
};  // namespace OrangeC