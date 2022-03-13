#pragma once
namespace OMAKE
{
// A class that's (currently) dedicated to being a callback, theoretically should retain some parts of the current job-server
// implementation so that we have a layer of indirection here...
class IJobServer
{
  public:
    // these are only possible when we have a system to work on, except release all jobs...
    // Takes a new job, allows you to declare that we are using one more job
    virtual bool TakeNewJob() = 0;
    // Attempts to take a new job or returns -1
    virtual bool TryTakeNewJob() = 0;
    // Releases that job for another worker to use
    virtual bool ReleaseJob() = 0;
};
}  // namespace OMAKE