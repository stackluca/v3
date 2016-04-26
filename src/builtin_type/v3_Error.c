#include <v3_core.h>


char *v3_err_cant_convert_undefined_to_obj = "can't convert undefined to object";
char *v3_err_incompatible_object = "incompatible object";
v3_function_object_t            Error;
v3_object_t                     Error_prototype;

v3_int_t v3_init_Error(v3_ctx_t *ctx)
{
    v3_init_Error_prototype(v3_ctx_t *ctx);

    Error.name.data = "Error";
    Error.name.len = 5;
    Error.length = 1;

    v3_object_set(ctx->global, "Error", Error);
    v3_object_set(Error, "prototype", Error_prototype);

    return V3_OK;
}

static v3_int_t v3_init_Error_prototype(v3_ctx_t *ctx)
{
#if 0
    v3_function_object_t            *to_string;

    to_string = v3_function_object_create(ctx, "toString", 2, toString);
    if (to_string == NULL) {
        return V3_ERROR;
    }
#endif
    
    v3_object_set(ctx, &Error_prototype, "constructor", &Error);
    v3_object_set(ctx, &Error_prototype, "toString", NULL);
    v3_object_set(ctx, &Error_prototype, "columnError", v3_number_create(ctx, 0));
    v3_object_set(ctx, &Error_prototype, "lineError", v3_number_create(ctx, 0));

    v3_object_set(ctx, &Error_prototype, "message", v3_string_create(ctx, ""));
    v3_object_set(ctx, &Error_prototype, "stack", v3_string_create(ctx, ""));
    v3_object_set(ctx, &Error_prototype, "fileName", v3_string_create(ctx, ""));
    v3_object_set(ctx, &Error_prototype, "name", v3_string_create(ctx, "Error"));
    //v3_object_set(Error_prototype, "toFixed", v3_number_toFixed);
    Error_prototype.__proto__ = Object_prototype;
    return V3_OK;
}
