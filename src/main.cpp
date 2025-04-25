#include <string>

#include "HttpServer.h"
#include "IndexFactory.h"
#include "logger.h"
#include "VectorDatabase.h"

int main() {
    init_global_logger();
    set_log_level(spdlog::level::debug);
    global_logger->info("Global logger initialized");

    int dim = 1; // Dimension of the vectors
    int num_data = 1000; // Number of data points
    get_global_index_factory().init(IndexFactory::IndexType::FLAT, dim);
    get_global_index_factory().init(IndexFactory::IndexType::HNSW, dim, num_data);
    get_global_index_factory().init(IndexFactory::IndexType::FILTER);
    global_logger->info("Global index factory initialized");

    VectorDatabase vector_database("scalar_storage.db");
    global_logger->info("Vector database initialized");

    HttpServer server("0.0.0.0", 8080, vector_database);
    global_logger->info("HTTP server created");
    server.start();

    return 0;
}
