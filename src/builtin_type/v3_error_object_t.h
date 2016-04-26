#ifndef _V3_ERROR_OBJECT_H
#define _V3_ERROR_OBJECT_H 


typedef struct {
    v3_base_object_t    *base;
    char*               msg;
} v3_error_object_t;

v3_error_object_t *v3_throw_error(char *msg);

extern char *v3_err_cant_convert_undefined_to_obj;
extern char *v3_err_incompatible_object;
#endif
