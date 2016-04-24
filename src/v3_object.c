
v3_object_t     Object_prototype;

void v3_init_Object()
{
    v3_function_object_t    object;
    v3_function_object_t    toString;

    v3_object_set(v3_global, "Object", object);
    v3_object_set(object, "prototype", Object_prototype);

    Object_prototype.__proto__ = NULL;

    v3_object_set(Object_prototype, "toString", );
}

