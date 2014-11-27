/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#ifndef SPAWNER_H
#define SPAWNER_H

#include <string>
#include <deque>

#include "os.h"

class Command
{
public:
    Command(const std::string &File, int Lineno) : file(File), lineno(Lineno) { }
    ~Command() { }
    void operator += (const std::string &cmd) { commands.push_back(cmd); }

    size_t size() const { return commands.size(); }	
    typedef std::deque<std::string>::iterator iterator;
    typedef std::deque<std::string>::const_iterator const_iterator;
    iterator begin() { return commands.begin(); }
    iterator end() { return commands.end(); }

private:
    std::deque<std::string> commands;
    std::string file;
    int lineno;
};
class RuleList;
class Rule;
class Spawner
{
    public:
        Spawner(const EnvironmentStrings &Environ, bool IgnoreErrors, bool Silent, bool DontRun) : environment(Environ),
            ignoreErrors(IgnoreErrors), silent(Silent), dontRun(DontRun), tempNum(1) { }
        ~Spawner() { }
        int Run(Command &commands, RuleList *ruleList = NULL, Rule *rule= NULL);
        std::string shell(const std::string &command);
        void SetLineLength(int length) { lineLength = length; }
        bool GetLineLength() const { return lineLength; }
        
        static const char escapeStart;
        static const char escapeEnd;
    protected:
        int Run(const std::string &cmd, bool ignoreErrors, bool silent, bool dontrun);
        bool split(const std::string &cmd);
        std::string QualifyFiles(const std::string &cmd);
    private:
        static int lineLength;
        static std::deque<std::string> cmdList;
        EnvironmentStrings environment;
        bool ignoreErrors;
        bool silent;
        bool dontRun;
        int tempNum;
};
#endif
