#include <v3_core.h>
#include "v3_parser.h"

// static v3_object_t v3_global;

v3_int_t v3_init_global(v3_ctx_t *ctx)
{
    // init global;
    v3_object_t     *global;
    v3_int_t        rc;

    global = v3_palloc(ctx->pool, sizeof(v3_object_t));
    if (global == NULL) return V3_ERROR;

    rc = v3_object_init(ctx, global, 1000);
    if (rc != V3_OK) return rc;

    ctx->global = global;

    // builtin
    v3_init_Object(ctx);
    v3_init_Number(ctx);

    return V3_OK;
}

v3_int_t v3_eval(v3_ctx_t *ctx, char *code)
{
    v3_int_t            rc;
    v3_program_node_t   *program;
    size_t              len;

    len = strlen(code);
    rc = v3_parse(ctx, code, len, &program);
    if (rc != V3_OK) {
        if (ctx->err != NULL) {
            printf("%s", ctx->err);
            ctx->err = NULL;
        } else {
            printf("Unkown error %d", rc);
        }
        printf(" \n");
    }

    return V3_OK;
}
