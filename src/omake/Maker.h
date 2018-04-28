/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#ifndef MAKER_H
#define MAKER_H

#include <string>
#include <map>
#include <list>
#include <set>
#include "Spawner.h"

class Variable;
class Maker
{
    class Depends
    {
    public:
        Depends(const std::string &Goal, Time &xx, bool ToDelete)
        : goal(Goal), time(xx), toDelete(ToDelete), ordered(false), isSecondary(false), rule(nullptr), ruleList(nullptr) { }
        ~Depends();
        
        void operator +=(Depends *depend) { subgoals.push_back(depend); }
        Time &GetTime() { return time; }
        static Depends *Lookup(const std::string &val) 
        { 
            auto it = all.find(val);
            if (it != all.end())
                return it->second;
            return nullptr;
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
        typedef std::list<Depends *>::iterator iterator;
        const iterator begin() { return subgoals.begin(); }
        const iterator end() { return subgoals.end(); }
        void Precious() { toDelete = false; }
        
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
        std::list<Depends *> subgoals;		
        static std::map<std::string, Depends *> all;
    };
public:
    Maker(bool Silent, bool DisplayOnly, bool IgnoreResults, bool Touch,
          bool rebuildAll = false, bool keepResponseFiles = false, std::string newFiles = "", std::string oldFiles = "");
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
    Depends *Dependencies(const std::string &goal, const std::string &preferredPath, Time &timeval, bool err, std::string file, int line);
    bool ExistsOrMentioned(const std::string &stem, RuleList *ruleList, const std::string &preferredPath, const std::string &dir, bool implicit, bool outerMost);
    bool SearchImplicitRules(const std::string &goal, const std::string &preferredPath, bool outerMost, Time &timeval);
    void EnterSpecificRule(RuleList *l, const std::string &stem, const std::string &preferredPath, bool outerMost);
    void EnterDefaultRule(const std::string &goal, RuleList *dflt);
    std::string GetFileTime(const std::string &goal, const std::string &preferredPath, Time &timeval);
    bool ScanList(const std::string &v, const std::string &goal);
    bool OnList(const std::string &goal, char *what);
    bool NoList(char *what);
    void GetEnvironment(EnvironmentStrings &env);
    void DeleteOne(Depends *depend);
    int RunOne(Depends *depend, EnvironmentStrings &env, bool keepGoing);
    void CancelOne(Depends *depend);
    void EnterSuffixTerminals();
private:
    typedef std::list<std::string> Goals;
    Goals goals;
    static std::string firstGoal;
    std::list<Depends *> depends;
    bool silent;
    bool displayOnly;
    bool ignoreResults;
    bool missingTarget;
    bool touch;
    bool rebuildAll;
    bool keepResponseFiles;
    bool lowResolutionTime;
    std::string newFiles;
    std::string oldFiles;
    std::set<std::string> ignoreFailedTargets;
    static std::map<std::string, std::string> filePaths;
    int dependsNesting;
} ;
#endif