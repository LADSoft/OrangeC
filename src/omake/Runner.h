/* Software License Agreement
 *
 *     Copyright(C) 1994-2026 David Lindauer, (LADSoft)
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

#ifndef RUNNER_H
#define RUNNER_H

#include <string>
#include <unordered_map>
#include <list>
#include <set>
#include <future>
#include <thread>
#include <chrono>
#include "Spawner.h"
#include "Maker.h"

class Depends;
class Variable;
struct future_holding_struct
{
    std::string command;
    std::future<int> return_value;
    future_holding_struct(const std::string& command, std::future<int>&& return_value) :
        command(command), return_value(std::future(std::move(return_value)))
    {
    }
    int get() { return return_value.get(); }
    bool valid() { return return_value.valid(); }
    template <class Rep, class Period>
    std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) const
    {
        return return_value.wait_for(timeout_duration);
    }
    future_holding_struct(future_holding_struct&& other) noexcept
    {
        this->command = std::move(other.command);
        this->return_value = std::move(other.return_value);
    }
    future_holding_struct& operator=(future_holding_struct&& holder) noexcept
    {
        this->command.swap(holder.command);
        this->return_value = std::move(holder.return_value);
        return *this;
    }
};
class Runner
{
  public:
    Runner(bool Silent, bool DisplayOnly, bool IgnoreResults, bool Touch, OutputType Type, bool KeepResponseFiles,
           std::string& FirstGoal, std::unordered_map<std::string, std::string>& FilePaths) :
        silent(Silent),
        displayOnly(DisplayOnly),
        ignoreResults(IgnoreResults),
        touch(Touch),
        keepResponseFiles(KeepResponseFiles),
        firstGoal(FirstGoal),
        filePaths(FilePaths),
        outputType(Type)
    {
    }
    void DeleteOne(Depends* depend);
    int RunOne(std::list<std::shared_ptr<RuleList>>* ruleStack, Depends* depend, EnvironmentStrings* env, bool keepGoing);
    void CancelOne(Depends* depend);

  protected:
    void WaitAll(int& rv, bool keepGoing, std::vector<future_holding_struct>& workingList);
    static void CallRunner(Runner* runner, std::list<std::shared_ptr<RuleList>>* ruleStack, Depends* depend,
                           EnvironmentStrings* env, bool keepGoing, std::promise<int> promise);

  private:
    OutputType outputType;
    bool silent;
    bool displayOnly;
    bool ignoreResults;
    bool touch;
    bool keepResponseFiles;
    std::string& firstGoal;
    std::unordered_map<std::string, std::string>& filePaths;
};
#endif
