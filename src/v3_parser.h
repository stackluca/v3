#ifndef _V3_PARSER_H
#define _V3_PARSER_H 
#include <v3_core.h>

#define c_lookahead ctx->tokenizer.lookahead
extern v3_str_t dec_kind_var;

v3_int_t v3_parse(v3_ctx_t *ctx, const char *code, size_t len, v3_program_node_t **program);
#endif
