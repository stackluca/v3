#include <v3_core.h>
#include <v3_object.h>

v3_object_t     Object_prototype;

void v3_init_Object(v3_ctx_t *ctx)
{
    v3_function_object_t    object;
    // v3_function_object_t    toString;

    v3_object_set(ctx->globals, "Object", object);
    v3_object_set(object, "prototype", Object_prototype);

    Object_prototype.__proto__ = NULL;

    // v3_object_set(Object_prototype, "toString", toString);
}

static inline  v3_int_t 
v3_object_init(v3_ctx_t *ctx, v3_object_t *obj, size_t capacity)
{
    obj->node.type = V3_DATA_TYPE_OBJECT;
    obj->__attrs__ = v3_dict_create(ctx->pool, capacity);    
    if (obj->__attrs__ == NULL) return V3_ERROR;
    
    return V3_OK;
}

static v3_base_object_t 
*v3_object_new(v3_ctx_t *ctx, v3_base_object_t *this, v3_arguments_t *args)
{
    // TODO:
    return NULL;    
}

v3_int_t 
v3_objec_set(v3_ctx_t *ctx, v3_object_t* obj, v3_string_object_t *key, v3_base_objet_t *value)
{
    //v3_base_objet_t     *evalue;
    return v3_dict_set(obj->__attrs__, key->value, value);
}
