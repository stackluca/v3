#include <string.h>
#include <strings.h>
#include <assert.h>
#include <v3_core.h>
#include "v3_tokenizer.h"

static v3_token_t *advance(v3_tokenizer_t *tokenizer);
// static int collectToken(v3_tokenizer_t *tokenizer);
static int isDecimalDigit(char ch);
static int isHexDigit(char ch); 
// static int isWhiteSpace(char ch);
static int isWhiteSpace(int ch);
static int isOctalDigit(char ch);
static int isIdentifierPart(char ch);
// static int isImplicitOctalLiteral(v3_tokenizer_t *tokenizer);
static int isLineTerminator(char ch);
static void skipSingleLineComment(v3_tokenizer_t *tokenizer, unsigned int offset);
static int  skipMultiLineComment(v3_tokenizer_t *tokenizer, unsigned int offset);
static v3_str_t *getEscapedIdentifier(v3_tokenizer_t *tokenizer);
static v3_str_t *getIdentifier(v3_tokenizer_t *tokenizer);
static v3_token_t * scanOctalLiteral(v3_tokenizer_t *tokenizer, unsigned int start);
static v3_token_t *scanPunctuator(v3_tokenizer_t *tokenizer);
static v3_token_t *scanIdentifier(v3_tokenizer_t *tokenizer);
static v3_token_t *scanNumericLiteral(v3_tokenizer_t *tokenizer);
static v3_token_t *scanHexLiteral(v3_tokenizer_t *tokenizer, unsigned int start);
static v3_token_t *v3_token_create(v3_tokenizer_t *tokenizer, int start, v3_tokentype_t type);
static v3_token_t *scanStringLiteral(v3_tokenizer_t *tokenizer);
static v3_token_t *advanceSlash(v3_tokenizer_t *tokenizer);
static v3_token_t *collectRegex(v3_tokenizer_t *tokenizer);
static v3_token_t *scanRegExp(v3_tokenizer_t *tokenizer);
static int scanRegExpBody(v3_tokenizer_t *tokenizer);
static void scanRegExpFlags(v3_tokenizer_t *tokenizer);
// static v3_token_t *scanBinaryLiteral(v3_tokenizer_t *tokenizer, unsigned int start);
#if 0
static char* TokenName[] = {
    NULL,
    "Boolean",
    "<end>",
    "Identifier", // 标识符，比如变量名，方法名
    "Keyword", // javascript的关键字
    "Null",
    "Numeric",
    "Punctuator", // 操作符
    "String",
    "RegularExpression"
};
#endif


static v3_str_t FnExprTokens[] = {
                    v3_string("("), v3_string("["), 
                    v3_string("in"), 
                    v3_string("typeof"), v3_string("instanceof"), 
                    v3_string("new"),
                    v3_string("return"), v3_string("case"), v3_string("delete"), 
                    v3_string("throw"), v3_string("void"),
                    // assignment operators
                    v3_string("="), v3_string("+="), v3_string("-="), v3_string("*="), 
                    v3_string("/="), v3_string("%="), 
                    v3_string("<<="), v3_string(">>="), v3_string(">>>="),
                    v3_string("&="), v3_string("|="), v3_string("^="), 
                    v3_string(","),
                    // binary/unary operators
                    v3_string("+"), v3_string("-"), v3_string("*"), v3_string("/"), v3_string("%"), 
                    v3_string("++"), v3_string("--"), 
                    v3_string("<<"), v3_string(">>"), v3_string(">>>"), 
                    v3_string("&"), v3_string("|"), v3_string("^"), v3_string("!"), v3_string("~"), 
                    v3_string("&&"), v3_string("||"), 
                    v3_string("?"), v3_string(":"), 
                    v3_string("==="), v3_string("=="), 
                    v3_string(">="), v3_string("<="), 
                    v3_string("<"), v3_string(">"), 
                    v3_string("!="), v3_string("!==")
            };

