/**
 * Made By Me: 
 *  - Marco Menchetti
 *  - marcomenchetti96@gmail.com
 */

#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <thread>
#include <future>

using namespace std::literals;

namespace thread_safe {
    
    class thread_queue_exception : public std::runtime_error{
    public:
        thread_queue_exception() : std::runtime_error("Empty queue") {};
        thread_queue_exception(const char *s) : std::runtime_error(s) {};
    };

    template<typename _T>
    class thread_queue { // as Rule of Five compliant
    private:
        std::mutex m_mutex;
        std::condition_variable m_queue_cv;
        std::queue<_T> m_queue;
        std::atomic<int> count;
    public:
        thread_queue() = default;
        /**
         * Rule Of Five+ Implemented:
         */
        ~thread_queue() = default;
        thread_queue( const thread_queue& ) = delete;
        thread_queue& operator=( const thread_queue& ) = delete;
        thread_queue( thread_queue&& ) noexcept = default;
        thread_queue& operator=( thread_queue&& ) noexcept = default;
        void swap( thread_queue&& );

        /**
         * Utility:
         */
        bool empty();
        /**
         * Thread-safe queue wrapper:
         */
        _T pop();
        void push( _T&& );
    };

    class thread_pool { // As a singleton
    private:
        thread_queue< std::function<void()> > m_work_queue;
        std::vector< std::thread > m_threads;
        std::atomic<bool> m_stop_flag{false};
        void worker();

        static std::mutex singleton_mutex;
        static thread_pool *m_instance;
    public:
        thread_pool();
        ~thread_pool();
        thread_pool( const thread_pool& ) = delete;
        thread_pool& operator=( const thread_pool& ) = delete;
        thread_pool( thread_pool&& ) = delete;
        thread_pool& operator=( thread_pool&& ) = delete;
        void stop();
        static thread_pool *get_pool();
        // managing stuff

        template<   typename _F,
                    typename ...Args,
                    typename _R = std::invoke_result_t< std::decay_t<_F>, std::decay_t<Args>... >,
                    typename = std::enable_if_t< std::is_void_v<_R> >
                >
        void submit(const _F &task, const Args &...args) {
            try{
                m_work_queue.push(
                    [task, args...]() {
                        task(args...);
                    } 
                );
            } catch( const std::exception& e ) {
                //todo
            }
        }

        template<   typename _F,
                    typename ...Args,
                    //Get the result type of the invocation of _F(Args..), stripped of const and stuff
                    typename _R = std::invoke_result_t< std::decay_t<_F>, std::decay_t<Args>... >,
                    //Check if _R is of type void
                    typename = std::enable_if_t< !std::is_void_v<_R> >
                >
        std::future<_R> submit(const _F &task, const Args &...args) {

            //shared_ptr so that exiting this we can still keep the promise alive
            std::shared_ptr< std::promise<_R> > promise_from_task{ new std::promise<_R> };

            try {
                m_work_queue.push(
                    [task, args..., promise_from_task]() {
                        promise_from_task->set_value( task(args...) );
                    }
                );
            } catch(const std::exception& e) {
                //todo
            }
            return promise_from_task->get_future();
        }
    };
}

// =================================== //
//             THREAD QUEUE            //
// =================================== //

template<typename _T>
_T thread_safe::thread_queue<_T>::pop() {
    std::unique_lock lk{m_mutex};
    m_queue_cv.wait_for( lk, 50ms, [this]() {
            return !m_queue.empty();
        }
    );
    if( m_queue.empty() )
        throw thread_queue_exception("After 50ms it is still empty...");
    _T out_val = std::move(m_queue.front());
    m_queue.pop();
    return out_val;
}

template<typename _T>
void thread_safe::thread_queue<_T>::push( _T&& val ) {
    std::lock_guard lk{m_mutex};
    m_queue.push( std::move(val) );
    m_queue_cv.notify_one();
}

template<typename _T>
void thread_safe::thread_queue<_T>::swap( thread_queue&& val) {
    //thread safe?
    _T tmp = std::move(val);
    val = std::move(*this);
    *this = std::move(tmp);
}

template<typename _T>
bool thread_safe::thread_queue<_T>::empty() {
    std::lock_guard lk{m_mutex};
    return m_queue.empty();
}

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
    std::lock_guard lk{singleton_mutex};
    if( m_instance == nullptr ) {
        m_instance = new thread_pool();
    }
    return m_instance;
}