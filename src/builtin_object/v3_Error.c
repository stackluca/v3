#include <v3_core.h>

static v3_int_t v3_init_Error_prototype(v3_ctx_t *ctx);

char *v3_err_cant_convert_undefined_to_obj = "can't convert undefined to object";
char *v3_err_incompatible_object = "incompatible object";

static v3_base_object_t *
v3_Error_construct(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args);

v3_function_object_t            *Error;
v3_object_t                     *Error_prototype;

v3_int_t v3_init_Error(v3_ctx_t *ctx)
{
    v3_function_object_t    *Error;

    v3_init_Error_prototype(ctx);

    Error = v3_function_create_native(ctx, v3_strobj("Error"), 
                                    v3_numobj(1), v3_Error_construct);
    if (Error == NULL) return V3_ERROR;
                                    
    v3_obj_set(ctx->global, v3_strobj("Error"), (v3_base_object_t *)Error);

    v3_function_set_prototype(ctx, Error, Error_prototype);

    v3_init_TypeError(ctx);
    return V3_OK;
}

static v3_int_t v3_init_Error_prototype(v3_ctx_t *ctx)
{
    v3_obj_set(Error_prototype, v3_strobj("toString"), NULL);
    v3_obj_set(Error_prototype, v3_strobj("columnError"), v3_numobj(0));
    v3_obj_set(Error_prototype, v3_strobj("lineError"), v3_numobj(0));

    v3_obj_set(Error_prototype, v3_strobj("message"), v3_strobj( ""));
    v3_obj_set(Error_prototype, v3_strobj("stack"), v3_strobj( ""));
    v3_obj_set(Error_prototype, v3_strobj("fileName"), v3_strobj( ""));
    v3_obj_set(Error_prototype, v3_strobj("name"), v3_strobj( "Error"));
    return V3_OK;
}

static v3_base_object_t *
v3_Error_construct(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    v3_object_t     *error;
    if (this == &v3_null) {
        error = v3_object_create(ctx, 5);
    } else {
        error = (v3_object_t *)this;
    }

    v3_obj_set(error, v3_strobj("prototype"), Error_prototype);
    return error;
}
