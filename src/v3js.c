#include <v3_core.h>
#include "v3_eval.h"
#include <stdio.h>
#include <string.h>

#define PROMPT "> "
#define V3_VERSION "0.1"
static void show_version_info();

static void v3_ctx_init(v3_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(v3_ctx_t));
}

int main(int argc, char ** argv)
{
    FILE        *stream;
    char        buf[4096];
    v3_ctx_t    ctx;
    size_t      len;

    stream = stdin;
    show_version_info();

    v3_ctx_init(&ctx);

    ctx.pool = v3_pool_create(1024);
    if (ctx.pool == NULL) {
        return V3_ERROR;
    }

    v3_options_t    options = {
        v3_palloc_wrapper,
        v3_pdealloc_wrapper,
        ctx.pool
    };

    ctx.options = &options;
    v3_init_global(&ctx);

    // pending
    // resolve external file passed by argv
        // parse
        // execute
        // show result or err
        // return 

    // read input
    printf(PROMPT);
    while (fgets(buf, sizeof(buf), stream) != NULL) {
        // execute
        len = strlen(buf);
        if (len == 1 && buf[0] == '\n') {
        } else {
            v3_eval(&ctx, buf);
        }
        // show result or err
        // read input
        printf(PROMPT);
    }

    printf("bye \n");
    return 0;
}

static void show_version_info()
{
    printf("welcome to v3 javascript interpretor %s\n", V3_VERSION);
    printf("\n");
}
