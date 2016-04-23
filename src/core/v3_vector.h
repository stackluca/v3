#ifndef _V3_VECTOR_H
#define _V3_VECTOR_H 

#include <v3_core.h>

struct  v3_vector_s {
    void *items;
    size_t length;
    size_t capacity;
    size_t  size;
};

extern void *v3_vector_new(v3_options_t *options, size_t itemSize, unsigned int capacity);
extern void *v3_vector_push(v3_options_t *options, v3_vector_t *vector);

#endif
