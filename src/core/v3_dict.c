#include <v3_core.h>
#include <v3_dict.h>

static v3_int_t hash(v3_str_t *key);

v3_dict_t *
v3_dict_create(v3_pool_t *pool, size_t capacity)
{
    v3_dict_t   *dict;

    dict = v3_palloc(pool, sizeof(v3_dict_t));
    if (dict == NULL) return NULL;

    dict->capacity  = capacity;
    dict->pool      = pool;

    dict->buckets = v3_palloc(pool, sizeof(v3_dict_bucket_t) * capacity);
    if (dict->buckets == NULL) return NULL;

    return dict; 
}

v3_int_t v3_dict_set(v3_dict_t *dict, v3_str_t *key, void *value)
{
    v3_int_t            hash_code; 
    v3_int_t            index;
    v3_dict_bucket_t    *bucket;
    v3_dict_slot_t      *slot;

    hash_code = hash(key);

    index = hash_code % dict->capacity;

    bucket = &dict->buckets[index];

    for (slot = bucket->head; slot != NULL; slot = slot->next) {
        if (slot->key.length == key->length
            && strncmp(slot->key.data, key->data, key->length) == 0) {
            // exists;
            slot->value = value;
            return V3_OK;
        }
    }

    slot = v3_palloc(dict->pool, sizeof(v3_dict_slot_t));
    if (slot == NULL) return V3_ERROR;

    slot->key = *key;
    slot->value = value;
    
    if (bucket->head == NULL) {
        bucket->head = slot;
        slot->next = NULL;
    } else {
        slot->next = bucket->head;
        bucket->head = slot;
    }
    bucket->len++;

#ifdef V3_DEBUG
    dict->count++;
#endif
    return V3_OK;
}

extern void *v3_dict_get(v3_dict_t *dict, v3_str_t *key)
{
    v3_int_t            hash_code; 
    v3_int_t            index;
    v3_dict_bucket_t    *bucket;
    v3_dict_slot_t      *slot = NULL;

    hash_code = hash(key);
    index = hash_code % dict->capacity;
    bucket = &dict->buckets[index];

    for (slot = bucket->head; slot != NULL; slot = slot->next) {
        if (slot->key.length == key->length
            && strncmp(slot->key.data, key->data, key->length) == 0) {
            return slot->value;
        }
    }

    return NULL;
}

static v3_int_t hash(v3_str_t *key)
{
    v3_int_t   hash_code = 0, i;
     
    for (i = 0; i < key->length; i++) {
        hash_code += key->data[i] ^ 31;
    }

    return hash_code;
}
