#include <v3_core.h>
#include "v3_eval.h"
#include "v3_parser.h"

// static v3_object_t v3_global;

typedef v3_base_object_t *(*v3_eval_pt)(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node);
static v3_base_object_t *v3_variable_declarator_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node);
static v3_base_object_t *v3_variable_statement_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node);
static v3_base_object_t *v3_literal_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node); 
static v3_base_object_t *
v3_program_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *program);

extern v3_int_t v3_init_Function(v3_ctx_t *ctx);
extern v3_int_t v3_init_Object(v3_ctx_t *ctx);
extern v3_int_t v3_init_Number(v3_ctx_t *ctx);

v3_eval_pt v3_evaleators[] = {
    v3_variable_statement_eval, /* v3_syntax_variable_declaration */
    v3_variable_declarator_eval, 
    NULL, /* v3_member_expr_eval */
    v3_program_eval,
    NULL, /* v3_identifier_eval */
    v3_literal_eval,
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
    }

    v3_program_eval(ctx, NULL, (v3_node_t *)program);

    return V3_OK;
}

static v3_base_object_t *
v3_program_eval(v3_ctx_t *ctx, v3_frame_t *aframe, v3_node_t *anode)
{
    size_t              i = 0;
    v3_program_node_t   *program;
    v3_node_t           *node, **nodes;
    
    v3_frame_t          frame;

    program = (v3_program_node_t *)anode;
    frame.global = ctx->global;
    frame.local  = ctx->global;
    frame.self   = NULL;
    frame.prev   = NULL;

    nodes = program->body->items;
    for (; i < program->body->length; i++) {
        node = nodes[i];
        v3_evaleators[node->type](ctx, &frame, node);
    }

    return NULL;
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

    dec = (v3_variable_declarator_t *)node;
    value = v3_evaleators[dec->init->type](ctx, frame, node);
    return value;
}

static v3_base_object_t *
v3_literal_eval(v3_ctx_t *ctx, v3_frame_t *frame, v3_node_t *node)
{
    v3_literal_t    *literal;
    literal = (v3_literal_t *)node;
    return literal->value;
}
