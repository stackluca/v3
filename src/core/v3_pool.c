#include <stdlib.h>
#include <v3_pool.h>

void *v3_palloc(v3_pool_t *pool, size_t size)
{
    return malloc(size);
}

void v3_pfree(v3_pool_t *pool, void *value)
{
    free(value);

}

v3_pool_t *v3_pool_create(size_t size)
{
    v3_pool_t   *pool; 
    pool = malloc(size);
    if (pool == NULL) return NULL;
#if 0
    // TODO:
    pool->size = size < 1024 ? 1024 : size;
    pool->mem = malloc(pool->size);
    pool->next = NULL;
#endif
    return pool;
}

void v3_pool_destory(v3_pool_t *pool)
{
#if 0
    for (; pool != NULL && pool->mem != NULL; pool = pool->next) {
        dealloc(pool->mem);
    }
#endif
}
