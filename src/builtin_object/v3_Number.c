#include <v3_core.h>
#include <math.h>
#include <assert.h>

static v3_int_t v3_init_Number_prototype(v3_ctx_t *ctx);
static v3_base_object_t *parseInt(v3_ctx_t *ctx);
static v3_base_object_t * v3_Number_construct(v3_ctx_t *ctx);
static v3_base_object_t *toString(v3_ctx_t *ctx);

v3_object_t                     *Number_prototype;
v3_number_object_t       *v3_NaN;
// static v3_number_object_t       max_value;

v3_int_t v3_init_Number(v3_ctx_t *ctx)
{
    v3_function_object_t            *Number;
    size_t                  i;
    // v3_function_object_t    *parseInt, *toFixed;

    Number = v3_function_create_native(ctx, v3_strobj("Object"), 
                                    v3_numobj(1), v3_Number_construct);
    v3_NaN = v3_numobj(NAN);

    v3_init_Number_prototype(ctx);
    // v3_number_init(&max_value, 2048 /*TODO:*/);
    for (i = 0; i < V3_NUMBER_POOL_MAX; i++) {
        v3_number_init(ctx, &v3_number_pool[i]);
        v3_number_pool[i].value = i;
    }
    

    v3_obj_set(ctx->global, v3_strobj("Number"), Number);
    v3_function_set_prototype(ctx, Number, Number_prototype);

    // v3_object_set(Number, "isNaN", isNaN);
    // v3_object_set(Number, "parseInt", parse_int);
    // v3_object_set(Number, "MAX_VALUE", max_value);

    return V3_OK;
}

static v3_base_object_t *
v3_Number_construct(v3_ctx_t *ctx)
{
    v3_number_object_t  *num;
    v3_object_t         *wrapper = NULL;

    if (ctx->frame->this != NULL) wrapper = (v3_object_t *)ctx->frame->this; /* by new operator */

    num = v3_numobj(0);
#if 0
    // TODO:
    if (args.length > 0) {
        num = parseFloat(args[0]);
    }
#endif

    if (wrapper == NULL) {
        return (v3_base_object_t *)num;
    } else {
        v3_obj_set(ctx->frame->this, v3_strobj("prototype"), Number_prototype);
        wrapper->wrappered_value = (v3_base_object_t *)num;
        return (v3_base_object_t *)wrapper;
    }
}

static v3_int_t v3_init_Number_prototype(v3_ctx_t *ctx)
{
    // v3_function_object_t            *to_string;

    Number_prototype = v3_object_create(ctx, 5);
    if (Number_prototype == NULL) return V3_ERROR;

    
    v3_obj_set_native_func(ctx, Number_prototype, toString, 2);
    v3_obj_set_native_func(ctx, Number_prototype, parseInt, 2);

    //v3_object_set(Number_prototype, "toFixed", v3_number_toFixed);
    return V3_OK;
}

static v3_base_object_t *toString(v3_ctx_t *ctx)
{
    v3_number_object_t  *num;
    //v3_string_object_t  *str;
    char                *buf;
    size_t              len;

    // if (this == NULL) return v3_type_error(ctx, v3_err_cant_convert_undefined_to_obj);
    // if (this->type != V3_TYPE_NUMBER) return v3_type_error(ctx, v3_err_incompatible_object);
    
    assert(ctx->frame->this->type == V3_DATA_TYPE_NUMBER);


    num = (v3_number_object_t *)ctx->frame->this;
    buf = v3_palloc(ctx->pool, 20);
    len = snprintf(buf, 20, "%f", num->value);
    buf[len] = '\0';
    
    // TODO:

    // TODO: args[0] is 
    // num = (v3_number_object_t *)this;

    return (v3_base_object_t *)v3_string_create(ctx, buf, len);
}

static v3_base_object_t *parseInt(v3_ctx_t *ctx)
{
    return NULL;
#if 0
    v3_number_object_t  *num;
    v3_base_object_t    *obj;
    if (args->len == 0) return &NaN;

    obj = args->args->get(0);

    num = v3_pcalloc(ctx->pool, sizeof(*num));
    if (num == NULL) return v3_throw_error("out of memory");
    v3_number_init(num);
    num->value = 9527;
    return num;
    #endif
}