// static const char *Msg_UnexpectedToken = "Unexpected token %0";
static const char *Msg_UnexpectedToken_ILLEGAL = "Unexpected token ILLEGAL";
// static const char *Msg_UnexpectedNumber = "Unexpected number";
// static const char *Msg_UnexpectedString = "Unexpected string";
// static const char *Msg_UnexpectedIdentifier = "Unexpected identifier";
// static const char *Msg_UnexpectedReserved = "Unexpected reserved word";
// static const char *Msg_UnexpectedEOS = "Unexpected end of input";
////  static const char *Msg_NewlineAfterThrow = "Illegal newline after throw";
// static const char *InvalidRegExp = "Invalid regular expression";
static const char *Msg_UnterminatedRegExp = "Invalid regular expression: missing /";
const char *Msg_InvalidLHSInAssignment = "Invalid left-hand side in assignment";
// static const char *Msg_InvalidLHSInForIn = "Invalid left-hand side in for-in";
// static const char *Msg_MultipleDefaultsInSwitch = "More than one default clause in switch statement";
// static const char *Msg_NoCatchOrFinally = "Missing catch or finally after try";
// static const char *Msg_UnknownLabel = "Undefined label '%0'";
// static const char *Msg_Redeclaration = "%0 '%1' has already been declared";
// static const char *Msg_IllegalContinue = "Illegal continue statement";
// static const char *Msg_IllegalBreak = "Illegal break statement";
// static const char *Msg_IllegalReturn = "Illegal return statement";
// static const char *Msg_StrictModeWith = "Strict mode code may not include a with statement";
// static const char *Msg_StrictCatchVariable = "Catch variable may not be eval or arguments in strict mode";
// static const char *Msg_StrictVarName = "Variable name may not be eval or arguments in strict mode";
// static const char *Msg_StrictParamName = "Parameter name eval or arguments is not allowed in strict mode";
// static const char *Msg_StrictParamDupe = "Strict mode function may not have duplicate parameter names";
// static const char *Msg_StrictFunctionName = "Function name may not be eval or arguments in strict mode";
// static const char *Msg_StrictOctalLiteral = "Octal literals are not allowed in strict mode.";
// static const char *Msg_StrictDelete = "Delete of an unqualified identifier in strict mode.";
// static const char *Msg_StrictDuplicateProperty = "Duplicate data property in object literal not allowed in strict mode";
// static const char *Msg_AccessorDataProperty = "Object literal may not have data and accessor property with the same name";
// static const char *Msg_AccessorGetSet = "Object literal may not have multiple get/set accessors with the same name";
// static const char *Msg_StrictLHSAssignment = "Assignment to eval or arguments is not allowed in strict mode";
// static const char *Msg_StrictLHSPostfix = "Postfix increment/decrement may not have eval or arguments operand in strict mode";
// static const char *Msg_StrictLHSPrefix = "Prefix increment/decrement may not have eval or arguments operand in strict mode";
// static const char *Msg_StrictReservedWord = "Use of future reserved word in strict mode";

void *std_alloc_wrapper(void *userdata, size_t size)
{
    return malloc(size);
}

void std_dealloc_wrapper(void *userdata, void* ptr)
{
    free(ptr);
}

v3_options_t cesDefaultOptions = {
    &std_alloc_wrapper,
    &std_dealloc_wrapper,
    NULL
};

v3_int_t v3_tokenizer_init(v3_tokenizer_t *tokenizer, v3_options_t *options, const char* code, int length)
{
    memset(tokenizer, 0, sizeof(v3_tokenizer_t));
    tokenizer->source.data = code;
    tokenizer->source.length = length;
    tokenizer->lineNumber = 0;
    tokenizer->lineStart = 0;
    tokenizer->lookahead = NULL;
    tokenizer->options = options;
    tokenizer->tokenize = V3_TRUE;
    // The following two fields are necessary to compute the Regex tokens.
    tokenizer->openParenToken = -1;
    tokenizer->openCurlyToken = -1;
    tokenizer->tokens = v3_vector_new(options, sizeof(v3_token_t), (size_t)100);
    if (tokenizer->tokens == NULL) {
        return V3_ERROR;
    }
    return V3_OK;
}

v3_output_t *v3_tokenize(v3_options_t *options, const char* code, int length)
{
    int     rc;
    // v3_token_t *token;
    v3_output_t *output = options->alloc(options->userdata, sizeof(*output));
    if (output == NULL) {
        return NULL;
    }

    v3_tokenizer_t *tokenizer = options->alloc(options->userdata, sizeof(v3_tokenizer_t)); 
    if(tokenizer == NULL) {
        output->rc = V3_ERROR;
        return output ;
    }

    if (v3_tokenizer_init(tokenizer, options, code, length) != V3_OK) {
        return output;
    }

    output->tokens = tokenizer->tokens;
    output->tokenizer = tokenizer;
    
    peek(tokenizer);
    if (tokenizer->lookahead == NULL) {
        return output;
    }

    if(tokenizer->lookahead->type == V3_TOKEN_EOF) {
        return output;
    }
    
    rc = lex(tokenizer, NULL);
    if (rc == V3_ERROR) {
        return output;
    }

    while (tokenizer->lookahead->type != V3_TOKEN_EOF) {
        rc = lex(tokenizer, NULL);
        if (rc == V3_ERROR) {
            return output;
        }
    }

    return output;
}

int lex(v3_tokenizer_t *tokenizer, v3_token_t **atoken) 
{
    v3_token_t token;
    token = *tokenizer->lookahead;
    if (atoken != NULL) {
        *atoken = tokenizer->lookahead;
    }
    tokenizer->index = token.end;
    tokenizer->lineNumber = token.lineNumber;
    tokenizer->lineStart = token.lineStart;

    tokenizer->lookahead = advance(tokenizer);
    if (tokenizer->lookahead == NULL) return V3_ERROR;

    tokenizer->index = token.end;
    tokenizer->lineNumber = token.lineNumber;
    tokenizer->lineStart = token.lineStart;

    return V3_OK;
}

void peek(v3_tokenizer_t *tokenizer)
{
    unsigned int pos, line, start;

    // save old value
    pos =  tokenizer->index;
    line = tokenizer->lineNumber;
    start = tokenizer->lineStart;

    tokenizer->lookahead = advance(tokenizer);
    // restore    
    tokenizer->index = pos;
    tokenizer->lineNumber = line;
    tokenizer->lineStart = start;
}

