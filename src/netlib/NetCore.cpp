/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#ifdef TARGET_OS_WINDOWS
#    include <windows.h>
#    include <fstream>
#    include <map>
#    include <string>
#    include <locale>
#    include <codecvt>
#    include "DotNetPELib.h"
#    include "PEFile.h"
#    include "MZHeader.h"
#    include "PEHeader.h"

#    include "libhostfxr\pal.h"
#    include "libhostfxr\hostfxr.h"
#    include "libhostfxr\fxr_resolver.h"

#    define EMBED_HASH_HI_PART_UTF8 "c3ab8ff13720e8ad9047dd39466b3c89"  // SHA-256 of "foobar" in UTF-8
#    define EMBED_HASH_LO_PART_UTF8 "74e592c2fa383d4a3960714caef0c4f2"
#    define EMBED_HASH_FULL_UTF8 (EMBED_HASH_HI_PART_UTF8 EMBED_HASH_LO_PART_UTF8)  // NUL terminated

#    define NETCORE_APP "Microsoft.NETCore.App"
#    define LNETCORE_APP L"Microsoft.NETCore.App"

namespace DotNetPELib
{
extern std::string DIR_SEP;

static std::map<std::string, std::pair<std::string, std::string>> frameworks;
static std::map<std::string, std::string> sdks;

static const char hash[] = EMBED_HASH_FULL_UTF8;
static bool initted;

pal::string_t fxr_path;

char NetCore::RunTimeConfig[] =
    R"abcd({
  "runtimeOptions": {
    "tfm": "net%d.0",
    "framework" : {
      "name": "Microsoft.NETCore.App",
      "version" : "%d.0.0"
    }
  }
})abcd";

static std::string char_t_to_string(const char_t* str)
{
    std::wstring string_to_convert(str);
    std::string rv(string_to_convert.begin(), string_to_convert.end());
    return rv;
}
static std::wstring char_to_wstring(const char* str)
{
    std::string string_to_convert(str);
    std::wstring rv(string_to_convert.begin(), string_to_convert.end());
    return rv;
}

static void hostfxr_get_dotnet_environment_info_result_callback(const struct hostfxr_dotnet_environment_info* info,
                                                                void* result_context)
{
    for (int i = 0; i < info->framework_count; i++)
    {
        auto&& t = info->frameworks[i];
        std::string name = char_t_to_string(t.name);
        std::string version = char_t_to_string(t.version);
        std::string path = char_t_to_string(t.path);
        frameworks[name] = std::pair<std::string, std::string>(version, path);
    }
}
bool inithostfxr()
{
    if (!initted)
    {
        pal::dll_t fxr;

        pal::string_t host_path;
        if (!pal::get_own_module_path(&host_path) || !pal::realpath(&host_path))
        {
            // can't retrieve path
            return false;
        }

        pal::string_t dotnet_root;
        if (fxr_resolver::try_get_existing_fxr(&fxr, &fxr_path))
        {
            // reusing loaded library
            dotnet_root = get_dotnet_root_from_fxr_path(fxr_path);
        }
        else
        {
            // Do not specify the root path. Getting a delegate does not support self-contained (app-local fxr)
            if (!fxr_resolver::try_get_path(pal::string_t{}, &dotnet_root, &fxr_path))
            {
                return false;
            }

            // Load library
            if (!pal::load_library(&fxr_path, &fxr))
            {
                // loading failed
                return false;
            }
        }
        auto info_fn =
            reinterpret_cast<hostfxr_get_dotnet_environment_info_fn>(pal::get_symbol(fxr, "hostfxr_get_dotnet_environment_info"));
        if (!info_fn)
        {
            // invalid dll, bad entry points????
            pal::unload_library(fxr);
            return false;
        }
        info_fn(nullptr, nullptr, hostfxr_get_dotnet_environment_info_result_callback, nullptr);
        pal::unload_library(fxr);
    }
    initted = true;
    return true;
}

