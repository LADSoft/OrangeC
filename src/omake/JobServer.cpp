#include "JobServer.h"
#include <random>
#include <array>
#include <algorithm>
#ifdef _WIN32
#include <process.h>
#endif
namespace OMAKE
{
std::shared_ptr<JobServer> JobServer::GetJobServer(int max_jobs)
{
#ifdef _WIN32
    // let's use a number that is gauranteed to be unique per-process so that no other process can *ACCIDENTALLY* generate this
    // value unless they're trying to be sneaky
    int pid = getpid();
    std::string omake_string = "OMAKE";
    std::string combined = omake_string + std::to_string(pid);
    return std::make_shared<WINDOWSJobServer>(combined, max_jobs);
#else
    return std::make_shared<POSIXJobServer>(max_jobs);
#endif
}
std::shared_ptr<JobServer> JobServer::GetJobServer(const std::string& auth_string)
{
#ifdef _WIN32
    return std::make_shared<WINDOWSJobServer>(auth_string);
#else
    int readfd, writefd;
    std::string read = auth_string.substr(0, auth_string.find(','));
    std::string write = auth_string.substr(auth_string.find(','));
    readfd = std::stoi(read);
    writefd = std::stoi(write);
    return std::make_shared<POSIXJobServer>(readfd, writefd);
#endif
}
std::shared_ptr<JobServer> JobServer::GetJobServer(const std::string& auth_string, int max_jobs)
{
    return std::make_shared<WINDOWSJobServer>(auth_string, max_jobs);
}
std::shared_ptr<JobServer> JobServer::GetJobServer(int max_jobs, bool ignored)
{
    (void)ignored;
    std::array<unsigned char, 10> rnd;

    std::uniform_int_distribution<int> distribution('0', '9');
    // note that there will be minor problems if the implementation of random_device
    // uses a prng with constant seed for the random_device implementation.
    // that shouldn't be a problem on OS we are interested in.
    std::random_device dev;
    std::mt19937 engine(dev());
    auto generator = std::bind(distribution, engine);

    std::generate(rnd.begin(), rnd.end(), generator);
    std::string generatedName = "OMAKE";
    for (auto v : rnd)
    {
        generatedName += v;
    }
    return JobServer::GetJobServer(generatedName, max_jobs);
}
}  // namespace OMAKE