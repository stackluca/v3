#include <v3_list.h>

v3_list_t *v3_list_create(v3_pool_t *pool)
{
    v3_list_t   *list;
    list = v3_palloc(pool, sizeof(*list));
    if (list == NULL) return list;

    list->pool = pool;
    list->nelts = 0;
    list->elts = NULL;

    return list;
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

void v3_list_pop(v3_list_t *list)
{
    //v3_list_part_t  *part;
    if (list->nelts == 0) return;

    list->elts = list->elts->next;
}

v3_list_t *v3_list_clone(v3_ctx_t *ctx, v3_list_t *src)
{
    v3_list_t       *dst;
    v3_list_part_t  *part;
    dst = v3_list_create(ctx->pool);
    if (dst == NULL) return NULL;

    for (part = src->elts; part != NULL; part = part->next) {
        CHECK_FCT2(v3_list_prepend(dst, part->value), NULL);
    }

    return dst;
}