#if 0
static int collectToken(v3_tokenizer_t *tokenizer)
{
    int rc;
    rc = skipComment(tokenizer);    
    if (rc != V3_OK) return rc;
    v3_token_t *token = (v3_token_t *)v3_vector_push(tokenizer->options, tokenizer->tokens);
    rc = advance(tokenizer);
    
    if(rc != V3_OK) {
        return rc;
    }
    
    tokenizer->lookahead = token;
    
    return V3_OK;
}
#endif

static int isKeyword(v3_str_t *id, int *keyword) 
{
/*
// TODO:
    if (strict && isStrictModeReservedWord(id)) {
        return true;
    }
*/
    // 'const' is specialized as Keyword in V8.
    // 'yield' and 'let' are for compatibility with SpiderMonkey and ES.next.
    // Some others are from future reserved words.

    switch (id->length) {
    case 2:
        return (!strncmp(id->data, "if", 2) && (*keyword = V3_KEYWORD_IF)) 
            || (!strncmp(id->data, "in", 2) && (*keyword = V3_KEYWORD_IN))
            || (!strncmp(id->data, "do", 2) && (*keyword = V3_KEYWORD_DO));
    case 3:
        return (!strncmp(id->data, "var", 3)&& (*keyword = V3_KEYWORD_VAR))
            || (!strncmp(id->data, "for", 3)&& (*keyword = V3_KEYWORD_FOR))
            || (!strncmp(id->data, "new", 3)&& (*keyword = V3_KEYWORD_NEW))
            || (!strncmp(id->data, "try", 3)&& (*keyword = V3_KEYWORD_TRY))
            || (!strncmp(id->data, "let", 3)&& (*keyword = V3_KEYWORD_LET));
    case 4:
        return (!strncmp(id->data, "this", 4)&& (*keyword = V3_KEYWORD_THIS))
            || (!strncmp(id->data, "else", 4)&& (*keyword = V3_KEYWORD_ELSE))
            || (!strncmp(id->data, "case", 4)&& (*keyword = V3_KEYWORD_CASE))
            || (!strncmp(id->data, "void", 4)&& (*keyword = V3_KEYWORD_VOID))
            || (!strncmp(id->data, "with", 4)&& (*keyword = V3_KEYWORD_WITH))
            || (!strncmp(id->data, "enum", 4)&& (*keyword = V3_KEYWORD_ENUM));
    case 5:
        return (!strncmp(id->data, "while", 5)&& (*keyword = V3_KEYWORD_WHILE))
            || (!strncmp(id->data, "break", 5)&& (*keyword = V3_KEYWORD_BREAK))
            || (!strncmp(id->data, "catch", 5)&& (*keyword = V3_KEYWORD_CATCH))
            || (!strncmp(id->data, "throw", 5)&& (*keyword = V3_KEYWORD_THROW))
            || (!strncmp(id->data, "const", 5)&& (*keyword = V3_KEYWORD_CONST))
            || (!strncmp(id->data, "yield", 5)&& (*keyword = V3_KEYWORD_YIELD))
            || (!strncmp(id->data, "class", 5)&& (*keyword = V3_KEYWORD_CLASS))
            || (!strncmp(id->data, "super", 5)&& (*keyword = V3_KEYWORD_SUPER));
    case 6:
        return (!strncmp(id->data, "return", 6)&& (*keyword = V3_KEYWORD_RETURN))
            || (!strncmp(id->data, "typeof", 6)&& (*keyword = V3_KEYWORD_TYPEOF))
            || (!strncmp(id->data, "delete", 6)&& (*keyword = V3_KEYWORD_DELETE))
            || (!strncmp(id->data, "switch", 6)&& (*keyword = V3_KEYWORD_SWITCH))
            || (!strncmp(id->data, "export", 6)&& (*keyword = V3_KEYWORD_EXPORT))
            || (!strncmp(id->data, "import", 6)&& (*keyword = V3_KEYWORD_IMPORT));
    case 7:
        return (!strncmp(id->data, "default", 7)&& (*keyword = V3_KEYWORD_DEFAULT))
            || (!strncmp(id->data, "finally", 7)&& (*keyword = V3_KEYWORD_FINALLY))
            || (!strncmp(id->data, "extends", 7)&& (*keyword = V3_KEYWORD_EXTENDS));
    case 8:
        return (!strncmp(id->data, "function", 8)&& (*keyword = V3_KEYWORD_FUNCTION))
            || (!strncmp(id->data, "continue", 8)&& (*keyword = V3_KEYWORD_CONTINUE))
            || (!strncmp(id->data, "debugger", 8)&& (*keyword = V3_KEYWORD_DEBUGGER));
    case 10:
        return (!strncmp(id->data, "instanceof", 10)&& (*keyword = V3_KEYWORD_INTANCEOF));
    default:
        return 0;
    }
}

