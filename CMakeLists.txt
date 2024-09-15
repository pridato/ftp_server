cmake_minimum_required(VERSION 3.29)
project(ftp_server C)

set(CMAKE_C_STANDARD 11)

add_executable(ftp_server main.c
        server.c
        commands.c
        utils.c
        server.h
        commands.h
        utils.h)
