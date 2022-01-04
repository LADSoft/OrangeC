#pragma once
#ifdef _WIN32
#    include <windows.h>
#elif defined(__linux__)
#    pragma error "This platform is unsupported due to POSIX semaphores being legitimately several times worse than Windows ones"
#else
#    pragma error "Your platform is currently not supported, contact author with semaphore implementation to get it in"
#endif
#include <string>
#include <iostream>
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
    int count = 0;

  public:
    Semaphore() : null(true), semaphoreName()
    {
        std::cout << "Default constructor" << std::endl;
    }
    Semaphore(int value) : semaphoreName()
    {
        std::cout << "Unnamed constructor" << std::endl;
        count--;

#ifdef _WIN32
        handle = CreateSemaphore(nullptr, value, value, nullptr);
        if (!handle)
        {
            throw std::runtime_error("CreateSemaphore failed, Error code: " + GetLastError());
        }
#elif defined(__linux__)
        semaphore_type sem;
        sem_init(&sem, 0, value);
        handle = &sem;
#endif
    }
    Semaphore(string_type name, int value) : named(true), semaphoreName(name)
    {
        count--;
        std::cout << "Named and initialize constructor" << std::endl;

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
        count--;
        std::cout << "Named constructor" << std::endl;

#ifdef _WIN32
        handle = OpenSemaphore(EVENT_ALL_ACCESS, false, name.c_str());
        if (!handle)
        {
            throw std::runtime_error("OpenSemaphore failed, Error code: " + GetLastError());
        }
#endif
    }
    ~Semaphore()
    {
        count++;
        if (null)
            return;
        std::cout << std::to_string((intptr_t)handle) << ", " << count << std::endl;
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