PELib* NetCore::init(std::string assemblyName)
{
    // load the runtime, and check if we support the selected runtime
    if (!SupportsRuntime())
    {
        return nullptr;
    }
    if (version_ == DummyChooseLatest)
    {
        std::string svers = frameworks[NETCORE_APP].first;
        int index = svers.find(".");
        if (index != std::string::npos)
            svers = svers.substr(0, index);
        version_ = atoi(svers.c_str());
    }

    peLib = new PELib(assemblyName, corFlags_, this);
    return peLib;
}

bool NetCore::SupportsRuntime(int svers)
{
    if (!inithostfxr())
        return false;
    if (svers == DummyChooseLatest)
        svers = version_;
    const auto& values = frameworks[NETCORE_APP];
    return initted && !values.second.empty() && (svers == DummyChooseLatest || (LookupDir(values.second, svers) != ""));
}

bool NetCore::WriteRuntimeConfig(std::string fileName)
{
    char buf[1024];
    sprintf(buf, RunTimeConfig, version_, version_);
    int index = fileName.find_last_of(".");
    if (index != std::string::npos)
    {
        fileName = fileName.substr(0, index);
    }
    fileName += ".runtimeconfig.json";
    std::fstream out(fileName, std::ios::out);
    bool rv = out.good();
    if (rv)
    {
        out.write(buf, strlen(buf));
        rv = out.good();
    }
    return rv;
}
bool NetCore::CreateExecutable(std::string fileName, std::string dllName)
{

    // this looks up the apphost.exe executable
    // unfortunately there doesn't seem to be a way to do it with hostfxr.dll...

    std::wstring runtimePath;
    // this next line assumes a win64 install..
    if (!pal::get_dotnet_self_registered_dir_for_arch(pal::architecture::x64, &runtimePath))
        return false;
    auto runtimeType = corFlags_ & PELib::bits32 ? L"win-x86" : L"win-x64";
    append_path(&runtimePath, _X("packs"));
    append_path(&runtimePath, (std::wstring(LNETCORE_APP) + _X(".Host.") + runtimeType).c_str());
    auto dir = LookupDir(char_t_to_string(runtimePath.c_str()), version_);
    runtimePath = char_to_wstring(dir.c_str());
    append_path(&runtimePath, _X("runtimes"));
    append_path(&runtimePath, runtimeType);
    append_path(&runtimePath, _X("native"));
    append_path(&runtimePath, _X("apphost.exe"));

    // now open it and read the data
    auto apphostFile = char_t_to_string(runtimePath.c_str());
    std::fstream in(apphostFile, std::ios::in | std::ios::binary);
    if (in.fail())
        return false;
    in.seekg(0, std::ios_base::end);
    std::streampos size = in.tellg();
    Byte* data = new Byte[size];
    in.seekg(0, std::ios_base::beg);
    in.read((char*)data, size);
    if (!in.good())
    {
        delete[] data;
        in.close();
        return false;
    }
    in.close();
    // got the data, replace the unique hash code that is in the application
    // with our DLL name.   I think we have 1K or something for that.
    // the dll name is in utf8 so we can just plop it in.   Null terminated of course...
    Byte* location = data;
    while ((location = (Byte*)memchr(location + 1, hash[0], data + size - location)) != 0)
    {
        if (memcmp(location, hash, sizeof(hash) - 1) == 0)
            break;
    }
    if (!location)
    {
        delete[] data;
        return false;
    }
    strcpy((char*)location, dllName.c_str());

    // now mark the proper subsystem for the app
    // that way it won't run a console if this is a gui program....

    if (*(short*)data == MZ_SIGNATURE)
    {
        auto pe = (PEHeader*)(*(DWord*)(data + 0x3c) + data);
        if (pe->signature != PESIG)
        {
            delete[] data;
            return false;
        }
        if (gui_)
        {
            pe->subsystem = PE_SUBSYS_WINDOWS;
        }
        else
        {
            pe->subsystem = PE_SUBSYS_CONSOLE;
        }
    }

    // all done, save our new executable to the target fileName.
    std::fstream out(fileName, std::ios::out | std::ios::binary);
    bool rv = out.good();
    if (rv)
    {
        out.write((char*)data, size);
        rv = out.good();
        WriteRuntimeConfig(std::move(fileName));
    }
    delete[] data;
    return rv;
}
std::string NetCore::LookupDir(std::string path, int version)
{
    std::vector<pal::string_t> dirs;
    std::wstring wpath = char_to_wstring(path.c_str());
    pal::readdir_onlydirectories(wpath, &dirs);
    wchar_t ver[100];
    swprintf(ver, sizeof(ver) / sizeof(wchar_t), L"%d.", version);
    std::wstring match;
    std::wstring last;
    for (const auto& t : dirs)
    {
        if (!wcsncmp(t.c_str(), ver, wcslen(ver)))
        {
            match = std::move(t);
            break;
        }
        last = t;
    }
    if (match.size())
    {
        return path + DIR_SEP + char_t_to_string(match.c_str());
    }
    else
    {
        return path + DIR_SEP + char_t_to_string(last.c_str());
    }
    return "";
}

