#pragma once
#include "JobServerAwareThread.h"
#include "IJobServer.h"
#include <atomic>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <memory>
#include "semaphores.h"
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
    // The current job count, starts at one and goes up, protected so that we know how many we need to release, and all calls assume
    // that you have the minimum number of jobs (1) thus, current_jobs starts at 1
    std::atomic<int> current_jobs;
    // Create a singleton so that we can use this to pass to our spawned threads our instance, when we get destructed, we destruct
    // this and it *SHOULD* work... An interesting solution to a dumb problem
    static std::shared_ptr<JobServer> job_server_instance;

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
    // The JobServerAwareThread is aware of the calling jobserver, thus having a wrapper in the job server allows us to create
    // threads which will update our worker numbers easily
    template <class Function, class... Args>
    JobServerAwareThread CreateNewThread(Function&& f, Args&&... args)
    {
        // Threads start upon creation, thus we need to take a job before the thread is created to reserve the slot before work
        // starts *NO MATTER WHAT*, also prevents us from having to have 2 things in the IJobServer... (Could be changed to work so
        // that the thread itself does this(?))
        return JobServerAwareThread(job_server_instance, std::forward<Function>(f), std::forward<Args>(args)...);
    }
    // A region of GetJobServer overloads, which all allow for the static creation of the Windows and POSIX job server types,
    // theoretically these classes *SHOULD* be private to the namespace but I'm not sure C++ allows this and we can technically let
    // this roam free
#pragma region GetJobServerOverloads
    static std::shared_ptr<JobServer> GetJobServer(int max_jobs)
    {
        if (!job_server_instance)
        {
            job_server_instance = std::reinterpret_pointer_cast<JobServer>(std::make_shared<POSIXJobServer>(max_jobs));
        }
        return job_server_instance;
    }
    static std::shared_ptr<JobServer> GetJobServer(int readfd, int writefd)
    {
        if (!job_server_instance)
        {
            job_server_instance = std::reinterpret_pointer_cast<JobServer>(std::make_shared<POSIXJobServer>(readfd, writefd));
        }
        return job_server_instance;
    }
    static std::shared_ptr<JobServer> GetJobServer(const std::string& server_name, int max_jobs)
    {
        if (!job_server_instance)
        {
            job_server_instance =
                std::reinterpret_pointer_cast<JobServer>(std::make_shared<WINDOWSJobServer>(server_name, max_jobs));
        }
        return job_server_instance;
    }
    static std::shared_ptr<JobServer> GetJobServer(const std::string& server_name)
    {
        if (!job_server_instance)
        {
            job_server_instance = std::reinterpret_pointer_cast<JobServer>(std::make_shared<WINDOWSJobServer>(server_name));
        }
        return job_server_instance;
    }
    static std::shared_ptr<JobServer> GetJobServer()
    {
        if (!job_server_instance)
        {
            throw std::runtime_error("Job server attempted to be created or gotten without first initializing a job server");
        }
        return job_server_instance;
    }
#pragma endregion
};
// Use composition to our advantage: if we have a JobServer and we know it's either one of these, we can use the same code
// without having to worry if it's POSIX or Windows except at the calling barrier
class POSIXJobServer : public JobServer
{
    int readfd = -1, writefd = -1;

  public:
    POSIXJobServer(int max_jobs);
    POSIXJobServer(int read, int write);
    std::string PassThroughCommandString();
    int get_read_fd() { return readfd; }
    int get_write_fd() { return writefd; }
    int TakeNewJob();
    int ReleaseJob();
};
class WINDOWSJobServer : public JobServer
{
  public:
#ifdef _WIN32
#    ifdef UNICODE
    // Use a consistent strategy so that windows doesn't yell at us, I know we don't work in wide strings except in obrc often
    // so this is here for posterity *AND* ease of use
    using string_type = std::wstring;
#    else
    using string_type = std::string;
#    endif
#else
    using string_type = std::string;
#endif
  private:
    // Name of the server, use std::string or std::wstring, we do this for compatibility reasons
    string_type server_name;
    // Since we already have a semaphore class that's sufficient, might as well use it so that we aren't carrying around a raw
    // HANDLE, this way we can also elide calls in this place and theoretically an optimization pass could bring the entire object
    // into the class in the future should anyone choose to research inserting an underlying object into another object
    Semaphore semaphore;

  public:
    // Since we use semaphores we want a name because these are interprocess named semaphores
    WINDOWSJobServer(const string_type& server_name, int max_jobs);
    // Interprocess semaphores require names
    WINDOWSJobServer(const string_type& server_name);
    std::string PassThroughCommandString();
    int TakeNewJob();
    int ReleaseJob();
    void ReleaseAllJobs();
};
}  // namespace OMAKE