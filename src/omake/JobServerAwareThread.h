#pragma once
#include "IJobServer.h"
#include <functional>
#include <thread>
#include <utility>
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
    std::thread underlying_thread;

  public:
    // We need this to allow for creation of vectors of threads so that we don't get yelled at, so we can do thread pooling with the
    // threads spawned by the job server, useful, I know
    JobServerAwareThread() {}
    // It doesn't like when I try to std::forward these arguments for some reason :/
    // Either way, this just gives us an easy way to gain the callback, we will be using this either way
    template <class Function, class... Args>
    explicit JobServerAwareThread(std::shared_ptr<IJobServer> callback_class, Function&& f, Args&&... args) :
        underlying_thread(std::forward<Function>(f), std::forward<Args>(args)...), callback(callback_class)
    {
        if(callback)
        {
            callback->TakeNewJob();
        }
    }
    // A wrapper around thread::join because we want to use the join call as a finalized callback to release the data, if this were
    // C++20 I'd actually recommend doing one *EXTREMELY* important thing: inherit from std::jthread so that *NO MATTER WHAT* join()
    // gets called, alas, this is written in C++14, and as such, no std::jthread so we can't confirm that this is called no matter
    // what, and I don't want to check if this dies...
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
        // We never actually checked if the callback was valid until now...
        if (callback)
            callback->ReleaseJob();
    }
};
}  // namespace OMAKE