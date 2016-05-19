#ifndef _V3_STRING_H
#define _V3_STRING_H 1

// #include <v3_core.h>

typedef struct {
    const char*     data;
    size_t          length;
} v3_str_t;

#define v3_string(str) { str, sizeof(str) - 1 }

#define v3_str_equal(str, value) \
 ((str)->length  == sizeof(value) - 1 \
        && strncasecmp((str)->data, value, sizeof(value) - 1) == 0)

#endif // _V3_STRING_H 
