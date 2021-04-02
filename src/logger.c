#include "logger.h"
#include <stdio.h>

#define LOG_FN_c(level)                                                                            \
    void log_##level(const char* message) {                                                        \
        char up_lev[] = #level;                                                                    \
        to_uppercase(up_lev);                                                                      \
        const size_t buf_len = 25;                                                                 \
        char datetime[buf_len];                                                                    \
        get_datetime(datetime, buf_len);                                                           \
        printf("%s - %s - %s:%d - %s\n", datetime, up_lev, __FILENAME__, __LINE__, message);       \
        return;                                                                                    \
    }
void to_uppercase(char up_lev[]) {
    for (size_t i = 0; up_lev[i]; i++) {
        if (islower(up_lev[i])) { up_lev[i] += 'A' - 'a'; }
    }
    return;
}

void get_datetime(char out_date[], size_t buf_len) {
    time_t now     = time(&now);
    struct tm* ptm = gmtime(&now);
    if ((ptm == NULL) || (now == -1)) {
        // Set to empty string.
        out_date[0] = 0;
        return;
    }
    strftime(out_date, buf_len, "%Y %b %d %X", ptm);
}
