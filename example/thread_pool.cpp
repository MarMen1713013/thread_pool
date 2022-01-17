/**
 * Made By Me: 
 *  - Marco Menchetti
 *  - marcomenchetti96@gmail.com
 */

#include <iostream>
#include "../thread_safe.h"

std::mutex m;

/**
 * dummy:
 **
 * Dummy class to show how to pass member functions.
 */
class dummy{
public:
    void dummy_say( std::string str ) {
        std::lock_guard lk{m};
        std::cout << "\nDummy! ahahah " << str << std::endl;
    }
    static int dummy_sum( int a, int b ) {
        return a+b;
    }
    static std::string class_name() {
        return "dummy";
    }
};

// Just an alias
using tp = thread_safe::thread_pool;

// 'void' returning task
void task1() {
    std::lock_guard lk{m};
    std::cout << "\nNew thread id: " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(100ms);
    std::cout << "Tread #" << std::this_thread::get_id() << " finished!" << std::endl;
}

// 'int' returning task
int task2(bool ignore_me) {
    std::lock_guard lk{m};
    std::cout << "\nNew thread id: " << std::this_thread::get_id() << " with 'int' return type" << std::endl;
    std::this_thread::sleep_for(100ms);
    std::cout << "Tread #" << std::this_thread::get_id() << " finished!" << std::endl;
    return 10;
}

int main( int argc, char *argv[] ) {
    // Initialize variables
    dummy dumdum;

    std::cout << "Creating a pool with " << std::thread::hardware_concurrency() << " threads." << std::endl;
    tp *thread_pool = tp::get_pool();

    // submit 20 void tasks
    for( int i{0}; i < 20; ++i )
        thread_pool->submit(task1);

    // subimt 20 'int' returning tasks
    std::vector<std::future<int>> return_values;
    for( int i{0}; i < 20; ++i )
        return_values.push_back(
            thread_pool->submit( task2, (i%2==0) ? true : false )
        );

    // submit calls to a thread-safe object's member function
    thread_pool->submit([&dumdum](std::string str){ dumdum.dummy_say(str);},"lol"); // non static
    auto dummysum = thread_pool->submit(&dummy::dummy_sum,1,2); // staic with arguments
    auto dummyname = thread_pool->submit(&dummy::class_name); // static with no argument

    std::this_thread::sleep_for(5s);
    thread_pool->stop();

    {
        std::lock_guard lk{m};
        std::cout << "\nDummy sum is: " << dummysum.get() << " from class " << dummyname.get() << std::endl;
    }

    return 0;
}