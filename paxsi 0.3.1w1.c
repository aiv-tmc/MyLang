#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char signature[4];
    uint16_t version;
    uint16_t reserved;
    uint32_t token_count;
} TokenFileHeader;

typedef enum {
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_CHAR,
    TOKEN_NONE,
    TOKEN_STRING,
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_DO,
    TOKEN_RETURN,
    TOKEN_FREE,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_QUIT,
    TOKEN_COMPILE,
    TOKEN_PREPROC_MACRO,
    TOKEN_PREPROC_INCLIB,
    TOKEN_PREPROC_INCFILE,
    TOKEN_PREPROC_DEFINE,
    TOKEN_PREPROC_UNDEF,
    TOKEN_PREPROC_IFDEF,
    TOKEN_PREPROC_IFNDEF,
    TOKEN_PREPROC_ENDIF,
    TOKEN_PREPROC_LINE,
    TOKEN_PREPROC_ERROR,
    TOKEN_PREPROC_PRAGMA,
    TOKEN_OUTSIDE_COMPILE,
    TOKEN_OUTSIDE_CODE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_DOUBLE_STAR,
    TOKEN_DOUBLE_AND,
    TOKEN_DOUBLE_PIPE,
    TOKEN_BANG,
    TOKEN_TILDE,
    TOKEN_PIPE,
    TOKEN_AMPERSAND,
    TOKEN_CARET,
    TOKEN_SHR,
    TOKEN_SHL,
    TOKEN_SAR,
    TOKEN_SAL,
    TOKEN_ROR,
    TOKEN_ROL,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_GE,
    TOKEN_LE,
    TOKEN_DOUBLE_EQ,
    TOKEN_TRIPLE_EQ,
    TOKEN_NE,
    TOKEN_ARROW,
    TOKEN_THIS,
    TOKEN_EQUAL,
    TOKEN_PLUS_EQ,
    TOKEN_MINUS_EQ,
    TOKEN_STAR_EQ,
    TOKEN_SLASH_EQ,
    TOKEN_PERCENT_EQ,
    TOKEN_PIPE_EQ,
    TOKEN_AMPERSAND_EQ,
    TOKEN_CARET_EQ,
    TOKEN_TILDE_EQ,
    TOKEN_DOUBLE_PLUS,
    TOKEN_DOUBLE_MINUS,
    TOKEN_DOLLAR,
    TOKEN_COLON,
    TOKEN_DOUBLE_COLON,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_QUESTION,
    TOKEN_ELLIPSIS,
    TOKEN_UNDERSCORE,
    TOKEN_DOUBLE_UNDERSCORE,
    TOKEN_MODIFIER,
    TOKEN_TYPE,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

const char* token_names[] = {
    [TOKEN_INT]                 =   "<INT>",
    [TOKEN_REAL]                =   "<REAL>",
    [TOKEN_CHAR]                =   "<CHAR>",
    [TOKEN_NONE]                =   "<NONE>",
    [TOKEN_STRING]              =   "<STRING>",
    [TOKEN_IF]                  =   "<IF>",
    [TOKEN_ELIF]                =   "<ELIF>",
    [TOKEN_ELSE]                =   "<ELSE>",
    [TOKEN_DO]                  =   "<DO>",
    [TOKEN_RETURN]              =   "<RETURN>",
    [TOKEN_FREE]                =   "<FREE>",
    [TOKEN_BREAK]               =   "<BREAK>",
    [TOKEN_CONTINUE]            =   "<CONTINUE>",
    [TOKEN_QUIT]                =   "<QUIT>",
    [TOKEN_COMPILE]             =   "<COMPILE>",
    [TOKEN_PREPROC_MACRO]       =   "<PREPROC_MACRO>",
    [TOKEN_PREPROC_INCLIB]      =   "<PREPROC_INCLIB>",
    [TOKEN_PREPROC_INCFILE]     =   "<PREPROC_INCFILE>",
    [TOKEN_PREPROC_DEFINE]      =   "<PREPROC_DEFINE>",
    [TOKEN_PREPROC_UNDEF]       =   "<PREPROC_UNDEF>",
    [TOKEN_PREPROC_IFDEF]       =   "<PREPROC_IFDEF>",
    [TOKEN_PREPROC_IFNDEF]      =   "<PREPROC_IFNDEF>",
    [TOKEN_PREPROC_ENDIF]       =   "<PREPROC_ENDIF>",
    [TOKEN_PREPROC_LINE]        =   "<PREPROC_LINE>",
    [TOKEN_PREPROC_ERROR]       =   "<PREPROC_ERROR>",
    [TOKEN_PREPROC_PRAGMA]      =   "<PREPROC_PRAGMA>",
    [TOKEN_OUTSIDE_COMPILE]     =   "<OUTSIDE_COMPILE>",
    [TOKEN_OUTSIDE_CODE]        =   "<OUTSIDE_CODE>",
    [TOKEN_DOLLAR]              =   "<DOLLAR>",
    [TOKEN_COLON]               =   "<COLON>",
    [TOKEN_DOUBLE_COLON]        =   "<DOUBLE_COLON>",
    [TOKEN_UNDERSCORE]          =   "<UNDERSCORE>",
    [TOKEN_DOUBLE_UNDERSCORE]   =   "<DOUBLE_UNDERSCORE>",
    [TOKEN_ELLIPSIS]            =   "<ELLIPSIS>",
    [TOKEN_MODIFIER]            =   "<MODIFIER>",
    [TOKEN_IDENTIFIER]          =   "<IDENTIFIER>",
    [TOKEN_SEMICOLON]           =   "<SEMICOLON>",
    [TOKEN_ARROW]               =   "<ARROW>",
    [TOKEN_THIS]                =   "<THIS>",
    [TOKEN_EQUAL]               =   "<EQUAL>",
    [TOKEN_COMMA]               =   "<COMMA>",
    [TOKEN_PLUS]                =   "<PLUS>",
    [TOKEN_MINUS]               =   "<MINUS>",
    [TOKEN_STAR]                =   "<STAR>",
    [TOKEN_SLASH]               =   "<SLASH>",
    [TOKEN_PERCENT]             =   "<PERCENT>",
    [TOKEN_TILDE]               =   "<TILDE>",
    [TOKEN_PIPE]                =   "<PIPE>",
    [TOKEN_AMPERSAND]           =   "<AMPERSAND>",
    [TOKEN_REFERENCE]           =   "<REFERENCE>",
    [TOKEN_BANG]                =   "<BANG>",
    [TOKEN_CARET]               =   "<CARET>",
    [TOKEN_AT]                  =   "<AT>",
    [TOKEN_GT]                  =   "<GT>",
    [TOKEN_LT]                  =   "<LT>",
    [TOKEN_SHR]                 =   "<SHR>",
    [TOKEN_SHL]                 =   "<SHL>",
    [TOKEN_SAR]                 =   "<SAR>",
    [TOKEN_SAL]                 =   "<SAL>",
    [TOKEN_ROR]                 =   "<ROR>",
    [TOKEN_ROL]                 =   "<ROL>",
    [TOKEN_GE]                  =   "<GE>",
    [TOKEN_LE]                  =   "<LE>",
    [TOKEN_DOUBLE_EQ]           =   "<DOUBLE_EQ>",
    [TOKEN_TRIPLE_EQ]           =   "<TRIPLE_EQ>",
    [TOKEN_NE]                  =   "<NE>",
    [TOKEN_PLUS_EQ]             =   "<PLUS_EQ>",
    [TOKEN_MINUS_EQ]            =   "<MINUS_EQ>",
    [TOKEN_STAR_EQ]             =   "<STAR_EQ>",
    [TOKEN_SLASH_EQ]            =   "<SLASH_EQ>",
    [TOKEN_PERCENT_EQ]          =   "<PERCENT_EQ>",
    [TOKEN_PIPE_EQ]             =   "<PIPE_EQ>",
    [TOKEN_AMPERSAND_EQ]        =   "<AMPERSAND_EQ>",
    [TOKEN_CARET_EQ]            =   "<CARET_EQ>",
    [TOKEN_TILDE_EQ]            =   "<TILDE_EQ>",
    [TOKEN_DOUBLE_PLUS]         =   "<DOUBLE_PLUS>",
    [TOKEN_DOUBLE_MINUS]        =   "<DOUBLE_MINUS>",
    [TOKEN_DOUBLE_AND]          =   "<DOUBLE_AND>",
    [TOKEN_DOUBLE_PIPE]         =   "<DOUBLE_PIPE>",
    [TOKEN_DOUBLE_STAR]         =   "<DOUBLE_STAR>",
    [TOKEN_QUESTION]            =   "<QUESTION>",
    [TOKEN_LCURLY]              =   "<LCURLY>",
    [TOKEN_RCURLY]              =   "<RCURLY>",
    [TOKEN_LBRACKET]            =   "<LBRACKET>",
    [TOKEN_RBRACKET]            =   "<RBRACKET>",
    [TOKEN_LBRACE]              =   "<LBRACE>",
    [TOKEN_RBRACE]              =   "<RBRACE>",
    [TOKEN_LPAREN]              =   "<LPAREN>",
    [TOKEN_RPAREN]              =   "<RPAREN>",
    [TOKEN_TYPE]                =   "<TYPE>",
    [TOKEN_EOF]                 =   "<EOF>",
    [TOKEN_ERROR]               =   "<ERROR>"
};

typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
    int length;
} Token;

