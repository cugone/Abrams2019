#pragma once

#include "Engine/Core/BuildConfig.hpp"

#include <cstdlib>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>
#include <type_traits>

namespace FileUtils {

enum class KnownPathID {
    None
    , Windows_AppDataRoaming
    , Windows_AppDataLocal
    , Windows_AppDataLocalLow
    , Windows_ProgramFiles
    , Windows_ProgramFilesx86
    , Windows_ProgramFilesx64
    , Windows_Documents
    , Windows_CommonDocuments
    , Windows_SavedGames
    , Windows_UserProfile
    , Windows_CommonProfile
    , Windows_CurrentUserDesktop
    , Windows_CommonDesktop
    , Linux_RootUser
    , Linux_Home
    , Linux_Etc
    , Linux_ConfigurationFiles = Linux_Etc
    , Linux_Bin
    , Linux_UserBinaries = Linux_Bin
    , Linux_SBin
    , Linux_SystemBinaries = Linux_SBin
    , Linux_Dev
    , Linux_DeviceFiles = Linux_Dev
    , Linux_Proc
    , Linux_ProcessInformation = Linux_Proc
    , Linux_Var
    , Linux_VariableFiles = Linux_Var
    , Linux_Usr
    , Linux_UserPrograms = Linux_Usr
    , Linux_UsrBin
    , Linux_UserProgramsBinaries = Linux_UsrBin
    , Linux_UsrSBin
    , Linux_UserProgramsSystemBinaries = Linux_UsrSBin
    , Linux_Boot
    , Linux_BootLoader = Linux_Boot
    , Linux_Lib
    , Linux_SystemLibraries = Linux_Lib
    , Linux_Opt
    , Linux_OptionalAddOnApps = Linux_Opt
    , Linux_Mnt
    , Linux_MountDirectory = Linux_Mnt
    , Linux_Media
    , Linux_RemovableDevices = Linux_Media
    , Linux_Src
    , Linux_ServiceData = Linux_Src
    , GameData
    , EngineData
    , Max
};

bool WriteBufferToFile(void* buffer, std::size_t size, const std::string& filePath);
bool WriteBufferToFile(const std::string& buffer, const std::string& filePath);
bool ReadBufferFromFile(std::vector<unsigned char>& out_buffer, const std::string& filePath);
bool ReadBufferFromFile(std::string& out_buffer, const std::string& filePath);
bool CreateFolders(const std::filesystem::path& filepath);
bool IsSystemPathId(const KnownPathID& pathid);
bool IsContentPathId(const KnownPathID& pathid);
std::filesystem::path GetKnownFolderPath(const KnownPathID& pathid);
std::filesystem::path GetExePath();
std::filesystem::path GetWorkingDirectory();
void SetWorkingDirectory(const std::filesystem::path& p);
bool IsSafeWritePath(const std::filesystem::path& p);
bool IsSafeReadPath(const std::filesystem::path& p);
bool HasWritePermissions(const std::filesystem::path& p);
bool HasReadPermissions(const std::filesystem::path& p);
bool HasDeletePermissions(const std::filesystem::path& p);
bool HasExecuteOrSearchPermissions(const std::filesystem::path& p);
bool HasExecutePermissions(const std::filesystem::path& p);
bool HasSearchPermissions(const std::filesystem::path& p);
bool IsParentOf(const std::filesystem::path& p, const std::filesystem::path& child);
bool IsSiblingOf(const std::filesystem::path& p, const std::filesystem::path& sibling);
bool IsChildOf(const std::filesystem::path& p, const std::filesystem::path& parent);
void ForEachFileInFolder(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, const std::function<void(const std::filesystem::path&)>& callback = [](const std::filesystem::path& /*p*/) { /* DO NOTHING */ }, bool recursive = false);
int CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false);
void RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, int mostRecentCountToKeep, const std::string& validExtensionList = std::string{});
std::vector<std::filesystem::path> GetAllPathsInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false);

//Unconditional byte order swap.
uint16_t EndianSwap(uint16_t value);
uint32_t EndianSwap(uint32_t value);
uint64_t EndianSwap(uint64_t value);

namespace detail {

    template<typename DirectoryIteratorType>
    void ForEachFileInFolders(const std::filesystem::path& preferred_folderpath, const std::vector<std::string>& validExtensions, const std::function<void(const std::filesystem::path&)>& callback) {
        if(validExtensions.empty()) {
            std::for_each(DirectoryIteratorType{ preferred_folderpath }, DirectoryIteratorType{},
                [&callback](const std::filesystem::directory_entry& entry) {
                    const auto& cur_path = entry.path();
                    bool is_directory = std::filesystem::is_directory(cur_path);
                    if(!is_directory) {
                        callback(cur_path);
                    }
                });
            return;
        }
        std::for_each(DirectoryIteratorType{ preferred_folderpath }, DirectoryIteratorType{},
            [&validExtensions, &callback](const std::filesystem::directory_entry& entry) {
                const auto& cur_path = entry.path();
                bool is_directory = std::filesystem::is_directory(cur_path);
                std::string my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
                bool valid_file_by_extension = std::find(std::begin(validExtensions), std::end(validExtensions), my_extension) != std::end(validExtensions);
                if(!is_directory) {
                    if(valid_file_by_extension) {
                        callback(cur_path);
                    }
                }
            });
    }
} //End detail

} //End FileUtils