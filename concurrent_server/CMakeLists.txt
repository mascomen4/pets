cmake_minimum_required(VERSION 3.20)
project(baum)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_library(utils src/sockets_io.cpp include/sockets_io.h)
add_library(net src/net.cpp include/net.h)
add_library(concurrency_utils src/concurrency_utils.cpp include/concurrency_utils.h)
add_library(Server src/Server.cpp include/Server.h)

add_executable(baum main.cpp)
target_link_libraries(concurrency_utils Threads::Threads)
target_link_libraries(Server concurrency_utils utils)
target_link_libraries(baum net Server)