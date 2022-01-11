#include "../../JobServer.h"
#include "../../semaphores.h"
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
int WINDOWSJobServer::TakeNewJob()
{
    if (server_name.length() == 0)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    // If we have no jobs running, we're allowed precisely *ONE* job, since this is atomic, we should be ensured (enough on a
    // completely sqt_cst CPU) that this is good enough for now to ensure no funny business
    if (current_jobs != 0)
    {
        current_jobs++;
        semaphore.Wait();  // Wait until you have a job to claim it, only do this if we need to actually have a job
        return 0;
    }
    current_jobs++;
    return 0;
}
int WINDOWSJobServer::ReleaseJob()
{
    if (server_name.length() == 0)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    if (current_jobs == 0)
    {
        throw std::runtime_error("Job server has returned more jobs than it has consumed");
    }
    else if (current_jobs != 1)
    {
        semaphore.Post();
    }
    current_jobs--;  // Wait until after the job is done to release it
    return 0;
}
std::string WINDOWSJobServer::PassThroughCommandString()
{
    std::string defaultStr = std::string("--jobserver-auth=");
    return defaultStr + server_name;
}
POSIXJobServer::POSIXJobServer(int max_jobs) { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
POSIXJobServer::POSIXJobServer(int read, int write) { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
int POSIXJobServer::ReleaseJob() { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
int POSIXJobServer::TakeNewJob() { throw std::runtime_error("POSIX jobservers are not supported on windows"); }
std::string POSIXJobServer::PassThroughCommandString()
{
    throw std::runtime_error("POSIX jobservers are not supported on windows");
}
}  // namespace OMAKE