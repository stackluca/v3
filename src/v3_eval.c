#include <v3_core.h>
#include "v3_eval.h"
#include "v3_parser.h"
#include "v3_exception.h"
#include <assert.h>

#define V3_EVAL_NODE(node) \
    v3_evaleators[((v3_node_t *)(node))->type](ctx, node)
#define V3_SET_RETURN(_name, _value) \
    ctx->ret.name = (_name); \
    ctx->ret.value = (v3_base_object_t *)(_value)
// static v3_object_t v3_global;
#define v3_get_value v3_ref_get_value

static v3_object_t *
v3_function_build_activation(v3_ctx_t *ctx, v3_function_object_t *func, v3_vector_t *arg_values);
static v3_base_object_t *
v3_function_construct(v3_ctx_t *ctx, v3_function_object_t *func, v3_vector_t *arg_values);
static v3_base_object_t *
v3_function_call(v3_ctx_t *ctx, v3_function_object_t *func, v3_base_object_t *this, v3_vector_t *arg_values);
static v3_vector_t *v3_arguments_eval(v3_ctx_t *ctx, v3_vector_t *params);
static v3_object_t *v3_arguments_object_create(v3_ctx_t *ctx, v3_vector_t *arg_values);

extern v3_int_t v3_init_Function(v3_ctx_t *ctx);
extern v3_int_t v3_init_Object(v3_ctx_t *ctx);
extern v3_int_t v3_init_Number(v3_ctx_t *ctx);

// typedef void (*v3_disp_result_pt)(v3_ctx_t *ctx, v3_base_object_t *value);

v3_eval_pt v3_evaleators[] = {
    NULL, // V3_SYNTAX_ASSIGNMENT_EXPR,
    v3_call_expr_eval, // V3_SYNTAX_CALL_EXPR,
    v3_identifier_eval,//V3_SYNTAX_IDENTIFIER,
    v3_literal_eval,//V3_SYNTAX_LITERAL,
    NULL,//V3_SYNTAX_MEMBER_EXPR,
    v3_program_eval,//V3_SYNTAX_PROGRAM,
    NULL,//V3_SYNTAX_SEQUENCE_EXPR,
    v3_variable_statement_eval, // V3_SYNTAX_VARIABLE_DECLARATION,
    v3_variable_declarator_eval,// V3_SYNTAX_VARIABLE_DECLARATOR,
    v3_expr_statement_eval,     // V3_SYNTAX_EXPR_STATEMENT = 9,   // 
    v3_new_expr_eval,     // V3_SYNTAX_NEW_EXPR = 10,   // 
    v3_block_statement_eval,
    v3_function_declaration_eval, 
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

    ret = v3_program_eval(ctx, (v3_node_t *)program);
    if (ret == NULL) {
        printf("an error occured\n");
        return V3_OK;
    }

    if (ret != (v3_base_object_t *)&v3_null) {
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

v3_base_object_t * 
v3_program_eval(v3_ctx_t *ctx, v3_node_t *anode)
{
    size_t                  i = 0;
    v3_program_node_t       *program;
    v3_node_t               *node, **nodes;
    v3_frame_t              frame;
    v3_base_object_t        *ret;

    program = (v3_program_node_t *)anode;
    frame.global = ctx->global;
    frame.scopes = v3_list_create(ctx->pool);
    if (frame.scopes == NULL) return NULL;

    v3_list_prepend(frame.scopes, ctx->global);

    frame.this   = NULL;
    frame.prev   = NULL;

    nodes = program->body->items;

    for (; i < program->body->length; i++) {
        node = nodes[i];
        // TODO: return statement
        ret = v3_evaleators[node->type](ctx, node);
    }

    return ret;
}

v3_base_object_t *
v3_new_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_new_expr_t           *expr;
    v3_base_object_t        *ret;
    v3_vector_t             *arg_values;
    v3_function_object_t    *func_obj;

    assert(node->type == V3_SYNTAX_NEW_EXPR);

    expr = (v3_new_expr_t *)node;
    assert(expr->callee != NULL);

    ret = v3_evaleators[expr->callee->type](ctx, expr->callee);

    ret = v3_ref_get_value(ctx, ret);
    if (ret == NULL) return NULL;
    if (ret->type != V3_DATA_TYPE_FUNCTION) {
        v3_set_error(ctx, v3_SyntaxError);
        return NULL;
    }
    func_obj = (v3_function_object_t *)ret;

    arg_values = v3_arguments_eval(ctx, expr->arguments);

    return v3_function_construct(ctx, func_obj, arg_values);
}

v3_base_object_t *
v3_variable_statement_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_variable_statement_t     *statement;
    size_t                      i;
    v3_base_object_t            *ret;
    v3_variable_declarator_t    **dec;

    statement = (v3_variable_statement_t *)node;

    // if (statement->declarations == NULL) return NULL;

    dec = statement->declarations->items;

    for (i = 0; i < statement->declarations->length; i++) {
        ret = v3_variable_declarator_eval(ctx, (v3_node_t *)dec[i]);
        if (ret == NULL) return NULL;
    }

    return ret;
}

