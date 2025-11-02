#pragma once
#ifdef TARGET_OS_WINDOWS
#    define NOMINMAX 1
#    include <windows.h>
#else
#    include <semaphore.h>
#    include <errno.h>
#    include <fcntl.h>
#    pragma warning \
        "This platform is unsupported for named sempahores due to POSIX semaphores (named ones) being legitimately several times worse than Windows ones"
#endif
#include <cassert>
#include <string>
#include <iostream>
#include <chrono>
class Semaphore
{
#ifdef TARGET_OS_WINDOWS
    using semaphore_type = HANDLE;
#    ifdef UNICODE
#        define USE_WIDE_STRING
#    endif
#else
    using semaphore_type = sem_t;
#endif
#ifdef USE_WIDE_STRING
    using string_type = std::wstring;
#else
    using string_type = std::string;
#endif
    semaphore_type handle;
    bool named = false;
    bool null = false;
    string_type semaphoreName;

  public:
    Semaphore() : null(true), semaphoreName(), handle(semaphore_type()) {}
    Semaphore(string_type name, int value) : named(true), semaphoreName(std::move(name))
    {
#ifdef TARGET_OS_WINDOWS
        handle = CreateSemaphore(nullptr, value, value, semaphoreName.c_str());
        if (!handle)
        {
            throw std::runtime_error("CreateSemaphore failed, Error code: " + std::to_string(GetLastError()));
        }
#else
        // 0 in this case means this is shared internally, not externally
        int ret = sem_init(&handle, 0, value);
        // https://www.man7.org/linux/man-pages/man3/sem_init.3.html
        // IT RETURNS 0 ON SUCCESS
        if (ret != 0)
        {
            throw std::system_error(errno, std::system_category());
        }
#endif
    }
    Semaphore(int value)
    {
        // TODO: in C++20 move to std::counting_semaphore and banish this away back to pure atomics instead of syscalls
// note this works slightly differently from the other constructor...
#ifdef TARGET_OS_WINDOWS
        handle = CreateSemaphore(nullptr, 0, value, nullptr);
        if (!handle)
        {
            throw std::runtime_error("CreateSemaphore failed, Error code: " + std::to_string(GetLastError()));
        }
#else
        // 0 in this case means this is shared internally, not externally
        int ret = sem_init(&handle, 0, value);
        if (ret != 0)
        {
            throw std::system_error(errno, std::system_category());
        }
#endif
    }
    Semaphore(int value, int init) : Semaphore(value) { Post(init); }
    Semaphore(const string_type& name) : named(true), semaphoreName(name)
    {
#ifdef TARGET_OS_WINDOWS
        handle = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, name.c_str());
        if (!handle)
        {
            throw std::invalid_argument("OpenSemaphore failed, presumably bad name, Error code: " + std::to_string(GetLastError()) +
                                        " name: " + name);
        }
#else
        auto ret = sem_open(name.c_str(), O_RDWR);
        if (ret == SEM_FAILED)
        {
            throw std::invalid_argument("sem_open failed, presumably bad name, Error code: " + std::to_string(errno));
        }
        handle = *ret;
#endif
    }
    Semaphore& operator=(const Semaphore& other)
    {
        if (!other.named)
        {
            throw std::runtime_error("Cannot copy an unnamed semaphore");
        }
        if (this != &other)
        {
            semaphoreName = other.semaphoreName;
            named = other.named;
            this->null = other.null;
            if (!this->null)
            {
#ifdef TARGET_OS_WINDOWS
                handle = OpenSemaphore(EVENT_ALL_ACCESS, false, semaphoreName.c_str());
#else
                auto ret = sem_open(semaphoreName.c_str(), O_RDWR);
                if (ret == SEM_FAILED)
                {
                    throw std::invalid_argument("OpenSemaphore failed, presumably bad name, Error code: " + std::to_string(errno));
                }
                handle = *ret;
#endif
            }
        }
        return *this;
    }
    Semaphore& operator=(Semaphore&& other)
    {
        if (!other.named)
        {
            throw std::runtime_error("Cannot move an unnamed semaphore");
        }
        if (this != &other)
        {
            if (!null)
            {
#ifdef TARGET_OS_WINDOWS
                CloseHandle(other.handle);
#else
                if (named)
                {
                    sem_close(&handle);
                }
                else
                {
                    sem_destroy(&handle);
                }
#endif
            }
            named = other.named;
            semaphoreName = other.semaphoreName;
            this->null = other.null;
            if (!this->null)
            {
#ifdef TARGET_OS_WINDOWS
                handle = OpenSemaphore(EVENT_ALL_ACCESS, false, semaphoreName.c_str());
#else
                auto ret = sem_open(semaphoreName.c_str(), O_RDWR);
                if (ret == SEM_FAILED)
                {
                    throw std::invalid_argument("OpenSemaphore failed, presumably bad name, Error code: " + std::to_string(errno));
                }
                handle = *ret;
#endif
            }
            other.named = false;
            other.null = true;
#ifdef TARGET_OS_WINDOWS
            CloseHandle(other.handle);
#else
            if (named)
            {
                sem_close(&handle);
            }
            else
            {
                sem_destroy(&handle);
            }
#endif
        }
        return *this;
    }
    ~Semaphore()
    {
        if (null)
            return;
#ifdef TARGET_OS_WINDOWS
        CloseHandle(handle);
#else
        if (named)
        {
            sem_close(&handle);
        }
        else
        {
            sem_destroy(&handle);
        }
#endif
    }
    template <typename Rep, typename Period>
    bool WaitUntil(std::chrono::duration<Rep, Period> time)
    {
        return WaitFor(time - std::chrono::steady_clock::now());
    }
    // return 0: success
    // return -1: failure
    template <typename Rep, typename Period>
    bool WaitFor(std::chrono::duration<Rep, Period> time)
    {
        std::chrono::milliseconds milli = std::chrono::duration_cast(time);
        int waitTime = milli.count();
#ifdef WIN32
        if (waitTime >= INFINITE)
        {
            throw std::invalid_argument("The time spent for waiting is too long to handle normally");
        }
        DWORD ret = WaitForSingleObject(handle, waitTime);
        switch (ret)
        {
            case WAIT_OBJECT_0:
                return true;
            case WAIT_TIMEOUT:
                return false;
            case WAIT_FAILED:
                throw std::system_error(std::error_code(GetLastError(), std::system_category()));
            default:
                return false;
        }
#else
        timespec ts = {waitTime / 1000, (waitTime % 1000) * 1000000};
        return !sem_timedwait(&handle, &ts);
#endif
    }
    bool TryWait()
    {
#ifdef TARGET_OS_WINDOWS
        DWORD ret = WaitForSingleObject(handle, 0);
        switch (ret)
        {
            case WAIT_OBJECT_0:
                return true;
            case WAIT_TIMEOUT:
                return false;
            case WAIT_FAILED:
                throw std::system_error(std::error_code(GetLastError(), std::system_category()));
            default:
                return false;
        }
#else
        return !!sem_trywait(&handle);
#endif
    }
    void Wait()
    {
        if (null)
        {
            throw std::runtime_error("Did not initialize this semaphore with a non-default constructor");
        }
#ifdef TARGET_OS_WINDOWS
        WaitForSingleObject(handle, INFINITE);
#else
        sem_wait(&handle);
#endif
    }
    void Post()
    {
        if (null)
        {
            throw std::runtime_error("Did not initialize this semaphore with a non-default constructor");
        }
#ifdef TARGET_OS_WINDOWS
        ReleaseSemaphore(handle, 1, nullptr);
#else
        sem_post(&handle);
#endif
    }
    void Post(int value)
    {
        if (null)
        {
            throw std::runtime_error("Did not initialize this semaphore with a non-default constructor");
        }
#ifdef TARGET_OS_WINDOWS
        ReleaseSemaphore(handle, value, nullptr);
#else
        for (int i = 0; i < value; i++)
        {
            sem_post(&handle);
        }
#endif
    }
};