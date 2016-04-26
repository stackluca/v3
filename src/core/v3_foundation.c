#include <string.h>
#include <stdlib.h>
#include <v3_core.h>

void* v3_alloc(v3_options_t *options, size_t size)
{
    return options->alloc(options->userdata, size);
}

void v3_dealloc(v3_options_t *options, void *ptr)
{
    return options->dealloc(options->userdata, ptr);
}

