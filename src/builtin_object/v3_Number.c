#include <v3_core.h>
#include <math.h>
#include <assert.h>
#include "../v3_exception.h"

static v3_int_t v3_init_Number_prototype(v3_ctx_t *ctx);
static v3_base_object_t *parseInt(v3_ctx_t *ctx);
static v3_base_object_t *toString(v3_ctx_t *ctx);
static v3_base_object_t *Number(v3_ctx_t *ctx);
static v3_base_object_t *valueOf(v3_ctx_t *ctx);

v3_object_t                     *Number_prototype;
v3_function_object_t            *v3_Number;
v3_number_object_t              *v3_NaN;
// static v3_number_object_t       max_value;

v3_int_t v3_init_Number(v3_ctx_t *ctx)
{
    v3_function_object_t            *number;
    size_t                  i;
    // v3_function_object_t    *parseInt, *toFixed;

    v3_obj_set_native_func(ctx, ctx->global, Number, 1);
    number = (v3_function_object_t *)v3_object_get(ctx->global, v3_strobj("Number"));
    assert(number != NULL);
    v3_Number = number;

    v3_NaN = v3_numobj(NAN);

    v3_init_Number_prototype(ctx);
    // v3_number_init(&max_value, 2048 /*TODO:*/);
    for (i = 0; i < V3_NUMBER_POOL_MAX; i++) {
        v3_number_init(ctx, &v3_number_pool[i]);
        v3_number_pool[i].value = i;
    }
    
    v3_function_set_prototype(ctx, number, Number_prototype);

    // v3_object_set(Number, "isNaN", isNaN);
    // v3_object_set(Number, "parseInt", parse_int);
    // v3_object_set(Number, "MAX_VALUE", max_value);

    return V3_OK;
}


/**
 * Number([value]) convert value to number
 * new Number([value]) inital 
 */
static v3_base_object_t *
Number(v3_ctx_t *ctx)
{
    v3_object_t             *wrapper = to_obj(ctx->frame->this);
    v3_object_t             *arguments;
    v3_base_object_t        *ret;
    v3_number_object_t      *length, *number;

    v3_obj_set(wrapper, v3_strobj(INTER_CLASS), v3_strobj("Number"));

    arguments = to_obj(v3_object_get(to_obj(ctx->frame->call_obj), v3_strobj("arguments")));
    assert(arguments != NULL);

    length = to_number(v3_object_get(arguments, v3_strobj("length")));
    assert(length != NULL);

    if (length->value > 0) {
        ret = v3_object_get(arguments, v3_strobj("0"));
        if (ret->type != V3_DATA_TYPE_NUMBER) {
            assert(0);
        }
        number = to_number(ret);
        v3_obj_set(wrapper, v3_strobj("[[Value]]"), number);
    } else {
        v3_obj_set(wrapper, v3_strobj("[[Value]]"), v3_numobj(0));
    }

    // v3_obj_set(wrapper, v3_strobj(INTER_PROTOTYPE), Number_prototype);
    wrapper->base.__proto__ = Number_prototype;
    // TODO: when arguments length > 0

    // TODO: when called as a function
    return to_base(&v3_undefined);
}

static v3_int_t v3_init_Number_prototype(v3_ctx_t *ctx)
{
    // v3_function_object_t            *to_string;

    Number_prototype = v3_object_create(ctx, 5);
    if (Number_prototype == NULL) return V3_ERROR;

    
    v3_obj_set_native_func(ctx, Number_prototype, toString, 2);
    v3_obj_set_native_func(ctx, Number_prototype, valueOf, 0);
    v3_obj_set_native_func(ctx, Number_prototype, parseInt, 2);

    //v3_object_set(Number_prototype, "toFixed", v3_number_toFixed);
    return V3_OK;
}


/**
 *
 */
static v3_base_object_t *valueOf(v3_ctx_t *ctx)
{
    v3_object_t*    wrapper;
    v3_number_object_t  *number;
    
    if (ctx->frame->this->type != V3_DATA_TYPE_OBJECT) {
        v3_set_error(ctx, v3_TypeError, "not a object");
        return NULL;
    }

    wrapper = to_obj(ctx->frame->this);
    if (!v3_class_is(ctx, wrapper, v3_strobj("Number"))) {
        v3_set_error(ctx, v3_TypeError, "not a Number object");
        return NULL;
    }
    
    number = to_number(v3_object_get(wrapper, v3_strobj("[[Value]]")));
    assert(number !=  NULL);

    return to_base(number);
}

static v3_base_object_t *toString(v3_ctx_t *ctx)
{
    if (ctx->frame->this->type != V3_DATA_TYPE_OBJECT) {
        v3_set_error(ctx, v3_TypeError, "need a object");
        return NULL;
    }

    if (!v3_class_is(ctx, to_obj(ctx->frame->this), v3_strobj("Number"))) {
        v3_set_error(ctx, v3_TypeError, "not a Number object");
        return NULL;
    }

    return to_base(v3_to_string(ctx, ctx->frame->this));
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
