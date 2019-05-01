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
    FileLogger(JobSystem& jobSystem, const std::string& logName);
    ~FileLogger();

    void Shutdown();
    void Log(const std::string& msg);
    void LogLine(const std::string& msg);
    void LogAndFlush(const std::string& msg);
    void LogLineAndFlush(const std::string& msg);
    void LogPrint(const std::string& msg);
    void LogWarn(const std::string& msg);
    void LogError(const std::string& msg);
    void LogTag(const std::string& tag, const std::string& msg);
    void LogPrintLine(const std::string& msg);
    void LogWarnLine(const std::string& msg);
    void LogErrorLine(const std::string& msg);
    void LogTagLine(const std::string& tag, const std::string& msg);
    void Flush();
    void SetIsRunning(bool value = true);

    void SaveLog();

protected:
private:
    void Initialize(JobSystem& jobSystem, const std::string& log_name);

    void InsertTimeStamp(std::stringstream& msg);
    void InsertTag(std::stringstream& msg, const std::string& tag);
    void InsertMessage(std::stringstream& msg, const std::string& messageLiteral);

    void Log_worker();
    void RequestFlush();
    bool IsRunning();

    void DoCopyLog();
    void CopyLog(void* user_data);
    void FinalizeLog();
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