typedef struct {
    const char* input;
    int length;
    int position;
    int line;
    int column;
    Token* tokens;
    int token_count;
    int token_capacity;
} Lexer;

Lexer* init_lexer(const char* input) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->input = input;
    lexer->length = strlen(input);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->token_count = 0;
    lexer->token_capacity = 100;
    lexer->tokens = malloc(lexer->token_capacity * sizeof(Token));
    return lexer;
}

void free_lexer(Lexer* lexer) {
    for (int i = 0; i < lexer->token_count; i++) {
        free(lexer->tokens[i].value);
    }
    free(lexer->tokens);
    free(lexer);
}

void add_token(Lexer* lexer, TokenType type, const char* value, int length) {
    if (lexer->token_count >= lexer->token_capacity) {
        lexer->token_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, lexer->token_capacity * sizeof(Token));
    }

    Token token;
    token.type = type;
    token.value = malloc(length + 1);
    strncpy(token.value, value, length);
    token.value[length] = '\0';
    token.line = lexer->line;
    token.column = lexer->column - length;
    token.length = length;

    lexer->tokens[lexer->token_count++] = token;
}

void add_error(Lexer* lexer, const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    add_token(lexer, TOKEN_ERROR, buffer, strlen(buffer));
}

void skip_whitespace(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        char character = lexer->input[lexer->position];

        if (character == ' ' || character == '\t') {
            lexer->position++;
            lexer->column++;
        }
        else if (character == '\n') {
            lexer->position++;
            lexer->line++;
            lexer->column = 1;
        }
        else {
            break;
        }
    }
}

void skip_comments(Lexer* lexer) {
    if (lexer->position < lexer->length - 1) {
        if (lexer->input[lexer->position] == '/' &&
            lexer->input[lexer->position + 1] == '/') {
            while (lexer->position < lexer->length &&
                lexer->input[lexer->position] != '\n') {
                lexer->position++;
            }
            lexer->column = 1;
        }
        else if (lexer->input[lexer->position] == '/' &&
            lexer->input[lexer->position + 1] == '*') {
            lexer->position += 2;
            lexer->column += 2;

            int depth = 1;
            while (depth > 0 && lexer->position < lexer->length - 1) {
                if (lexer->input[lexer->position] == '*' &&
                    lexer->input[lexer->position + 1] == '/') {
                    lexer->position += 2;
                    lexer->column += 2;
                    depth--;
                } else if (lexer->input[lexer->position] == '/' &&
                           lexer->input[lexer->position + 1] == '*') {
                    lexer->position += 2;
                    lexer->column += 2;
                    depth++;
                } else {
                    if (lexer->input[lexer->position] == '\n') {
                        lexer->line++;
                        lexer->column = 1;
                    } else {
                        lexer->column++;
                    }
                    lexer->position++;
                }
            }

            if (depth > 0) {
                add_error(lexer, "Unclosed comment");
            }
        }
    }
}

static bool is_valid_digit(char c, int base) {
    if (base <= 10) {
        return c >= '0' && c < '0' + base;
    }
    if (c >= '0' && c <= '9') return true;
    if (c >= 'A' && c <= 'A' + base - 11) return true;
    return false;
}