v3_base_object_t * 
v3_function_declaration_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_function_node_t          *func_node;
    v3_function_object_t        *func_obj;

    func_node = (v3_function_node_t *)node;
    // TODO: push scope if func_node's name is not null 

    func_obj = v3_function_from_node(ctx, func_node, ctx->frame->scopes);
    if (func_obj == NULL) return NULL;

    v3_object_set_by_str((v3_object_t *)ctx->frame->scopes->elts->value, 
                            func_node->id->name.data, 
                            func_node->id->name.length, 
                            (v3_base_object_t *)func_obj);

    return (v3_base_object_t *)func_obj;
}

v3_base_object_t * 
v3_function_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
#if 0
    v3_function_node_t          *func_node;
    v3_function_object_t        *func_obj;
    func_node = (v3_function_node_t *)node;

    func_obj = v3_function_from_node(ctx, func_node, ctx->frame->scopes);
    if (func_obj == NULL) return NULL;

    ctx->ret.name = func_node->id->name;
    ctx->ret.value = (v3_base_object_t *)func_obj;
#endif

    return (v3_base_object_t *)&v3_null;
}

v3_base_object_t * 
v3_variable_declarator_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_variable_declarator_t    *dec;
    v3_base_object_t            *ret, *ref, *value;

    assert(node->type == V3_SYNTAX_VARIABLE_DECLARATOR);

    dec = (v3_variable_declarator_t *)node;

    ref = v3_identifier_eval(ctx, (v3_node_t *)dec->id);    
    if (ref == NULL) return NULL;

    value = v3_evaleators[dec->init->type](ctx, (v3_node_t *)dec->init);
    if (value == NULL) return NULL;

    value = v3_ref_get_value(ctx, value);
    if (value == NULL) return NULL;

    ret = v3_ref_put_value(ctx, ref, value);
    if (ret == NULL) return NULL;

    return (v3_base_object_t *)v3_str2string(&dec->id->name);
}

/** get value of literl */
v3_base_object_t * 
v3_literal_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_literal_t    *literal;

    assert(node->type == V3_SYNTAX_LITERAL);
    literal = (v3_literal_t *)node;

    //ctx->ret.value = literal->value;

    return literal->value;
}

v3_base_object_t *
v3_identifier_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_idetifier_t          *ident;
    v3_base_object_t        *value;
    v3_list_part_t          *part;
    v3_object_t             *scope;
    v3_reference_t          *ref;

    ident = (v3_idetifier_t *)node;

    for (part = ctx->frame->scopes->elts; part != NULL; part = part->next) {
        scope = (v3_object_t *)part;
        value = v3_object_get_by_str(scope, ident->name.data, ident->name.length); 
        if (value != NULL) {
            // ctx->ret.value = v3_ref_create(ctx, ident->name, (v3_base_object_t*)scope);
            // if (ctx->ret.value == NULL) return V3_ERROR;
            ref = v3_ref_create(ctx, v3_str2string(&ident->name), (v3_base_object_t *)scope);
            return (v3_base_object_t *)ref;
        }
    }

    return (v3_base_object_t *)v3_ref_create(ctx, 
                v3_str2string(&ident->name), 
                (v3_base_object_t *)&v3_null);
}

