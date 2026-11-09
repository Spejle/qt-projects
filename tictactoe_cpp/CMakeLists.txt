cmake_minimum_required(VERSION 3.16)
project(tictactoe_table LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(tictactoe_table main.cpp tablemodel.h)
target_link_libraries(tictactoe_table PRIVATE Qt6::Widgets)