void parse_number(Lexer* lexer) {
    int start = lexer->position;
    int base = 10;
    bool has_base = false;
    bool is_real = false;
    bool has_exponent = false;
    bool has_prefix = false;

    if (lexer->position < lexer->length - 1 &&
        lexer->input[lexer->position] == '0') {
        char next_char = lexer->input[lexer->position + 1];
        if (next_char == 'x') {
            base = 16;
            has_prefix = true;
        } else if (next_char == 'd') {
            base = 10;
            has_prefix = true;
        } else if (next_char == 'o') {
            base = 8;
            has_prefix = true;
        } else if (next_char == 'b') {
            base = 2;
            has_prefix = true;
        } else if (next_char == 'q') {
            base = 4;
            has_prefix = true;
        }
        
        if (has_prefix) {
            lexer->position += 2;
            lexer->column += 2;
            start = lexer->position;
            has_base = true;
        }
    }

    while (lexer->position < lexer->length) {
        char character = lexer->input[lexer->position];

        if (character == '.') {
            if (is_real || has_base || base != 10) {
                add_error(lexer, "Invalid real number format");
                return;
            }
            is_real = true;
            lexer->position++;
            lexer->column++;
        }
        else if (character == 'e') {
            if (has_exponent || has_base || base != 10) {
                add_error(lexer, "Invalid real number format");
                return;
            }
            is_real = true;
            has_exponent = true;
            lexer->position++;
            lexer->column++;

            if (lexer->position < lexer->length &&
                (lexer->input[lexer->position] == '+' ||
                    lexer->input[lexer->position] == '-')) {
                lexer->position++;
                lexer->column++;
            }

            if (lexer->position >= lexer->length ||
                !is_valid_digit(lexer->input[lexer->position], base)) {
                add_error(lexer, "Exponent has no digits");
                return;
            }
        }
        else if (is_valid_digit(character, base)) {
            lexer->position++;
            lexer->column++;
        }
        else {
            break;
        }
    }

    int length = lexer->position - start;
    if (length == 0) {
        add_error(lexer, "Empty number literal");
        return;
    }
    
    TokenType type = is_real ? TOKEN_REAL : TOKEN_INT;
    add_token(lexer, type, lexer->input + start, length);
}

void parse_char(Lexer* lexer) {
    lexer->position++;
    lexer->column++;

    int start = lexer->position;

    if (lexer->position < lexer->length && lexer->input[lexer->position] == '\\') {
        lexer->position++;
        lexer->column++;
        if (lexer->position < lexer->length) {
            lexer->position++;
            lexer->column++;
        }
    }
    else if (lexer->position < lexer->length) {
        lexer->position++;
        lexer->column++;
    }

    if (lexer->position >= lexer->length || lexer->input[lexer->position] != '\'') {
        add_error(lexer, "Unclosed character literal");
        return;
    }

    int length = lexer->position - start;
    add_token(lexer, TOKEN_CHAR, lexer->input + start, length);

    lexer->position++;
    lexer->column++;
}

void parse_string(Lexer* lexer) {
    lexer->position++;
    lexer->column++;

    int start = lexer->position;

    while (lexer->position < lexer->length) {
        if (lexer->input[lexer->position] == '\\') {
            lexer->position++;
            lexer->column++;
            if (lexer->position < lexer->length) {
                lexer->position++;
                lexer->column++;
            }
        }
        else if (lexer->input[lexer->position] == '"') {
            break;
        }
        else {
            lexer->position++;
            lexer->column++;
        }
    }

    if (lexer->position >= lexer->length || lexer->input[lexer->position] != '"') {
        add_error(lexer, "Unclosed string literal");
        return;
    }

    int length = lexer->position - start;
    add_token(lexer, TOKEN_STRING, lexer->input + start, length);

    lexer->position++;
    lexer->column++;
}

bool is_valid_modifier(const char* modifier) {
    const char* valid_modifiers[] = { 
        "const", "unsigned", "extern", "static", 
        "protected", "dynam", "regis", "local", "global", 
        "signed", "volatile", "restrict", "inline", "register"
    };
    int count = sizeof(valid_modifiers) / sizeof(valid_modifiers[0]);
    for (int i = 0; i < count; i++) {
        if (strcmp(modifier, valid_modifiers[i]) == 0) return true;
    }
    return false;
}

bool is_valid_type(const char* type) {
    const char* valid_types[] = { "int", "real", "char", "bool" };
    int count = sizeof(valid_types) / sizeof(valid_types[0]);
    for (int i = 0; i < count; i++) {
        if (strcmp(type, valid_types[i]) == 0) return true;
    }
    return false;
}

static void parse_variable_access(Lexer* lexer) {
    add_token(lexer, TOKEN_DOLLAR, "$", 1);
    lexer->position++;
    lexer->column++;

    if (lexer->position < lexer->length &&
        (isalpha(lexer->input[lexer->position]) ||
            lexer->input[lexer->position] == '_')) {
        int start = lexer->position;
        while (lexer->position < lexer->length &&
            (isalnum(lexer->input[lexer->position]) ||
                lexer->input[lexer->position] == '_')) {
            lexer->position++;
            lexer->column++;
        }
        int length = lexer->position - start;
        add_token(lexer, TOKEN_IDENTIFIER, lexer->input + start, length);
    }
    else {
        add_error(lexer, "Expected identifier after '$' in variable access");
    }
}

