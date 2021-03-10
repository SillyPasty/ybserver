#include "../log/logger.h"
#include <unistd.h>

int main() {
    Logger *logger = Logger::get_instance();
    logger->init("mylog");
    LOG_DEBUG("wyb_debug%d", 5);
    getchar();
    return 0;
}