#pragma once

#include "Engine/Core/ThreadSafeQueue.hpp"

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

class JobSystem;

class FileLogger {
public:
    FileLogger(JobSystem& jobSystem, const std::string& logName) noexcept;
    ~FileLogger() noexcept;

    void Shutdown() noexcept;
    void Log(const std::string& msg) noexcept;
    void LogLine(const std::string& msg) noexcept;
    void LogAndFlush(const std::string& msg) noexcept;
    void LogLineAndFlush(const std::string& msg) noexcept;
    void LogPrint(const std::string& msg) noexcept;
    void LogWarn(const std::string& msg) noexcept;
    void LogError(const std::string& msg) noexcept;
    void LogTag(const std::string& tag, const std::string& msg) noexcept;
    void LogPrintLine(const std::string& msg) noexcept;
    void LogWarnLine(const std::string& msg) noexcept;
    void LogErrorLine(const std::string& msg) noexcept;
    void LogTagLine(const std::string& tag, const std::string& msg) noexcept;
    void Flush() noexcept;
    void SetIsRunning(bool value = true) noexcept;

    void SaveLog() noexcept;

protected:
private:
    void Initialize(JobSystem& jobSystem, const std::string& log_name) noexcept;

    void InsertTimeStamp(std::stringstream& msg) noexcept;
    void InsertTag(std::stringstream& msg, const std::string& tag) noexcept;
    void InsertMessage(std::stringstream& msg, const std::string& messageLiteral) noexcept;

    void Log_worker() noexcept;
    void RequestFlush() noexcept;
    bool IsRunning() const noexcept;

    void DoCopyLog() noexcept;
    void CopyLog(void* user_data) noexcept;
    void FinalizeLog() noexcept;
    mutable std::mutex _cs{};
    std::ofstream _stream{};
    std::filesystem::path _current_log_path{};
    decltype(std::cout.rdbuf()) _old_cout{};
    std::thread _worker{};
    std::condition_variable _signal{};
    ThreadSafeQueue<std::string> _queue;
    JobSystem* _job_system = nullptr;
    std::atomic_bool _is_running = false;
    std::atomic_bool _requesting_flush = false;
};