bool NetCore::LoadAssembly(AssemblyDef* assembly, std::string assemblyName, int major, int minor, int build, int revision,
                           bool created)
{
    if (peLib)
    {
        int n = 1;
        PEReader r;
        if (created)
        {
            const auto& values = frameworks[NETCORE_APP];
            if (values.first != "" && values.second != "")
            {
                std::string path;
                path = LookupDir(values.second, version_);
                if (path != "")
                {
                    n = r.ManagedLoad(assemblyName, path + DIR_SEP + assemblyName + ".dll");
                }
                if (n)
                {
                    n = r.ManagedLoad(std::move(assemblyName), major, minor, build, revision);
                }
            }
        }
        else
        {
            n = r.ManagedLoad(std::move(assemblyName), major, minor, build, revision);
        }
        if (!n)
        {
            assembly->Load(*peLib, r);
            assembly->SetLoaded();
        }
        return n;
    }
    return false;
}
bool NetCore::LoadAssembly(std::string assemblyName, int major, int minor, int build, int revision)
{
    if (peLib)
    {
        AssemblyDef* assembly = peLib->FindAssembly(assemblyName);
        bool created = assembly == nullptr;
        if (created)
        {
            peLib->AddExternalAssembly(assemblyName);
            assembly = peLib->FindAssembly(assemblyName);
            if (assemblyName == peLib->GetRuntimeName())
            {
                // we probably should look at the dll and load its forwarding references...   but this is enough for now...
                return LoadAssembly(assembly, "System.Private.CoreLib", major, minor, build, revision, created);
            }
        }
        else
        {
            if (assembly->IsLoaded())
                return 1;
        }
        auto rv = LoadAssembly(assembly, std::move(assemblyName), major, minor, build, revision, created);
        if (rv)
        {
            peLib->RemoveAssembly(assembly);
        }
        return rv;
    }
    return 1;
}
bool NetCore::DumpOutputFile(const std::string& file, PELib::OutputMode mode)
{
    if (peLib)
    {
        std::string outputFile = file;
        if (mode == PELib::OutputMode::peexe)
        {

            int index = outputFile.find_last_of(".");
            if (index != std::string::npos)
            {
                outputFile = outputFile.substr(0, index);
            }
            // the exe file is still an exe file but it is renamed to a dll...
            outputFile += ".dll";
        }
        bool rv = peLib->DumpOutputFile(outputFile, mode, gui_);

        if (rv && mode == PELib::OutputMode::peexe)
        {
            int index = outputFile.find_last_of("/");
            if (index != std::string::npos)
            {
                outputFile = outputFile.substr(index + 1);
            }
            CreateExecutable(file, std::move(outputFile));
        }
        return rv;
    }
    return false;
}
}  // namespace DotNetPELib
#endif