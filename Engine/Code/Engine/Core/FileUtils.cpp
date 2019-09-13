#include "Engine/Core/FileUtils.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ShlObj.h>


namespace FileUtils {

GUID GetKnownPathIdForOS(const KnownPathID& pathid) noexcept;

bool WriteBufferToFile(void* buffer, std::size_t size, std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    filepath = FS::absolute(filepath);
    filepath.make_preferred();
    bool not_valid_path = FS::is_directory(filepath);
    bool invalid = not_valid_path;
    if(invalid) {
        return false;
    }

    std::ofstream ofs{filepath, std::ios_base::binary};
    ofs.write(reinterpret_cast<const char*>(buffer), size);
    ofs.close();
    return true;
}

bool WriteBufferToFile(const std::string& buffer, std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    filepath = FS::absolute(filepath);
    filepath.make_preferred();
    bool not_valid_path = FS::is_directory(filepath);
    bool invalid = not_valid_path;
    if(invalid) {
        return false;
    }

    std::ofstream ofs{ filepath };
    ofs.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    ofs.close();
    return true;

}

bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, std::filesystem::path filepath) noexcept {
    namespace FS = std::filesystem;
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    bool path_is_directory = FS::is_directory(filepath);
    bool path_not_exist = !FS::exists(filepath);
    bool not_valid_path = path_is_directory || path_not_exist;
    if(not_valid_path) {
        return false;
    }

    std::size_t byte_size = FS::file_size(filepath);
    out_buffer.resize(byte_size);
    std::ifstream ifs{ filepath, std::ios_base::binary };
    ifs.read(reinterpret_cast<char*>(out_buffer.data()), out_buffer.size());
    ifs.close();
    out_buffer.shrink_to_fit();
    return true;
}

bool ReadBufferFromFile(std::string& out_buffer, std::filesystem::path filepath) noexcept {

    namespace FS = std::filesystem;
    filepath = FS::canonical(filepath);
    filepath.make_preferred();
    bool path_is_directory = FS::is_directory(filepath);
    bool path_not_exist = !FS::exists(filepath);
    bool not_valid_path = path_is_directory || path_not_exist;
    if(not_valid_path) {
        return false;
    }

    std::ifstream ifs{filepath};
    out_buffer = std::string(static_cast<const std::stringstream&>(std::stringstream() << ifs.rdbuf()).str());
    return true;
}

bool CreateFolders(const std::filesystem::path& filepath) noexcept {
    namespace FS = std::filesystem;
    auto p = filepath;
    p.make_preferred();
    return FS::create_directories(p);
}

bool IsContentPathId(const KnownPathID& pathid) noexcept {
    if(!IsSystemPathId(pathid)) {
        switch(pathid) {
        case KnownPathID::GameData:                               return true;
        case KnownPathID::EngineData:                             return true;
        case KnownPathID::None:                                   return false;
        case KnownPathID::Max:                                    return false;
        default:
            ERROR_AND_DIE("UNSUPPORTED KNOWNPATHID")
        }
    }
    return false;
}

