cmake_minimum_required(VERSION 3.27)
project(lab_2_os C)

set(CMAKE_C_STANDARD 11)

add_library(lab-2_os
        kernel.c
        user.c)
