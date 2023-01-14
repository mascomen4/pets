//
// Created by pi on 1/10/23.
//

#include "../include/concurrency_utils.h"
#include "../include/Server.h"

static const bool SEND_WITH_INTERRUPT = false;
static const int SLEEP_TIME_MS = 333;

template<typename T>
void threadsafe_queue<T>::push(T new_value){
    std::lock_guard<std::mutex> lk(mut);
    data_queue.push(std::move(new_value));
    data_cond.notify_one();
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop(){
    std::lock_guard<std::mutex> lk(mut);
    if(data_queue.empty())
        return std::shared_ptr<T>();
    std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
    data_queue.pop();
    return res;
}

template<typename T>
bool threadsafe_queue<T>::empty() const{
    std::lock_guard<std::mutex> lk(mut);
    return data_queue.empty();
}


void thread_pool::worker_thread(){
    while(!done){
        // I decided to not change the design of try_pop(), it's better to adapt the client code
        std::shared_ptr<std::shared_ptr<Handler>> res = work_queue.try_pop();
        std::shared_ptr<Handler> handler;
        if( res != nullptr ){
            handler = std::move(*res);
            HandleStatus handle_res = handler->handle();
            if (handle_res == HandleStatus::disconnected || handle_res == HandleStatus::fatal_error) continue; // we don't push it back, so the object will be destroyed.
            if (SEND_WITH_INTERRUPT){
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));
            }
            submit(std::move(handler));
        }
        else{
            std::this_thread::yield();
        }
    }
}

/**
 * The constructor of a thread_pool object. If it fails to instantiate the thread objects, the program exits with throw.
 */
thread_pool::thread_pool(): done(false), joiner(threads){
    unsigned const thread_count=std::thread::hardware_concurrency();
    try{
        for(unsigned i=0;i<thread_count;++i){
            threads.push_back(
                    std::thread(&thread_pool::worker_thread, this));
        }
    }
    catch(std::exception&){
        done=true;
        std::cerr << "Cannot instantiate " << std::thread::hardware_concurrency() << " objects defined by your environment. Exiting..." << std::endl;
        throw;
    }
}

void thread_pool::submit(std::shared_ptr<Handler> ch){
    work_queue.push(std::move(ch));
}
