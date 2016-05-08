#include <v3_core.h>
#include <v3_dict.h>

static v3_int_t hash(const char *key, size_t len);

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

v3_int_t v3_dict_set(v3_dict_t *dict, const char *key, size_t key_len, void *value)
{
    v3_int_t            hash_code; 
    v3_int_t            index;
    v3_dict_bucket_t    *bucket;
    v3_dict_slot_t      *slot;

    hash_code = hash(key, key_len);

    index = hash_code % dict->capacity;

    bucket = &dict->buckets[index];

    for (slot = bucket->head; slot != NULL; slot = slot->next) {
        if (slot->key.length == key_len
            && strncmp(slot->key.data, key, key_len) == 0) {
            // exists;
            slot->value = value;
            return V3_OK;
        }
    }

    slot = v3_palloc(dict->pool, sizeof(v3_dict_slot_t));
    if (slot == NULL) return V3_ERROR;

    slot->key.data = key;
    slot->key.length = key_len;
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

void *v3_dict_get(v3_dict_t *dict, const char *key, size_t len)
{
    v3_int_t            hash_code; 
    v3_int_t            index;
    v3_dict_bucket_t    *bucket;
    v3_dict_slot_t      *slot = NULL;

    hash_code = hash(key, len);
    index = hash_code % dict->capacity;
    bucket = &dict->buckets[index];

    for (slot = bucket->head; slot != NULL; slot = slot->next) {
        if (slot->key.length == len
            && strncmp(slot->key.data, key, len) == 0) {
            return slot->value;
        }
    }

    return NULL;
}

static v3_int_t hash(const char *key, size_t len)
{
    v3_int_t   hash_code = 0, i;
     
    for (i = 0; i < len; i++) {
        hash_code += key[i] ^ 31;
    }

    return hash_code;
}
