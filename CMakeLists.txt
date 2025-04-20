cmake_minimum_required(VERSION 3.25)
project(myVectorDB)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

find_package(OpenMP REQUIRED)
find_package(faiss REQUIRED)
find_package(spdlog REQUIRED)

find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h REQUIRED)

find_library(ROCKSDB_LIBRARY rocksdb REQUIRED)

FetchContent_Declare(
        roaring
        GIT_REPOSITORY https://github.com/RoaringBitmap/CRoaring.git
        GIT_TAG v4.3.1
        GIT_SHALLOW TRUE
)
set(ENABLE_ROARING_TESTS OFF CACHE INTERNAL "")
set(ROARING_BUILD_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(roaring)

add_executable(myVectorDB
        main.cpp
        FaissIndex.cpp
        IndexFactory.cpp
        HttpServer.cpp
        logger.cpp
        HNSWLibIndex.cpp
        ScalarStorage.cpp
        VectorDatabase.cpp
)

target_include_directories(myVectorDB PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/cpp-httplib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hnswlib
        ${ROCKSDB_INCLUDE_DIR}
)

target_link_libraries(myVectorDB PRIVATE
        faiss
        spdlog::spdlog
        ${ROCKSDB_LIBRARY}
        z
        bz2
        lz4
        snappy
        zstd
        roaring
)
