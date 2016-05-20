#include <v3_core.h>
#include "v3_eval.h"
#include "v3_parser.h"
#include "v3_exception.h"
#include <assert.h>

#define V3_SET_RETURN(_name, _value) \
    ctx->ret.name = (_name); \
    ctx->ret.value = (v3_base_object_t *)(_value)
// static v3_object_t v3_global;
#define v3_get_value v3_ref_get_value
#define v3_put_value v3_ref_put_value
#define to_result(value) ((v3_statement_result_t *)(value))

static char *to_cstr(char *buf, const char *value, size_t len);
static v3_object_t *
v3_function_build_activation(v3_ctx_t *ctx, v3_function_object_t *func, v3_vector_t *arg_values);
static v3_base_object_t *
v3_function_call(v3_ctx_t *ctx, v3_function_object_t *func, v3_base_object_t *this, v3_vector_t *arg_values);
static v3_vector_t *v3_arguments_eval(v3_ctx_t *ctx, v3_vector_t *params);
static v3_object_t *v3_arguments_object_create(v3_ctx_t *ctx, v3_vector_t *arg_values);
v3_base_object_t *v3_return_statement_eval(v3_ctx_t *ctx, v3_node_t *node);

extern v3_int_t v3_init_Function(v3_ctx_t *ctx);
extern v3_int_t v3_init_Object(v3_ctx_t *ctx);
extern v3_int_t v3_init_Number(v3_ctx_t *ctx);
v3_base_object_t *v3_operator_addition(v3_ctx_t *ctx, v3_node_t *left, v3_node_t *right);
v3_base_object_t *
v3_operator_multi(v3_ctx_t *ctx, v3_node_t *left, v3_node_t *right);

// typedef void (*v3_disp_result_pt)(v3_ctx_t *ctx, v3_base_object_t *value);

v3_eval_pt v3_evaleators[] = {
    v3_assignment_expr_eval, // V3_SYNTAX_ASSIGNMENT_EXPR,
    v3_call_expr_eval, // V3_SYNTAX_CALL_EXPR,
    v3_identifier_eval,//V3_SYNTAX_IDENTIFIER,
    v3_literal_eval,//V3_SYNTAX_LITERAL,
    v3_member_expr_eval,//V3_SYNTAX_MEMBER_EXPR,
    v3_program_eval,//V3_SYNTAX_PROGRAM,
    v3_sequence_expr_eval,//V3_SYNTAX_SEQUENCE_EXPR,
    v3_variable_statement_eval, // V3_SYNTAX_VARIABLE_DECLARATION,
    v3_variable_declarator_eval,// V3_SYNTAX_VARIABLE_DECLARATOR,
    v3_expr_statement_eval,     // V3_SYNTAX_EXPR_STATEMENT = 9,   // 
    v3_new_expr_eval,     // V3_SYNTAX_NEW_EXPR = 10,   // 
    v3_block_statement_eval,
    v3_function_declaration_eval, 
    v3_function_expr_eval, 
    v3_return_statement_eval, 
    v3_binary_expr_eval, 
    v3_this_expr_eval, 
};

    //{assert(((v3_node_t *)(node))->type < sizeof(v3_evaleators) / sizeof(v3_evaleators[0])); 
    //assert(v3_evaleators[((v3_node_t *)(node))->type] != NULL); 

#define V3_EVAL_NODE(node) v3_evaleators[((v3_node_t *)(node))->type](ctx, node) 


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

    CHECK_FCT(v3_object_set(global, v3_strobj("global"), to_base(global)));

    // builtin
    v3_init_Function(ctx);
    v3_init_Object(ctx);
    v3_init_Number(ctx);

    return V3_OK;
}

void v3_show_err(v3_ctx_t *ctx, v3_int_t err_code)
{
    char                    buf[1024];
    if (err_code != 0) {
        printf(" error_code:%d(%s)", err_code, v3_str_error(err_code));
    }

    if (ctx->err != NULL) {
        printf(" %s", ctx->err);
    }

    printf(" index:%d, token:", ctx->tokenizer.index);
    to_cstr(buf, c_lookahead->value.data, c_lookahead->value.length);

    printf("%s \n", buf);
}

