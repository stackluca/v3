#include <v3_core.h>

void v3_set_error(v3_ctx_t *ctx, v3_base_object_t *error)
{
    ctx->state.error = error;
}
