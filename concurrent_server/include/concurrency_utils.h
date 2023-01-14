//
// Created by pi on 1/10/23.
//

#ifndef BAUM_CONCURRENCY_UTILS_H
#define BAUM_CONCURRENCY_UTILS_H

#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <memory>
#include <iostream>
#include <queue>
#include <condition_variable>

/**
 * ---- Description ----
 * Classes and functions to handle the new incoming users concurrently
 */

// Needed to escape infinite #include of "Server.h" and "concurrency_utils.h"
class Handler;

class join_threads
{
    std::vector<std::thread>& threads;
public:
    explicit join_threads(std::vector<std::thread>& threads_):
            threads(threads_)
    {}
    ~join_threads()
    {
        for(unsigned long i=0;i<threads.size();++i)
        {
            if(threads[i].joinable())
                threads[i].join();
        }
    }
};

/**
 * We define a threadsafe_queue to safely push and pop from the server.
 * @tparam T
 */
template<typename T>
class threadsafe_queue
{
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue()
    = default;
    void push(T new_value);
    std::shared_ptr<T> try_pop();
    bool empty() const;
};

/**
 * The class to make the threads work in concurrently, which supports to have number of users much more that available
 * number of threads
 */
class thread_pool{
    std::atomic_bool done;
    threadsafe_queue<std::shared_ptr<Handler>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;
    void worker_thread();
public:
    thread_pool();
    ~thread_pool()
    {
        // call destructor explicitly
        joiner.~join_threads();
        done=true;
    }

    void submit(std::shared_ptr<Handler>);
};

#endif //BAUM_CONCURRENCY_UTILS_H
