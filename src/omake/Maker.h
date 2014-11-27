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
#ifndef MAKER_H
#define MAKER_H

#include <string>
#include <map>
#include <deque>
#include <set>
#include "Spawner.h"

class Variable;
class Maker
{
    class Depends
    {
    public:
        Depends(const std::string &Goal, Time &xx, bool ToDelete)
        : goal(Goal), time(xx), toDelete(ToDelete), ordered(false), isSecondary(false), rule(NULL), ruleList(NULL) { }
        ~Depends();
        
        void operator +=(Depends *depend) { subgoals.push_back(depend); }
        Time &GetTime() { return time; }
        static Depends *Lookup(const std::string &val) 
        { 
            std::map<std::string, Depends *>::iterator it = all.find(val);
            if (it != all.end())
                return it->second;
            return NULL;
        }
        size_t size() const { return subgoals.size(); }
        void SetRuleList(RuleList *r) { ruleList = r; }
        RuleList *GetRuleList() const { return ruleList; }
        void SetRule(Rule *r) { rule = r; }
        Rule *GetRule() const { return rule; }
        void SetSilent(bool flag) { silent = flag; }
        void SetDisplayOnly(bool flag) { displayOnly = flag; }
        void SetIgnoreResults(bool flag) { ignoreResults = flag; }
        void SetOrdered(bool flag) { ordered = flag; }
        bool GetOrdered() { return ordered; }		
        bool ShouldDelete() { return toDelete; }
        void SetSecondary(bool flag) { isSecondary = flag; }
        bool IsSecondary() { return isSecondary; }
        std::string GetGoal() const { return goal; }
        typedef std::deque<Depends *>::iterator iterator;
        const iterator begin() { return subgoals.begin(); }
        const iterator end() { return subgoals.end(); }
        
    private:
        std::string goal;
        std::string foundName;
        RuleList *ruleList;
        Rule *rule;
        Time time;
        bool silent;
        bool displayOnly;
        bool ignoreResults;
        bool toDelete;
        bool ordered;
        bool isSecondary;
        std::deque<Depends *> subgoals;		
        static std::map<std::string, Depends *> all;
    };
public:
    Maker(bool Silent, bool DisplayOnly, bool IgnoreResults, bool Touch,
          bool rebuildAll = false, std::string newFiles = "", std::string oldFiles = "");
    ~Maker();
    static void SetFirstGoal(const std::string &name);
    static void ClearFirstGoal() { firstGoal = "" ; }
    void AddGoal(const std::string &name) { goals.push_back(name); }
    void Clear();
    bool HasCommands() { return depends.size() != 0; }
    bool CreateDependencyTree();
    int RunCommands(bool keepGoing = false);
    void SetIgnoreFailed(const std::string &name) { ignoreFailedTargets.insert(name); }
    static std::string GetFullName(std::string name);
protected:
    Depends *Dependencies(const std::string &goal, const std::string &preferredPath, Time &timeval);
    bool ExistsOrMentioned(const std::string &stem, RuleList *ruleList, const std::string &preferredPath, const std::string &dir, bool implicit, bool outerMost);
    bool SearchImplicitRules(const std::string &goal, const std::string &preferredPath, bool outerMost, bool err, Time &timeval);
    void EnterSpecificRule(RuleList *l, const std::string &stem, const std::string &preferredPath, bool outerMost);
    void EnterDefaultRule(const std::string &goal, RuleList *dflt);
    std::string GetFileTime(const std::string &goal, const std::string &preferredPath, Time &timeval);
    bool ScanList(const std::string &v, const std::string &goal);
    bool OnList(const std::string &goal, char *what);
    void GetEnvironment(EnvironmentStrings &env);
    void DeleteOne(Depends *depend);
    int RunOne(Depends *depend, EnvironmentStrings &env, bool keepGoing);
    void CancelOne(Depends *depend);
    void EnterSuffixTerminals();
private:
    typedef std::deque<std::string> Goals;
    Goals goals;
    static std::string firstGoal;
    std::deque<Depends *> depends;
    bool silent;
    bool displayOnly;
    bool ignoreResults;
    bool missingTarget;
    bool touch;
    bool rebuildAll;
    std::string newFiles;
    std::string oldFiles;
    std::set<std::string> ignoreFailedTargets;
    static std::map<std::string, std::string> filePaths;
} ;
#endif