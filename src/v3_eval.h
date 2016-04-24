#ifndef _V3_PARSER_H
#define _V3_PARSER_H 
typedef struct v3_frame_s v3_frame_t;

struct v3_frame_s {
    v3_object_t     *globals;
    v3_object_t     *locals;
    v3_object_t     *self;
    // v3_object_t     * 
    v3_frame_t      *prev;
};

extern void v3_init(v3_ctx_t *ctx);
extern v3_int_t v3_eval(v3_ctx_t *ctx, char *code);

#endif
