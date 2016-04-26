#include <v3_core.h>
#include "v3_number_object.h"

v3_function_object_t            Number;
v3_object_t                     Number_prototype;
// static v3_number_object_t       NaN;
// static v3_number_object_t       max_value;

v3_int_t v3_init_Number(v3_ctx_t *ctx)
{

#if 0
    v3_function_object_t    *parse_int;
    v3_function_object_t    *to_string;
    v3_function_object_t    *number_toFixed;

    // parse_int.length    = 1;
    // parse_int.name      = "parseInt";
    v3_number_init(&max_value, 2048 /*TODO:*/);

    v3_init_Number_prototype(v3_ctx_t *ctx);
    Number.name = v3_string("Number");

    v3_object_set(ctx->global, "Number", Number);
    v3_object_set(Number, "prototype", Number_prototype);
    // v3_object_set(Number, "isNaN", isNaN);
    v3_object_set(Number, "parseInt", parse_int);
    v3_object_set(Number, "MAX_VALUE", max_value);

#endif
    return V3_OK;
}

static void v3_init_Number_prototype(v3_ctx_t *ctx)
{
    // v3_object_set(Number_prototype, "constructor", number);
    //v3_object_set(Number_prototype, "toString", to_string);
    //v3_object_set(Number_prototype, "toFixed", v3_number_toFixed);
    //number_prototype.__proto__ = Object_prototype;
}

static v3_string_object_t *v3_number_toString(v3_base_object_t *this)
{
    return NULL;
}


static v3_base_object_t *toString(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    // v3_number_object_t  *num;
    // v3_string_object_t  *str;

#if 0
    if (this == NULL) return v3_type_error(v3_err_cant_convert_undefined_to_obj);
    if (this->type != V3_TYPE_NUMBER) return v3_type_error(v3_err_incompatible_object);

    // TODO: args[0] is 
    nm = this;
    
    str = v3_pcalloc(ctx->pool, sizeof(*str));
    if (str == NULL) return v3_throw_error("out of memory");
    v3_string_object_init(num);
    str->len = 4;
    str->data = "9527";// TODO:snprintf();
    return num;
#endif
    return NULL;
}

static v3_base_object_t *parseInt(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
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
