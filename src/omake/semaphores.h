#pragma once
#ifdef _WIN32
#    define NOMINMAX
#    include <windows.h>
#elif defined(__linux__)
#    pragma error "This platform is unsupported due to POSIX semaphores being legitimately several times worse than Windows ones"
#else
#    pragma error "Your platform is currently not supported, contact author with semaphore implementation to get it in"
#endif
#include <string>
#include <iostream>
#include <chrono>
class Semaphore
{

#ifdef _WIN32
    using semaphore_type = HANDLE;
    using semaphore_pointer_type = HANDLE;
#    ifdef UNICODE
#        define USE_WIDE_STRING
#    endif
#elif defined(__linux__)
    using semaphore_type = sem_t;
    using semaphore_pointer_type = sem_t*;
#endif
#ifdef USE_WIDE_STRING
    using string_type = std::wstring;
#else
    using string_type = std::string;
#endif
    semaphore_pointer_type handle = nullptr;
    bool named = false;
    bool null = false;
    string_type semaphoreName;

  public:
    Semaphore() : null(true), semaphoreName() {}
    Semaphore(string_type name, int value) : named(true), semaphoreName(name)
    {
#ifdef _WIN32
        handle = CreateSemaphore(nullptr, value, value, name.c_str());
        if (!handle)
        {
            throw std::runtime_error("CreateSemaphore failed, Error code: " + GetLastError());
        }
#elif defined(__linux__)
        handle = sem_open(name.c_str(), O_CREAT, O_RDWR, value);
#endif
    }
    Semaphore(const string_type& name) : named(true), semaphoreName(name)
    {
#ifdef _WIN32
        handle = OpenSemaphore(EVENT_ALL_ACCESS, false, name.c_str());
        if (!handle)
        {
            throw std::runtime_error("OpenSemaphore failed, Error code: " + GetLastError());
        }
#endif
    }
    Semaphore& operator=(const Semaphore& other)
    {
        if (this != &other)
        {
            semaphoreName = other.semaphoreName;
            this->null = other.null;
            if (!this->null)
            {
                handle = OpenSemaphore(EVENT_ALL_ACCESS, false, semaphoreName.c_str());
            }
        }
        return *this;
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
    template <typename Rep, typename Period>
    int WaitUntil(std::chrono::duration<Rep, Period> time)
    {
        return WaitFor(time - std::chrono::steady_clock::now());
    }
    // return 0: success
    // return -1: failure
    template <typename Rep, typename Period>
    int WaitFor(std::chrono::duration<Rep, Period> time)
    {
        std::chrono::milliseconds milli = std::chrono::duration_cast(time);
        if (milli.count() >= INFINITE)
        {
            throw std::invalid_argument("The time spent for waiting is too long to handle normally");
        }
        int waitTime = milli.count();
        int ret = WaitForSingleObject(handle, waitTime);
        switch (ret)
        {
            case WAIT_OBJECT_0:
                return 0;
            case WAIT_TIMEOUT:
                return -1;
            case WAIT_FAILED:
                throw std::system_error(std::error_code(GetLastError(), std::system_category()));
            default:
                return -1;
        }
    }
    int TryWait()
    {
        int ret = WaitForSingleObject(handle, 0);
        switch (ret)
        {
            case WAIT_OBJECT_0:
                return 0;
            case WAIT_TIMEOUT:
                return -1;
            case WAIT_FAILED:
                throw std::system_error(std::error_code(GetLastError(), std::system_category()));
            default:
                return -1;
        }
    }
    void Wait()
    {
        if (null)
        {
            throw std::runtime_error("Did not initialize this semaphore with a non-default constructor");
        }
#ifdef _WIN32
        WaitForSingleObject(handle, INFINITE);
#elif defined(__linux__)
        sem_wait(handle);
#endif
    }
    void Post()
    {
        if (null)
        {
            throw std::runtime_error("Did not initialize this semaphore with a non-default constructor");
        }
#ifdef _WIN32
        ReleaseSemaphore(handle, 1, nullptr);
#elif defined(__linux__)
        sem_post(handle);
#endif
    }
    void Post(int value)
    {
        if (null)
        {
            throw std::runtime_error("Did not initialize this semaphore with a non-default constructor");
        }
#ifdef _WIN32
        ReleaseSemaphore(handle, value, nullptr);
#elif defined(__linux__)
        for (int i = 0; i < value; i++)
        {
            sem_post(handle);
        }
#endif
    }
};