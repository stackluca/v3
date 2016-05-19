#include <v3_core.h>
#include <v3_conversion.h>
#include <assert.h>
#include "../v3_exception.h"

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
        // TODO:
        v3_set_error(ctx, v3_SyntaxError, "not support object to primitive yet");
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
