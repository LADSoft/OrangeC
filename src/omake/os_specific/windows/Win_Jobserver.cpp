#include "../../JobServer.h"
#include "../../semaphores.h"
#include "BasicLogging.h"
#include <stdexcept>

namespace OMAKE
{
// Notes and assumptions:
// A null name for a job server means that we didn't initialize the server
WINDOWSJobServer::WINDOWSJobServer(const string_type& server_name, int max_jobs)
{
    if (server_name.length() == 0)
    {
        throw std::invalid_argument("server_name cannot be null");
    }
    if (max_jobs < 1)
    {
        throw std::invalid_argument("max_jobs cannot be less than one");
    }
    this->semaphore = std::move(Semaphore(server_name, max_jobs));
    this->server_name = server_name;
}
WINDOWSJobServer::WINDOWSJobServer(const string_type& server_name)
{
    if (server_name.length() == 0)
    {
        throw std::invalid_argument("server_name cannot be null");
    }
    this->semaphore = std::move(Semaphore(server_name));
    this->server_name = server_name;
}
bool WINDOWSJobServer::TryTakeNewJob()
{
    if (server_name.length() == 0)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    // Increment beforehand so that we don't accidentally wait too much with this...
    current_jobs++;
    if (current_jobs != 1)
    {
        bool ret = semaphore.TryWait();  // Wait until you have a job to claim it, only do this if we need to actually have a job
        if (ret == false)
        {
            current_jobs--;
        }
        return ret;
    }
    return true;
}
bool WINDOWSJobServer::TakeNewJob()
{
    OrangeC::Utils::BasicLogger::extremedebug("Taking new job, current number of jobs: ", current_jobs.load());
    if (server_name.length() == 0)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    if (current_jobs >= 1)
    {
        semaphore.Wait();  // Wait until you have a job to claim it, only do this if we need to actually have a job
    }
    current_jobs++;
    return true;
}
bool WINDOWSJobServer::ReleaseJob()
{
    if (server_name.length() == 0)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    if (current_jobs == 0)
    {
        throw std::runtime_error("Job server has returned more jobs than it has consumed");
    }
    else if (current_jobs > 1)
    {
        semaphore.Post();
    }
    current_jobs--;  // Wait until after the job is done to release it
    OrangeC::Utils::BasicLogger::extremedebug("Released job, current number of jobs: ", current_jobs.load());

    return true;
}
std::string WINDOWSJobServer::PassThroughCommandString() { return server_name; }
POSIXJobServer::POSIXJobServer(int max_jobs) { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
POSIXJobServer::POSIXJobServer(int read, int write) { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
bool POSIXJobServer::ReleaseJob() { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
bool POSIXJobServer::TakeNewJob() { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
bool POSIXJobServer::TryTakeNewJob() { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
std::string POSIXJobServer::PassThroughCommandString()
{
    throw std::runtime_error("POSIX jobservers are not supported on windows");
}
}  // namespace OMAKE