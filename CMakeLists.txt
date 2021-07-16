cmake_minimum_required(VERSION 3.5)
project(highPerformanceLinuxServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(highPerformanceLinuxServer main.cpp)

add_executable(5_1 chapter05/5_1.cpp)
add_executable(5_2 chapter05/5_2.cpp)
add_executable(5_3 chapter05/5_3.cpp)
add_executable(5_5 chapter05/5_5.cpp)
add_executable(5_6 chapter05/5_6.cpp)
add_executable(5_7 chapter05/5_7.cpp)

add_executable(6_1 chapter06/6_1.cpp)
add_executable(6_2 chapter06/6_2.cpp)
add_executable(6_3 chapter06/6_3.cpp)
add_executable(6_4 chapter06/6_4.cpp)
add_executable(6_5 chapter06/6_5.cpp)

add_executable(syslog chapter07/syslog.cpp)
add_executable(7_1 chapter07/7_1.cpp)
add_executable(7_2 chapter07/7_2.cpp)
add_executable(chdir chapter07/chdir.cpp)
add_executable(chroot chapter07/chroot.cpp)
add_executable(7_3 chapter07/7_3.cpp)

add_executable(8_3 chapter08/8_3.cpp)

add_executable(9_1 chapter09/9_1.cpp)
add_executable(9_4 chapter09/9_4.cpp)
target_link_libraries(9_4 pthread)
add_executable(9_5 chapter09/9_5.cpp)
add_executable(9_8 chapter09/9_8.cpp)

add_executable(10_1 chapter10/10_1.cpp)
target_link_libraries(10_1 pthread)
add_executable(10_3 chapter10/10_3.cpp)

add_executable(11_3 chapter11/11_3.cpp chapter11/timeLst.h)
target_link_libraries(11_3 pthread)
add_executable(11_4 chapter11/11_4.cpp)

add_executable(processpool chapter15_processpool/processpool.cpp chapter15_processpool/processpool.h)
add_executable(cgi chapter15_processpool/execgi.cpp)
add_executable(client chapter15_processpool/client.cpp)

add_executable(threadpool chapter15_threadpool/server.cpp chapter15_threadpool/http_conn.cpp)
target_link_libraries(threadpool pthread)
