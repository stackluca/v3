#include <v3_core.h>

v3_dict_t *v3_string_pool;

v3_string_object_t *v3_string_create(v3_ctx_t *ctx, const char *value, size_t len)
{
    v3_string_object_t  *str;
    v3_string_object_t  *estr;

    if (v3_string_pool == NULL) {
        v3_string_pool = v3_dict_create(ctx->pool, 2000);
        if (v3_string_pool == NULL) return NULL;
    }

    estr = v3_dict_get(v3_string_pool, value, len);
    if (estr != NULL) {
        return estr; 
    }

    str = v3_palloc(ctx->pool, sizeof(*str));
    if (str == NULL) return NULL;

    v3_string_init(ctx, str);
    str->value.data = value;
    str->value.length  = len;

    return str;
}

