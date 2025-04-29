cmake_minimum_required(VERSION 3.25)
project(my-vector-db)

set(CMAKE_CXX_STANDARD 17)

add_executable(my_vector_db
        src/main.cpp
        src/FaissIndex.cpp
        src/IndexFactory.cpp
        src/HttpServer.cpp
        src/logger.cpp
        src/HNSWLibIndex.cpp
        src/ScalarStorage.cpp
        src/VectorDatabase.cpp
        src/FilterIndex.cpp
        src/utils.cpp
        src/Persistence.cpp
)

target_include_directories(my_vector_db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(OpenMP REQUIRED)
find_package(faiss REQUIRED)
find_package(spdlog REQUIRED)

find_path(ROCKSDB_INCLUDE_DIR rocksdb/db.h REQUIRED)

find_library(ROCKSDB_LIBRARY rocksdb REQUIRED)

target_include_directories(my_vector_db PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/cpp-httplib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hnswlib
        ${ROCKSDB_INCLUDE_DIR}
)

target_link_libraries(my_vector_db PRIVATE
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
