#include <v3_core.h>
#include <v3_object.h>
#include <v3_number_object.h>
#include <v3_parser.h>

// static v3_object_t v3_global;

v3_int_t v3_init(v3_ctx_t *ctx)
{
    // init global;
    v3_object_t     *global;
    global = v3_palloc(ctx->pool, sizeof(v3_object_t));
    if (globals == NULL) return V3_ERROR;

    rc = v3_object_init(global, 1000);
    if (rc != V3_OK) return rc;

    ctx->global = global;

    v3_object_init(v3_global);

    // builtin
    v3_init_Object(ctx);
    v3_init_Number(ctx);
}

v3_int_t v3_eval(v3_ctx_t *ctx, char *code)
{
    v3_program_node_t *program;

    rc = v3_parse(ctx, &program);
    if (rc != V3_OK) {
        return rc;
    }

    return V3_OK;
}