v3_int_t v3_eval(v3_ctx_t *ctx, char *code)
{
    v3_int_t            rc;
    v3_program_node_t   *program;
    size_t              len, i;
    v3_base_object_t        *ret;
    char                    buf[1024];
    v3_string_object_t      *str;

    len = strlen(code);
    rc = v3_parse(ctx, code, len, &program);
    if (rc != V3_OK) {
        v3_show_err(ctx, rc);
        ctx->err = NULL;
        ctx->error = NULL;
        return rc;
    }

    ret = v3_program_eval(ctx, (v3_node_t *)program);
    if (ret == NULL) {
        v3_show_err(ctx, 0);
        return V3_OK;
    }

    if (ret->type == V3_DATA_TYPE_STATMENT_RESULT
        || ret->type == V3_DATA_TYPE_UNDEFINED) {
        printf("undefined\n");
    } else if (ret != (v3_base_object_t *)&v3_null) {
        ret = v3_get_value(ctx, ret);
        if (ret == NULL) {
            v3_show_err(ctx, 0);
            return V3_OK;
        } else if (ret->type == V3_DATA_TYPE_FUNCTION) { 
            printf("function ");
            to_cstr(buf, to_func(ret)->name->value.data, to_func(ret)->name->value.length);
            printf("%s()\n", buf);
            return V3_OK;
        } else if(ret->type == V3_DATA_TYPE_OBJECT) {
            v3_vector_t *keys;
            v3_str_t    **key;
            keys = v3_dict_keys(ctx, to_obj(ret)->__attrs__);
            key = keys->items;
            for (i = 0; i < keys->length; i++) {
                to_cstr(buf, key[i]->data, key[i]->length);
                printf("%s\n", buf);
            }
        } else {
            str = v3_to_string(ctx, ret);
            if (str == NULL) {
                v3_show_err(ctx, 0);
            } else {
                printf("%s\n", to_string(str)->value.data);
            }
        }
    } else {
        printf("undefined\n");
    }

    return V3_OK;
}

static char *to_cstr(char *buf, const char *value, size_t len)
{
    memcpy(buf, value, len);
    buf[len] = '\0';
    return buf + len;
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

    frame.this   = to_base(&v3_null);
    frame.prev   = NULL;
    ctx->frame  = &frame;

    nodes = program->body->items;

    for (; i < program->body->length; i++) {
        node = nodes[i];
        // TODO: return statement
        ret = v3_evaleators[node->type](ctx, node);
    }

    return ret;
}

v3_base_object_t *
v3_assignment_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_base_object_t        *ret, *left_ret, *right_ret, *value;
    v3_assignment_expr_t    *assign_expr;

    assert(node->type == V3_SYNTAX_ASSIGNMENT_EXPR);

    assign_expr = (v3_assignment_expr_t *)node;
    left_ret = V3_EVAL_NODE(assign_expr->left);
    if (left_ret == NULL) return NULL;

    right_ret = V3_EVAL_NODE(assign_expr->right);
    if (right_ret == NULL) return NULL;

    value = v3_get_value(ctx, right_ret);
    if (value == NULL) return NULL;

    ret = v3_put_value(ctx, left_ret, value);
    if (ret == NULL) return NULL;

    return value;
}

v3_base_object_t *
v3_sequence_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    assert("not support sequence expr" == NULL);
    return NULL;
}

