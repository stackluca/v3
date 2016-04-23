#include <v3_core.h>
#include "v3_tokenizer.h"

typedef struct {
    v3_pool_t       *pool;
    v3_tokenizer_t  tokenizer;
} v3_ctx_t;

static v3_node_t *parseProgram(v3_ctx_t *ctx);


// v3_object_t v3_global;

static void v3_ctx_init(v3_ctx_t *ctx)
{
    memset(&ctx, 0, sizeof(v3_ctx_t));
}

static void* v3_palloc_wrapper(void *userdata, size_t size)
{
    v3_pool_t *pool;
    pool = userdata;
    return v3_palloc(pool, size);
}

static void v3_pdealloc_wrapper(void *userdata, void *value)
{
    v3_pool_t *pool;
    pool = userdata;
    v3_pfree(pool, value);
}

v3_node_t *parse(const char *code, size_t len)
{
    v3_node_t   *program;
    v3_ctx_t    context;

    context.pool = v3_pool_create();

    v3_options_t    options = {
        v3_palloc_wrapper,
        v3_pdealloc_wrapper,
        context.pool
    };

    if (context.pool == NULL) {
        return NULL;
    }

    if (v3_tokenizer_init(&context.tokenizer, &options, code, len) != V3_OK) {
        return NULL;
    }

    v3_ctx_init(&context);
    
    program = parseProgram(&context);

    return program;
}

static v3_node_t *parseProgram(v3_ctx_t *ctx)
{
    v3_node_t   *node;
    skipComment(&ctx->tokenizer);    
    peek(&ctx->tokenizer);
    node = v3_node_create(ctx);
    if (node == NULL) return NULL;
}

