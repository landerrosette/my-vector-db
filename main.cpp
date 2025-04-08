#include "HttpServer.h"
#include "IndexFactory.h"
#include "logger.h"

int main() {
    init_global_logger();
    GlobalLogger->info("Global logger initialized");

    int dim = 1;
    IndexFactory *globalIndexFactory = getGlobalIndexFactory();
    globalIndexFactory->init(IndexFactory::IndexType::FLAT, dim);
    GlobalLogger->info("Global index factory initialized");

    HttpServer server("0.0.0.0", 8080);
    GlobalLogger->info("HTTP server created");
    server.start();

    return 0;
}
