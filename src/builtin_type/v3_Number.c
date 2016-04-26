#include <v3_core.h>
#include <math.h>

static v3_int_t v3_init_Number_prototype(v3_ctx_t *ctx)

v3_function_object_t            *Number;
v3_object_t                     *Number_prototype;
// static v3_number_object_t       NaN;
// static v3_number_object_t       max_value;

v3_int_t v3_init_Number(v3_ctx_t *ctx)
{
    size_t                  i;
    v3_function_object_t    *parseInt; *toFixed;

    Number = v3_function_create(ctx, v3_strobj("Object"), 
                                    v3_numobj(1), v3_Number_construct);

    // v3_number_init(&max_value, 2048 /*TODO:*/);
    for (i = 0; i < V3_NUMBER_POOL_MAX; i++) {
        v3_number_init(ctx, &v3_nubmer_pool[i]);
        v3_number_pool[i].value = i;
    }
    
    v3_init_Number_prototype(v3_ctx_t *ctx);

    v3_object_set(ctx->global, v3_strobj("Number"), Number);
    v3_set_function_prototype(ctx, Number, Number_prototype);

    // v3_object_set(Number, "isNaN", isNaN);
    // v3_object_set(Number, "parseInt", parse_int);
    // v3_object_set(Number, "MAX_VALUE", max_value);

#endif
    return V3_OK;
}

static v3_base_object_t *
v3_Number_construct(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    v3_number_object_t  *num;
    v3_object_t         *wrapper = NULL;

    if (this != NULL) wrapper = this; /* by new operator */

    num = v3_numobj(0);
#if 0
    // TODO:
    if (args.length > 0) {
        num = parseFloat(args[0]);
    }
#endif

    if (wrapper == NULL) {
        return num;
    } else {
        wrapper->wrappered_value = num;
        return wrapper;
    }
}

static v3_int_t v3_init_Number_prototype(v3_ctx_t *ctx)
{
    v3_function_object_t            *to_string;

    to_string = v3_function_object_create(ctx, "toString", 2, toString);
    if (to_string == NULL) {
        return V3_ERROR;
    }
    
    v3_object_set(ctx, Number_prototype, "constructor", Number);
    v3_object_set(ctx, Number_prototype, "toString", to_string);
    //v3_object_set(Number_prototype, "toFixed", v3_number_toFixed);
    //number_prototype.__proto__ = Object_prototype;
}

static v3_base_object_t *v3_Number_toString(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    // v3_number_object_t  *num;
    // v3_string_object_t  *str;

    // TODO:
    if (this == NULL) return v3_type_error(ctx, v3_err_cant_convert_undefined_to_obj);
    if (this->type != V3_TYPE_NUMBER) return v3_type_error(ctx, v3_err_incompatible_object);

    // TODO: args[0] is 
    nm = this;
    
    str = v3_pcalloc(ctx->pool, sizeof(*str));
    if (str == NULL) return v3_throw_error("out of memory");
    v3_string_object_init(num);
    str->len = 4;
    str->data = "9527";// TODO:snprintf();
    return num;
}

static v3_base_object_t *v3_Number_parseInt(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
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