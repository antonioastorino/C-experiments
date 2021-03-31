#include <stdlib.h>
#include "string_utils.h"


void string_split(const char *in_string, char separator, char *out_string) {
  size_t i = 0;
  char c;
  do {
    c = in_string[i];
    if (c == separator) {
      out_string[i] = '\0';
    } else {
      out_string[i] = c;
    }
    i++;
  } while (c != '\0');
}
