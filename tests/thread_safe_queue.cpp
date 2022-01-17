/**
 * Made By Me: 
 *  - Marco Menchetti
 *  - marcomenchetti96@gmail.com
 */

#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <future>
#include "../thread_safe.h"

template<typename T>
using t_queue = thread_safe::thread_queue<T>;

t_queue<std::unique_ptr<std::string>> q;

void reader() {
    try { // to pop data from queue
        std::cout << "Reader calling pop..." << std::endl;
        auto data = std::move( q.pop() );
        std::cout << "Reader received data: " << *data << std::endl;
    } catch(std::exception& e) {
        std::cout << "Exception caught: " << e.what() << std::endl;
    }
}

void writer(std::string val) {
    std::cout << "Writer pushing data..." << std::endl;
    q.push( std::make_unique<std::string>( val ) );
    std::cout << "Writer returning after push!" << std::endl;
}

int main( int argc, char *argv[] ) {
    auto r1 = std::async( std::launch::async, reader );
    std::this_thread::sleep_for( 6s );
    auto wa = std::async( std::launch::async, writer, "aaA" );
    auto r2 = std::async( std::launch::async, reader );
    auto wb = std::async( std::launch::async, writer, "Bbb" );
    auto r3 = std::async( std::launch::async, reader );
    auto wc = std::async( std::launch::async, writer, "CcC" );
    auto r4 = std::async( std::launch::async, reader );
    auto wd = std::async( std::launch::async, writer, "dDd" );
    auto r5 = std::async( std::launch::async, reader );
    auto r6 = std::async( std::launch::async, reader );

    return 0;
}