#include <v3_vector.h>

void* v3_vector_new(v3_options_t *options, size_t itemSize, unsigned int capacity)
{
    v3_vector_t *vector = v3_alloc(options, sizeof(v3_vector_t));     
    if (vector == NULL) {
        return NULL;
    }

    vector->items = v3_alloc(options, itemSize * capacity);
    if (vector->items == NULL) {
        return NULL;
    }

    vector->length = 0;
    vector->capacity = capacity;
    vector->size = itemSize;
    return vector;
}

void* v3_vector_push(v3_options_t *options, v3_vector_t *vector)
{
    void *result, *newItems;
    size_t newCapacity;
    if (vector->length < vector->capacity) {
        result = (char *)vector->items + vector->size * vector->length; 
        vector->length++;
    } else {
        newCapacity = vector->capacity * 2;
        newItems = v3_alloc(options, vector->size * newCapacity);
        if(newItems == NULL) {
            return NULL;
        }
        vector->capacity = newCapacity;
        // memmove(newItems, vector->items, vector->length * vector->size);
        memcpy(newItems, vector->items, vector->length * vector->size);
        v3_dealloc(options, vector->items);
        vector->items = newItems;
        result = (char *)vector->items + vector->size * vector->length; 
        vector->length++;
    }

    return result;
}
