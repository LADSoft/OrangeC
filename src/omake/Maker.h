/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 * 
 */

#ifndef MAKER_H
#define MAKER_H

#include <string>
#include <unordered_map>
#include <list>
#include <set>
#include "Spawner.h"
#include "Depends.h"

class Variable;
class Runner;

class Maker
{
  public:
    Maker(bool Silent, bool DisplayOnly, bool IgnoreResults, bool Touch, OutputType Type, bool rebuildAll = false,
          bool keepResponseFiles = false, std::string newFiles = "", std::string oldFiles = "");
    ~Maker();
    static void SetFirstGoal(const std::string& name);
    static void ClearFirstGoal() { firstGoal = ""; }
    void AddGoal(const std::string& name) { goals.push_back(name); }
    void Clear();
    int GoalCount() { return goals.size() + (firstGoal.size() != 0); }
    bool HasCommands() { return depends.size() != 0; }
    bool CreateDependencyTree();
    int RunCommands(bool keepGoing = false);
    void SetIgnoreFailed(const std::string& name) { ignoreFailedTargets.insert(name); }
    static std::string GetFullName(std::string name);

  protected:
    static void CallRunner(Runner&& runner, Depends* depend, EnvironmentStrings* env, bool keepGoing, std::promise<int> promise);
    std::unique_ptr<Depends> Dependencies(const std::string& goal, const std::string& preferredPath, Time& timeval, bool err,
                                          bool top, std::string file, int line);
    bool ExistsOrMentioned(const std::string& stem, std::shared_ptr<RuleList>& ruleList, const std::string& preferredPath,
                           bool implicit, bool outerMost);
    bool SearchImplicitRules(const std::string& goal, const std::string& preferredPath, bool outerMost, Time& timeval);
    void EnterSpecificRule(std::shared_ptr<RuleList>& l, const std::string& stem, const std::string& preferredPath,
                           bool outerMost);
    void EnterDefaultRule(const std::string& goal, std::shared_ptr<RuleList>& dflt);
    std::string GetFileTime(const std::string& goal, const std::string& preferredPath, Time& timeval);
    bool OnList(const std::string& goal, char* what);
    bool NoList(char* what);
    void GetEnvironment(EnvironmentStrings& env);
    void EnterSuffixTerminals();

  private:
    typedef std::list<std::string> Goals;
    Goals goals;
    static std::string firstGoal;
    std::list<std::unique_ptr<Depends>> depends;
    OutputType outputType;
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
    static std::unordered_map<std::string, std::string> filePaths;
    int dependsNesting;
};
#endif