static v3_token_t *
advance(v3_tokenizer_t *tokenizer)
{
    char      ch;
    int         rc;

    rc = skipComment(tokenizer);
    if (rc == V3_ERROR) return NULL;

    if (tokenizer->index >= tokenizer->source.length) {
        return v3_token_create(tokenizer, tokenizer->index, V3_TOKEN_EOF);
    }

    ch = CURRENT_CHAR;
    if(isIdentifierStart(ch)) {
        return scanIdentifier(tokenizer);
    }

    if (ch == '(' || ch == ')' || ch == ';') {
        return scanPunctuator(tokenizer);
    }

    if (ch == '\'' || ch == '"') {
        return scanStringLiteral(tokenizer);
    }

    // Dot (.) U+002E can also start a floating-point number, hence the need
    // to check the next character.
    if (ch == 0x2E) {
        if (isDecimalDigit(NEXT_CHAR)) {
            return scanNumericLiteral(tokenizer);
        }
        return scanPunctuator(tokenizer);
    }

    if (isDecimalDigit(ch)) {
        return scanNumericLiteral(tokenizer);
    }

    // Slash (/) U+002F can also start a regex.
    if (tokenizer->tokenize && ch == 0x2F) {
        return advanceSlash(tokenizer);
    }

    return scanPunctuator(tokenizer);
}

static v3_token_t *
scanHexLiteral(v3_tokenizer_t *tokenizer, unsigned int start) 
{
    while (tokenizer->index < tokenizer->source.length) {
        if (!isHexDigit(CURRENT_CHAR)) {
            break;
        }

        tokenizer->index++;
    }

    if (tokenizer->index == start) {
        tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
        return NULL;
    }

    if (isIdentifierStart(CURRENT_CHAR)) {
        tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
        return NULL;
    }

    return v3_token_create(tokenizer, start, V3_TOKEN_NumericLiteral);
}
#if 0
static v3_token_t * 
scanBinaryLiteral(v3_tokenizer_t *tokenizer, unsigned int start) 
{
    char    ch;
    v3_str_t *number = v3_alloc(tokenizer->options, sizeof(*number)); 
    if (number == NULL) {
        return V3_ERROR;
    }
    // 0b
    number->data = tokenizer->source.data + start;
    number->length = 2;

    while (tokenizer->index < tokenizer->source.length) {
        ch = CURRENT_CHAR;
        if (ch != '0' && ch != '1') {
            break;
        }

        number->length++;
        tokenizer->index++;
    }

    if (number->length == 2) {
        // only 0b or 0B
        return V3_ERROR;
    }

    if (tokenizer->index < tokenizer->source.length) {
        ch = CURRENT_CHAR;
        /* istanbul ignore else */
        if (isIdentifierStart(ch) || isDecimalDigit(ch)) {
            return V3_ERROR;
        }
    }

    return v3_token_create(tokenizer, start, V3_TOKEN_NumericLiteral);
}
#endif

static v3_token_t *
scanOctalLiteral(v3_tokenizer_t *tokenizer, unsigned int start) 
{
    // int     octal;
    // ch = CURRENT_CHAR;
#if 0
    if (isOctalDigit(prefix)) {
        octal = 1;
        //number = '0' + source[index++];
    //    number->length++;
        tokenizer->index++;
    } else {
        octal = 0;
        tokenizer->index++;
    }
#endif

    while (tokenizer->index < tokenizer->source.length) {
        if (!isOctalDigit(CURRENT_CHAR)) {
            break;
        }
     //   number->length++;
        tokenizer->index++;
    }
#if 0
    if (!octal && number->length == 2) {
        // only 0o or 0O
        return V3_ERROR;
    }
#endif

    if (isIdentifierStart(CURRENT_CHAR) 
        || isDecimalDigit(CURRENT_CHAR)) {
        tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
        return NULL;
    }

    return v3_token_create(tokenizer, start, V3_TOKEN_NumericLiteral);
}

#if 0
static int isImplicitOctalLiteral(v3_tokenizer_t *tokenizer) {
    unsigned int i;
    char ch;
    // Implicit octal, unless there is a non-octal digit.
    // (Annex B.1.1 on Numeric Literals)
    for (i = tokenizer->index + 1; i < tokenizer->source.length; ++i) {
        ch = tokenizer->source.data[i];
        if (ch == '8' || ch == '9') {
            return 0;
        }
        if (!isOctalDigit(ch)) {
            return 1;
        }
    }

    return 1;
}
#endif

