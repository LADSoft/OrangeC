#include "../../JobServer.h"
#include <unistd.h>
#include <cstdint>
#include <sys/types.h>
#include <errno.h>
#include <thread>
#include <stdexcept>
#include <sstream>
namespace OMAKE
{
bool POSIXJobServer::TryTakeNewJob()
{
    if (writefd == -1 || readfd == -1)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    //    if (current_jobs != 0)
    {
        int err = 0;
        char only_buffer;
        ssize_t bytes_read;
    try_again:
        // Wait while we can't get our own
        while ((bytes_read = read(readfd, &only_buffer, 1)) == 0)
        {
            // Yield our thread to other executions while we're waiting for a thread to actually execute and take up time
            std::this_thread::yield();
        }
        if (bytes_read == -1)
        {
            switch (err = errno)
            {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
#endif
                    return false;
                default:
                    throw std::system_error(err, std::system_category());
            }
        }
    }
    //    current_jobs++;
    //    return true;
}
bool POSIXJobServer::TakeNewJob()
{
    if (writefd == -1 || readfd == -1)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    //    if (current_jobs != 0)
    {
        int err = 0;
        char only_buffer;
        ssize_t bytes_read;
    try_again:
        // Wait while we can't get our own
        while ((bytes_read = read(readfd, &only_buffer, 1)) == 0)
        {
            // Yield our thread to other executions while we're waiting for a thread to actually execute and take up time
            std::this_thread::yield();
        }
        if (bytes_read == -1)
        {
            switch (err = errno)
            {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
#endif
                    goto try_again;
                    break;
                default:
                    throw std::system_error(err, std::system_category());
            }
        }
    }
    //    current_jobs++;
    //    return true;
}
bool POSIXJobServer::ReleaseJob()
{
    if (writefd == -1 || readfd == -1)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    if (current_jobs == 0)
    {
        throw std::runtime_error("Job server has returned more jobs than it has consumed");
    }
    else  // if (current_jobs != 1)
    {
        int err = 0;
        char write_buffer = '1';
        ssize_t bytes_written = 0;
    try_again:
        if ((bytes_written = write(writefd, &write_buffer, 1)) != -1)
        {
            return true;
        }
        else
        {
            switch (err = errno)
            {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
#endif
                    // yield execution in hopes that it's not blocked next time, this is recommended
                    // practice for spinloops
                    std::this_thread::yield();
                    goto try_again;
                    break;
                default:
                    throw std::system_error(err, std::system_category());
            }
        }
    }
    current_jobs--;
    return true;
}
// Populates the write pipe with the maximum number of jobs available in the pipe, only used on the first construction of the pipe
static int populate_pipe(int writefd, int max_jobs)
{
    for (int i = 0; i < max_jobs; i++)
    {
        int err = 0;
        char write_buffer = '1';
        ssize_t bytes_written = 0;
    try_again:
        if ((bytes_written = write(writefd, &write_buffer, 1)) != -1)
        {
            return true;
        }
        else
        {
            switch (err = errno)
            {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
#endif
                    std::this_thread::yield();
                    goto try_again;
                    break;
                default:
                    throw std::system_error(err, std::system_category());
            }
        }
    }
}
POSIXJobServer::POSIXJobServer(int max_jobs)
{
    int readwrite[2];
    if (max_jobs < 1)
    {
        throw std::invalid_argument("The max_jobs that a JobServer has cannot be less than one at the time of construction");
    }
    if (pipe(readwrite) == -1)
    {
        readfd = readwrite[0];
        writefd = readwrite[1];
    }
    populate_pipe(writefd, max_jobs);
}
POSIXJobServer::POSIXJobServer(int read, int write)
{
    readfd = read;
    writefd = write;
}
std::string POSIXJobServer::PassThroughCommandString()
{
    std::stringstream stream;
    stream << readfd << ',' << writefd;
    return stream.str();
}
#ifdef _WIN32
WINDOWSJobServer::WINDOWSJobServer(const string_type& server_name)
{
    throw std::runtime_error("Windows job servers are unavailable on POSIX");
}
WINDOWSJobServer::WINDOWSJobServer(const string_type& server_name, int max_jobs)
{
    throw std::runtime_error("Windows job servers are unavailable on POSIX");
}
std::string WINDOWSJobServer::PassThroughCommandString()
{
    throw std::runtime_error("Windows job servers are unavailable on POSIX");
}
bool WINDOWSJobServer::TryTakeNewJob() { throw std::runtime_error("Windows job servers are unavailable on POSIX"); }
bool WINDOWSJobServer::TakeNewJob() { throw std::runtime_error("Windows job servers are unavailable on POSIX"); }
bool WINDOWSJobServer::ReleaseJob() { throw std::runtime_error("Windows job servers are unavailable on POSIX"); }
#endif
}  // namespace OMAKE