v3_base_object_t * 
v3_expr_statement_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_expr_statement_t         *expr;
    expr = (v3_expr_statement_t *)node;

    return v3_evaleators[expr->expr->type](ctx, node);
}



static v3_vector_t *
v3_arguments_eval(v3_ctx_t *ctx, v3_vector_t *params)
{
    v3_vector_t         *arguments;    
    v3_base_object_t    **item, *ret;
    size_t              i;
    v3_node_t           **node;

    //v3_object_t         *arguments;

    if (params == NULL || params->length == 0) {
        //arguments = v3_object_create(ctx, 5);
        //v3_object_set(arguments, v3_strobj("length"), v3_numobj(0));
        arguments = v3_vector_new(ctx->options, sizeof(void *), 0);
    } else {
        arguments = v3_vector_new(ctx->options, sizeof(void *), params->length);
        //v3_object_set(arguments, v3_strobj("length"), v3_numobj(params->length));
        node = params->items;
        for (i = 0; i< params->length; i++) {
            ret = V3_EVAL_NODE(node[i]);
            item = v3_vector_push(ctx->options, arguments);
            ret = v3_ref_get_value(ctx, ret);
            *item = ret;
        }
    }

    return arguments;
}

v3_base_object_t * 
v3_call_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_call_expr_t          *call_expr;
    v3_base_object_t        *ret, *member, *this;
    v3_function_object_t    *func_obj;
    v3_reference_t          *ref;
    v3_vector_t             *arg_values;

    assert(node->type == V3_SYNTAX_CALL_EXPR);

    call_expr = (v3_call_expr_t *)node;
    // TODO: get this
    member = (V3_EVAL_NODE(call_expr->callee)); // result 1

    ret = v3_get_value(ctx, member);
    if (ret == NULL) return NULL;

    if (ret->type != V3_DATA_TYPE_FUNCTION) {
        v3_set_error(ctx, v3_TypeError); 
        return NULL;
    }

    func_obj = (v3_function_object_t *)ret; // result 3

    // set this
    if (member->type == V3_DATA_TYPE_REF) {
        ref = (v3_reference_t *)member;
        if (ref->scope == (v3_base_object_t *)ctx->frame->call_obj) {
            // foo();
            this = (v3_base_object_t *)&v3_null;
        } else {
            // for a.foo();
            this = ref->scope;
        }
    } else {
        // 1.toString();
        this = (v3_base_object_t *)&v3_null;
    }

    arg_values = v3_arguments_eval(ctx, call_expr->arguments);
    if (arg_values == NULL) return NULL;

    ret = v3_function_call(ctx, func_obj, this, arg_values);

    return ret;
}

v3_base_object_t *
v3_block_statement_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_block_statement_t    *block;
    size_t                  i;
    v3_base_object_t        *ret;
    v3_node_t               **item;

    assert(node->type == V3_SYNTAX_BLOCK_STATEMENT);
    block = (v3_block_statement_t *)node;

    item = block->body->items;
    for (i = 0; i < block->body->length; i++) {
        // node = items[i];
        ret = v3_evaleators[item[i]->type](ctx, item[i]);
        if (item[i]->type == V3_SYNTAX_RETURN_STATEMENT) {
            return ret;
        }
    }

    return ret;
}


#define v3_frame_push(ctx, newFrame) \
    (newFrame)->prev = ctx->frame; \
    ctx->frame = (newFrame);\
    
#define v3_frame_pop(ctx) \
    ctx->frame = ctx->frame->prev;\
    

// entering execution context
static v3_base_object_t *
v3_function_call(v3_ctx_t *ctx, v3_function_object_t *func, v3_base_object_t *this, v3_vector_t *arg_values)
{
    v3_frame_t                  frame;
    v3_object_t                 *activation;
    v3_base_object_t            *ret;

    activation = v3_function_build_activation(ctx, func, arg_values);
    // frame.call_obj = v3_object_clone(func_obj->call_obj);
    // TODO: create argument objct by arguments
    // v3_object_set(frame.call_obj, v3_str2string("arguments"), arg_obj);
    v3_list_prepend(func->scopes, activation);
    frame.scopes = func->scopes;
    frame.call_obj = activation;

    v3_frame_push(ctx, &frame);

    if (func->is_native) {
        ret = func->native_func(ctx);
    } else {
        ret = v3_block_statement_eval(ctx, (v3_node_t *)func->node->body);
    }

    v3_list_pop(func->scopes);

    // restore prev frame;
    v3_frame_pop(ctx);
    return ret;
}

