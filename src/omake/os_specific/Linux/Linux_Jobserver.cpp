#include "../../JobServer.h"
#include <unistd.h>
#include <cstdint>
#include <sys/types.h>
#include <errno.h>
#include <thread>
#include <stdexcept>
#include <sstream>
#include <sys/stat.h>
#include "BasicLogging.h"
namespace OMAKE
{
bool POSIXJobServer::TryTakeNewJob()
{
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "TryTakeNewJob function start");
    if (writefd == -1 || readfd == -1)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    if (current_jobs != 0)
    {
        int err = 0;
        char only_buffer;
        ssize_t bytes_read;
        int zero_jobs = 0;
        bool success = false;
        // Wait while we can't get our own
        if (!(success = current_jobs.compare_exchange_strong(zero_jobs, 1)) && (bytes_read = read(readfd, &only_buffer, 1)) <= 0)
        {
            OrangeC::Utils::BasicLogger::extremedebug("Tried to take one new job");
            return false;
        }
        if (success)
        {
            OrangeC::Utils::BasicLogger::extremedebug("Took one new job");
            return true;
        }

        if (bytes_read == -1)
        {
            switch (err = errno)
            {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
#endif
                    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG,
                                                     "TryTakeNewJob function returning -1 after a blocking error");

                    return false;
                default:
                    throw std::system_error(err, std::system_category());
            }
        }
        else if (bytes_read == 0)
        {
            // Do nothing
        }
        else
        {
            OrangeC::Utils::BasicLogger::extremedebug("Took one new job");
            char_stack_mutex.lock();
            popped_char_stack.push(only_buffer);
            char_stack_mutex.unlock();
            current_jobs++;
            OrangeC::Utils::BasicLogger::extremedebug("TryTakeNewJob function return true end");

            return true;
        }
    }
    current_jobs++;
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "TryTakeNewJob function final end");

    return false;
}
bool POSIXJobServer::TakeNewJob()
{
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "TakeNewJob start");
    if (writefd == -1 || readfd == -1)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters");
    }
    int err = 0;
    char only_buffer;
    ssize_t bytes_read;
    int exchange_value = 0;
    // Wait while we can't get our own
    int start_jobs = current_jobs.load();
    do
    {
        exchange_value = 0;
        bool exchanged = current_jobs.compare_exchange_strong(exchange_value, 1);
        if (exchanged)
        {
            OrangeC::Utils::BasicLogger::extremedebug("Setting current_jobs to one via compare_exchange_strong, exchange_value: ",
                                                      exchange_value);
            OrangeC::Utils::BasicLogger::extremedebug("TakeNewJob end");
            return true;
        }
        bytes_read = read(readfd, &only_buffer, 1);
        if (bytes_read == -1)
        {
            switch (err = errno)
            {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
#endif
                    continue;
                    break;
                default:
                    throw std::system_error(err, std::system_category());
            }
        }
        if (bytes_read >= 1)
        {
            char_stack_mutex.lock();

            popped_char_stack.push(only_buffer);
            char_stack_mutex.unlock();
            OrangeC::Utils::BasicLogger::extremedebug("Adding one to current_jobs via read()");
            current_jobs++;
            OrangeC::Utils::BasicLogger::extremedebug("Took one new job via TakeNewJob. original_jobs: ", start_jobs,
                                                      " current_jobs: ", current_jobs.load());
            OrangeC::Utils::BasicLogger::extremedebug("TakeNewJob end");
            return true;
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    } while (true);

    return true;
}
bool POSIXJobServer::ReleaseJob()
{
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "ReleaseJob start");

    if (writefd == -1 || readfd == -1)
    {
        throw std::runtime_error("Job server used without initializing the underlying parameters, popped_char_stack count: " +
                                 std::to_string(popped_char_stack.size()));
    }
    if (current_jobs == 0)
    {
        throw std::runtime_error("Job server has returned more jobs than it has consumed");
    }
    else if (current_jobs > 1)
    {
        if (popped_char_stack.size() == 0)
        {
            throw std::runtime_error("Somehow current_jobs is: " + std::to_string(current_jobs.load()) +
                                     " but popped_char_stack is: 0");
        }
        int err = 0;
        char_stack_mutex.lock();
        char write_buffer = popped_char_stack.top();
        char_stack_mutex.unlock();
        ssize_t bytes_written = 0;
    try_again:
        if ((bytes_written = write(writefd, &write_buffer, 1)) != -1)
        {
            char_stack_mutex.lock();
            popped_char_stack.pop();
            char_stack_mutex.unlock();
            OrangeC::Utils::BasicLogger::extremedebug("ReleaseJob job popping write()");
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
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG,
                                     "ReleaseJob end, current_jobs: ", current_jobs.load());

    return true;
}
// Populates the write pipe with the maximum number of jobs available in the pipe, only used on the first construction of the pipe
static int populate_pipe(int writefd, int max_jobs)
{
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "populate_pipe start");
    int total_written = 0;
    for (int i = 0; i < max_jobs; i++)
    {
        int err = 0;
        char write_buffer = '1';
        ssize_t bytes_written = 0;
    try_again:
        bytes_written = write(writefd, &write_buffer, 1);

        if (bytes_written != -1)
        {
            if (max_jobs != total_written)
            {
                continue;
            }
            std::cout << "populate_pipe end write early return" << std::endl;
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
                default:
                    throw std::system_error(err, std::system_category());
            }
        }
    }
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "populate_pipe end");
    return true;
}
POSIXJobServer::POSIXJobServer(int max_jobs)
{
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "PosixJobServer(max_jobs) start");

    int readwrite[2];
    if (max_jobs < 1)
    {
        throw std::invalid_argument("The max_jobs that a JobServer has cannot be less than one at the time of construction");
    }
    if (pipe(readwrite) != -1)
    {
        readfd = readwrite[0];
        writefd = readwrite[1];
    }
    else
    {
        throw std::system_error(errno, std::generic_category());
    }
    populate_pipe(writefd, max_jobs);
    OrangeC::Utils::BasicLogger::log(OrangeC::Utils::VerbosityLevels::VERB_EXTREMEDEBUG, "PosixJobServer(max_jobs) end");
}
POSIXJobServer::POSIXJobServer(int read, int write)
{
    readfd = read;
    writefd = write;
}
POSIXJobServer::POSIXJobServer(std::string auth_name, int max_jobs)
{
    std::string fifo_location = "/tmp/" + auth_name;
    this->fifo_name = fifo_location;
    int ret = mkfifo(fifo_location.c_str(), 0666);
    if (ret == -1)
    {
        throw std::system_error(errno, std::system_category(), "mkfifo failed");
    }
    readfd = open(fifo_location.c_str(), O_RDONLY | O_NONBLOCK);
    int errno1 = errno;
    writefd = open(fifo_location.c_str(), O_WRONLY | O_NONBLOCK);
    int errno2 = errno;

    if (readfd == -1 || writefd == -1)
    {
        throw std::runtime_error(std::string("Writefd or readfd were created as -1! errnos: ") + "1: " + std::to_string(errno1) +
                                 " 2: " + std::to_string(errno2));
    }
    this->fifo_name = auth_name;
    populate_pipe(writefd, max_jobs);
}
POSIXJobServer::POSIXJobServer(std::string auth_name)
{
    this->fifo_name = auth_name;
    readfd = open(fifo_name.c_str(), O_RDONLY | O_NONBLOCK);
    int errno1 = errno;
    writefd = open(fifo_name.c_str(), O_WRONLY | O_NONBLOCK);
    int errno2 = errno;
    if (readfd == -1 || writefd == -1)
    {
        throw std::runtime_error(std::string("Writefd or readfd were created as -1! errnos: ") + "1: " + std::to_string(errno1) +
                                 " 2: " + std::to_string(errno2));
    }
}
POSIXJobServer::~POSIXJobServer()
{
    this->ReleaseAllJobs();
    if (this->readfd != -1)
    {
        close(readfd);
        close(writefd);
    }
}
std::string POSIXJobServer::PassThroughCommandString()
{
    if (fifo_name != "")
    {
        return "auth:" + std::string("/tmp/") + this->fifo_name;
    }
    std::stringstream stream;
    stream << readfd << ',' << writefd;
    return stream.str();
}
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
WINDOWSJobServer::~WINDOWSJobServer() = default;
}  // namespace OMAKE