static v3_token_t *
scanNumericLiteral(v3_tokenizer_t *tokenizer) 
{
    unsigned int    start, num_index;
    char            ch, number[20];
    v3_token_t      *token;

    ch = CURRENT_CHAR;
    assert(isDecimalDigit(ch) || (ch == '.'));
    num_index = 0;
    start = tokenizer->index;
    if (ch != '.') {
        number[num_index++] = CURRENT_CHAR;
        // num = (char *)tokenizer->source.data + tokenizer->index;
        tokenizer->index++;
        ch = CURRENT_CHAR;

        // Hex number starts with '0x'.
        // Octal number starts with '0'.
        // Octal number in ES6 starts with '0o'.
        // Binary number in ES6 starts with '0b'.
        if (number[0] == '0') {
            if (ch == 'x' || ch == 'X') {
                tokenizer->index++;
                return scanHexLiteral(tokenizer, start);
            }
#if 0
            if (ch == 'b' || ch == 'B') {
                tokenizer->index++;
                return scanBinaryLiteral(tokenizer, start);
            }
            if (ch == 'o' || ch == 'O') {
                return scanOctalLiteral(tokenizer, ch, start);
            }
            if (isOctalDigit(ch)) {
                if (isImplicitOctalLiteral(tokenizer)) {
                    return scanOctalLiteral(tokenizer, ch, start);
                }
            }
#endif

            if (isOctalDigit(ch)) {
                return scanOctalLiteral(tokenizer, start);
            }

            // decimal number starts with '0' such as '09' is illegal.
            if (ch && isDecimalDigit(ch)) {
                tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
                return NULL;
            }
        }

        while (isDecimalDigit(CURRENT_CHAR)) {
            // number->length++;
            number[num_index++] = CURRENT_CHAR;
            tokenizer->index++;
        }
        ch = CURRENT_CHAR;
    }
    // .1234
    if (ch == '.') {
        tokenizer->index++;
        while (isDecimalDigit(CURRENT_CHAR)) {
            number[num_index++] = CURRENT_CHAR;
            tokenizer->index++;
        }
        ch = CURRENT_CHAR;
    }
    // 科学计数法
    if (ch == 'e' || ch == 'E') {
        tokenizer->index++;
        ch = CURRENT_CHAR;
        if (ch == '+' || ch == '-') {
            tokenizer->index++;
        }
        if (isDecimalDigit(CURRENT_CHAR)) {
            number[num_index++] = CURRENT_CHAR;
            while (isDecimalDigit(CURRENT_CHAR)) {
                number[num_index++] = CURRENT_CHAR;
                tokenizer->index++;
            }
        } else {
            tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
            return NULL;
        }
    }

    if (isIdentifierStart(CURRENT_CHAR)) {
        tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
        return NULL;
    }
    number[num_index] = '\0';
    token = v3_token_create(tokenizer, start, V3_TOKEN_NumericLiteral);
    token->v.num = strtod(number, NULL);
    return token;
} 

static v3_token_t *v3_token_create(v3_tokenizer_t *tokenizer, int start, v3_tokentype_t type)
{
    v3_token_t     *token;
//    v3_str_t       *value; 

//    value  = v3_alloc(tokenizer->options, sizeof(v3_str_t));
//    if (value == NULL) return NULL;
    token = (v3_token_t *)v3_vector_push(tokenizer->options, tokenizer->tokens);
    if (token == NULL) return NULL;

//    value->data = tokenizer->source.data + start;
//    value->length = tokenizer->index - start;
//    token->value = id;
    token->lineNumber = tokenizer->lineNumber;
    token->lineStart  = tokenizer->lineStart;
    token->start      = start;
    token->end        = tokenizer->index;  
    token->type       = type;
#if 0
    token->value.data = token->
    token->value.length = token->end - token->start;
#endif


    token->value.length = token->end - token->start;
    token->value.data = v3_alloc(tokenizer->options, token->value.length + 1);
    memcpy((char *)token->value.data, tokenizer->source.data+start, token->value.length);
    ((char *)token->value.data)[token->value.length] = '\0';

    return token;
}

int scanHexEscape(v3_tokenizer_t *tokenizer, int prefix)
{
    int i, len;
    //, code = 0;

    len = (prefix == 'u') ? 4 : 2;
    for (i = 0; i < len; ++i) {
        if (tokenizer->index < tokenizer->source.length 
            && isHexDigit(CURRENT_CHAR)) {
            tokenizer->index++;
            //ch = tokenizer->source.data[tokenizer->index++];
            // code = code * 16 + strchr("0123456789abcdef", tolower(ch));
        } else {
            return -1;
        }
    }

    // return code;
    return len;
}

static v3_token_t *
scanStringLiteral(v3_tokenizer_t *tokenizer) {
    int quote, start, ch;//, code;//, unescaped;//, restore;
    // int     octal = 0;
    char      *source;

    source = (char *)tokenizer->source.data;
    quote = source[tokenizer->index];
    assert((quote == '\'' || quote == '"') &&
        "String literal must starts with a quote");

    start = tokenizer->index;
    tokenizer->index++;

    while (tokenizer->index < tokenizer->source.length) {
        ch = source[tokenizer->index++];

        if (ch == quote) {
            quote = 0;
            break;
        } else if (ch == '\\') {
            ch = source[tokenizer->index++];
            if (!ch || !isLineTerminator(ch)) {
                // continue;    
            } else {
                ++tokenizer->lineNumber;
                if (ch ==  '\r' && source[tokenizer->index] == '\n') {
                    ++tokenizer->index;
                }
            }
        } else if (isLineTerminator(ch)) {
            break;
        } else {
            // str += ch;
        }
    }

    if (quote != 0) {
        tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
        return NULL;
    }

    return v3_token_create(tokenizer, start, V3_TOKEN_StringLiteral);
}


