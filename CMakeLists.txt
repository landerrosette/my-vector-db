cmake_minimum_required(VERSION 3.20)
project(myVectorDB)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
find_package(faiss REQUIRED)

add_executable(myVectorDB main.cpp
        FaissIndex.cpp
        FaissIndex.h
)

target_include_directories(myVectorDB PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/cpp-httplib
)
