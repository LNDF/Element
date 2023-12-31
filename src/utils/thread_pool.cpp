#include "thread_pool.h"

#include <core/log.h>
#include <thread>
#include <vector>
#include <queue>

static std::vector<std::thread> pool_threads;
static std::queue<element::thread_pool::task_type> tasks;
static std::mutex sync_mutex;
static std::condition_variable cv;
static bool stop_worker = true;

static void run_task(element::thread_pool::task_type& task) {
    try {
        task();
    } catch(...) {}
}

static void worker() {
    std::unique_lock<std::mutex> lk(sync_mutex);
    while (!stop_worker) {
        if (tasks.empty()) {
            cv.wait(lk);
            continue;
        } else {
            element::thread_pool::task_type task = std::move(tasks.front());
            tasks.pop();
            lk.unlock();
            run_task(task);
            lk.lock();
        }
    }
}

using namespace element;

void thread_pool::init(std::uint32_t num_threads) {
    {
        std::lock_guard<std::mutex> lk(sync_mutex);
        if (!stop_worker) return;
        stop_worker = false;
    }
    if (!pool_threads.empty()) {
        ELM_ERROR("pool_threads is not empty!");
        throw std::runtime_error("pool_threads is not empty!");
    }
    ELM_INFO("Starting thread pool with {0} threads...", num_threads);
    pool_threads.reserve(num_threads);
    for (std::uint32_t i = 0; i < num_threads; ++i) {
        pool_threads.emplace_back(worker);
    }
}

void thread_pool::stop() {
    {
        std::lock_guard<std::mutex> lk(sync_mutex);
        if (stop_worker) return;
        stop_worker = true;
    }
    cv.notify_all();
    ELM_INFO("Waiting for all threads to stop...");
    for (std::thread& th : pool_threads) {
        th.join();
    }
    pool_threads.clear();
}

void thread_pool::run_task(task_type& task) {
    {
        std::lock_guard<std::mutex> lk(sync_mutex);
        if (stop_worker) {
            ELM_WARN("Tried to run task but thread poll has not started yet.");
            run_task(task);
            return;
        }
        tasks.push(task);
    }
    cv.notify_one();
}

void thread_pool::run_task(task_type&& task) {
    {
        std::lock_guard<std::mutex> lk(sync_mutex);
        if (stop_worker) {
            ELM_WARN("Tried to run task but thread poll has not started yet.");
            run_task(task);
            return;
        }
        tasks.push(std::move(task));
    }
    cv.notify_one();
}