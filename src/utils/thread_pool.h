#pragma once

#include <functional>
#include <cinttypes>

namespace element {
    namespace thread_pool {

        using task_type = std::function<void()>;

        void init(std::uint32_t num_threads);
        void stop();
        void run_task(task_type& task);
        void run_task(task_type&& task);

    } //namespace thread_pool
} //namespace element