static v3_token_t *
scanPunctuator(v3_tokenizer_t *tokenizer) 
{
    unsigned int    start;
    int             code, code2;
    int             ch1, ch2, ch3, ch4;
    char          *source;
    // v3_token_t     *token;

    source = (char *)tokenizer->source.data;
    ch1 = CURRENT_CHAR;
    start = tokenizer->index;

    code = CURRENT_CHAR;
    switch (code) {
    case '.':
    case '(':
    case ')':
    case ';':
    case ',':
    case '{':
    case '}':
    case '[':
    case ']':
    case ':':
    case '?':
    case '~':
        tokenizer->index++;
        if (tokenizer->tokenize) {
            if (code == 0x28) {
                tokenizer->openParenToken = tokenizer->tokens->length;
            } else if (code == 0x7B) {
                tokenizer->openCurlyToken = tokenizer->tokens->length;
            }
        }
        return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
    default:
        code2 = NEXT_CHAR;
        if (code2 == '=') {
            switch (code) {
            case '+':
            case '-':
            case '/':
            case '<':
            case '>':
            case '^':
            case '|':
            case '%':
            case '&':
            case '*':
                tokenizer->index += 2;
                return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
            case '!':
            case '=':
                tokenizer->index += 2;
                // !== and ===
                if(CURRENT_CHAR == '=') {
                    tokenizer->index++;
                }
                return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
            }
        }
        
    }

    ch2 = source[tokenizer->index + 1];
    ch3 = source[tokenizer->index + 2];
    ch4 = source[tokenizer->index + 3];
    // 4-charater punctuator: >>>=
    if (ch1 == '>' && ch2 == '>' && ch3 == '>') {
        if (ch4 == '=') {
            tokenizer->index += 4;
            return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
        }
    }

    // 3-character punctuators: == != >>> <<= >>= 
    if (ch1 == '>' && ch2 == '>' && ch3 == '>') {
        tokenizer->index += 3;
        return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
    } 

    if (ch1 == '<' && ch2 == '<' && ch3 == '=') {
        tokenizer->index += 3;
        return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
    }

    if (ch1 == '>' && ch2 == '>' && ch3 == '=') {
        tokenizer->index += 3;
        return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
    }

     // Other 2-character punctuators: ++ -- << >> && ||

    if (ch1 == ch2 && 
        (ch1 == '+' || ch1 == '-' || ch1 =='<' || ch1 == '>'
        || ch1 == '&' || ch1 == '|')) {
        tokenizer->index += 2;
        return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
    }

    // 一元运算符
    if (ch1 == '<'
        || ch1 == '>'
        || ch1 == '='
        || ch1 == '!'
        || ch1 == '+'
        || ch1 == '-'
        || ch1 == '*'
        || ch1 == '%'
        || ch1 == '&'
        || ch1 == '|'
        || ch1 == '^'
        || ch1 == '/') {
            tokenizer->index++; 
            return v3_token_create(tokenizer, start, V3_TOKEN_Punctuator);
    }

    
    tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
    return NULL;
}

static v3_token_t *
scanIdentifier(v3_tokenizer_t *tokenizer)
{
    unsigned int    start;
    // int             rc;
    v3_tokentype_t    type;
    v3_str_t       *id; 
    int             keyword;
    v3_token_t      *token;

    start = tokenizer->index;
    // \u代表一个unicode字符
    id  = CURRENT_CHAR == '\\' ? getEscapedIdentifier(tokenizer) : getIdentifier(tokenizer);
    if (id == NULL) return NULL;

    if (id->length == 1) {
        type = V3_TOKEN_Identifier;
    } else if (isKeyword(id, &keyword)) {
        type = V3_TOKEN_Keyword;
    } else if (!strncmp(id->data, "null", 4)) {
        type = V3_TOKEN_NullLiteral;
    } else if (!strncmp(id->data, "true", 4) || !strncmp(id->data, "false", 4)) {
        type = V3_TOKEN_BooleanLiteral; 
    } else {
        type = V3_TOKEN_Identifier;
    }


    token = v3_token_create(tokenizer, start, type);

    if (type == V3_TOKEN_Keyword) {
        token->v.keyword = keyword;    
    }

    return token;
}

static v3_str_t *
getEscapedIdentifier(v3_tokenizer_t *tokenizer) 
{
    // TODO:识别\uXXXX代表的标识符
    assert(0);
    return NULL;
}

static v3_str_t *
getIdentifier(v3_tokenizer_t *tokenizer)
{
    int start;
    char    ch;
    v3_str_t   *id;
    start = tokenizer->index;
    while(tokenizer->index < tokenizer->source.length) {
        ch = CURRENT_CHAR;
        if(ch == '\\') {
            tokenizer->index = start;
            return getEscapedIdentifier(tokenizer);
        }
        if(isIdentifierPart(ch)) {
            tokenizer->index++;
        } else {
            break;
        }
    }
    
    id = v3_alloc(tokenizer->options, sizeof(v3_str_t));
    if (id == NULL) return NULL;
    id->data = tokenizer->source.data + start;
    id->length = tokenizer->index - start;

    return id;
}

static int isDecimalDigit(char ch) {
    return (ch >= 0x30 && ch <= 0x39);   // 0..9
}

static int isHexDigit(char ch) {
    return strchr("0123456789abcdefABCDEF", ch) != NULL;
}

static int isOctalDigit(char ch) {
    return strchr("01234567", ch) != NULL;
}

static int isWhiteSpace(int ch)
{
    return  (ch == 0x20) 
            || (ch == 0x09) 
            || (ch == 0x0B) 
            || (ch == 0x0C) 
            || (ch == 0xA0);
}

 // 7.3 Line Terminators
