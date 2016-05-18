#include <v3_core.h>
#include "v3_exception.h"

v3_object_t *v3_SyntaxError = NULL;
v3_object_t *v3_TypeError = NULL;
v3_object_t *v3_ReferenceError = NULL;

void v3_set_error(v3_ctx_t *ctx, v3_object_t *error)
{
    ctx->error = error;
}
