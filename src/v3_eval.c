#include <v3_core.h>
#include "v3_eval.h"
#include "v3_parser.h"
#include <assert.h>

// static v3_object_t v3_global;

typedef v3_base_object_t *(*v3_eval_pt)(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node);
static v3_base_object_t *v3_variable_declarator_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node);
static v3_base_object_t *v3_variable_statement_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node);
static v3_base_object_t *v3_literal_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node); 
static v3_base_object_t *
v3_program_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *program);
static v3_base_object_t *
v3_identifier_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node);
static v3_base_object_t *
v3_find_property(v3_base_object_t *owner, v3_string_object_t *key);

extern v3_int_t v3_init_Function(v3_ctx_t *ctx);
extern v3_int_t v3_init_Object(v3_ctx_t *ctx);
extern v3_int_t v3_init_Number(v3_ctx_t *ctx);

// typedef void (*v3_disp_result_pt)(v3_ctx_t *ctx, v3_base_object_t *value);

v3_eval_pt v3_evaleators[] = {
    NULL, // V3_SYNTAX_ASSIGNMENT_EXPR,
    NULL, // V3_SYNTAX_CALL_EXPR,
    v3_identifier_eval,//V3_SYNTAX_IDENTIFIER,
    v3_literal_eval,//V3_SYNTAX_LITERAL,
    NULL,//V3_SYNTAX_MEMBER_EXPR,
    v3_program_eval,//V3_SYNTAX_PROGRAM,
    NULL,//V3_SYNTAX_SEQUENCE_EXPR,
    v3_variable_statement_eval,//V3_SYNTAX_VARIABLE_DECLARATION,
    v3_variable_declarator_eval,//V3_SYNTAX_VARIABLE_DECLARATOR,
};


v3_int_t v3_init_global(v3_ctx_t *ctx)
{
    // init global;
    v3_object_t     *global;
    v3_int_t        rc;

    global = v3_palloc(ctx->pool, sizeof(v3_object_t));
    if (global == NULL) return V3_ERROR;

    rc = v3_object_init(ctx, global, 1000);
    if (rc != V3_OK) return rc;

    ctx->global = global;

    // builtin
    v3_init_Function(ctx);
    v3_init_Object(ctx);
    v3_init_Number(ctx);

    return V3_OK;
}

v3_int_t v3_eval(v3_ctx_t *ctx, char *code)
{
    v3_int_t            rc;
    v3_program_node_t   *program;
    size_t              len;
    v3_base_object_t        *ret, *owner;
    v3_string_object_t      *str;
    char                    buf[1024];
    v3_function_object_t    *func;

    len = strlen(code);
    rc = v3_parse(ctx, code, len, &program);
    if (rc != V3_OK) {
        if (ctx->err != NULL) {
            printf("%s", ctx->err);
            ctx->err = NULL;
        } else {
            printf("Unkown error %d", rc);
        }
        printf(" \n");
        return rc;
    }

    ret = v3_program_eval(ctx, NULL, (v3_node_t *)program);

    if (ret != NULL) {
        owner = ret;
        ret = v3_find_property(ret, v3_strobj("toString"));
        if (ret->type == V3_DATA_TYPE_FUNCTION) {
            func = (v3_function_object_t *)ret;
            ret = v3_function_apply(ctx, func, owner, NULL);
            if (ret->type == V3_DATA_TYPE_STRING) {
                str = (v3_string_object_t *)ret;
                memcpy(buf, str->value.data, str->value.length);
                buf[str->value.length] = '\0';
                printf("%s\n", buf);
            }
        }
    } else {
        printf("undefined\n");
    }

    return V3_OK;
}

static v3_base_object_t *
v3_program_eval(v3_ctx_t *ctx, v3_frame_t *aframe, v3_node_t *anode)
{
    size_t                  i = 0;
    v3_program_node_t       *program;
    v3_node_t               *node, **nodes;
    v3_base_object_t        *ret;
    v3_frame_t              frame;

    program = (v3_program_node_t *)anode;
    frame.global = ctx->global;
    frame.local  = ctx->global;
    frame.self   = NULL;
    frame.prev   = NULL;

    nodes = program->body->items;

    for (; i < program->body->length; i++) {
        node = nodes[i];
        ret = v3_evaleators[node->type](ctx, &frame, node);
    }

    return ret;
}

static v3_base_object_t *
v3_variable_statement_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node)
{
    v3_variable_statement_t     *statement;
    size_t                      i;
    v3_base_object_t            *value;
    v3_variable_declarator_t    **dec;

    statement = (v3_variable_statement_t *)node;

    if (statement->declarations == NULL) return NULL;

    dec = statement->declarations->items;

    for (i = 0; i < statement->declarations->length; i++) {
        value = v3_variable_declarator_eval(ctx, frame, (v3_node_t *)dec[i]);

        if (statement->kind.data == dec_kind_var.data) {
            v3_obj_set(frame->local, v3_str2string(&dec[i]->id->name), value);
        } else {
            v3_obj_set(frame->global, v3_str2string(&dec[i]->id->name), value);
        }
    }

    return value;
}

static v3_base_object_t *
v3_variable_declarator_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node)
{
    v3_variable_declarator_t    *dec;
    v3_base_object_t            *value;

    assert(node->type == V3_SYNTAX_VARIABLE_DECLARATOR);
    dec = (v3_variable_declarator_t *)node;
    value = v3_evaleators[dec->init->type](ctx, frame, dec->init);
    return value;
}

/** get value of literl */
static v3_base_object_t *
v3_literal_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node)
{
    v3_literal_t    *literal;
    assert(node->type == V3_SYNTAX_LITERAL);
    literal = (v3_literal_t *)node;
    return literal->value;
}

static v3_base_object_t *
v3_identifier_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node)
{
    v3_idetifier_t  *ident;
    v3_base_object_t    *value;
    ident = (v3_idetifier_t *)node;

    value = v3_object_get_by_str(frame->local, ident->name.data, ident->name.length); 
    // TODO: find in closure 
    if (value == NULL
        && frame->local != frame->global) {
        value = v3_object_get_by_str(frame->local, ident->name.data, ident->name.length); 
    }

    return value;
}

static v3_base_object_t *
v3_find_property(v3_base_object_t *owner, v3_string_object_t *key)
{
    v3_object_t         *prototype, *object;
    v3_base_object_t    *ret;
    if (owner->type == V3_DATA_TYPE_OBJECT
        || owner->type == V3_DATA_TYPE_FUNCTION) {
        object = (v3_object_t *)owner;

        ret = v3_object_get(object, key);
        if (ret != NULL) return ret;

        prototype = v3_object_prototype(object);
        if (prototype != NULL) {
            ret = v3_object_get(prototype, key);
            if (ret != NULL) return ret;
        }

        return v3_find_property((v3_base_object_t *)object->base.__proto__, key);
        ret = v3_object_get(object->base.__proto__, key);
        return ret;
    } else {
        ret = v3_find_property((v3_base_object_t *)owner->__proto__, key);
        return ret;
    }
}
