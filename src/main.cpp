#include <string>

#include "HttpServer.h"
#include "IndexFactory.h"
#include "logger.h"
#include "VectorDatabase.h"

int main() {
    init_global_logger();
    set_log_level(spdlog::level::debug);
    GlobalLogger->info("Global logger initialized");

    int dim = 1; // Dimension of the vectors
    int num_data = 1000; // Number of data points
    IndexFactory *globalIndexFactory = getGlobalIndexFactory();
    globalIndexFactory->init(IndexFactory::IndexType::FLAT, dim);
    globalIndexFactory->init(IndexFactory::IndexType::HNSW, dim, num_data);
    globalIndexFactory->init(IndexFactory::IndexType::FILTER);
    GlobalLogger->info("Global index factory initialized");

    std::string db_path = "scalar_storage.db";
    VectorDatabase vector_database(db_path);
    GlobalLogger->info("Vector database initialized");

    HttpServer server("0.0.0.0", 8080, &vector_database);
    GlobalLogger->info("HTTP server created");
    server.start();

    return 0;
}