bool IsSystemPathId(const KnownPathID& pathid) noexcept {
    switch(pathid) {
    case KnownPathID::None:                                   return false;
    case KnownPathID::GameData:                               return false;
    case KnownPathID::EngineData:                             return false;
    case KnownPathID::Max:                                    return false;
#if defined(PLATFORM_WINDOWS)
    case KnownPathID::Windows_AppDataRoaming:                  return true;
    case KnownPathID::Windows_AppDataLocal:                    return true;
    case KnownPathID::Windows_AppDataLocalLow:                 return true;
    case KnownPathID::Windows_ProgramFiles:                    return true;
    case KnownPathID::Windows_ProgramFilesx86:                 return true;
    case KnownPathID::Windows_ProgramFilesx64:                 return true;
    case KnownPathID::Windows_Documents:                       return true;
    case KnownPathID::Windows_CommonDocuments:                 return true;
    case KnownPathID::Windows_SavedGames:                      return true;
    case KnownPathID::Windows_UserProfile:                     return true;
    case KnownPathID::Windows_CommonProfile:                   return true;
    case KnownPathID::Windows_CurrentUserDesktop:              return true;
#elif PLATFORM_LINUX
    case KnownPathID::Linux_RootUser:                          return true;
    case KnownPathID::Linux_Home:                              return true;
    case KnownPathID::Linux_Etc:                               return true;
    case KnownPathID::Linux_ConfigurationFiles:                return true;
    case KnownPathID::Linux_Bin:                               return true;
    case KnownPathID::Linux_UserBinaries:                      return true;
    case KnownPathID::Linux_SBin:                              return true;
    case KnownPathID::Linux_SystemBinaries:                    return true;
    case KnownPathID::Linux_Dev:                               return true;
    case KnownPathID::Linux_DeviceFiles:                       return true;
    case KnownPathID::Linux_Proc:                              return true;
    case KnownPathID::Linux_ProcessInformation:                return true;
    case KnownPathID::Linux_Var:                               return true;
    case KnownPathID::Linux_VariableFiles:                     return true;
    case KnownPathID::Linux_Usr:                               return true;
    case KnownPathID::Linux_UserPrograms:                      return true;
    case KnownPathID::Linux_UsrBin:                            return true;
    case KnownPathID::Linux_UserProgramsBinaries:              return true;
    case KnownPathID::Linux_UsrSBin:                           return true;
    case KnownPathID::Linux_UserProgramsSystemBinaries:        return true;
    case KnownPathID::Linux_Boot:                              return true;
    case KnownPathID::Linux_BootLoader:                        return true;
    case KnownPathID::Linux_Lib:                               return true;
    case KnownPathID::Linux_SystemLibraries:                   return true;
    case KnownPathID::Linux_Opt:                               return true;
    case KnownPathID::Linux_OptionalAddOnApps:                 return true;
    case KnownPathID::Linux_Mnt:                               return true;
    case KnownPathID::Linux_MountDirectory:                    return true;
    case KnownPathID::Linux_Media:                             return true;
    case KnownPathID::Linux_RemovableDevices:                  return true;
    case KnownPathID::Linux_Src:                               return true;
    case KnownPathID::Linux_ServiceData:                       return true;
#endif
    default:
        ERROR_AND_DIE("UNSUPPORTED KNOWNPATHID")
    }
}

std::filesystem::path GetKnownFolderPath(const KnownPathID& pathid) noexcept {
    namespace FS = std::filesystem;
    FS::path p{};
    if(!(IsSystemPathId(pathid) || IsContentPathId(pathid))) {
        return p;
    }
    if(pathid == KnownPathID::GameData) {
        p = GetWorkingDirectory() / FS::path{"Data/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else if(pathid == KnownPathID::EngineData) {
        p = GetWorkingDirectory() / FS::path{"Engine/"};
        if(FS::exists(p)) {
            p = FS::canonical(p);
        }
    } else {
        {
            PWSTR ppszPath = nullptr;
            auto hr_path = ::SHGetKnownFolderPath(GetKnownPathIdForOS(pathid), KF_FLAG_DEFAULT, nullptr, &ppszPath);
            bool success = SUCCEEDED(hr_path);
            if(success) {
                p = FS::path(ppszPath);
                ::CoTaskMemFree(ppszPath);
                p = FS::canonical(p);
            }
        }
    }
    p.make_preferred();
    return p;
}

GUID GetKnownPathIdForOS(const KnownPathID& pathid) noexcept {
    switch(pathid) {
    case KnownPathID::Windows_AppDataRoaming:
        return FOLDERID_RoamingAppData;
    case KnownPathID::Windows_AppDataLocal:
        return FOLDERID_LocalAppData;
    case KnownPathID::Windows_AppDataLocalLow:
        return FOLDERID_LocalAppDataLow;
    case KnownPathID::Windows_ProgramFiles:
        return FOLDERID_ProgramFiles;
    case KnownPathID::Windows_ProgramFilesx86:
        return FOLDERID_ProgramFilesX86;
#if defined(_WIN64)
    case KnownPathID::Windows_ProgramFilesx64:
        return FOLDERID_ProgramFiles;
#elif defined(_WIN32)
    case KnownPathID::Windows_ProgramFilesx64:
        return FOLDERID_ProgramFiles;
#else
    ERROR_AND_DIE("Unknown known folder path id.");
    break;
#endif
    case KnownPathID::Windows_SavedGames:
        return FOLDERID_SavedGames;
    case KnownPathID::Windows_UserProfile:
        return FOLDERID_Profile;
    case KnownPathID::Windows_CommonProfile:
        return FOLDERID_Public;
    case KnownPathID::Windows_CurrentUserDesktop:
        return FOLDERID_Desktop;
    case KnownPathID::Windows_CommonDesktop:
        return FOLDERID_PublicDesktop;
    case KnownPathID::Windows_Documents:
        return FOLDERID_Documents;
        break;
    case KnownPathID::Windows_CommonDocuments:
        return FOLDERID_PublicDocuments;
        break;
    default:
        ERROR_AND_DIE("Unknown known folder path id.");
        break;
    }
}

std::filesystem::path GetExePath() noexcept {
    namespace FS = std::filesystem;
    FS::path result{};
    {
        TCHAR filename[MAX_PATH];
        ::GetModuleFileName(nullptr, filename, MAX_PATH);
        result = FS::path(filename);
        result = FS::canonical(result);
        result.make_preferred();
    }
    return result;
}

std::filesystem::path GetWorkingDirectory() noexcept {
    namespace FS = std::filesystem;
    return FS::current_path();
}

void SetWorkingDirectory(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    FS::current_path(p);
}

std::filesystem::path GetTempDirectory() noexcept {
    return std::filesystem::temp_directory_path();
}

bool HasDeletePermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    auto parent_path = p.parent_path();
    auto parent_status = FS::status(parent_path);
    auto parent_perms = parent_status.permissions();
    if(FS::perms::none == (parent_perms & (FS::perms::owner_write | FS::perms::group_write | FS::perms::others_write))) {
        return false;
    }
    return true;
}

bool HasExecuteOrSearchPermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(FS::is_directory(p)) {
        return HasSearchPermissions(p);
    } else {
        return HasExecutePermissions(p);
    }
}

bool HasExecutePermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(FS::is_directory(p)) {
        return false;
    }
    auto status = FS::status(p);
    auto perms = status.permissions();
    if(FS::perms::none == (perms & (FS::perms::owner_exec | FS::perms::group_exec | FS::perms::others_exec))) {
        return false;
    }
    return true;
}

bool HasSearchPermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(!FS::is_directory(p)) {
        return false;
    }
    auto parent_path = p.parent_path();
    auto parent_status = FS::status(parent_path);
    auto parent_perms = parent_status.permissions();
    if(FS::perms::none == (parent_perms & (FS::perms::owner_exec | FS::perms::group_exec | FS::perms::others_exec))) {
        return false;
    }
    return true;
}

bool HasWritePermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    auto my_status = FS::status(p);
    auto my_perms = my_status.permissions();
    if(FS::perms::none == (my_perms & (FS::perms::owner_write | FS::perms::group_write | FS::perms::others_write))) {
        return false;
    }
    return true;
}

bool HasReadPermissions(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    auto my_status = FS::status(p);
    auto my_perms = my_status.permissions();
    if(FS::perms::none == (my_perms & (FS::perms::owner_read | FS::perms::group_read | FS::perms::others_read))) {
        return false;
    }
    return true;
}

bool IsSafeWritePath(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    //Check for any write permissions on the file and parent directory
    if(!(HasWritePermissions(p) || HasDeletePermissions(p))) {
        return false;
    }

    try {
        auto working_dir = GetWorkingDirectory();
        bool is_in_working_dir = IsChildOf(p, working_dir);
        bool is_in_data_dir = IsChildOf(p, FS::path{ "Data/" });
        bool is_next_to_exe = IsSiblingOf(p, GetExePath());
        bool is_temp_dir = IsChildOf(p, GetTempDirectory());
        bool safe = is_in_working_dir || is_in_data_dir || is_next_to_exe || is_temp_dir;
        return safe;
    } catch(const std::filesystem::filesystem_error& e) {
        std::ostringstream ss{};
        ss << "\nFilesystem Error:"
            << "\nWhat: " << e.what()
            << "\nCode: " << e.code()
            << "\nPath1: " << e.path1()
            << "\nPath2: " << e.path2()
            << '\n';
        ss.flush();
        DebuggerPrintf(ss.str().c_str());
        return false;
    }

}

bool IsSafeReadPath(const std::filesystem::path& p) noexcept {
    namespace FS = std::filesystem;
    if(!FS::exists(p)) {
        return false;
    }
    //Check for any write permissions on the file and parent directory
    if(!(HasReadPermissions(p) || HasExecuteOrSearchPermissions(p))) {
        return false;
    }

    try {
        auto working_dir = GetWorkingDirectory();
        bool is_in_working_dir = IsChildOf(p, working_dir);
        bool is_in_gamedata_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::GameData));
        bool is_in_enginedata_dir = IsChildOf(p, GetKnownFolderPath(KnownPathID::EngineData));
        bool is_known_OS_dir = false;

        bool is_next_to_exe = IsSiblingOf(p, GetExePath());
        bool safe = is_in_working_dir || is_in_gamedata_dir || is_in_enginedata_dir || is_next_to_exe || is_known_OS_dir;
        return safe;
    } catch(const std::filesystem::filesystem_error& e) {
        std::ostringstream ss{};
        ss << "\nFilesystem Error:"
            << "\nWhat: " << e.what()
            << "\nCode: " << e.code()
            << "\nPath1: " << e.path1()
            << "\nPath2: " << e.path2()
            << '\n';
        ss.flush();
        DebuggerPrintf(ss.str().c_str());
        return false;
    }

}

