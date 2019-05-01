#include "Engine/Core/FileLogger.hpp"

#include "Engine/Core/BuildConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Core/Win.hpp"

#include "Engine/Profiling/Memory.hpp"

#include <cstdio>
#include <cstdarg>
#include <chrono>
#include <filesystem>
#include <iostream>

namespace FS = std::filesystem;

FileLogger::FileLogger(JobSystem& jobSystem, const std::string& logName) {
    Initialize(jobSystem, logName);
}

FileLogger::~FileLogger() {
    Shutdown();
    _job_system = nullptr;
}

void FileLogger::Log_worker() {
    JobConsumer jc;
    jc.AddCategory(JobType::Logging);
    _job_system->SetCategorySignal(JobType::Logging, &_signal);

    while(IsRunning()) {
        std::unique_lock<std::mutex> lock(_cs);
        //Condition to wake up: not running or queue has jobs.
        _signal.wait(lock, [this]()->bool { return !_is_running || !_queue.empty(); });
        if(!_queue.empty()) {
            auto str = _queue.front();
            _queue.pop();
            _stream << str;
            RequestFlush();
            jc.ConsumeAll();
        }
    }
}

void FileLogger::RequestFlush() {
    if(_requesting_flush) {
        _stream.flush();
        _requesting_flush = false;
    }
}

bool FileLogger::IsRunning() {
    bool running = false;
    {
    std::scoped_lock<std::mutex> lock(_cs);
    running = _is_running;
    }
    return running;
}

struct copy_log_job_t {
    std::filesystem::path from{};
    std::filesystem::path to{};
};

void FileLogger::DoCopyLog() {
    if(IsRunning()) {
        auto job_data = new copy_log_job_t;
        auto from_p = _current_log_path;
        from_p = FS::canonical(from_p);
        from_p.make_preferred();
        auto to_p = from_p.parent_path();
        to_p = to_p / TimeUtils::GetDateTimeStampFromNow() / ".log";
        to_p = FS::canonical(to_p);
        to_p.make_preferred();
        job_data->to = to_p;
        job_data->from = from_p;
        _job_system->Run(JobType::Generic, [this](void* user_data) { this->CopyLog(user_data); }, job_data);
    }
}

void FileLogger::CopyLog(void* user_data) {
    if(IsRunning()) {
        auto job_data = reinterpret_cast<copy_log_job_t*>(user_data);
        auto from = job_data->from;
        auto to = job_data->to;
        std::scoped_lock<std::mutex> _lock(_cs);
        _stream.flush();
        _stream.close();
        std::cout.rdbuf(_old_cout);
        std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing);
        _stream.open(from, std::ios_base::app);
        _old_cout = std::cout.rdbuf(_stream.rdbuf());
    }
}

void FileLogger::FinalizeLog() {
    auto from_p = _current_log_path;
    from_p = FS::canonical(from_p);
    from_p.make_preferred();
    auto to_p = from_p;
    auto logname = to_p.filename().stem().string();
    TimeUtils::DateTimeStampOptions opts;
    opts.use_separator = true;
    opts.is_filename = true;
    to_p.replace_filename(logname + "_" + TimeUtils::GetDateTimeStampFromNow(opts) + ".log");
    //Canonicalizing output file that doesn't already exist is an error.
    to_p.make_preferred();
    _stream << "Copied log to: " << to_p << "...\n";
    _stream.flush();
    _stream.close();
    std::cout.rdbuf(_old_cout);
    std::filesystem::copy_file(from_p, to_p, std::filesystem::copy_options::overwrite_existing);
}

