#ifndef _V3_CONVERSION_TYPE_H
#define _V3_CONVERSION_TYPE_H 1
#include <v3_core.h>

extern v3_base_object_t *v3_to_primitive(v3_ctx_t *ctx, v3_base_object_t *value);
extern v3_number_object_t *v3_to_number(v3_ctx_t *ctx, v3_base_object_t *value);
extern v3_object_t *v3_to_object(v3_ctx_t *ctx, v3_base_object_t *value);
extern v3_string_object_t *v3_to_string(v3_ctx_t *ctx, v3_base_object_t *value);
#endif
