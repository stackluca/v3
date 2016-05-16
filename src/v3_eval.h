#ifndef _V3_EVAL_H
#define _V3_EVAL_H 

typedef v3_base_object_t *(*v3_eval_pt)(v3_ctx_t *ctx, v3_node_t *node);

extern v3_int_t v3_init_global(v3_ctx_t *ctx);
extern v3_int_t v3_eval(v3_ctx_t *ctx, char *code);
extern v3_eval_pt v3_evaleators[];

v3_base_object_t *v3_variable_declarator_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_variable_statement_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_literal_eval(v3_ctx_t *ctx, v3_node_t *node); 
v3_base_object_t *v3_program_eval(v3_ctx_t *ctx, v3_node_t *program);
v3_base_object_t *v3_identifier_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_expr_statement_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_new_expr_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_call_expr_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_block_statement_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_function_declaration_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_function_expr_eval(v3_ctx_t *ctx, v3_node_t *node);
#endif
