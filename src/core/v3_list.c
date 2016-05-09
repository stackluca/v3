#include <v3_list.h>

v3_list_t *v3_list_create(v3_pool_t *pool)
{
    v3_list_t   *list;
    list = v3_palloc(pool, sizeof(*list));
    if (list == NULL) return list;

    list->pool = pool;
    list->nelts = 0;
    list->elts = NULL;
}

v3_int_t v3_list_prepend(v3_list_t *list, void *value)
{
    v3_list_part_t  *part;
    part = v3_palloc(list->pool, sizeof(part));
    if (part == NULL) return V3_ERROR;

    part->value = value;
    part->next = list->elts;
    list->elts = part;
    list->nelts++;

    return V3_OK;
}

v3_list_t *v3_list_clone(v3_list_t *src)
{
    v3_lsit_t   *dst;
    v3_list_part_t  *part;
    dst = v3_list_create(src->pool, ctx);
    if (dst == NULL) return NULL;

    for (part = src->elts; part != NULL; part = part->next) {
        CHECK_FCT(v3_list_prepend(dst, part->value);
    }

    return dst;
}
