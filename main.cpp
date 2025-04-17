#include "HttpServer.h"
#include "IndexFactory.h"
#include "logger.h"

int main() {
    init_global_logger();
    set_log_level(spdlog::level::debug);
    GlobalLogger->info("Global logger initialized");

    int dim = 1; // Dimension of the vectors
    int num_data = 1000; // Number of data points
    IndexFactory *globalIndexFactory = getGlobalIndexFactory();
    globalIndexFactory->init(IndexFactory::IndexType::FLAT, dim);
    globalIndexFactory->init(IndexFactory::IndexType::HNSW, dim, num_data);
    GlobalLogger->info("Global index factory initialized");

    HttpServer server("0.0.0.0", 8080);
    GlobalLogger->info("HTTP server created");
    server.start();

    return 0;
}
