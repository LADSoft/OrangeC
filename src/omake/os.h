/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef OS_H
#define OS_H

#include <list>
#include <string>

#undef GetCurrentTime

class Time
{
public:
    Time() : seconds(0), ms(0) { }
    Time(unsigned S, unsigned MS) : seconds(S), ms(MS) { }
    unsigned seconds;
    unsigned ms;
    bool operator >(const Time &last);
    bool operator !() { return seconds ==0 && ms == 0; }
    void Clear() { seconds = 0; ms = 0; }
};
struct EnvEntry {
    EnvEntry() {}
    EnvEntry(const std::string Name, const std::string Value) :
        name(Name), value(Value) { }
    std::string name;
    std::string value;
} ;
typedef std::list<EnvEntry> EnvironmentStrings;

class OS
{
public:
    static int Spawn(const std::string command, EnvironmentStrings &environment);
    static std::string SpawnWithRedirect(const std::string command);
    static Time GetCurrentTime();
    static Time GetFileTime(const std::string fileName);
    static void SetFileTime(const std::string fileName, Time time);
    static std::string GetWorkingDir();
    static bool SetWorkingDir(const std::string name);
    static void RemoveFile(const std::string name);
} ;
#endif