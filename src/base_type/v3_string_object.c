#include <v3_core.h>

v3_dict_t *v3_string_pool;

v3_string_object_t *v3_string_create(v3_ctx_t *ctx, const char *value)
{
    v3_string_object_t  *str;
    v3_string_object_t  *estr;

    if (v3_string_pool == NULL) {
        v3_string_pool = v3_dict_create(ctx, 2000);
        if (v3_string_pool == NULL) return NULL;
    }

    estr = v3_dict_get(ctx, v3_string_pool);
    if (estr != NULL) {
        return estr; 
    }

    str = v3_palloc(ctx->pool, sizeof(*str));
    if (astring == NULL) return NULL;

    v3_string_init(ctx, str);
    str->value.data = value;
    str->value.len  = strlen(value);

    return str;
}

