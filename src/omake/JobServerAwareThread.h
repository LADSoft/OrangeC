#pragma once
#include "IJobServer.h"
#include <functional>
#include <thread>
#include <utility>
#include <memory>
namespace OMAKE
{
// A wrapper class for threads that exists to take a job when created and to
// release that job when destroyed, this is a class that's custom-designed to
// ensure that we get as much as possible out of this, since this is a thread, when it dies, we *MUST* return the callback data
class JobServerAwareThread
{
    // A poor man's hack to allow for a class-type function to have this without referencing the actual class, interfaces can be
    // awesome sometimes I swear on my life as someone who did a lot of C# programming, and who figured out that this is a cheap
    // hack around a major problem I was originally having
    // This is private so we can't prematurely release the job server data, same with it being a *shared pointer*, so that the
    // JobServer that we choose is alive until we no longer need it to be
    std::shared_ptr<IJobServer> callback;
    // Use an underlying thread so that we don't get any inheritance shenanigans, theoretically, we could handle inheriting from
    // std::thread, which was my original plan, but I scrapped that during a bug hunt to see if it fixed a bug
    std::thread underlying_thread;

  public:
    // We need this to allow for creation of vectors of threads so that we don't get yelled at, so we can do thread pooling with the
    // threads spawned by the job server, useful, I know
    JobServerAwareThread() {}
    // Easy callback method, we pass in the callback class first to ensure that the args don't catch it accidentally
    template <class Function, class... Args>
    explicit JobServerAwareThread(std::shared_ptr<IJobServer> callback_class, Function&& f, Args&&... args) :
        callback(callback_class)
    {
        if (callback)
        {
            callback->TakeNewJob();
        }
        // make sure we take a job before we actually spawn the thread
        underlying_thread = std::thread(std::forward<Function>(f), std::forward<Args>(args)...);
    }
    // Get an id so that if anyone needs the underlying thread id for any reason they can get it
    std::thread::id get_id() { return underlying_thread.get_id(); }
    // A way of testing if this is joined is to run this program under the original GNU make and have it do multithreaded stuff,
    // since the original make on the main exit will check if all the jobs are taken or not and then complain at us if there are
    // jobs that haven't been returned, we should also implement this functionality but as of right now we don't, what could be done
    // is see if we do any recursive calls and see how many threads each make instance takes and give back from the make instances
    // internally through some weird inter-make stuff but IDK how to do that and GNU Make is already dense as it is to read through,
    // and I want to do clean-room interpretation based off docs and not source code
    void join()
    {
        // Call the parent join so this actually works properly as a "join" call
        underlying_thread.join();
        // We never know if the callback is valid or not, probably should be checked manually
        if (callback)
        {
            callback->ReleaseJob();
        }
    }
};
}  // namespace OMAKE