v3_base_object_t *
v3_member_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_member_expr_t        *expr;
    v3_base_object_t        *ret, *object, *property;
    v3_object_t             *scope;
    v3_string_object_t      *name;
    v3_idetifier_t          *id;

    assert(node->type == V3_SYNTAX_MEMBER_EXPR);
    expr = (v3_member_expr_t *)node;

    ret = V3_EVAL_NODE(expr->object);
    if (ret == NULL) return NULL;

    object = v3_get_value(ctx, ret);
    if (object == NULL) return NULL;

    if (expr->computed) {
        ret = V3_EVAL_NODE(expr->property);
        if (ret == NULL) return NULL;

        property = v3_get_value(ctx, ret);
        if (property == NULL) return NULL;

        name = v3_to_string(ctx, property);
        if (name == NULL) return NULL;

    } else {
        id = (v3_idetifier_t *)expr->property;
        name = v3_str2string(&id->name);
    }

    scope = v3_to_object(ctx, object);
    if (scope == NULL) return NULL;

    return to_base(v3_ref_create(ctx, name, to_base(scope)));
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
    if (ret == NULL) return NULL;

    ret = v3_ref_get_value(ctx, ret);
    if (ret == NULL) return NULL;

    if (ret->type != V3_DATA_TYPE_FUNCTION) {
        v3_set_error(ctx, v3_SyntaxError, "new exprssion need a function");
        return NULL;
    }
    func_obj = (v3_function_object_t *)ret;

    arg_values = v3_arguments_eval(ctx, expr->arguments);
    if (arg_values == NULL) return NULL;

    return v3_function_construct(ctx, func_obj, arg_values);
}

v3_base_object_t *
v3_variable_statement_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_variable_statement_t     *statement;
    size_t                      i;
    v3_base_object_t            *ret;
    v3_variable_declarator_t    **dec;
    v3_statement_result_t       *result;

    statement = (v3_variable_statement_t *)node;

    // if (statement->declarations == NULL) return NULL;

    dec = statement->declarations->items;

    for (i = 0; i < statement->declarations->length; i++) {
        ret = v3_variable_declarator_eval(ctx, (v3_node_t *)dec[i]);
        if (ret == NULL) return NULL;
    }

    result = v3_statement_result_create(ctx, V3_RESULT_NORMAL, NULL, NULL);
    return to_base(result);
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

    return (v3_base_object_t *)v3_statement_result_create(ctx, V3_RESULT_NORMAL, NULL, NULL);
}

v3_base_object_t * 
v3_function_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_function_node_t          *func_node;
    v3_function_object_t        *func_obj;
    func_node = (v3_function_node_t *)node;

    if (func_node->id != NULL) { 
        // TODO: add identifier to scope 
    }

    func_obj = v3_function_from_node(ctx, func_node, ctx->frame->scopes);
    if (func_obj == NULL) return NULL;

    return to_base(func_obj);
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
        scope = (v3_object_t *)part->value;
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
            if (ret == NULL) return NULL;
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
    if (member == NULL) return NULL;

    ret = v3_get_value(ctx, member);
    if (ret == NULL) return NULL;

    if (ret->type != V3_DATA_TYPE_FUNCTION) {
        v3_set_error(ctx, v3_TypeError, "call exprssion need a function"); 
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
v3_return_statement_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_return_statement_t    *return_stmt;
    v3_base_object_t        *ret;
    v3_statement_result_t   *result;

    assert(node->type == V3_SYNTAX_RETURN_STATEMENT);
    return_stmt = (v3_return_statement_t *)node;
    result = v3_statement_result_create(ctx, V3_RESULT_RETURN, NULL, NULL);

    if (return_stmt->argument == NULL) {
        result->value = to_base(&v3_undefined);     
        return to_base(result);
    }

    assert(is_expr(return_stmt->argument));

    ret = v3_evaleators[return_stmt->argument->type](ctx, return_stmt->argument);
    if (ret == NULL) return NULL;
    result->value = v3_get_value(ctx, ret);

    return to_base(result);
}

