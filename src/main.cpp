#include <utility>

#include "HttpServer.h"
#include "IndexFactory.h"
#include "logger.h"
#include "VectorDatabase.h"

int main() {
    init_global_logger();
    set_global_log_level(spdlog::level::debug);

    int dim = 1; // Dimension of the vectors
    int num_data = 1000; // Number of data points
    IndexFactory index_factory;
    index_factory.make_index(IndexFactory::IndexType::FLAT, dim);
    index_factory.make_index(IndexFactory::IndexType::HNSW, dim, num_data);
    index_factory.make_index(IndexFactory::IndexType::FILTER);

    VectorDatabase vector_database(std::move(index_factory));
    vector_database.reload_database();
    get_global_logger()->info("Vector database initialized");

    HttpServer server("0.0.0.0", 8080, vector_database);
    get_global_logger()->info("HTTP server created");
    server.start();

    return 0;
}
