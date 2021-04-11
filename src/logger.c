#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

void get_datetime(char out_date[], size_t buf_len)
{
    time_t now     = time(&now);
    struct tm* ptm = gmtime(&now);
    if ((ptm == NULL) || (now == -1))
    {
        // Set to empty string.
        out_date[0] = 0;
        return;
    }
    strftime(out_date, buf_len, "%Y %b %d %X", ptm);
}