bool IsParentOf(const std::filesystem::path& p, const std::filesystem::path& child) noexcept {
    namespace FS = std::filesystem;
    auto p_canon = FS::canonical(p);
    auto child_canon = FS::canonical(child);
    for(auto iter = FS::recursive_directory_iterator{ p_canon }; iter != FS::recursive_directory_iterator{}; ++iter) {
        auto entry = *iter;
        auto sub_p = entry.path();
        if(sub_p == child_canon) {
            return true;
        }
    }
    return false;
}

bool IsSiblingOf(const std::filesystem::path& p, const std::filesystem::path& sibling) noexcept {
    namespace FS = std::filesystem;
    auto my_parent_path = FS::canonical(p.parent_path());
    auto sibling_parent_path = FS::canonical(sibling.parent_path());
    return my_parent_path == sibling_parent_path;
}

bool IsChildOf(const std::filesystem::path& p, const std::filesystem::path& parent) noexcept {
    namespace FS = std::filesystem;
    auto parent_canon = FS::canonical(parent);
    auto p_canon = FS::canonical(p);
    for(auto iter = FS::recursive_directory_iterator{ parent_canon }; iter != FS::recursive_directory_iterator{}; ++iter) {
        auto entry = *iter;
        auto sub_p = entry.path();
        if(sub_p == p_canon) {
            return true;
        }
    }
    return false;
}

void ForEachFileInFolder(const std::filesystem::path& folderpath, const std::string& validExtensionList /*= std::string{}*/, const std::function<void(const std::filesystem::path&)>& callback /*= [](const std::filesystem::path& p) { (void*)p; }*/, bool recursive /*= false*/) noexcept {
    namespace FS = std::filesystem;
    auto preferred_folderpath = FS::canonical(folderpath);
    preferred_folderpath.make_preferred();
    bool exists = FS::exists(preferred_folderpath);
    bool is_directory = FS::is_directory(preferred_folderpath);
    bool is_folder = exists && is_directory;
    if(!is_folder) {
        return;
    }
    auto validExtensions = StringUtils::Split(StringUtils::ToLowerCase(validExtensionList));
    if(!recursive) {
        detail::ForEachFileInFolders<FS::directory_iterator>(preferred_folderpath, validExtensions, callback);
    } else {
        detail::ForEachFileInFolders<FS::recursive_directory_iterator>(preferred_folderpath, validExtensions, callback);
    }
}

int CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList /*= std::string{}*/, bool recursive /*= false*/) noexcept {
    namespace FS = std::filesystem;
    int count = 0;
    auto cb = [&count](const FS::path& /*p*/)->void { ++count; };
    ForEachFileInFolder(folderpath, validExtensionList, cb, recursive);
    return count;
}

std::vector<std::filesystem::path> GetAllPathsInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList /*= std::string{}*/, bool recursive /*= false*/) noexcept {
    namespace FS = std::filesystem;
    std::vector<FS::path> paths{};
    auto add_path_cb = [&paths](const FS::path& p) { paths.push_back(p); };
    ForEachFileInFolder(folderpath, validExtensionList, add_path_cb, recursive);
    return paths;
}

void FileUtils::RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, int mostRecentCountToKeep, const std::string& validExtensionList /*= std::string{}*/) noexcept {
    auto working_dir = std::filesystem::current_path();
    if(!IsSafeWritePath(folderpath)) {
        return;
    }
    namespace FS = std::filesystem;
    if(mostRecentCountToKeep < CountFilesInFolders(folderpath, validExtensionList)) {
        std::vector<FS::path> paths = GetAllPathsInFolders(folderpath);
        auto sort_pred = [](const FS::path& a, const FS::path& b) { return FS::last_write_time(a) > FS::last_write_time(b); };
        std::sort(std::begin(paths), std::end(paths), sort_pred);
        if(mostRecentCountToKeep > 0) {
            auto erase_end = std::begin(paths) + mostRecentCountToKeep;
            paths.erase(std::begin(paths), erase_end);
        }
        for(auto& p : paths) {
            FS::remove(p);
        }
    }
}


uint16_t EndianSwap(uint16_t value) noexcept {
    return _byteswap_ushort(value);
}

uint32_t EndianSwap(uint32_t value) noexcept {
    return _byteswap_ulong(value);
}

uint64_t EndianSwap(uint64_t value) noexcept {
    return _byteswap_uint64(value);
}

} //End FileUtils