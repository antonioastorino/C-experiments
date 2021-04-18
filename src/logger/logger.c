#include "logger.h"

#if LOG_LEVEL > 0
void get_datetime(char* datetime, const size_t buf_len)
{

    time_t now     = time(&now);
    struct tm* ptm = gmtime(&now);
    if ((ptm == NULL) || (now == -1))
    {
        datetime[0] = 0;
    }
    strftime(datetime, buf_len, "%Y %b %d %X", ptm);
}
#endif
