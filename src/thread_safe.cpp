#include "thread_safe.h"


// =================================== //
//             THREAD POOL             //
// =================================== //
thread_safe::thread_pool::thread_pool() {
    const unsigned n_threads = std::thread::hardware_concurrency();
    for( unsigned i{0}; i < n_threads; ++i )
        m_threads.push_back(
            std::thread{ &thread_safe::thread_pool::worker, this }
        );
}

thread_safe::thread_pool::~thread_pool() {
    for(auto& th : m_threads)
        th.join();
}

void thread_safe::thread_pool::stop() {
    while( !m_work_queue.empty() ) {}
    m_stop_flag = true;
}

void thread_safe::thread_pool::worker() {
    while( !m_stop_flag ) {
        try {
            auto task = std::move( m_work_queue.pop() );
            task();
        } catch( const std::exception& e) {
            /*Do nothing*/
        }
    }
}

/*Static pool stuff*/
thread_safe::thread_pool *thread_safe::thread_pool::m_instance{nullptr};
std::mutex thread_safe::thread_pool::singleton_mutex;
thread_safe::thread_pool *thread_safe::thread_pool::get_pool() {
    std::lock_guard<std::mutex> lk{singleton_mutex};
    if( m_instance == nullptr ) {
        m_instance = new thread_pool();
    }
    return m_instance;
}
