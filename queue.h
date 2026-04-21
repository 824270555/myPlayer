#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <atomic>

template<typename T>

class Queue
{
public:
    Queue() {}
    ~Queue() {}

    void Abort() {
        std::lock_guard<std::mutex> lock(mutex_);
        abort_ = true;
        cond_.notify_all();
    }

    int Push(T var) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (abort_ == 1)
            return -1;
        queue_.push(var);
        cond_.notify_one();
        return 0;
    }

    int Pop(T &var, const int timeout = 0) {
        std::unique_lock<std::mutex> lock(mutex_);

        if (queue_.empty()) {
            cond_.wait_for(lock, std::chrono::milliseconds(timeout), [this]{
                return !queue_.empty() || abort_;
            });
        }

        if (abort_ == 1)
            return -1;
        if (queue_.empty())
            return -2;
        var = queue_.front();
        queue_.pop();
        return 0;
    }

    int Front(T &var) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (abort_ == 1)
            return -1;
        if (queue_.empty())
            return -2;
        var = queue_.front();
        return 0;
    }

    int Size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }


private:
    std::atomic_bool abort_{false};
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<T> queue_;
};

#endif // QUEUE_H
