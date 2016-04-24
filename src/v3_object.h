#ifndef _V3_OBJECT_H
#define _V3_OBJECT_H 1


static inline void v3_object_init(v3_object_t *obj)
{

}

// key could be number_object (used in array) and string_object
void v3_object_add(v3_object_t *obj, v3_base_object_t *key, v3_base_object_t *value);
#endif