v3_base_object_t *
v3_block_statement_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_block_statement_t    *block;
    size_t                  i;
    v3_base_object_t        *ret;
    v3_node_t               **item;
    v3_statement_result_t   *result;

    assert(node->type == V3_SYNTAX_BLOCK_STATEMENT);
    block = (v3_block_statement_t *)node;
    result = v3_statement_result_create(ctx, V3_RESULT_NORMAL, NULL, NULL);

    item = block->body->items;

    for (i = 0; i < block->body->length; i++) {
        // node = items[i];
        ret = V3_EVAL_NODE(item[i]);
        if (ret == NULL) {
            result->type = V3_RESULT_THROW;
            result->value = to_base(ctx->error);
        }

        if (ret->type == V3_DATA_TYPE_STATMENT_RESULT
            && to_result(ret)->type == V3_RESULT_RETURN) {
            return ret; 
        }

        if (item[i]->type == V3_SYNTAX_RETURN_STATEMENT) {
            return ret;
        }
    }

    return to_base(result);
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
    v3_statement_result_t       *result;

    activation = v3_function_build_activation(ctx, func, arg_values);

    if (func->is_native) {
        ctx->frame->this = this;
        ctx->frame->call_obj = activation;
        ret = func->native_func(ctx);
        return ret;
    } else {
        frame.this = this;
        frame.call_obj = activation;
        // frame.call_obj = v3_object_clone(func_obj->call_obj);
        // TODO: create argument objct by arguments
        v3_list_prepend(func->scopes, activation);
        frame.scopes = func->scopes;

        v3_frame_push(ctx, &frame);
        result  = (v3_statement_result_t *)v3_block_statement_eval(ctx, (v3_node_t *)func->node->body);
        v3_list_pop(func->scopes);

        // restore prev frame;
        v3_frame_pop(ctx);
    }

    if (result->type == V3_RESULT_THROW) {
        return NULL;
    }

    if (result->type == V3_RESULT_RETURN) {
        return result->value;
    }

    assert(result->type == V3_RESULT_NORMAL);
    return to_base(&v3_undefined);
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
    v3_idetifier_t              **ids;

    func_node = func->node;

    // activation = v3_object_create(ctx, func_node->params->length 
                                        //+ func_node->func_decs->length 
                                        //+ func_node->var_decs->length);
    activation = v3_object_create(ctx, 10);

    /*---- set activation  */
    // set argumetns
    arg_obj = v3_arguments_object_create(ctx, arg_values);
    v3_object_set(activation, v3_strobj("arguments"), (v3_base_object_t *)arg_obj); 
    if (func->is_native) {
        return activation;
    }

    // parameters;
    values = arg_values->items;
    ids = func_node->params->items;
    for (i = 0; i < func_node->params->length; i++) {
        if (i < arg_values->length) {
            //assert(ids[i]->type == V3_SYNTAX_IDENTIFIER);
            v3_object_set_by_str(activation, 
                            ids[i]->name.data,
                            ids[i]->name.length,
                            values[i]);
        } else {
            v3_object_set_by_str(activation,  
                            ids[i]->name.data,
                            ids[i]->name.length,
                            to_base(&v3_undefined));
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

v3_base_object_t *
v3_function_construct(v3_ctx_t *ctx, v3_function_object_t *func, v3_vector_t *arg_values)
{
    v3_object_t     *obj;
    v3_base_object_t    *ret;
    
    obj = v3_object_create(ctx, 20);
    v3_object_set(obj, v3_strobj(INTER_CLASS), (v3_base_object_t *)v3_strobj("Object"));

    ret = v3_object_get(to_obj(func), v3_strobj("prototype"));

    if (ret->type == V3_DATA_TYPE_OBJECT) {
        obj->base.__proto__ = to_obj(ret);
    }

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
    v3_int_t        i;
    v3_base_object_t **arg;
    
    arguments = v3_object_create(ctx, 5);
    
    v3_object_set(arguments, v3_strobj("length"), to_base(v3_numobj(arg_values->length)));
    
    if (arg_values != NULL) {
        arg = arg_values->items;
        for (i = 0; i < arg_values->length; i++) {
            v3_object_set(arguments, v3_to_string(ctx, to_base(v3_numobj(i))), *arg);
        }
    }
    
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

v3_statement_result_t *v3_statement_result_create(v3_ctx_t *ctx, int type, v3_base_object_t *value, v3_base_object_t *label)
{
    v3_statement_result_t *result;

    result = v3_palloc(ctx->pool, sizeof(*result));
    result->base.type = V3_DATA_TYPE_STATMENT_RESULT;
    result->base.ref_count = 0;
    result->base.__proto__ = NULL;

    result->type = type;
    result->value = value;
    result->label = label;
    return result;
}

v3_base_object_t *
v3_this_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    return ctx->frame->this;
}

v3_base_object_t *
v3_binary_expr_eval(v3_ctx_t *ctx, v3_node_t *node)
{
    v3_binary_expr_t    *expr;
    assert(node->type == V3_SYNTAX_BINARY_EXPR);
    expr = (v3_binary_expr_t *)node;


    if (v3_str_equal(&expr->_operator, "+")) {
        return v3_operator_addition(ctx, expr->left, expr->right);         
    } else if (v3_str_equal(&expr->_operator, "*")) {
        return v3_operator_multi(ctx, expr->left, expr->right);         
    } else {
        v3_set_error(ctx, v3_SyntaxError, "not support operator");
        return NULL;
    }
}

#define CHECK_FCNULL(__call__, __left__)\
    __left__ = __call__; \
    if (__left__ == NULL) return NULL;

v3_base_object_t *
v3_operator_multi(v3_ctx_t *ctx, v3_node_t *left, v3_node_t *right)
{
    v3_base_object_t    *ret, *left_result, *right_result;
    v3_number_object_t  *left_number, *right_number;

    assert(is_expr(left) && is_expr(right));
    
    // get left value;
    ret = V3_EVAL_NODE(left);
    if (ret == NULL) return NULL;

    left_result = v3_get_value(ctx, ret);
    if (left_result == NULL) return NULL;

    left_result = v3_to_primitive(ctx, left_result);
    if (left_result == NULL) return NULL;


    ret = V3_EVAL_NODE(right);  
    if (ret == NULL) return NULL;

    right_result = v3_get_value(ctx, ret);
    if (right_result == NULL) return NULL;

    right_result = v3_to_primitive(ctx, right_result);
    if (right_result == NULL) return NULL;


    if (left_result->type == V3_DATA_TYPE_STRING
        || right_result->type == V3_DATA_TYPE_STRING) {
        v3_set_error(ctx, v3_SyntaxError, "not support string multi");
        return NULL;
    }

    left_number = v3_to_number(ctx, left_result);    
    right_number = v3_to_number(ctx, right_result);    
    
    return to_base(v3_numobj(left_number->value * right_number->value));
}

v3_base_object_t *
v3_operator_addition(v3_ctx_t *ctx, v3_node_t *left, v3_node_t *right)
{
    v3_base_object_t    *ret, *left_result, *right_result;
    v3_number_object_t  *left_number, *right_number;

    assert(is_expr(left) && is_expr(right));
    
    // get left value;
    ret = V3_EVAL_NODE(left);
    if (ret == NULL) return NULL;

    left_result = v3_get_value(ctx, ret);
    if (left_result == NULL) return NULL;

    left_result = v3_to_primitive(ctx, left_result);
    if (left_result == NULL) return NULL;


    ret = V3_EVAL_NODE(right);  
    if (ret == NULL) return NULL;

    right_result = v3_get_value(ctx, ret);
    if (right_result == NULL) return NULL;

    right_result = v3_to_primitive(ctx, right_result);
    if (right_result == NULL) return NULL;


    if (left_result->type == V3_DATA_TYPE_STRING
        || right_result->type == V3_DATA_TYPE_STRING) {
        v3_set_error(ctx, v3_SyntaxError, "not support string addition");
        return NULL;
    }

    left_number = v3_to_number(ctx, left_result);    
    right_number = v3_to_number(ctx, right_result);    
    
    return to_base(v3_numobj(left_number->value + right_number->value));
}
