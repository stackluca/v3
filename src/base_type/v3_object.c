#include <v3_core.h>
#include <v3_object.h>

v3_int_t 
v3_object_init(v3_ctx_t *ctx, v3_object_t *obj, size_t capacity)
{
    obj->base.type = V3_DATA_TYPE_OBJECT;
    obj->base.__proto__ = Object_prototype;
    obj->base.ref_count = 0;
    obj->__attrs__ = v3_dict_create(ctx->pool, capacity);    
    if (obj->__attrs__ == NULL) return V3_ERROR;
    
    return V3_OK;
}

v3_object_t *v3_object_create(v3_ctx_t *ctx, size_t capacity)
{
    v3_object_t  *aobj;
    v3_int_t        rc;

    aobj = v3_palloc(ctx->pool, sizeof(*aobj));
    if (aobj == NULL) return NULL;

    rc = v3_object_init(ctx, aobj, capacity);
    if (rc != V3_OK) {
        return NULL;
    }

    return aobj;
}
#if 0
v3_int_t 
v3_object_set_property(v3_object_t* obj, v3_property_t *property)
{
    //v3_base_objet_t     *evalue;
    return v3_dict_set(obj->__attrs__, property->name.data, property->name.length, property);
}
#endif

v3_int_t 
v3_object_set(v3_object_t* obj, v3_string_object_t *key, v3_base_object_t *value)
{
    //v3_base_objet_t     *evalue;
    return v3_dict_set(obj->__attrs__, key->value.data, key->value.length, value);
}

v3_int_t 
v3_object_set_by_str(v3_object_t* obj, const char *key, size_t len, v3_base_object_t *value)
{
    //v3_base_objet_t     *evalue;
    return v3_dict_set(obj->__attrs__, key, len, value);
}

v3_base_object_t * 
v3_object_get(v3_object_t* obj, v3_string_object_t *key)
{
    //v3_base_objet_t     *evalue;
    return v3_dict_get(obj->__attrs__, key->value.data, key->value.length);
}

v3_base_object_t * 
v3_object_get_by_str(v3_object_t* obj, const char *key, size_t len)
{
    //v3_base_objet_t     *evalue;
    return v3_dict_get(obj->__attrs__, key, len);
}

v3_base_object_t *
v3_find_property(v3_base_object_t *owner, v3_string_object_t *key)
{
    v3_object_t         *prototype, *object;
    v3_base_object_t    *ret;

    if (owner == NULL) return to_base(&v3_undefined);

    if (owner->type == V3_DATA_TYPE_OBJECT
        || owner->type == V3_DATA_TYPE_FUNCTION) {
        object = (v3_object_t *)owner;

        ret = v3_object_get(object, key);
        if (ret != NULL) return ret;

        prototype = (v3_object_t *)v3_object_prototype(object);
        if (prototype != NULL) {
            //ret = v3_object_get(prototype, key);
            ret = v3_find_property((v3_base_object_t *)prototype, key);
            if (ret != NULL) return ret;
        }

        return v3_find_property((v3_base_object_t *)object->base.__proto__, key);
        // ret = v3_object_get(object->base.__proto__, key);
        // return ret;
    } else {
        ret = v3_find_property((v3_base_object_t *)owner->__proto__, key);
        return ret;
    }
}

v3_bool_t v3_class_is(v3_ctx_t *ctx, v3_object_t *object, v3_string_object_t *other)
{
    v3_string_object_t  *class;

    class = to_string(v3_object_get(object, v3_strobj(INTER_CLASS)));
    if (class == NULL) return V3_FALSE;
    
    if (v3_strobj_equal(class, other)) {
        return V3_TRUE;
    }

    return V3_FALSE;
}
