#ifndef _V3_FOUNDATION_H
#define _V3_FOUNDATION_H 1

#include <v3_core.h>

extern void *v3_alloc(v3_options_t *options, size_t size);
extern void v3_dealloc(v3_options_t *options, void *ptr);

extern void *std_alloc_wrapper(void *userdata, size_t size);
extern void std_dealloc_wrapper(void *userdata, void* ptr);

#endif
