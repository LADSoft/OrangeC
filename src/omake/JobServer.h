#pragma once
#include "JobServerAwareThread.h"
#include "IJobServer.h"
#include <atomic>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <memory>
#ifdef _WIN32
#    include "semaphores.h"
#endif
// The main issue with using semaphores alone for Linux is that named semaphores
// on linux can't be used without knowing the state of every application taking
// the named semaphores, a solution to this is to use a jobserver, in order for
// GNU Make compatability we'll use
// https://www.gnu.org/software/make/manual/html_node/Job-Slots.html#Job-Slots
// as a reference here to show how we can "give and take" slots, not only this,
// but using this as a compatability guide should be a good thing

namespace OMAKE
{
class POSIXJobServer;
class WINDOWSJobServer;
// Master job server that allows us to properly have polymorphic job servers, I believe this is the correct method to create a
// job-server for our purposes,
class JobServer : public IJobServer
{
  protected:
    // The maximum number of jobs, can be any value we like so long as it's above or equal to one
    int max_jobs;
    // The current job count, starts at zero and we can *ALWAYS* assume that there is at least one job available, because of this,
    // the expected jobs is always at least 1 as the max of the current jobs
    std::atomic<int> current_jobs;

  public:
    // This is *REALLY* ineffecient, but is the only *easy* way to do this on POSIX without taking an unfathomably long amount of
    // extra time AFAICT
    virtual void ReleaseAllJobs()
    {
        while (current_jobs > 1)
        {
            ReleaseJob();
        }
    }
    virtual std::string PassThroughCommandString() = 0;
    // Creates a job server with a maximum number of jobs
    static std::shared_ptr<JobServer> GetJobServer(int max_jobs);
    // Opens a job server with a specified authorization code, this is *AFAR* parsing the --jobserver-auth string
    static std::shared_ptr<JobServer> GetJobServer(const std::string& auth_string);
#ifdef _WIN32

    // temporary GetJobServer for compatibility reasons with old code, instead of moving things into JobServer here, we're moving
    // them out
    static std::shared_ptr<JobServer> GetJobServer(const std::string& auth_string, int max_jobs);
#endif
    // Creates an OMAKE compatible job server, allowing for the main class to move out when needed
    static std::shared_ptr<JobServer> GetJobServer(int max_jobs, bool ignored);
};
// Use composition to our advantage: if we have a JobServer and we know it's either one of these, we can use the same code
// without having to worry if it's POSIX or Windows except at the calling barrier
class POSIXJobServer : public JobServer
{
    friend class JobServer;
    int readfd = -1, writefd = -1;
    int get_read_fd() { return readfd; }
    int get_write_fd() { return writefd; }

  public:
    std::string PassThroughCommandString();
    bool TakeNewJob();
    bool TryTakeNewJob();
    bool ReleaseJob();
    // Need these to be public in order to do std::make_shared on em'
    POSIXJobServer(int max_jobs);
    POSIXJobServer(int read, int write);
};
#ifdef _WIN32
class WINDOWSJobServer : public JobServer
{
    friend class JobServer;
#    ifdef _WIN32
#        ifdef UNICODE
    // Use a consistent strategy so that windows doesn't yell at us, I know we don't work in wide strings except in obrc often
    // so this is here for posterity *AND* ease of use
    using string_type = std::wstring;
#        else
    using string_type = std::string;
#        endif
#    else
    using string_type = std::string;
#    endif
  private:
    // Name of the server, use std::string or std::wstring, we do this for compatibility reasons
    string_type server_name;
    // Since we already have a semaphore class that's sufficient, might as well use it so that we aren't carrying around a raw
    // HANDLE, this way we can also elide calls in this place and theoretically an optimization pass could bring the entire object
    // into the class in the future should anyone choose to research inserting an underlying object into another object
    Semaphore semaphore;

  public:
    std::string PassThroughCommandString();
    bool TakeNewJob();
    bool TryTakeNewJob();
    bool ReleaseJob();
    // Need these to be public in order to do std::make_shared on em'
    // Since we use semaphores we want a name because these are interprocess named semaphores
    WINDOWSJobServer(const string_type& server_name, int max_jobs);
    // Interprocess semaphores require names
    WINDOWSJobServer(const string_type& server_name);
};
#endif
}  // namespace OMAKE