static int isLineTerminator(char ch) 
{
    return (ch == 0x0A) || (ch == 0x0D);// || (ch == 0x2028) || (ch == 0x2029);
}

int isIdentifierStart(char ch) {
    return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch == 0x5C);// ||                      // \ (backslash)
         //        ((ch >= 0x80) && Regex.NonAsciiIdentifierStart.test(String.fromCharCode(ch)));
}

static int isIdentifierPart(char ch) {
    return (ch == 0x24) || (ch == 0x5F) ||  // $ (dollar) and _ (underscore)
         (ch >= 0x41 && ch <= 0x5A) ||         // A..Z
         (ch >= 0x61 && ch <= 0x7A) ||         // a..z
         (ch >= 0x30 && ch <= 0x39) ||         // 0..9
         (ch == 0x5C);// ||                      // \ (backslash)
         //((ch >= 0x80) && Regex.NonAsciiIdentifierPart.test(String.fromCharCode(ch)));
}

int skipComment(v3_tokenizer_t *tokenizer)
{
    char    ch;
    int     start;//, rc;
    start = tokenizer->index == 0;
    while (tokenizer->index < tokenizer->source.length) {
        ch = CURRENT_CHAR;
        if(isWhiteSpace(ch)) {
            tokenizer->index++; // 跳过空格
        } else if(isLineTerminator(ch)) {
            // 处理回车换行
            tokenizer->index++;
            if(ch == 13 && CURRENT_CHAR == 10) {
                tokenizer->index++;
            }
            tokenizer->lineNumber++;
            tokenizer->lineStart = tokenizer->index;
            start = 1;
        } else if (ch == '/') {
            // 处理注释
            ch = NEXT_CHAR;
            if (ch == '/'){
                // 单行注释
                tokenizer->index++;
                tokenizer->index++;
                skipSingleLineComment(tokenizer, 2); 
                start = 1;
            } else if (ch == '*') {
                // 多行注释
                tokenizer->index++;
                tokenizer->index++;
                if(skipMultiLineComment(tokenizer, 2) == V3_ERROR) {
                    return V3_ERROR;
                }
            } else {
                break;
            }
        } else if (start && ch == '-') {
            if(NEXT_CHAR == '-' && NEXT_NEXT_CHAR == '>') {
                // -->
                tokenizer->index += 3;
                skipSingleLineComment(tokenizer, 3);
            } else {
                break;
            }
        } else if (ch == '<') {
            if(!strncmp(tokenizer->source.data + tokenizer->index + 1, "!--", 3)) {
                // <!--
                tokenizer->index++; // '<'
                tokenizer->index++; // '!'
                tokenizer->index++; // '-'
                tokenizer->index++; // '-'
                skipSingleLineComment(tokenizer, 4);
            } else {
                break;
            }
        } else {
            break;
        }
    }
    return V3_OK;
}

// 跳过单行注释
static void skipSingleLineComment(v3_tokenizer_t *tokenizer, unsigned int offset) {
    char    ch;
    while (tokenizer->index < tokenizer->source.length) {
        ch = CURRENT_CHAR; 
        tokenizer->index++;
        if (isLineTerminator(ch)) {
            if (ch == 13 && CURRENT_CHAR == 10) {
                tokenizer->index++;
            }
            tokenizer->lineNumber++;
            tokenizer->lineStart = tokenizer->index;
            return;
        }
    }
}

static int skipMultiLineComment(v3_tokenizer_t *tokenizer, unsigned int offset) 
{
    char    ch;
    while (tokenizer->index < tokenizer->source.length) {
        ch = CURRENT_CHAR;
        if (isLineTerminator(ch)) {
            if (ch == 13 && NEXT_CHAR == 10) {
                tokenizer->index++;
            }
            tokenizer->lineNumber++;
            tokenizer->index++;
            tokenizer->lineStart = tokenizer->index;
            if (tokenizer->index >= tokenizer->source.length) {
                tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
                return V3_ERROR;
            }
        } else if (ch == '*') {
            // block comment ends with */
            if (NEXT_CHAR == '/') {
                tokenizer->index++;
                tokenizer->index++;
                return V3_OK;
            }
            tokenizer->index++;
        } else {
            tokenizer->index++;
        }
    }

    tokenizer->err = Msg_UnexpectedToken_ILLEGAL;
    return V3_ERROR;
}

