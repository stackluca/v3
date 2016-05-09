#ifndef _V3_EVAL_H
#define _V3_EVAL_H 
typedef struct v3_frame_s v3_frame_t;

struct v3_frame_s {
    v3_object_t     *global;
    v3_object_t     *local;
    v3_object_t     *self;
    // v3_object_t     * 
    v3_frame_t      *prev;
    v3_list_t       *scope_chain;
    v3_object_t     *call_obj;
};

extern v3_int_t v3_init_global(v3_ctx_t *ctx);
extern v3_int_t v3_eval(v3_ctx_t *ctx, char *code);

#endif
