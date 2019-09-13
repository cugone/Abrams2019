#pragma once

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Core/ThreadSafeQueue.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

class Job;
class JobSystem;

enum class JobType : std::size_t {
    Generic,
    Logging,
    Io,
    Render,
    Main,
    Max,
};

enum class JobState : unsigned int {
    None,
    Created,
    Dispatched,
    Enqueued,
    Running,
    Finished,
    Max,
};

class Job {
public:
    explicit Job(JobSystem& jobSystem) noexcept;
    ~Job() noexcept;
    JobType type{};
    JobState state{};
    std::function<void(void*)> work_cb;
    void* user_data{};

    void DependencyOf(Job* dependency) noexcept;
    void DependentOn(Job* parent) noexcept;
    void OnDependancyFinished() noexcept;
    void OnFinish() noexcept;

    std::vector<Job*> dependents{};
    std::atomic<unsigned int> num_dependencies{ 0u };
private:
    void AddDependent(Job* dependent) noexcept;
    JobSystem* _job_system = nullptr;
};

class JobConsumer {
public:
    void AddCategory(const JobType& category) noexcept;
    bool ConsumeJob() noexcept;
    unsigned int ConsumeAll() noexcept;
    void ConsumeFor(TimeUtils::FPMilliseconds consume_duration) noexcept;
    bool HasJobs() const noexcept;
private:
    std::vector<ThreadSafeQueue<Job*>*> _consumables{};
    friend class JobSystem;
};

class JobSystem {
public:
    JobSystem(int genericCount, std::size_t categoryCount, std::condition_variable* mainJobSignal) noexcept;
    ~JobSystem() noexcept;

    void BeginFrame() noexcept;
    void Shutdown() noexcept;

    void SetCategorySignal(const JobType& category_id, std::condition_variable* signal) noexcept;
    Job* Create(const JobType& category, const std::function<void(void*)>& cb, void* user_data) noexcept;
    void Run(const JobType& category, const std::function<void(void*)>& cb, void* user_data) noexcept;
    void Dispatch(Job* job) noexcept;
    bool Release(Job* job) noexcept;
    void Wait(Job* job) noexcept;
    void DispatchAndRelease(Job* job) noexcept;
    void WaitAndRelease(Job* job) noexcept;
    bool IsRunning() const noexcept;
    void SetIsRunning(bool value = true) noexcept;

    std::condition_variable* GetMainJobSignal() const noexcept;
protected:
private:
    void Initialize(int genericCount, std::size_t categoryCount) noexcept;
    void MainStep() noexcept;
    void GenericJobWorker(std::condition_variable* signal) noexcept;

    static std::vector<ThreadSafeQueue<Job*>*> _queues;
    static std::vector<std::condition_variable*> _signals;
    static std::vector<std::thread> _threads;
    std::condition_variable* _main_job_signal = nullptr;
    std::mutex _cs{};
    std::atomic_bool _is_running = false;
    friend class JobConsumer;
};