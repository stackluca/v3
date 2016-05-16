#include <v3_core.h>

v3_base_object_t *v3_SyntaxError;
v3_base_object_t *v3_TypeError;
v3_base_object_t *v3_ReferenceError;

void v3_set_error(v3_ctx_t *ctx, v3_base_object_t *error)
{
    ctx->state.error = error;
}
