#include <v3_core.h>
#include <v3_conversion.h>
#include <assert.h>
#include "../v3_exception.h"
#include "../v3_eval.h"

v3_base_object_t *v3_to_primitive(v3_ctx_t *ctx, v3_base_object_t *value)
{
    assert(v3_is_js_type(value));
    
    switch (value->type) {
    case (V3_DATA_TYPE_UNDEFINED):
    case (V3_DATA_TYPE_NULL):
        return value;
    case (V3_DATA_TYPE_BOOL):
        return value;
    case (V3_DATA_TYPE_NUMBER):
    case (V3_DATA_TYPE_STRING):
        return value;
    case (V3_DATA_TYPE_OBJECT):
    case (V3_DATA_TYPE_FUNCTION):
        return v3_object_get(to_obj(value), v3_strobj("[[Value]]"));
        // TODO:
        //v3_set_error(ctx, v3_SyntaxError, "not support object to primitive yet");
        return NULL;
    default:
        assert(0);
    }
}

v3_number_object_t *v3_to_number(v3_ctx_t *ctx, v3_base_object_t *value)
{
    assert(v3_is_js_type(value));
    
    switch (value->type) {
    case (V3_DATA_TYPE_UNDEFINED):
        return v3_NaN;
    case (V3_DATA_TYPE_NULL):
        return v3_numobj(0);
    case (V3_DATA_TYPE_BOOL):
        return to_bool(value)->value == V3_TRUE ? v3_numobj(1) : v3_numobj(0);
    case (V3_DATA_TYPE_NUMBER):
        return to_number(value);
    case (V3_DATA_TYPE_STRING):
        v3_set_error(ctx, v3_SyntaxError, "not support convert string to number yet");
        return NULL;
    case (V3_DATA_TYPE_OBJECT):
    case (V3_DATA_TYPE_FUNCTION):
        // TODO:
        v3_set_error(ctx, v3_SyntaxError, "not support convert object to number yet");
        return NULL;
    default:
        assert(0);
    }
}

v3_object_t *v3_to_object(v3_ctx_t *ctx, v3_base_object_t *value)
{
    assert(v3_is_js_type(value));

    v3_vector_t         *arg_values;
    v3_base_object_t    **arg;
    arg_values = v3_vector_new(ctx->options, sizeof(v3_base_object_t *), 1);
    if (arg_values == NULL) return NULL;
    arg = v3_vector_push(ctx->options, arg_values);
    *arg = value;

    switch (value->type) {
    case (V3_DATA_TYPE_UNDEFINED):
        v3_set_error(ctx, v3_TypeError, "");
        return NULL;
    case (V3_DATA_TYPE_NULL):
        v3_set_error(ctx, v3_TypeError, "");
        return NULL;
    case (V3_DATA_TYPE_BOOL):
        v3_set_error(ctx, v3_SyntaxError, "not suppport bool to object");
        return NULL;
    case (V3_DATA_TYPE_NUMBER):
        return to_obj(v3_function_construct(ctx, v3_Number, arg_values));
    case (V3_DATA_TYPE_STRING):
        v3_set_error(ctx, v3_SyntaxError, "not suppport string to object");
        return NULL;
    case (V3_DATA_TYPE_OBJECT):
    case (V3_DATA_TYPE_FUNCTION):
        return to_obj(value);
    default:
        assert(0);
    }
}

v3_string_object_t *v3_to_string(v3_ctx_t *ctx, v3_base_object_t *value)
{
    char                *buf;
    v3_number_object_t  *num;
    size_t              len;
    v3_base_object_t    *primitive;

    switch (value->type) {
    case (V3_DATA_TYPE_UNDEFINED):
        return v3_strobj("undefined");
    case (V3_DATA_TYPE_NULL):
        return v3_strobj("null");
    case (V3_DATA_TYPE_BOOL):
        return to_bool(value)->value == V3_TRUE ? v3_strobj("true") : v3_strobj("false");
    case (V3_DATA_TYPE_NUMBER):
        num = to_number(value);
        if (num == v3_NaN) return v3_strobj("NaN");
        if (num->value == 0) return v3_strobj("0");

        buf = v3_palloc(ctx->pool, 20);
        len = snprintf(buf, 20, "%d", (int)num->value);
        buf[len] = '\0';
        return v3_strobj(buf);

    case (V3_DATA_TYPE_STRING):
        return to_string(value);
    case (V3_DATA_TYPE_OBJECT):
    case (V3_DATA_TYPE_FUNCTION):
        primitive = v3_to_primitive(ctx, value);
        assert(v3_is_primitive_type(primitive));
        return v3_to_string(ctx, primitive);
    default:
        assert(0);
    }
}
