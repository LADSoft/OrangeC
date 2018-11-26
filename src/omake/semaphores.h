#pragma once
#ifdef _WIN32
#    include <windows.h>
#elif defined(__linux__)
#    include <semaphore.h>
#    include <fcntl.h>
#    include <sys/stat.h>
// reason it doesn't work well: POSIX semaphores mandate that you unlink the semaphore first, which means that no new semaphores
// that reference that one can be made This would mean we would be creating about a million semaphores just for this
#    pragma warning "The current semaphore implementation for linux does not work well with omake at all"
#else
#    pragma error "Your platform is currently not supported, contact author with semaphore implementation to get it in"
#endif
#include <string>
class Semaphore
{
#ifdef _WIN32
    using semaphore_type = HANDLE;
    using semaphore_pointer_type = HANDLE;
#elif defined(__linux__)
    using semaphore_type = sem_t;
    using semaphore_pointer_type = sem_t*;
#endif
    semaphore_pointer_type handle;
    bool named;
    bool null = true;

  public:
    Semaphore() : null(true) {}
    Semaphore(int value) : named(false)
    {
        semaphore_type sem;
#ifdef _WIN32
        handle = CreateSemaphore(nullptr, value, value, nullptr);
#elif defined(__linux__)
        sem_init(&sem, 0, value);
        handle = &sem;
#endif
    }
    Semaphore(std::string name, int value) : named(true)
    {
#ifdef _WIN32
        handle = CreateSemaphore(nullptr, value, value, name.c_str());
#elif defined(__linux__)
        handle = sem_open(name.c_str(), O_CREAT, O_RDWR, value);
#endif
    }
    ~Semaphore()
    {
        if (null)
            return;
#ifdef _WIN32
        CloseHandle(handle);
#elif defined(__linux__)
        if (named)
        {
            sem_close(handle);
        }
        else
        {
            sem_destroy(handle);
        }
#endif
    }
    void Wait()
    {
#ifdef _WIN32
        WaitForSingleObject(handle, INFINITE);
#elif defined(__linux__)
        sem_wait(handle);
#endif
    }
    void Post()
    {
#ifdef _WIN32
        ReleaseSemaphore(handle, 1, nullptr);
#elif defined(__linux__)
        sem_post(handle);
#endif
    }
    void Post(int value)
    {
#ifdef _WIN32
        ReleaseSemaphore(handle, value, nullptr);
#elif defined(__linux__)
        for (int i = 0; i < value; i++)
        {
            sem_post(value);
        }
#endif
    }
};