static v3_token_t *
advanceSlash(v3_tokenizer_t *tokenizer) 
{
    const char  *source;
    char        pch;
    int         index;
    unsigned int    i;
    v3_token_t *prevToken, *checkToken;
    v3_token_t *items;

    source = tokenizer->source.data;
    if (tokenizer->tokens->length == 0) {
        // Nothing before that: it cannot be a division.
        return collectRegex(tokenizer);
    }
    // Using the following algorithm:
    // https://github.com/mozilla/sweet.js/wiki/design
    items = tokenizer->tokens->items;
    prevToken = &items[tokenizer->tokens->length - 1];
    checkToken = NULL;

    if (prevToken->type == V3_TOKEN_Punctuator) {
        pch = source[prevToken->start]; 
        if (pch == ']') { // eg: foo[1]/bar;
            return scanPunctuator(tokenizer);
        }
        if (pch == ')') {
            index = tokenizer->openParenToken - 1;
            if (index >= 0 && (size_t)index < tokenizer->tokens->length) {
                checkToken = &items[index];
                if (checkToken->type == V3_TOKEN_Keyword &&
                        (token_value_eq(checkToken, "if") ||
                         token_value_eq(checkToken, "while") ||
                         token_value_eq(checkToken, "for") ||
                         token_value_eq(checkToken, "with"))) {
                    return collectRegex(tokenizer);
                }
            }
            return scanPunctuator(tokenizer);
        }
        if (pch == '}') { // eg: foo(){return 1}/2;
            // Dividing a function by anything makes little sense,
            // but we have to check for that.
            index = tokenizer->openCurlyToken - 3;
            if (index >= 0 && (size_t)index < tokenizer->tokens->length &&
                    items[index].type == V3_TOKEN_Keyword) {
                // Anonymous function.
                // if (!checkToken) {
                index = tokenizer->openCurlyToken - 4;
                if (index >= 0 && (size_t)index < tokenizer->tokens->length) {
                    checkToken = &items[index];
                    return scanPunctuator(tokenizer);
                }
            } else if ((index = tokenizer->openCurlyToken - 4) >= 0
                        && (size_t)index < tokenizer->tokens->length 
                        && items[index].type == V3_TOKEN_Keyword) {
                // Named function.
                index = tokenizer->openCurlyToken - 5;
                if (index >= 0 
                    && (size_t)index < tokenizer->tokens->length) {
                    checkToken = &items[index];
                    return collectRegex(tokenizer);
                }
            } else {
                return scanPunctuator(tokenizer);
            }
            // checkToken determines whether the function is
            // a declaration or an expression.
            if (checkToken != NULL) {
                for (i = 0; i < sizeof(FnExprTokens) / sizeof(FnExprTokens[0]); i++) {
                    if (FnExprTokens[i].length == token_value_len(checkToken)
                        && strncmp(FnExprTokens[i].data, token_value(checkToken), FnExprTokens[i].length) == 0) {
                            return scanPunctuator(tokenizer);
                        }
                }
            }
            // It is a declaration.
            return collectRegex(tokenizer);
        }
        return collectRegex(tokenizer);
    }

    if (prevToken->type == V3_TOKEN_Keyword) {
        return collectRegex(tokenizer);
    }

    return scanPunctuator(tokenizer);
}

static v3_token_t *
collectRegex(v3_tokenizer_t *tokenizer) 
{
    int rc;
    rc = skipComment(tokenizer);
    if (rc == V3_ERROR) return NULL;

    return scanRegExp(tokenizer);
}

static v3_token_t *scanRegExp(v3_tokenizer_t *tokenizer) 
{
    int     start, rc;

    tokenizer->lookahead = NULL;
    rc = skipComment(tokenizer);
    if (rc == V3_ERROR) return NULL;

    start = tokenizer->index;
    // skip regexpression
    rc = scanRegExpBody(tokenizer);
    if (rc == V3_ERROR) return NULL;

    scanRegExpFlags(tokenizer);
    // value = testRegExp(body.value, flags.value);

    return v3_token_create(tokenizer, start, V3_TOKEN_RegularExpression);
}

static int scanRegExpBody(v3_tokenizer_t *tokenizer) 
{
    int ch, /*str,*/ classMarker, terminated/*, body*/;
    const char    *source;

    source = tokenizer->source.data;
    ch = source[tokenizer->index++];
    // int         start;
    // Regular expression literal must start with a slash
    assert(ch == '/');
    // start = tokenizer->index;
    // str = source[tokenizer->index++];

    classMarker = V3_FALSE;
    terminated = V3_FALSE;

    while (tokenizer->index < tokenizer->source.length) {
        ch = source[tokenizer->index++];
        // str += ch;
        if (ch == '\\') {
            ch = source[tokenizer->index++];
            // ECMA-262 7.8.5
            if (isLineTerminator(ch)) {
                tokenizer->err = Msg_UnterminatedRegExp;
                return V3_ERROR;
            }
            // str += ch;
        } else if (isLineTerminator(ch)) {
            tokenizer->err = Msg_UnterminatedRegExp;
            return V3_ERROR;
        } else if (classMarker) {
            if (ch == ']') {
                classMarker = V3_FALSE;
            }
        } else {
            if (ch == '/') {
                terminated = V3_TRUE;
                break;
            } else if (ch == '[') {
                classMarker = V3_TRUE;
            }
        }
    }

    if (!terminated) {
        tokenizer->err = Msg_UnterminatedRegExp;
        return V3_ERROR;
    }

    return V3_OK;
    // Exclude leading and trailing slash.
    // body = str.substr(1, str.length - 2);
}

static void scanRegExpFlags(v3_tokenizer_t *tokenizer) 
{
    int     ch;
    // const char    *source;
    // source = tokenizer->source.data;
//    str = '';
//    flags = '';

    while (tokenizer->index < tokenizer->source.length) {
        ch = CURRENT_CHAR;
        if (!isIdentifierPart(ch)) {
            break;
        }

        tokenizer->index++;
    }
}
