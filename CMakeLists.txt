cmake_minimum_required(VERSION 3.29)
project(ftp_server C)

set(CMAKE_C_STANDARD 11)

find_package(OpenSSL REQUIRED)

include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(ftp_server main.c
        server.c
        commands.c
        utils.c
        server.h
        commands.h
        utils.h)

target_link_libraries(ftp_server ${OPENSSL_LIBRARIES})
