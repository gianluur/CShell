cmake_minimum_required(VERSION 3.15)
project(CShell)

file(GLOB SOURCES src/*.c src/history/*.c)
add_executable(${PROJECT_NAME} ${SOURCES})