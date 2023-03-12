#include "logger.h"

void hl_error(const char *msg)
{
    perror(msg);
    exit(errno);
}
