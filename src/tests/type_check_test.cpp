/**
 * Made By Me: 
 *  - Marco Menchetti
 *  - marcomenchetti96@gmail.com
 */

#include <iostream>
#include <string>

// Wrapper for functions with return
template<   typename _F,
            typename ...Args,
            typename _R = std::invoke_result_t<std::decay_t<_F>,std::decay_t<Args>...>,
            typename = std::enable_if_t<!std::is_void_v<_R>>>
_R doIt(const _F &task, const Args&... args) {
    return task(args...);
}

// wrapper for functions without return
template<   typename _F,
            typename ...Args,
            typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<_F>, std::decay_t<Args>...>>>>
void doIt(const _F &task, const Args&... args) {
    task(args...);
}

// Dummy function 1
void first(int a, float b, std::string out) {
    std::cout << "Should not return anything" << std::endl;
}

// Dummy function 2
int second( std::string str, int n) {
    std::cout << "Here is good, " << str << " " << n << std::endl;
    return n;
}

int main( int argc, char *argv[] ) {
    doIt(first,1,1.2,"aaa");
    //auto a = doIt(first,1,1.2,"aaa"); // gives error: no return from doIt

    auto secondOut = doIt(second, "AAA", 3);
    //doIt(second, "AAA", 3); // works but we are ignoring return value
    std::cout << "Call to: doIt(second, \"AAA\", 3) = " << secondOut << std::endl;
    return 0;
}
