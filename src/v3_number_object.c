#include "v3_function_object.h"

v3_function_object_t    Number;
v3_function_object_t    number_toFixed;
v3_object_t             Number_prototype;
v3_function_object_t    parse_int;
v3_function_object_t    to_string;
v3_number_object_t      NaN;

void v3_init_Number()
{
    
    parse_int.length = 1;
    parse_int.name = "parseInt";

    Number.name = "Number";
    v3_object_set(v3_global, "Number", Number);
    v3_object_set(Number, "prototype", number_prototype);
    // v3_object_set(number, "isNaN", number_prototype);
    v3_object_set(Number, "parseInt", parse_int);
    // v3_object_set(number, "MAX_VALUE", parse_int);
    // v3_object_set(number, "parseFloat", number_prototype);
    v3_object_set(number_prototype, "constructor", number);
    v3_object_set(number_prototype, "toString", to_string);
    number_prototype.__proto__ = Object_prototype;

    v3_object_set(number.prototype, "toFixed", v3_number_toFixed);
}

static v3_string_object_t v3_number_toString(v3_base_object_t *this)
{

}

static inline v3_number_init(v3_ctx_t *ctx, v3_number_object_t *num)
{
    num->base.type = V3_TYPE_NUMBER;
    num->__proto__ = Number_prototype; //v3_object_get("prototype", ctx->globals);
    num->value = 0;
}

static v3_base_object_t *toString(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    v3_number_object_t  *num;
    v3_string_object_t  *str;

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
}

static v3_base_object_t *parseInt(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    v3_number_object_t  *num;
    v3_base_object_t    *obj;
    if (args->len == 0) return NaN;

    obj = args->args->get(0);

    num = v3_pcalloc(ctx->pool, sizeof(*num));
    if (num == NULL) return v3_throw_error("out of memory");
    v3_number_init(num);
    num->value = 9527;
    return num;
}