void FileLogger::Initialize(JobSystem& jobSystem, const std::string& log_name) {
    if(IsRunning()) {
        LogLine("FileLogger already running.");
        return;
    }
    _job_system = &jobSystem;
    namespace FS = std::filesystem;
    std::string folder_str = "Data/Logs/";
    std::string log_str = folder_str + log_name + ".log";
    FS::path folder_p{ folder_str };
    FS::path log_p{ log_str };
    if(FS::exists(log_p)) {
        log_p = FS::canonical(log_p);
        log_p.make_preferred();
    } else {
        log_p = FS::path{ folder_p / std::string{log_name + ".log"}};
        log_p.make_preferred();
    }
    folder_p.make_preferred();
    _current_log_path = log_p;
    FileUtils::CreateFolders(folder_p);
    //Removes only if it exists.
    FS::remove(log_p);
    FileUtils::RemoveExceptMostRecentFiles(folder_p, MAX_LOGS, ".log");
    _is_running = true;
    _stream.open(_current_log_path);
    if(_stream.fail()) {
        DebuggerPrintf("FileLogger failed to initialize.\n");
        _stream.clear();
        _is_running = false;
        return;
    }
    _old_cout = std::cout.rdbuf(_stream.rdbuf());
    _worker = std::thread(&FileLogger::Log_worker, this);
    ::SetThreadDescription(_worker.native_handle(), L"FileLogger");
    std::ostringstream ss;
    ss << "Initializing Logger: " << _current_log_path << "...";
    LogLine(ss.str().c_str());
}

void FileLogger::Shutdown() {
    if(IsRunning()) {
        {
            std::ostringstream ss;
            ss << Memory::status();
            ss << "Shutting down Logger: " << _current_log_path << "...";
            LogLine(ss.str().c_str());
        }
        SetIsRunning(false);
        _signal.notify_all();
        if(_worker.joinable()) {
            _worker.join();
        }
        FinalizeLog();
        _job_system->SetCategorySignal(JobType::Logging, nullptr);
    }
}

void FileLogger::Log(const std::string& msg) {
    {
        std::scoped_lock<std::mutex> _lock(_cs);
        _queue.push(msg);
    }
    _signal.notify_all();
}

void FileLogger::LogLine(const std::string& msg) {
    Log(msg + '\n');
}

void FileLogger::LogAndFlush(const std::string& msg) {
    Log(msg);
    Flush();
}

void FileLogger::LogLineAndFlush(const std::string& msg) {
    LogLine(msg);
    Flush();
}

void FileLogger::LogPrint(const std::string& msg) {
    LogTag("log", msg);
}

void FileLogger::LogWarn(const std::string& msg) {
    LogTag("warning", msg);
}

void FileLogger::LogError(const std::string& msg) {
    LogTag("error", msg);
}

void FileLogger::LogTag(const std::string& tag, const std::string& msg) {

    std::stringstream ss;
    InsertTimeStamp(ss);
    InsertTag(ss, tag);
    InsertMessage(ss, msg);

    Log(ss.str());
}

void FileLogger::LogPrintLine(const std::string& msg) {
    LogTagLine("log", msg);
}

void FileLogger::LogWarnLine(const std::string& msg) {
    LogTagLine("warning", msg);
}

void FileLogger::LogErrorLine(const std::string& msg) {
    LogTagLine("error", msg);
}

void FileLogger::LogTagLine(const std::string& tag, const std::string& msg) {
    LogTag(tag, msg + '\n');
}

void FileLogger::InsertTimeStamp(std::stringstream& msg) {
    TimeUtils::DateTimeStampOptions opts{};
    opts.use_separator = true;
    msg << "[" << TimeUtils::GetDateTimeStampFromNow(opts) << "]";
}

void FileLogger::InsertTag(std::stringstream& msg, const std::string& tag) {
    msg << "[" << tag << "]";
}

void FileLogger::InsertMessage(std::stringstream& msg, const std::string& messageLiteral) {
    msg << messageLiteral;
}

void FileLogger::Flush() {
    _requesting_flush = true;
    while(_requesting_flush) {
        std::this_thread::yield();
    }
}

void FileLogger::SetIsRunning(bool value /*= true*/) {
    std::scoped_lock<std::mutex> lock(_cs);
    _is_running = value;
}

void FileLogger::SaveLog() {
    DoCopyLog();
}
