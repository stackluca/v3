#ifndef _v3_prima_h_
#define _v3_prima_h_ 1

#include <v3_core.h>


typedef enum {
    V3_TOKEN_BooleanLiteral = 1,
    V3_TOKEN_EOF,
    V3_TOKEN_Identifier,
    V3_TOKEN_Keyword,
    V3_TOKEN_NullLiteral,
    V3_TOKEN_NumericLiteral,
    V3_TOKEN_Punctuator,
    V3_TOKEN_StringLiteral,
    V3_TOKEN_RegularExpression
} v3_tokentype_t;

typedef struct {
    v3_tokentype_t  type;
    v3_str_t        value;
    unsigned int    lineNumber;
    unsigned int    lineStart;
    unsigned int    start;
    unsigned int    end;
    union decoded_value {
        v3_str_t        *text;
        int             keyword;
        double          num;
    } v;
} v3_token_t;

struct v3_tokenizer_s {
    v3_str_t        source;
    unsigned int    index;
    unsigned int    lineNumber;
    unsigned int    lineStart;
    v3_token_t      *lookahead;
    v3_options_t    *options;
    v3_vector_pt    tokens;
    const char      *err;
    int             openParenToken;
    int             openCurlyToken;
    unsigned        tokenize:1;
};

typedef struct {
    v3_vector_pt    tokens;   
    v3_tokenizer_t  *tokenizer;
    int             rc;
} v3_output_t;

#define token_value(token) (tokenizer->source.data + token->start)
#define token_value_len(token) (token->end - token->start)
#define token_value_eq(token, str) (token_value_len(token) == sizeof(str) - 1 \
                    && strncasecmp(token_value(token), str, sizeof(str) - 1) == 0)

extern v3_options_t cesDefaultOptions;

extern v3_output_t *v3_tokenize(v3_options_t *options, const char* code, int length);
extern void peek(v3_tokenizer_t *tokenizer);
extern int lex(v3_tokenizer_t *tokenizer, v3_token_t **atoken);
extern int skipComment(v3_tokenizer_t *tokenizer);
extern v3_int_t v3_tokenizer_init(v3_tokenizer_t *tokenizer, v3_options_t *options, const char* code, int length);
#endif // _v3_prima_h_