static void parse_return_type(Lexer* lexer) {
    if (lexer->input[lexer->position] == '[') {
        add_token(lexer, TOKEN_LBRACKET, "[", 1);
        lexer->position++;
        lexer->column++;

        while (lexer->position < lexer->length) {
            skip_whitespace(lexer);

            int mod_start = lexer->position;
            while (lexer->position < lexer->length &&
                isalpha(lexer->input[lexer->position])) {
                lexer->position++;
                lexer->column++;
            }

            if (lexer->position > mod_start) {
                int length = lexer->position - mod_start;
                char* modifier = strndup(lexer->input + mod_start, length);

                if (is_valid_modifier(modifier)) {
                    add_token(lexer, TOKEN_MODIFIER, modifier, length);
                }
                else {
                    add_error(lexer, "Invalid modifier: %s", modifier);
                }
                free(modifier);
            }

            skip_whitespace(lexer);

            if (lexer->input[lexer->position] == ',') {
                add_token(lexer, TOKEN_COMMA, ",", 1);
                lexer->position++;
                lexer->column++;
                skip_whitespace(lexer);
            }
            else if (lexer->input[lexer->position] == ']') {
                break;
            }
            else {
                add_error(lexer, "Expected ',' or ']' in modifier list");
                return;
            }
        }

        if (lexer->position >= lexer->length || lexer->input[lexer->position] != ']') {
            add_error(lexer, "Expected ']' after modifiers");
            return;
        }

        add_token(lexer, TOKEN_RBRACKET, "]", 1);
        lexer->position++;
        lexer->column++;
        skip_whitespace(lexer);
    }

    int token_start = lexer->position;
    while (lexer->position < lexer->length &&
        isalpha(lexer->input[lexer->position])) {
        lexer->position++;
        lexer->column++;
    }
    if (lexer->position > token_start) {
        int length = lexer->position - token_start;
        char* type_str = strndup(lexer->input + token_start, length);
        if (is_valid_type(type_str)) {
            add_token(lexer, TOKEN_TYPE, type_str, length);
        }
        else {
            add_error(lexer, "Invalid type: %s", type_str);
        }
        free(type_str);
    }
    else {
        add_error(lexer, "Expected type after colon");
        return;
    }

    if (lexer->position < lexer->length && lexer->input[lexer->position] == ':') {
        add_token(lexer, TOKEN_COLON, ":", 1);
        lexer->position++;
        lexer->column++;
        token_start = lexer->position;

        while (lexer->position < lexer->length &&
            isdigit(lexer->input[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }

        if (lexer->position > token_start) {
            int length = lexer->position - token_start;
            add_token(lexer, TOKEN_INT, lexer->input + token_start, length);
        }
        else {
            add_error(lexer, "Expected bit size after ':'");
            return;
        }
    }
}

static void parse_variable_declaration(Lexer* lexer) {
    add_token(lexer, TOKEN_DOLLAR, "$", 1);
    lexer->position++;
    lexer->column++;

    skip_whitespace(lexer);

    if (lexer->input[lexer->position] == '[') {
        add_token(lexer, TOKEN_LBRACKET, "[", 1);
        lexer->position++;
        lexer->column++;

        while (lexer->position < lexer->length) {
            skip_whitespace(lexer);

            int mod_start = lexer->position;
            while (lexer->position < lexer->length &&
                isalpha(lexer->input[lexer->position])) {
                lexer->position++;
                lexer->column++;
            }

            if (lexer->position > mod_start) {
                int length = lexer->position - mod_start;
                char* modifier = strndup(lexer->input + mod_start, length);

                if (is_valid_modifier(modifier)) {
                    add_token(lexer, TOKEN_MODIFIER, modifier, length);
                }
                else {
                    add_error(lexer, "Invalid modifier: %s", modifier);
                }
                free(modifier);
            }

            skip_whitespace(lexer);

            if (lexer->input[lexer->position] == ',') {
                add_token(lexer, TOKEN_COMMA, ",", 1);
                lexer->position++;
                lexer->column++;
                skip_whitespace(lexer);
            }
            else if (lexer->input[lexer->position] == ']') {
                break;
            }
            else {
                add_error(lexer, "Expected ',' or ']' in modifier list");
                return;
            }
        }

        if (lexer->position >= lexer->length || lexer->input[lexer->position] != ']') {
            add_error(lexer, "Expected ']' after modifiers");
            return;
        }

        add_token(lexer, TOKEN_RBRACKET, "]", 1);
        lexer->position++;
        lexer->column++;
        skip_whitespace(lexer);
    }

    int token_start = lexer->position;
    while (lexer->position < lexer->length &&
        isalpha(lexer->input[lexer->position])) {
        lexer->position++;
        lexer->column++;
    }
    if (lexer->position > token_start) {
        int length = lexer->position - token_start;
        char* type_str = strndup(lexer->input + token_start, length);
        if (is_valid_type(type_str)) {
            add_token(lexer, TOKEN_TYPE, type_str, length);
        }
        else {
            add_error(lexer, "Invalid type: %s", type_str);
        }
        free(type_str);
    }
    else {
        add_error(lexer, "Expected type after '$'");
        return;
    }

    if (lexer->position < lexer->length && lexer->input[lexer->position] == ':') {
        add_token(lexer, TOKEN_COLON, ":", 1);
        lexer->position++;
        lexer->column++;
        token_start = lexer->position;

        while (lexer->position < lexer->length &&
            isdigit(lexer->input[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }

        if (lexer->position > token_start) {
            int length = lexer->position - token_start;
            add_token(lexer, TOKEN_INT, lexer->input + token_start, length);
        }
        else {
            add_error(lexer, "Expected bit size after ':'");
            return;
        }
    }

    skip_whitespace(lexer);

    if (lexer->position < lexer->length) {
        if (lexer->input[lexer->position] == '@') {
            add_token(lexer, TOKEN_AT, "@", 1);
            lexer->position++;
            lexer->column++;
            skip_whitespace(lexer);
        }
        else if (lexer->input[lexer->position] == '&') {
            add_token(lexer, TOKEN_REFERENCE, "&", 1);
            lexer->position++;
            lexer->column++;
            skip_whitespace(lexer);
        }
    }

    token_start = lexer->position;
    while (lexer->position < lexer->length &&
        (isalnum(lexer->input[lexer->position]) ||
            lexer->input[lexer->position] == '_')) {
        lexer->position++;
        lexer->column++;
    }

    if (lexer->position > token_start) {
        int length = lexer->position - token_start;
        add_token(lexer, TOKEN_IDENTIFIER, lexer->input + token_start, length);
    }
    else {
        add_error(lexer, "Expected identifier after type");
        return;
    }

    while (lexer->position < lexer->length && lexer->input[lexer->position] == '[') {
        add_token(lexer, TOKEN_LBRACKET, "[", 1);
        lexer->position++;
        lexer->column++;
        skip_whitespace(lexer);

        while (lexer->position < lexer->length && lexer->input[lexer->position] != ']') {
            if (lexer->input[lexer->position] == '?') {
                add_token(lexer, TOKEN_QUESTION, "?", 1);
                lexer->position++;
                lexer->column++;
            }
            else if (lexer->position < lexer->length - 2 &&
                lexer->input[lexer->position] == '.' &&
                lexer->input[lexer->position + 1] == '.' &&
                lexer->input[lexer->position + 2] == '.') {
                add_token(lexer, TOKEN_ELLIPSIS, "...", 3);
                lexer->position += 3;
                lexer->column += 3;
            }
            else if (lexer->input[lexer->position] == ':') {
                add_token(lexer, TOKEN_COLON, ":", 1);
                lexer->position++;
                lexer->column++;
            }
            else if (isdigit(lexer->input[lexer->position]) ||
                lexer->input[lexer->position] == '-' ||
                lexer->input[lexer->position] == '+') {
                parse_number(lexer);
            }
            else if (isalpha(lexer->input[lexer->position]) || 
                     lexer->input[lexer->position] == '_') {
                int id_start = lexer->position;
                while (lexer->position < lexer->length &&
                    (isalnum(lexer->input[lexer->position]) ||
                        lexer->input[lexer->position] == '_')) {
                    lexer->position++;
                    lexer->column++;
                }
                int id_length = lexer->position - id_start;
                add_token(lexer, TOKEN_IDENTIFIER, lexer->input + id_start, id_length);
            }
            else if (lexer->input[lexer->position] == '$') {
                parse_variable_access(lexer);
            }
            else {
                add_error(lexer, "Unexpected character in array dimension");
                lexer->position++;
                lexer->column++;
            }

            skip_whitespace(lexer);
        }

        if (lexer->position >= lexer->length || lexer->input[lexer->position] != ']') {
            add_error(lexer, "Expected ']' after array dimension");
            return;
        }

        add_token(lexer, TOKEN_RBRACKET, "]", 1);
        lexer->position++;
        lexer->column++;
        skip_whitespace(lexer);
    }

    skip_whitespace(lexer);

    if (lexer->position < lexer->length && lexer->input[lexer->position] == '=') {
        add_token(lexer, TOKEN_EQUAL, "=", 1);
        lexer->position++;
        lexer->column++;

        skip_whitespace(lexer);

        if (lexer->position < lexer->length && lexer->input[lexer->position] == '{') {
            add_token(lexer, TOKEN_LBRACE, "{", 1);
            lexer->position++;
            lexer->column++;

            while (lexer->position < lexer->length) {
                skip_whitespace(lexer);
                skip_comments(lexer);

                if (lexer->input[lexer->position] == '}') break;

                if (lexer->input[lexer->position] == '\'') {
                    parse_char(lexer);
                }
                else if (lexer->input[lexer->position] == '"') {
                    parse_string(lexer);
                }
                else if (lexer->input[lexer->position] == '$') {
                    parse_variable_access(lexer);
                }
                else if (isdigit(lexer->input[lexer->position]) ||
                    lexer->input[lexer->position] == '-' ||
                    lexer->input[lexer->position] == '+') {
                    parse_number(lexer);
                }
                else if (isalpha(lexer->input[lexer->position]) || 
                         lexer->input[lexer->position] == '_') {
                    if (strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
                        add_token(lexer, TOKEN_NONE, "NONE", 4);
                        lexer->position += 4;
                        lexer->column += 4;
                    }
                    else {
                        int func_start = lexer->position;
                        while (lexer->position < lexer->length &&
                            (isalnum(lexer->input[lexer->position]) ||
                                lexer->input[lexer->position] == '_')) {
                            lexer->position++;
                            lexer->column++;
                        }

                        int func_length = lexer->position - func_start;
                        char* func_name = strndup(lexer->input + func_start, func_length);

                        skip_whitespace(lexer);
                        if (lexer->input[lexer->position] == '(') {
                            add_token(lexer, TOKEN_IDENTIFIER, func_name, func_length);
                            lexer->position++;
                            lexer->column++;
                            int depth = 1;
                            while (lexer->position < lexer->length && depth > 0) {
                                if (lexer->input[lexer->position] == '(') depth++;
                                else if (lexer->input[lexer->position] == ')') depth--;
                                lexer->position++;
                                lexer->column++;
                            }
                        }
                        else {
                            add_token(lexer, TOKEN_IDENTIFIER, func_name, func_length);
                        }
                        free(func_name);
                    }
                }
                else {
                    add_error(lexer, "Unexpected character in initialization");
                    return;
                }

                skip_whitespace(lexer);

                if (lexer->input[lexer->position] == ',') {
                    add_token(lexer, TOKEN_COMMA, ",", 1);
                    lexer->position++;
                    lexer->column++;
                }
                else if (lexer->input[lexer->position] != '}') {
                    add_error(lexer, "Expected ',' or '}' after array element");
                    return;
                }
            }

            if (lexer->position >= lexer->length || lexer->input[lexer->position] != '}') {
                add_error(lexer, "Expected '}' after array initialization");
                return;
            }

            add_token(lexer, TOKEN_RBRACE, "}", 1);
            lexer->position++;
            lexer->column++;
        }
        else {
            if (lexer->input[lexer->position] == '\'') {
                parse_char(lexer);
            }
            else if (lexer->input[lexer->position] == '"') {
                parse_string(lexer);
            }
            else if (lexer->input[lexer->position] == '$') {
                parse_variable_access(lexer);
            }
            else if (isdigit(lexer->input[lexer->position]) ||
                lexer->input[lexer->position] == '-' ||
                lexer->input[lexer->position] == '+') {
                parse_number(lexer);
            }
            else if (isalpha(lexer->input[lexer->position]) || 
                     lexer->input[lexer->position] == '_') {
                if (strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
                    add_token(lexer, TOKEN_NONE, "NONE", 4);
                    lexer->position += 4;
                    lexer->column += 4;
                }
                else {
                    int func_start = lexer->position;
                    while (lexer->position < lexer->length &&
                        (isalnum(lexer->input[lexer->position]) ||
                            lexer->input[lexer->position] == '_')) {
                        lexer->position++;
                        lexer->column++;
                    }

                    int func_length = lexer->position - func_start;
                    char* func_name = strndup(lexer->input + func_start, func_length);

                    skip_whitespace(lexer);
                    if (lexer->input[lexer->position] == '(') {
                        add_token(lexer, TOKEN_IDENTIFIER, func_name, func_length);
                        lexer->position++;
                        lexer->column++;
                        int depth = 1;
                        while (lexer->position < lexer->length && depth > 0) {
                            if (lexer->input[lexer->position] == '(') depth++;
                            else if (lexer->input[lexer->position] == ')') depth--;
                            lexer->position++;
                            lexer->column++;
                        }
                    }
                    else {
                        add_token(lexer, TOKEN_IDENTIFIER, func_name, func_length);
                    }
                    free(func_name);
                }
            }
            else {
                add_error(lexer, "Unexpected character in initialization");
                return;
            }
        }
    }
}

void parse_macro(Lexer* lexer) {
    lexer->position++;
    lexer->column++;

    if (strncmp(lexer->input + lexer->position, "inclib", 6) == 0) {
        lexer->position += 6;
        lexer->column += 6;

        skip_whitespace(lexer);

        if (lexer->input[lexer->position] != '(') {
            add_error(lexer, "Expected '(' after #inclib");
            return;
        }

        lexer->position++;
        lexer->column++;
        skip_whitespace(lexer);

        int start = lexer->position;
        while (lexer->position < lexer->length &&
            lexer->input[lexer->position] != ')') {
            lexer->position++;
            lexer->column++;
        }

        if (lexer->position >= lexer->length) {
            add_error(lexer, "Unclosed #inclib");
            return;
        }

        int length = lexer->position - start;
        add_token(lexer, TOKEN_PREPROC_INCLIB, lexer->input + start, length);

        lexer->position++;
        lexer->column++;
    }
    else if (strncmp(lexer->input + lexer->position, "incfile", 7) == 0) {
        lexer->position += 7;
        lexer->column += 7;

        skip_whitespace(lexer);

        if (lexer->input[lexer->position] != '(') {
            add_error(lexer, "Expected '(' after #incfile");
            return;
        }

        lexer->position++;
        lexer->column++;
        skip_whitespace(lexer);

        int start = lexer->position;
        while (lexer->position < lexer->length &&
            lexer->input[lexer->position] != ')') {
            lexer->position++;
            lexer->column++;
        }

        if (lexer->position >= lexer->length) {
            add_error(lexer, "Unclosed #incfile");
            return;
        }

        int length = lexer->position - start;
        add_token(lexer, TOKEN_PREPROC_INCFILE, lexer->input + start, length);

        lexer->position++;
        lexer->column++;
    }
    else if (strncmp(lexer->input + lexer->position, "define", 6) == 0) {
        lexer->position += 6;
        lexer->column += 6;
        add_token(lexer, TOKEN_PREPROC_DEFINE, "#define", 7);
    }
    else if (strncmp(lexer->input + lexer->position, "undef", 5) == 0) {
        lexer->position += 5;
        lexer->column += 5;
        add_token(lexer, TOKEN_PREPROC_UNDEF, "#undef", 6);
    }
    else if (strncmp(lexer->input + lexer->position, "ifdef", 5) == 0) {
        lexer->position += 5;
        lexer->column += 5;
        add_token(lexer, TOKEN_PREPROC_IFDEF, "#ifdef", 6);
    }
    else if (strncmp(lexer->input + lexer->position, "ifndef", 6) == 0) {
        lexer->position += 6;
        lexer->column += 6;
        add_token(lexer, TOKEN_PREPROC_IFNDEF, "#ifndef", 7);
    }
    else if (strncmp(lexer->input + lexer->position, "endif", 5) == 0) {
        lexer->position += 5;
        lexer->column += 5;
        add_token(lexer, TOKEN_PREPROC_ENDIF, "#endif", 6);
    }
    else if (strncmp(lexer->input + lexer->position, "line", 4) == 0) {
        lexer->position += 4;
        lexer->column += 4;
        add_token(lexer, TOKEN_PREPROC_LINE, "#line", 5);
    }
    else if (strncmp(lexer->input + lexer->position, "error", 5) == 0) {
        lexer->position += 5;
        lexer->column += 5;
        add_token(lexer, TOKEN_PREPROC_ERROR, "#error", 6);
    }
    else if (strncmp(lexer->input + lexer->position, "pragma", 6) == 0) {
        lexer->position += 6;
        lexer->column += 6;
        add_token(lexer, TOKEN_PREPROC_PRAGMA, "#pragma", 7);
    }
    else {
        int start = lexer->position;
        while (lexer->position < lexer->length &&
            lexer->input[lexer->position] != '\n') {
            lexer->position++;
            lexer->column++;
        }
        int length = lexer->position - start;
        add_token(lexer, TOKEN_PREPROC_MACRO, lexer->input + start, length);
    }
}

void tokenize(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        skip_whitespace(lexer);
        skip_comments(lexer);

        if (lexer->position >= lexer->length) break;

        char character = lexer->input[lexer->position];
        char next = (lexer->position < lexer->length - 1) ? lexer->input[lexer->position + 1] : '\0';
        char double_next = (lexer->position < lexer->length - 2) ? lexer->input[lexer->position + 2] : '\0';

        if (character == '+' && next == '+') {
            add_token(lexer, TOKEN_DOUBLE_PLUS, "++", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '+' && next == '=') {
            add_token(lexer, TOKEN_PLUS_EQ, "+=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '-' && next == '-') {
            add_token(lexer, TOKEN_DOUBLE_MINUS, "--", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '-' && next == '=') {
            add_token(lexer, TOKEN_MINUS_EQ, "-=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '=' && next == '=') {
            add_token(lexer, TOKEN_DOUBLE_EQ, "==", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '=' && next == '=' && double_next == '=') {
            add_token(lexer, TOKEN_TRIPLE_EQ, "===", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        }
        else if (character == '!' && next == '=') {
            add_token(lexer, TOKEN_NE, "!=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '-' && next == '>') {
            add_token(lexer, TOKEN_ARROW, "->", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '=' && next == '=' && double_next == '>') {
            add_token(lexer, TOKEN_ROR, "==>", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        }
        else if (character == '>' && next == '>' && double_next == '>') {
            add_token(lexer, TOKEN_SAR, ">>>", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        }
        else if (character == '>' && next == '>') {
            add_token(lexer, TOKEN_SHR, ">>", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '>' && next == '=') {
            add_token(lexer, TOKEN_GE, ">=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '<' && next == '<' && double_next == '<') {
            add_token(lexer, TOKEN_SAL, "<<<", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        }
        else if (character == '<' && next == '=' && double_next == '=') {
            add_token(lexer, TOKEN_ROL, "<==", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        }
        else if (character == '<' && next == '<') {
            add_token(lexer, TOKEN_SHL, "<<", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '<' && next == '=') {
            add_token(lexer, TOKEN_LE, "<=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '&' && next == '&') {
            add_token(lexer, TOKEN_DOUBLE_AND, "&&", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '|' && next == '|') {
            add_token(lexer, TOKEN_DOUBLE_PIPE, "||", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '*' && next == '*') {
            add_token(lexer, TOKEN_DOUBLE_STAR, "**", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '*' && next == '=') {
            add_token(lexer, TOKEN_STAR_EQ, "*=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '/' && next == '=') {
            add_token(lexer, TOKEN_SLASH_EQ, "/=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '%' && next == '=') {
            add_token(lexer, TOKEN_PERCENT_EQ, "%=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '|' && next == '=') {
            add_token(lexer, TOKEN_PIPE_EQ, "|=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '&' && next == '=') {
            add_token(lexer, TOKEN_AMPERSAND_EQ, "&=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '^' && next == '=') {
            add_token(lexer, TOKEN_CARET_EQ, "^=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        else if (character == '~' && next == '=') {
            add_token(lexer, TOKEN_TILDE_EQ, "~=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }

        switch (character) {
        case '$': {
            int save_pos = lexer->position;
            int save_line = lexer->line;
            int save_col = lexer->column;

            int temp_pos = save_pos + 1;
            int temp_col = save_col + 1;
            int temp_line = save_line;

            while (temp_pos < lexer->length &&
                (lexer->input[temp_pos] == ' ' || lexer->input[temp_pos] == '\t')) {
                temp_pos++;
                temp_col++;
            }

            if (temp_pos >= lexer->length) {
                add_token(lexer, TOKEN_DOLLAR, "$", 1);
                lexer->position++;
                lexer->column++;
                break;
            }

            char first_char = lexer->input[temp_pos];
            bool is_declaration = false;

            if (first_char == '[') {
                is_declaration = true;
            }
            else if (isalpha(first_char) || first_char == '_') {
                int word_start = temp_pos;
                while (temp_pos < lexer->length &&
                    (isalnum(lexer->input[temp_pos]) || lexer->input[temp_pos] == '_')) {
                    temp_pos++;
                    temp_col++;
                }
                int word_len = temp_pos - word_start;
                char* word = strndup(lexer->input + word_start, word_len);

                if (is_valid_modifier(word)) {
                    is_declaration = true;
                }
                else if (is_valid_type(word)) {
                    is_declaration = true;
                }
                free(word);
            }

            if (is_declaration) {
                parse_variable_declaration(lexer);
            }
            else {
                parse_variable_access(lexer);
            }
            break;
        }

        case '@':
            add_token(lexer, TOKEN_AT, "@", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '{':
            add_token(lexer, TOKEN_LCURLY, "{", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '}':
            add_token(lexer, TOKEN_RCURLY, "}", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '[':
            add_token(lexer, TOKEN_LBRACKET, "[", 1);
            lexer->position++;
            lexer->column++;
            break;

        case ']':
            add_token(lexer, TOKEN_RBRACKET, "]", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '(':
            add_token(lexer, TOKEN_LPAREN, "(", 1);
            lexer->position++;
            lexer->column++;
            break;

        case ')':
            add_token(lexer, TOKEN_RPAREN, ")", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '=':
            add_token(lexer, TOKEN_EQUAL, "=", 1);
            lexer->position++;
            lexer->column++;
            break;

        case ',':
            add_token(lexer, TOKEN_COMMA, ",", 1);
            lexer->position++;
            lexer->column++;
            break;

        case ';':
            add_token(lexer, TOKEN_SEMICOLON, ";", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '\'':
            parse_char(lexer);
            break;

        case '"':
            parse_string(lexer);
            break;

        case '_':
            if (lexer->input[lexer->position + 1] != '_') {
                add_token(lexer, TOKEN_UNDERSCORE, "_", 1);
                lexer->position++;
                lexer->column++;
            } else if (lexer->position < lexer->length - 1 &&
                lexer->input[lexer->position + 1] == '_') {
                add_token(lexer, TOKEN_DOUBLE_UNDERSCORE, "__", 2);
                lexer->position += 2;
                lexer->column += 2;
            }
            break;

        case ':':
            if (lexer->position < lexer->length - 1 &&
                lexer->input[lexer->position + 1] == ':') {
                add_token(lexer, TOKEN_DOUBLE_COLON, "::", 2);
                lexer->position += 2;
                lexer->column += 2;
            }
            else {
                add_token(lexer, TOKEN_COLON, ":", 1);
                lexer->position++;
                lexer->column++;
                
                skip_whitespace(lexer);
                if (isalpha(lexer->input[lexer->position]) || 
                    lexer->input[lexer->position] == '[') {
                    parse_return_type(lexer);
                }
            }
            break;

        case '.':
            if (lexer->position < lexer->length - 2 &&
                lexer->input[lexer->position + 1] == '.' &&
                lexer->input[lexer->position + 2] == '.') {
                add_token(lexer, TOKEN_ELLIPSIS, "...", 3);
                lexer->position += 3;
                lexer->column += 3;
            }
            else if (isdigit(lexer->input[lexer->position + 1])) {
                parse_number(lexer);
            }
            else {
                add_error(lexer, "Unexpected dot");
                lexer->position++;
                lexer->column++;
            }
            break;

        case '#':
            parse_macro(lexer);
            break;

        case '+':
            add_token(lexer, TOKEN_PLUS, "+", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '-':
            add_token(lexer, TOKEN_MINUS, "-", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '*':
            add_token(lexer, TOKEN_STAR, "*", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '/':
            add_token(lexer, TOKEN_SLASH, "/", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '%':
            add_token(lexer, TOKEN_PERCENT, "%", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '|':
            add_token(lexer, TOKEN_PIPE, "|", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '&':
            add_token(lexer, TOKEN_REFERENCE, "&", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '!':
            add_token(lexer, TOKEN_BANG, "!", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '^':
            add_token(lexer, TOKEN_CARET, "^", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '>':
            add_token(lexer, TOKEN_GT, ">", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '<':
            add_token(lexer, TOKEN_LT, "<", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '~':
            add_token(lexer, TOKEN_TILDE, "~", 1);
            lexer->position++;
            lexer->column++;
            break;

        case '?':
            add_token(lexer, TOKEN_QUESTION, "?", 1);
            lexer->position++;
            lexer->column++;
            break;

        default:
            if (isalpha(character) || character == '_') {
                int start = lexer->position;
                while (lexer->position < lexer->length &&
                    (isalnum(lexer->input[lexer->position]) ||
                        lexer->input[lexer->position] == '_')) {
                    lexer->position++;
                    lexer->column++;
                }
                int length = lexer->position - start;
                char* word = strndup(lexer->input + start, length);

                if (strcmp(word, "if") == 0) {
                    add_token(lexer, TOKEN_IF, word, length);
                }
                else if (strcmp(word, "free") == 0) {
                    add_token(lexer, TOKEN_FREE, word, length);
                }
                else if (strcmp(word, "elif") == 0) {
                    add_token(lexer, TOKEN_ELIF, word, length);
                }
                else if (strcmp(word, "else") == 0) {
                    add_token(lexer, TOKEN_ELSE, word, length);
                }
                else if (strcmp(word, "do") == 0) {
                    add_token(lexer, TOKEN_DO, word, length);
                }
                else if (strcmp(word, "compile") == 0) {
                    add_token(lexer, TOKEN_COMPILE, word, length);
                    skip_whitespace(lexer);
                    if (lexer->position >= lexer->length || lexer->input[lexer->position] != '(') {
                        add_error(lexer, "Expected '(' after 'compile'");
                    }
                    else {
                        lexer->position++;
                        lexer->column++;
                        skip_whitespace(lexer);
                        int start_arg = lexer->position;
                        while (lexer->position < lexer->length && lexer->input[lexer->position] != ')') {
                            lexer->position++;
                            lexer->column++;
                        }
                        if (lexer->position >= lexer->length) {
                            add_error(lexer, "Unclosed '(' in compile");
                        }
                        else {
                            int length_arg = lexer->position - start_arg;
                            add_token(lexer, TOKEN_OUTSIDE_COMPILE, lexer->input + start_arg, length_arg);
                            lexer->position++;
                            lexer->column++;
                        }
                    }
                    skip_whitespace(lexer);
                    if (lexer->position >= lexer->length || lexer->input[lexer->position] != '{') {
                        add_error(lexer, "Expected '{' after compile directive");
                    }
                    else {
                        lexer->position++;
                        lexer->column++;
                        int start_brace = lexer->position;
                        int brace_depth = 1;
                        while (lexer->position < lexer->length && brace_depth > 0) {
                            if (lexer->input[lexer->position] == '{') {
                                brace_depth++;
                            }
                            else if (lexer->input[lexer->position] == '}') {
                                brace_depth--;
                            }
                            lexer->position++;
                            lexer->column++;
                        }
                        if (brace_depth != 0) {
                            add_error(lexer, "Unclosed '{' in compile");
                        }
                        else {
                            int length_code = (lexer->position - start_brace) - 1;
                            if (length_code > 0) {
                                add_token(lexer, TOKEN_OUTSIDE_CODE, lexer->input + start_brace, length_code);
                            }
                            else {
                                add_token(lexer, TOKEN_OUTSIDE_CODE, "", 0);
                            }
                        }
                    }
                }
                else if (strcmp(word, "break") == 0) {
                    add_token(lexer, TOKEN_BREAK, word, length);
                }
                else if (strcmp(word, "quit") == 0) {
                    add_token(lexer, TOKEN_QUIT, word, length);
                }
                else if (strcmp(word, "return") == 0) {
                    add_token(lexer, TOKEN_RETURN, word, length);
                }
                else if (strcmp(word, "continue") == 0) {
                    add_token(lexer, TOKEN_CONTINUE, word, length);
                }
                else if (strcmp(word, "NONE") == 0) {
                    add_token(lexer, TOKEN_NONE, word, length);
                }
                else if (is_valid_type(word)) {
                    add_token(lexer, TOKEN_TYPE, word, length);
                }
                else {
                    add_token(lexer, TOKEN_IDENTIFIER, word, length);
                }
                free(word);
            }
            else if (isdigit(character) || character == '-' || character == '+') {
                parse_number(lexer);
            }
            else {
                add_error(lexer, "Unexpected character: '%c'", character);
                lexer->position++;
                lexer->column++;
            }
            break;
        }
    }

    add_token(lexer, TOKEN_EOF, "EOF", 3);
}

Token* read_tokens_from_file(const char* filename, int* token_count) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open token file");
        return NULL;
    }

    TokenFileHeader header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return NULL;
    }

    if (strncmp(header.signature, "PAXT", 4) != 0) {
        fprintf(stderr, "Invalid file format\n");
        fclose(file);
        return NULL;
    }

    Token* tokens = malloc(header.token_count * sizeof(Token));
    if (!tokens) {
        fclose(file);
        return NULL;
    }

    for (int i = 0; i < header.token_count; i++) {
        uint32_t type, value_len, line, column, length;
        
        fread(&type, sizeof(uint32_t), 1, file);
        fread(&value_len, sizeof(uint32_t), 1, file);
        
        tokens[i].value = malloc(value_len + 1);
        fread(tokens[i].value, 1, value_len, file);
        tokens[i].value[value_len] = '\0';
        
        fread(&line, sizeof(uint32_t), 1, file);
        fread(&column, sizeof(uint32_t), 1, file);
        fread(&length, sizeof(uint32_t), 1, file);
        
        tokens[i].type = (TokenType)type;
        tokens[i].line = line;
        tokens[i].column = column;
        tokens[i].length = length;
    }
    
    fclose(file);
    *token_count = header.token_count;
    return tokens;
}

void free_tokens(Token* tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage:\n");
        printf("  %s tokenize <source_file.txt>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "tokenize") == 0) {
        FILE* file = fopen(argv[2], "r");
        if (file == NULL) {
            perror("Couldn't open the file");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        rewind(file);

        char* buffer_file = (char*)malloc(file_size + 1);
        if (buffer_file == NULL) {
            perror("Insufficient memory error");
            fclose(file);
            return 1;
        }

        size_t bytes_read = fread(buffer_file, 1, file_size, file);
        if (bytes_read != (size_t)file_size) {
            perror("Failed to read file");
            free(buffer_file);
            fclose(file);
            return 1;
        }
        buffer_file[file_size] = '\0';
        fclose(file);

        Lexer* lexer = init_lexer(buffer_file);
        tokenize(lexer);

        printf("Line  Col  Type               Value\n");
        printf("-----------------------------------\n");
        for (int i = 0; i < lexer->token_count; i++) {
            Token token = lexer->tokens[i];
            printf("%-4d  %-4d %-18s %s\n",
                token.line, token.column,
                token_names[token.type], token.value);
        }

        free_lexer(lexer);
        free(buffer_file);
    }
    return 0;
}