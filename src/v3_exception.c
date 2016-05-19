#include <v3_core.h>
#include "v3_exception.h"

v3_object_t *v3_SyntaxError = NULL;
v3_object_t *v3_TypeError = NULL;
v3_object_t *v3_ReferenceError = NULL;

void v3_set_error(v3_ctx_t *ctx, v3_object_t *error, const char *msg)
{
    ctx->err = msg;
    ctx->error = error;
}

char *v3_str_error(v3_int_t code)
{

    switch(code) {
    case V3_ERROR:
        return "system error";
    case V3_OUT_OF_MEMORY:
        return "out of memory";
    case V3_UNEXPEDTED_TOKEN:
        return "unexpected token";
    case V3_ALLOC_ERROR:
        return "alloc error";
    case V3_SYNTAX_ERROR:
        return "syntax error";
    case V3_ETYPE:
        return "type error";
    case V3_NOT_SUPPORT:
        return "not support";
    default:
        return "unkown error";
    }
}
