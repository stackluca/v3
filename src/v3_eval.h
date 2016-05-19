#ifndef _V3_EVAL_H
#define _V3_EVAL_H 


typedef v3_base_object_t *(*v3_eval_pt)(v3_ctx_t *ctx, v3_node_t *node);
typedef v3_int_t (*v3_node_do_pt)(v3_ctx_t *ctx, v3_node_t *node, void *userdata);

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
v3_int_t v3_reverse_block_do(v3_ctx_t *ctx, v3_block_statement_t * block, v3_node_do_pt handler, void *userdata);
v3_statement_result_t *v3_statement_result_create(v3_ctx_t *ctx, int type, v3_base_object_t *value, v3_base_object_t *label);
v3_base_object_t *v3_binary_expr_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_assignment_expr_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_sequence_expr_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_member_expr_eval(v3_ctx_t *ctx, v3_node_t *node);
v3_base_object_t *v3_function_construct(v3_ctx_t *ctx, v3_function_object_t *func, v3_vector_t *arg_values);
#endif
