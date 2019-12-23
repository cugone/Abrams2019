#pragma once

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/StringUtils.hpp"

#include <cstdlib>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace FileUtils {

enum class KnownPathID {
    None
#ifdef PLATFORM_WINDOWS
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
#elif PLATFORM_LINUX
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
#endif
    , GameData
    , EngineData
    , Max
};

bool WriteBufferToFile(void* buffer, std::size_t size, std::filesystem::path filepath) noexcept;
bool WriteBufferToFile(const std::string& buffer, std::filesystem::path filepath) noexcept;
std::optional<std::vector<uint8_t>> ReadBinaryBufferFromFile(std::filesystem::path filepath) noexcept;
std::optional<std::string> ReadStringBufferFromFile(std::filesystem::path filepath) noexcept;
bool CreateFolders(const std::filesystem::path& filepath) noexcept;
bool IsSystemPathId(const KnownPathID& pathid) noexcept;
bool IsContentPathId(const KnownPathID& pathid) noexcept;
std::filesystem::path GetKnownFolderPath(const KnownPathID& pathid) noexcept;
std::filesystem::path GetExePath() noexcept;
std::filesystem::path GetWorkingDirectory() noexcept;
void SetWorkingDirectory(const std::filesystem::path& p) noexcept;
bool IsSafeWritePath(const std::filesystem::path& p) noexcept;
bool IsSafeReadPath(const std::filesystem::path& p) noexcept;
bool HasWritePermissions(const std::filesystem::path& p) noexcept;
bool HasReadPermissions(const std::filesystem::path& p) noexcept;
bool HasDeletePermissions(const std::filesystem::path& p) noexcept;
bool HasExecuteOrSearchPermissions(const std::filesystem::path& p) noexcept;
bool HasExecutePermissions(const std::filesystem::path& p) noexcept;
bool HasSearchPermissions(const std::filesystem::path& p) noexcept;
bool IsParentOf(const std::filesystem::path& p, const std::filesystem::path& child) noexcept;
bool IsSiblingOf(const std::filesystem::path& p, const std::filesystem::path& sibling) noexcept;
bool IsChildOf(const std::filesystem::path& p, const std::filesystem::path& parent) noexcept;
void ForEachFileInFolder(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, const std::function<void(const std::filesystem::path&)>& callback = [](const std::filesystem::path& /*p*/) { /* DO NOTHING */ }, bool recursive = false) noexcept;
int CountFilesInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false) noexcept;
void RemoveExceptMostRecentFiles(const std::filesystem::path& folderpath, int mostRecentCountToKeep, const std::string& validExtensionList = std::string{}) noexcept;
std::vector<std::filesystem::path> GetAllPathsInFolders(const std::filesystem::path& folderpath, const std::string& validExtensionList = std::string{}, bool recursive = false) noexcept;

//Unconditional byte order swap.
uint16_t EndianSwap(uint16_t value) noexcept;
uint32_t EndianSwap(uint32_t value) noexcept;
uint64_t EndianSwap(uint64_t value) noexcept;

namespace detail {

    template<typename DirectoryIteratorType>
    void ForEachFileInFolders(const std::filesystem::path& preferred_folderpath, const std::vector<std::string>& validExtensions, const std::function<void(const std::filesystem::path&)>& callback) noexcept {
        if(validExtensions.empty()) {
            std::for_each(DirectoryIteratorType{ preferred_folderpath }, DirectoryIteratorType{},
                [&callback](const std::filesystem::directory_entry& entry) {
                    const auto& cur_path = entry.path();
                    bool is_file = std::filesystem::is_regular_file(cur_path);
                    if(is_file) {
                        callback(cur_path);
                    }
                });
            return;
        }
        std::for_each(DirectoryIteratorType{ preferred_folderpath }, DirectoryIteratorType{},
            [&validExtensions, &callback](const std::filesystem::directory_entry& entry) {
                const auto& cur_path = entry.path();
                bool is_file = std::filesystem::is_regular_file(cur_path);
                std::string my_extension = StringUtils::ToLowerCase(cur_path.extension().string());
                if(is_file) {
                    bool valid_file_by_extension = std::find(std::begin(validExtensions), std::end(validExtensions), my_extension) != std::end(validExtensions);
                    if(valid_file_by_extension) {
                        callback(cur_path);
                    }
                }
            });
    }
} //End detail

} //End FileUtils