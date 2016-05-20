#include <v3_core.h>
#include "../v3_eval.h"
#include "../v3_parser.h"

static v3_int_t v3_find_var(v3_ctx_t *ctx, v3_node_t *node, void *userdata);

v3_function_object_t *
v3_function_create(v3_ctx_t *ctx, v3_string_object_t *name, v3_number_object_t *arg_count)
{
    v3_function_object_t    *afunc;
    afunc = v3_palloc(ctx->pool, sizeof(v3_function_object_t));
    if (afunc == NULL) return NULL;
    
    memset(afunc, 0, sizeof(*afunc));

    v3_object_init(ctx, &afunc->base, 5);

    afunc->base.base.type = V3_DATA_TYPE_FUNCTION;
    afunc->base.base.__proto__ = Function_prototype;

    v3_object_set(to_obj(afunc), v3_strobj(INTER_CLASS), to_base(v3_strobj("Function")));

    afunc->name = name;
    afunc->length = arg_count;

    return afunc;
}

v3_function_object_t *
v3_function_create_native(v3_ctx_t *ctx, v3_string_object_t *name, v3_number_object_t *arg_count, v3_func_pt pt)
{
    v3_function_object_t *func;
    func = v3_function_create(ctx, name, arg_count);
    if (func == NULL) return NULL;

    func->is_native = 1;
    func->native_func = pt;
    return func;
}

v3_function_object_t *
v3_function_from_node(v3_ctx_t *ctx, v3_function_node_t *func_node, v3_list_t *scope)
{
    v3_function_object_t    *func_obj;
    v3_string_object_t      *strobj;
    size_t                  arg_count;
    v3_int_t                rc;
    v3_object_t             *prototype;

    if (func_node->id != NULL) {
        strobj = v3_str2string(&func_node->id->name);
    } else {
        strobj = v3_strobj("");
    }

    arg_count = func_node->params == NULL ? 0 : func_node->params->length;
    prototype = v3_object_create(ctx, 10);

    func_obj = v3_function_create(ctx, strobj, v3_numobj(arg_count));
    if (func_obj == NULL) return NULL;

    v3_object_set(to_obj(func_obj), v3_strobj("constructor"), to_base(func_obj));
    v3_object_set(to_obj(func_obj), v3_strobj("prototype"), to_base(prototype));

    func_obj->node = func_node;

    func_obj->node->var_decs = v3_vector_new(ctx->options, 10, sizeof(void *));
    rc = v3_reverse_block_do(ctx, func_node->body, v3_find_var, (void *)func_obj->node->var_decs);
    if (rc != V3_OK) return NULL;

    // TODO: add func_decs;

    func_obj->scopes = v3_list_clone(ctx, ctx->frame->scopes);

    return func_obj;
}

static v3_int_t v3_find_var(v3_ctx_t *ctx, v3_node_t *node, void *userdata)
{
    v3_vector_t                 *vars; 
    v3_variable_statement_t     *var_stmt;
    v3_variable_declarator_t    **decs, **adec;
    size_t                      i;

    vars = (v3_vector_t *)userdata;

    if (node->type == V3_SYNTAX_VARIABLE_DECLARATION) {
        var_stmt = (v3_variable_statement_t *)node;

        decs = var_stmt->declarations->items; 
        for (i = 0; i < var_stmt->declarations->length; i++) {
            
            if (var_stmt->kind.data == dec_kind_var.data) {
                adec = v3_vector_push(ctx->options, vars);
                if (adec == NULL) return V3_ERROR;
                *adec = decs[i];
            } 
        }
    }

    return V3_OK;
}

v3_base_object_t *
v3_function_apply(v3_ctx_t *ctx, v3_function_object_t *func, v3_base_object_t *this, v3_arguments_t *arguments)
{
    if (func->is_native) {
        return func->native_func(ctx);
    } else {
        return NULL;
    }
}