static v3_object_t *
v3_function_build_activation(v3_ctx_t *ctx, v3_function_object_t *func, v3_vector_t *arg_values)
{
    v3_object_t                 *activation;
    v3_variable_declarator_t    **var_decs;
    v3_object_t                 *arg_obj;
    size_t                      i;
    v3_function_node_t          *func_node;
    v3_base_object_t            **values;
    v3_string_object_t          **para_names;

    func_node = func->node;

    activation = v3_object_create(ctx, func_node->params->length 
                                        + func_node->func_decs->length 
                                        + func_node->var_decs->length);
    /*---- set activation  */
    // set argumetns
    arg_obj = v3_arguments_object_create(ctx, arg_values);
    v3_object_set(activation, v3_strobj("arguments"), (v3_base_object_t *)arg_obj); 

    // parameters;
    values = arg_values->items;
    para_names = func_node->params->items;
    for (i = 0; i < func_node->params->length; i++) {
        if (i < arg_values->length) {
            v3_object_set(activation, 
                    para_names[i],
                    values[i]);
        } else {
            v3_object_set(activation, para_names[i], 
                            (v3_base_object_t *)&v3_undefined);
        }
    }

    // func dec
    // TODO: create func
#if 0
    for (i = 0; i < func->func_dec->length; i++) {
        // v3_function_declaration_eval();
    }
#endif

    // var dec
    var_decs = func_node->var_decs->items;
    for (i = 0; func_node->var_decs->length; i++) {
        if (v3_object_get_by_str(activation, 
                    var_decs[i]->id->name.data, 
                    var_decs[i]->id->name.length) != NULL) {
            v3_object_set_by_str(activation, 
                    var_decs[i]->id->name.data, 
                    var_decs[i]->id->name.length, 
                    (v3_base_object_t *)&v3_undefined);
        }
    }

    return activation;
}

static v3_base_object_t *
v3_function_construct(v3_ctx_t *ctx, v3_function_object_t *func, v3_vector_t *arg_values)
{
    v3_object_t     *obj;
    // v3_object_t     *prototype;
    v3_base_object_t    *ret;
    
    obj = v3_object_create(ctx, 20);
    v3_object_set(obj, v3_strobj(INTER_CLASS), (v3_base_object_t *)v3_strobj("Object"));
    #if 0
    prototype = (v3_object_t *)V3_OBJ_GET(obj, "prototype");

    if (prototype != NULL) {
        obj->base.__proto__ = prototype;
    } 
    #endif

    ret = v3_function_call(ctx, func, (v3_base_object_t *)obj, arg_values);
    if (ret->type == V3_DATA_TYPE_OBJECT) {
        return ret;
    } else {
        return (v3_base_object_t *)obj;
    }
}

static v3_object_t *v3_arguments_object_create(v3_ctx_t *ctx, v3_vector_t *arg_values)
{
    // TODO:
    v3_object_t     *arguments;
    arguments = v3_object_create(ctx, 5);
    return arguments;
}

v3_int_t 
v3_reverse_block_do(v3_ctx_t *ctx, v3_block_statement_t * block, v3_node_do_pt handler, void *userdata)
{
    size_t      i;
    v3_node_t       **nodes;

    nodes = block->body->items; 
    for (i = 0; i < block->body->length; i++) {
        if (nodes[i]->type == V3_SYNTAX_BLOCK_STATEMENT) {
            CHECK_FCT(v3_reverse_block_do(ctx, (v3_block_statement_t *)nodes[i], handler, userdata));
        } else {
            CHECK_FCT(handler(ctx, nodes[i], userdata));
        }
    }

    return V3_OK;
}
