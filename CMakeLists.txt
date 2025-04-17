cmake_minimum_required(VERSION 3.25)
project(myVectorDB)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
find_package(faiss REQUIRED)

add_executable(myVectorDB
        main.cpp
        FaissIndex.cpp
        IndexFactory.cpp
        HttpServer.cpp
        logger.cpp
)

target_include_directories(myVectorDB PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/cpp-httplib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hnswlib
)

target_link_libraries(myVectorDB PRIVATE faiss)
