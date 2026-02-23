#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    condition_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push(std::move(task));
    }
    condition_.notify_one();
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            // Wait until: stop signal OR task available
            condition_.wait(lock, [this] {
                return stop_ || !tasks_.empty();
            });
            
            // If stopping and no tasks, exit
            if (stop_ && tasks_.empty()) {
                return;
            }
            
            // Get task from queue
            task = std::move(tasks_.front());
            tasks_.pop();
        }
        
        // execute task outside the lock
    }
}
