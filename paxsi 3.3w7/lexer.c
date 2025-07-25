#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

// Macro to shift lexer position forward by 'num' characters
// Also updates column position accordingly
#define SHIFT(lexer, num) { \
    lexer->position += (num); \
    lexer->column += (num); \
}

// Macro to look ahead 'num' characters without consuming them
// Returns '\0' if out of bounds
#define NEXT(lexer, num) ((lexer->position < lexer->length - num) ? lexer->input[lexer->position + num] : '\0')

// File header structure for token binary files
typedef struct {
    char signature[4];      // File identifier signature
    uint16_t version;       // File format version
    uint16_t reserved;      // Reserved for future use
    uint32_t token_count;   // Number of tokens in file
} TokenFileHeader;

// Enumeration of all possible token types
typedef enum {
    // Data types and literals
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_CHAR,
    TOKEN_NONE,
    TOKEN_STRING,
    
    // Control flow keywords
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_DO,
    TOKEN_RETURN,
    TOKEN_FREE,
    TOKEN_BREAK,
    TOKEN_GOTO,
    TOKEN_CONTINUE,
    TOKEN_COMPILE,
    
    // Preprocessor and special tokens
    TOKEN_PERCENT,
    TOKEN_PREPROC_MACRO,
    TOKEN_PREPROC_INCLIB,
    TOKEN_PREPROC_INCFILE,
    TOKEN_PREPROC_DEFINE,
    TOKEN_PREPROC_ASSIGN,
    TOKEN_PREPROC_UNDEF,
    TOKEN_PREPROC_IFDEF,
    TOKEN_PREPROC_IFNDEF,
    TOKEN_PREPROC_ENDIF,
    TOKEN_PREPROC_LINE,
    TOKEN_PREPROC_ERROR,
    TOKEN_PREPROC_PRAGMA,
    TOKEN_OUTSIDE_COMPILE,
    TOKEN_OUTSIDE_CODE,
    
    // Operators and punctuation
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_DOUBLE_STAR,
    TOKEN_DOUBLE_AMPERSAND,
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
    TOKEN_AT,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_GE,
    TOKEN_LE,
    TOKEN_DOUBLE_EQ,
    TOKEN_NE,
    TOKEN_THIS,
    TOKEN_EQUAL,
    TOKEN_PLUS_EQ,
    TOKEN_MINUS_EQ,
    TOKEN_STAR_EQ,
    TOKEN_SLASH_EQ,
    TOKEN_PIPE_EQ,
    TOKEN_AMPERSAND_EQ,
    TOKEN_CARET_EQ,
    TOKEN_TILDE_EQ,
    TOKEN_DOUBLE_PLUS,
    TOKEN_DOUBLE_MINUS,
    TOKEN_DOLLAR,
    TOKEN_COLON,
    TOKEN_ELLIPSIS,
    TOKEN_DOUBLE_DOT,
    TOKEN_DOT,
    TOKEN_DOUBLE_COLON,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_QUESTION,
    TOKEN_UNDERSCORE,
    TOKEN_DOUBLE_UNDERSCORE,
    
    // Brackets and braces
    TOKEN_LCURLY,
    TOKEN_RCURLY,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    
    // Type system tokens
    TOKEN_MODIFIER,
    TOKEN_SIZE,
    TOKEN_PARSE,
    TOKEN_DELETE,
    TOKEN_MALLOC,
    TOKEN_EALLOC,
    TOKEN_RALLOC,
    TOKEN_ALLOC,
    TOKEN_TYPE,
    TOKEN_VAR_SIZE,
    
    // Special tokens
    TOKEN_ID,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

// Mapping of token types to their string names
const char* token_names[] = {
    [TOKEN_INT]                 =   "INT",
    [TOKEN_REAL]                =   "REAL",
    [TOKEN_CHAR]                =   "CHAR",
    [TOKEN_NONE]                =   "NONE",
    [TOKEN_STRING]              =   "STRING",
    [TOKEN_IF]                  =   "IF",
    [TOKEN_ELIF]                =   "ELIF",
    [TOKEN_ELSE]                =   "ELSE",
    [TOKEN_DO]                  =   "DO",
    [TOKEN_RETURN]              =   "RETURN",
    [TOKEN_FREE]                =   "FREE",
    [TOKEN_BREAK]               =   "BREAK",
    [TOKEN_GOTO]                =   "GOTO",
    [TOKEN_CONTINUE]            =   "CONTINUE",
    [TOKEN_COMPILE]             =   "COMPILE",
    [TOKEN_PERCENT]             =   "PERCENT",
    [TOKEN_PREPROC_MACRO]       =   "PREPROC_MACRO",
    [TOKEN_PREPROC_INCLIB]      =   "PREPROC_INCLIB",
    [TOKEN_PREPROC_INCFILE]     =   "PREPROC_INCFILE",
    [TOKEN_PREPROC_DEFINE]      =   "PREPROC_DEFINE",
    [TOKEN_PREPROC_ASSIGN]      =   "PREPROC_ASSIGN",
    [TOKEN_PREPROC_UNDEF]       =   "PREPROC_UNDEF",
    [TOKEN_PREPROC_IFDEF]       =   "PREPROC_IFDEF",
    [TOKEN_PREPROC_IFNDEF]      =   "PREPROC_IFNDEF",
    [TOKEN_PREPROC_ENDIF]       =   "PREPROC_ENDIF",
    [TOKEN_PREPROC_LINE]        =   "PREPROC_LINE",
    [TOKEN_PREPROC_ERROR]       =   "PREPROC_ERROR",
    [TOKEN_PREPROC_PRAGMA]      =   "PREPROC_PRAGMA",
    [TOKEN_OUTSIDE_COMPILE]     =   "OUTSIDE_COMPILE",
    [TOKEN_OUTSIDE_CODE]        =   "OUTSIDE_CODE",
    [TOKEN_DOLLAR]              =   "DOLLAR",
    [TOKEN_COLON]               =   "COLON",
    [TOKEN_ELLIPSIS]            =   "ELLIPSIS",
    [TOKEN_DOUBLE_DOT]          =   "DOUBLE_DOT",
    [TOKEN_DOT]                 =   "DOT",
    [TOKEN_DOUBLE_COLON]        =   "DOUBLE_COLON",
    [TOKEN_UNDERSCORE]          =   "UNDERSCORE",
    [TOKEN_DOUBLE_UNDERSCORE]   =   "DOUBLE_UNDERSCORE",
    [TOKEN_MODIFIER]            =   "MODIFIER",
    [TOKEN_ID]                  =   "ID",
    [TOKEN_SEMICOLON]           =   "SEMICOLON",
    [TOKEN_THIS]                =   "THIS",
    [TOKEN_EQUAL]               =   "EQUAL",
    [TOKEN_COMMA]               =   "COMMA",
    [TOKEN_PLUS]                =   "PLUS",
    [TOKEN_MINUS]               =   "MINUS",
    [TOKEN_STAR]                =   "STAR",
    [TOKEN_SLASH]               =   "SLASH",
    [TOKEN_TILDE]               =   "TILDE",
    [TOKEN_PIPE]                =   "PIPE",
    [TOKEN_AMPERSAND]           =   "AMPERSAND",
    [TOKEN_BANG]                =   "BANG",
    [TOKEN_CARET]               =   "CARET",
    [TOKEN_AT]                  =   "AT",
    [TOKEN_GT]                  =   "GT",
    [TOKEN_LT]                  =   "LT",
    [TOKEN_SHR]                 =   "SHR",
    [TOKEN_SHL]                 =   "SHL",
    [TOKEN_SAR]                 =   "SAR",
    [TOKEN_SAL]                 =   "SAL",
    [TOKEN_ROR]                 =   "ROR",
    [TOKEN_ROL]                 =   "ROL",
    [TOKEN_GE]                  =   "GE",
    [TOKEN_LE]                  =   "LE",
    [TOKEN_DOUBLE_EQ]           =   "DOUBLE_EQ",
    [TOKEN_NE]                  =   "NE",
    [TOKEN_PLUS_EQ]             =   "PLUS_EQ",
    [TOKEN_MINUS_EQ]            =   "MINUS_EQ",
    [TOKEN_STAR_EQ]             =   "STAR_EQ",
    [TOKEN_SLASH_EQ]            =   "SLASH_EQ",
    [TOKEN_PIPE_EQ]             =   "PIPE_EQ",
    [TOKEN_AMPERSAND_EQ]        =   "AMPERSAND_EQ",
    [TOKEN_CARET_EQ]            =   "CARET_EQ",
    [TOKEN_TILDE_EQ]            =   "TILDE_EQ",
    [TOKEN_DOUBLE_PLUS]         =   "DOUBLE_PLUS",
    [TOKEN_DOUBLE_MINUS]        =   "DOUBLE_MINUS",
    [TOKEN_DOUBLE_AMPERSAND]    =   "DOUBLE_AMPERSAND",
    [TOKEN_DOUBLE_PIPE]         =   "DOUBLE_PIPE",
    [TOKEN_DOUBLE_STAR]         =   "DOUBLE_STAR",
    [TOKEN_QUESTION]            =   "QUESTION",
    [TOKEN_LCURLY]              =   "LCURLY",
    [TOKEN_RCURLY]              =   "RCURLY",
    [TOKEN_LBRACKET]            =   "LBRACKET",
    [TOKEN_RBRACKET]            =   "RBRACKET",
    [TOKEN_LBRACE]              =   "LBRACE",
    [TOKEN_RBRACE]              =   "RBRACE",
    [TOKEN_LPAREN]              =   "LPAREN",
    [TOKEN_RPAREN]              =   "RPAREN",
    [TOKEN_SIZE]                =   "SIZE",
    [TOKEN_PARSE]               =   "PARSE",
    [TOKEN_DELETE]              =   "DELETE",
    [TOKEN_MALLOC]              =   "MALLOC",
    [TOKEN_EALLOC]              =   "EALLOC",
    [TOKEN_RALLOC]              =   "RALLOC",
    [TOKEN_ALLOC]               =   "ALLOC",
    [TOKEN_TYPE]                =   "TYPE",
    [TOKEN_VAR_SIZE]            =   "VAR_SIZE",
    [TOKEN_EOF]                 =   "EOF",
    [TOKEN_ERROR]               =   "ERROR"
};

// Token structure definition
typedef struct {
    TokenType type;  // Type of the token
    char* value;     // String value of the token
    int line;        // Source line number
    int column;      // Starting column position
    int length;      // Length of token in characters
} Token;

// Lexer state structure
typedef struct {
    const char* input;        // Input source code
    int length;               // Length of input
    int position;             // Current position in input
    int line;                 // Current line number
    int column;               // Current column number
    Token* tokens;            // Array of tokens
    int token_count;          // Current number of tokens
    int token_capacity;       // Capacity of tokens array
} Lexer;

// Initialize lexer with source code input
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

// Free lexer and all allocated resources
void free_lexer(Lexer* lexer) {
    for (int i = 0; i < lexer->token_count; i++) {
        free(lexer->tokens[i].value);
    }
    free(lexer->tokens);
    free(lexer);
}

// Add a new token to the lexer's token list
void add_token(Lexer* lexer, TokenType type, const char* value, int length) {
    // Expand token array if needed
    if (lexer->token_count >= lexer->token_capacity) {
        lexer->token_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, lexer->token_capacity * sizeof(Token));
    }

    // Create new token
    Token token;
    token.type = type;
    token.value = malloc(length + 1);
    strncpy(token.value, value, length);
    token.value[length] = '\0';
    token.line = lexer->line;
    token.column = lexer->column - length;  // Adjust for current position
    token.length = length;

    // Add to token list
    lexer->tokens[lexer->token_count++] = token;
}

// Add an error token with formatted message
void add_error(Lexer* lexer, const char* format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    add_token(lexer, TOKEN_ERROR, buffer, strlen(buffer));
}

// Skip whitespace characters (space, tab)
void skip_whitespace(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        if (lexer->input[lexer->position] == ' ' || 
            lexer->input[lexer->position]  == '\t') { 
            SHIFT(lexer, 1);
        } else if (lexer->input[lexer->position] == '\n') {
            lexer->position++;
            lexer->line++;
            lexer->column = 1;
        } else break;
    }
}

// Skip comments and preprocessing directives
void skip_comments(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        // Single-line comments starting with #
        if (lexer->input[lexer->position] == '#') {
            while (lexer->position < lexer->length &&
                   lexer->input[lexer->position] != '\n') {
                lexer->position++;
            }
            lexer->column = 1;
        } 
        // Multi-line comments: </ ... />
        else if (lexer->position < lexer->length - 1 &&
                   lexer->input[lexer->position] == '<' && 
                   lexer->input[lexer->position + 1] == '/') {
            SHIFT(lexer, 2);
            int depth = 1;
            while (depth > 0 && lexer->position < lexer->length - 1) {
                if (lexer->input[lexer->position] == '<' &&
                    lexer->input[lexer->position + 1] == '/') {
                    depth++;
                    SHIFT(lexer, 2);
                } else if (lexer->input[lexer->position] == '/' &&
                           lexer->input[lexer->position + 1] == '>') {
                    depth--;
                    SHIFT(lexer, 2);
                } else {
                    if (lexer->input[lexer->position] == '\n') {
                        lexer->line++;
                        lexer->column = 1;
                    } else SHIFT(lexer, 1);
                }
            }
            if (depth > 0) add_error(lexer, "Unclosed comment");
        } else break;
    }
}

// Check if character is valid digit in given base
static bool is_valid_digit(char character, int base) {
    if (base <= 10) return character >= '0' && character < '0' + base;

    if (character >= '0' && character <= '9') return true;
    if (character >= 'A' && character <= 'A' + base - 11) return true;
    if (character >= 'a' && character <= 'a' + base - 11) return true;
    return false;
}

// Parse number literals (integers and floats)
void parse_number(Lexer* lexer) {
    int start = lexer->position;
    int base = 10;
    bool has_base = false;
    bool is_real = false;
    bool has_exponent = false;

    // Check for base prefixes (0x, 0d, 0o, etc.)
    if (lexer->position < lexer->length - 1 &&
        lexer->input[lexer->position] == '0') {
        bool valid_prefix = true;
        switch (NEXT(lexer, 1)) {
            case 'x': base = 16; break;
            case 'd': base = 10; break;
            case 'o': base = 8; break;
            case 'b': base = 2; break;
            case 'q': base = 4; break;
            default: valid_prefix = false;
        }
        if (valid_prefix) {
            SHIFT(lexer, 2);
            has_base = true;
        }
    }

    // Process number digits
    while (lexer->position < lexer->length) {
        if (NEXT(lexer, 0) == '.') is_real = true;
        else if (NEXT(lexer, 0) == '_') {SHIFT(lexer, 1);}
        else if (NEXT(lexer, 0) == 'e') { 
            is_real = true;
            has_exponent = true;
            // Skip exponent sign if present
            if (lexer->position + 1 < lexer->length) {
                if (NEXT(lexer, 1) == '+' || NEXT(lexer, 1) == '-') SHIFT(lexer, 1);
            }
        } else if (!is_valid_digit(NEXT(lexer, 0), base)) break;
        SHIFT(lexer, 1);
    }

    // Validate and add token
    int length = lexer->position - start;
    if (length == 0) {
        add_error(lexer, "Empty number literal");
        return;
    }
    
    TokenType type = is_real ? TOKEN_REAL : TOKEN_INT;
    add_token(lexer, type, lexer->input + start, length);
}

// Parse character literals
void parse_char(Lexer* lexer) {
    SHIFT(lexer, 1); // Skip opening quote
    char value = 0;

    // Check for unclosed character
    if (lexer->position >= lexer->length) {
        add_error(lexer, "Unclosed character literal");
        return;
    }

    // Handle escape sequences
    if (NEXT(lexer, 0) == '\\') {
        SHIFT(lexer, 1); 
        if (lexer->position >= lexer->length) {
            add_error(lexer, "Incomplete escape sequence");
            return;
        }
        switch (NEXT(lexer, 0)) {
            case 'n': value = '\n'; break;
            case 't': value = '\t'; break;
            case 'r': value = '\r'; break;
            case '0': value = '\0'; break;
            case '\'': value = '\''; break;
            case '"': value = '\"'; break;
            case '\\': value = '\\'; break;
            default: value = NEXT(lexer, 0);
        }
        SHIFT(lexer, 1);
    } else {
        value = NEXT(lexer, 0);
        SHIFT(lexer, 1);
    }

    // Check for closing quote
    if (lexer->position >= lexer->length || NEXT(lexer, 0) != '\'') {
        add_error(lexer, "Unclosed character literal");
        return;
    }

    // Add character token
    char str_val[2] = { value, '\0' };
    add_token(lexer, TOKEN_CHAR, str_val, 1);
    SHIFT(lexer, 1); // Skip closing quote
}

// Parse string literals
void parse_string(Lexer* lexer) {
    SHIFT(lexer, 1); // Skip opening quote
    int buf_size = 128;
    char* buffer = malloc(lexer->length - lexer->position + 1);
    int buf_index = 0;

    while (lexer->position < lexer->length) {
        // Expand buffer if needed
        if (buf_index >= buf_size - 1) {
            buf_size *= 2;
            buffer = realloc(buffer, buf_size);
        }

        // Handle escape sequences
        if (NEXT(lexer, 0) == '\\') {
            SHIFT(lexer, 1);
            if (lexer->position >= lexer->length) {
                add_error(lexer, "Incomplete escape sequence");
                free(buffer);
                return;
            }
            switch (NEXT(lexer, 0)) {
                case 'n': buffer[buf_index++] = '\n'; break;
                case 't': buffer[buf_index++] = '\t'; break;
                case 'r': buffer[buf_index++] = '\r'; break;
                case '0': buffer[buf_index++] = '\0'; break;
                case '\'': buffer[buf_index++] = '\''; break;
                case '"': buffer[buf_index++] = '"'; break;
                case '\\': buffer[buf_index++] = '\\'; break;
                default: buffer[buf_index++] = NEXT(lexer, 0);
            }
            SHIFT(lexer, 1);
        } 
        // Check for closing quote
        else if (NEXT(lexer, 0) == '"') break;
        // Check for newline in string (invalid)
        else if (NEXT(lexer, 0) == '\n') {
            add_error(lexer, "Unclosed string literal");
            free(buffer);
            return;
        } 
        // Normal character
        else {
            buffer[buf_index++] = NEXT(lexer, 0);
            SHIFT(lexer, 1);
        }
    }

    // Validate closing quote
    if (lexer->position >= lexer->length || NEXT(lexer, 0) != '"') {
        add_error(lexer, "Unclosed string literal");
        free(buffer);
        return;
    }

    // Add string token
    buffer[buf_index] = '\0';
    add_token(lexer, TOKEN_STRING, buffer, buf_index);
    free(buffer);
    SHIFT(lexer, 1); // Skip closing quote
}

// Check if identifier is a valid modifier
bool is_valid_modifier(const char* modifier) {
    const char* valid_modifiers[] = { 
        "const", "unsig", "signed", "extern", "static", 
        "protected", "dynam", "regis", "local", "global"
    };
    int count = sizeof(valid_modifiers) / sizeof(valid_modifiers[0]);
    for (int i = 0; i < count; i++) {
        if (strcmp(modifier, valid_modifiers[i]) == 0) return true;
    }
    return false;
}
 
// Check if identifier is a valid type
bool is_valid_type(const char* type) {
    const char* valid_types[] = { "int", "real", "char", "void" };
    int count = sizeof(valid_types) / sizeof(valid_types[0]);
    for (int i = 0; i < count; i++) {
        if (strcmp(type, valid_types[i]) == 0) return true;
    }
    return false;
}

// Main tokenization function
void tokenize(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        // Skip whitespace and comments before processing tokens
        skip_whitespace(lexer);
        skip_comments(lexer);

        if (lexer->position >= lexer->length) break;

        // Main token recognition switch
        switch (NEXT(lexer, 0)) {
            case '$': 
                add_token(lexer, TOKEN_DOLLAR, "$", 1);
                SHIFT(lexer, 1);
                break;

            case '@':
                add_token(lexer, TOKEN_AT, "@", 1);
                SHIFT(lexer, 1);
                break;
            
            case '?':
                add_token(lexer, TOKEN_QUESTION, "?", 1);
                SHIFT(lexer, 1);
                break;

            case 'i':
                if (strncmp(lexer->input + lexer->position, "if", 2) == 0) {
                    add_token(lexer, TOKEN_IF, "if", 2);
                    SHIFT(lexer, 2);
                } else goto identifier;
                break;

            case 'f':
                if (strncmp(lexer->input + lexer->position, "free", 4) == 0) {
                    add_token(lexer, TOKEN_FREE, "free", 4);
                    SHIFT(lexer, 4);
                } else goto identifier;
                break;

            case 'e':
                if (strncmp(lexer->input + lexer->position, "elif", 4) == 0) {
                    add_token(lexer, TOKEN_ELIF, "elif", 4);
                    SHIFT(lexer, 4);
                } else if (strncmp(lexer->input + lexer->position, "else", 4) == 0) {
                    add_token(lexer, TOKEN_ELSE, "else", 4);
                    SHIFT(lexer, 4);
                } else if (strncmp(lexer->input + lexer->position, "ealloc", 6) == 0) {
                    add_token(lexer, TOKEN_EALLOC, "ealloc", 6);
                    SHIFT(lexer, 6);
                } else goto identifier;
                break;

            case 't':
                if (strncmp(lexer->input + lexer->position, "this", 4) == 0) {
                    add_token(lexer, TOKEN_THIS, "this", 4);
                    SHIFT(lexer, 4);
                } else goto identifier;
                break;

            case 'm':
                if (strncmp(lexer->input + lexer->position, "malloc", 6) == 0) {
                    add_token(lexer, TOKEN_MALLOC, "malloc", 6);
                    SHIFT(lexer, 6);
                } else goto identifier;
                break;
        
            case 's':
                if (strncmp(lexer->input + lexer->position, "size", 4) == 0) {
                    add_token(lexer, TOKEN_SIZE, "size", 4);
                    SHIFT(lexer, 4);
                } else goto identifier;
                break;

            case 'g':
                if (strncmp(lexer->input + lexer->position, "goto", 4) == 0) {
                    add_token(lexer, TOKEN_GOTO, "goto", 4);
                    SHIFT(lexer, 4);
                } else goto identifier;
                break;


            case 'd':
                if (strncmp(lexer->input + lexer->position, "do", 2) == 0) {
                    add_token(lexer, TOKEN_DO, "do", 2);
                    SHIFT(lexer, 2);
                } else if (strncmp(lexer->input + lexer->position, "delete", 6) == 0) {
                    add_token(lexer, TOKEN_DELETE, "delete", 6);
                    SHIFT(lexer, 6);
                } else goto identifier;
                break;

            case 'c':
                if (strncmp(lexer->input + lexer->position, "continue", 8) == 0) {
                    add_token(lexer, TOKEN_CONTINUE, "continue", 8);
                    SHIFT(lexer, 8);
                } else if (strncmp(lexer->input + lexer->position, "compile", 7) == 0) {
                    add_token(lexer, TOKEN_COMPILE, "compile", 7);
                    SHIFT(lexer, 7);

                    skip_whitespace(lexer);

                    if (lexer->position >= lexer->length || NEXT(lexer, 0) != '(') add_error(lexer, "Expected '(' after 'compile'");
                    else {
                        SHIFT(lexer, 1);
                        skip_whitespace(lexer);

                        int start = lexer->position;
                        while (lexer->position < lexer->length && NEXT(lexer, 0) != ')') {
                            SHIFT(lexer, 1);
                        }

                        if (lexer->position >= lexer->length) add_error(lexer, "Unclosed '(' in compile");
                        else {
                            int length = lexer->position - start;
                            add_token(lexer, TOKEN_OUTSIDE_COMPILE, lexer->input + start, length);
                            SHIFT(lexer, 1);
                        }
                    }

                    skip_whitespace(lexer);

                    if (lexer->position >= lexer->length || NEXT(lexer, 0) != '{') add_error(lexer, "Expected '{' after compile directive");
                    else {
                        SHIFT(lexer, 1);
                        int start_brace = lexer->position;
                        int brace_depth = 1;
                        while (lexer->position < lexer->length && brace_depth > 0) {
                            if (NEXT(lexer, 0) == '{') brace_depth++;
                            else if (NEXT(lexer, 0) == '}') brace_depth--;
                            SHIFT(lexer, 1);
                        }

                        if (brace_depth != 0) add_error(lexer, "Unclosed '{' in compile");
                        else {
                            int length = (lexer->position - start_brace) - 1;
                            if (length > 0) add_token(lexer, TOKEN_OUTSIDE_CODE, lexer->input + start_brace, length);
                            else add_token(lexer, TOKEN_OUTSIDE_CODE, "", 0);
                        }
                    }
                } else goto identifier;
                break;

            case 'a':
                if (strncmp(lexer->input + lexer->position, "alloc", 5) == 0) {
                    add_token(lexer, TOKEN_ALLOC, "alloc", 5);
                    SHIFT(lexer, 5);
                } else goto identifier;
                break;
        
            case 'p':
                if (strncmp(lexer->input + lexer->position, "parse", 5) == 0) {
                    add_token(lexer, TOKEN_PARSE, "parse", 5);
                    SHIFT(lexer, 5);
                } else goto identifier;
                break; 

        case 'b':
            if (strncmp(lexer->input + lexer->position, "break", 5) == 0) {
                add_token(lexer, TOKEN_BREAK, "break", 5);
                SHIFT(lexer, 5);
            } else goto identifier;
            break;

        case '+':
            if (NEXT(lexer, 1) == '+') {
                add_token(lexer, TOKEN_DOUBLE_PLUS, "++", 2);
                SHIFT(lexer, 2);
            } else if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_PLUS_EQ, "+=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_PLUS, "+", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '-':
            if (NEXT(lexer, 1) == '-') {
                add_token(lexer, TOKEN_DOUBLE_MINUS, "--", 2);
                SHIFT(lexer, 2);
            } else if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_MINUS_EQ, "-=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_MINUS, "-", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '*':
            if (NEXT(lexer, 1) == '*') {
                add_token(lexer, TOKEN_DOUBLE_STAR, "**", 2);
                SHIFT(lexer, 2);
            } else if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_STAR_EQ, "*=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_STAR, "*", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '/':
            if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_SLASH_EQ, "/=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_SLASH, "/", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '|':
            if (NEXT(lexer, 1) == '|') {
                add_token(lexer, TOKEN_DOUBLE_PIPE, "||", 2);
                SHIFT(lexer, 2);
            } else if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_PIPE_EQ, "|=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_PIPE, "|", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '&':
            if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_AMPERSAND_EQ, "&=", 2);
                SHIFT(lexer, 2);
            } else if (NEXT(lexer, 1) == '&') {
                add_token(lexer, TOKEN_DOUBLE_AMPERSAND, "&&", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_AMPERSAND, "&", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '!':
            if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_NE, "!=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_BANG, "!", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '^':
            if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_CARET_EQ, "^=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_CARET, "^", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '>':
            if (strncmp(lexer->input + lexer->position, ">>>>", 4) == 0) {
                add_token(lexer, TOKEN_ROR, ">>>>", 4);
                SHIFT(lexer, 4);
            } else if (strncmp(lexer->input + lexer->position, ">>>", 3) == 0) {
                add_token(lexer, TOKEN_SAR, ">>>", 3);
                SHIFT(lexer, 3);
            } else if (NEXT(lexer, 1) == '>') {
                add_token(lexer, TOKEN_SHR, ">>", 2);
                SHIFT(lexer, 2);
            } else if (NEXT(lexer, 2) == '=') {
                add_token(lexer, TOKEN_GE, ">=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_GT, ">", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '<':
            if (strncmp(lexer->input + lexer->position, "<<<<", 4) == 0) {
                add_token(lexer, TOKEN_ROL, "<<<<", 4);
                SHIFT(lexer, 4);
            } else if (strncmp(lexer->input + lexer->position, "<<<", 3) == 0) {
                add_token(lexer, TOKEN_SAL, "<<<", 3);
                SHIFT(lexer, 3);
            } else if (NEXT(lexer, 1) == '<') {
                add_token(lexer, TOKEN_SHL, "<<", 2);
                SHIFT(lexer, 2);
            } else if (NEXT(lexer, 2) == '=') {
                add_token(lexer, TOKEN_LE, "<=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_LT, "<", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '~':
            if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_TILDE_EQ, "~=", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_TILDE, "~", 1);
                SHIFT(lexer, 1);
            }
            break;

        case ':':
            if (NEXT(lexer, 1) == ':') {
                add_token(lexer, TOKEN_DOUBLE_COLON, "::", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_COLON, ":", 1);
                SHIFT(lexer, 1);
                
                skip_whitespace(lexer);
                if (isalpha(NEXT(lexer, 0)) || 
                    NEXT(lexer, 0) == '[') {
                    if (NEXT(lexer, 0) == '[') {
                        add_token(lexer, TOKEN_LBRACKET, "[", 1);
                        SHIFT(lexer, 1);

                        while (lexer->position < lexer->length) {
                            skip_whitespace(lexer);

                            int mod_start = lexer->position;
                            while (lexer->position < lexer->length &&
                                isalpha(NEXT(lexer, 0))) {
                                SHIFT(lexer, 1);
                            }

                            if (lexer->position > mod_start) {
                                int length = lexer->position - mod_start;
                                char* modifier = strndup(lexer->input + mod_start, length);

                                if (is_valid_modifier(modifier)) add_token(lexer, TOKEN_MODIFIER, modifier, length);
                                else add_error(lexer, "Invalid modifier: %s", modifier);
                                free(modifier);
                            }

                            skip_whitespace(lexer);

                            if (NEXT(lexer, 0) == ',') {
                                add_token(lexer, TOKEN_COMMA, ",", 1);
                                SHIFT(lexer, 1);
                                skip_whitespace(lexer);
                            } else if (NEXT(lexer, 0) == ']') break;
                            else {
                                add_error(lexer, "Expected ',' or ']' in modifier list");
                                return;
                            }
                        }

                        if (lexer->position >= lexer->length || 
                            NEXT(lexer, 0) != ']') {
                            add_error(lexer, "Expected ']' after modifiers");
                            return;
                        }

                        add_token(lexer, TOKEN_RBRACKET, "]", 1);
                        SHIFT(lexer, 1);
                        skip_whitespace(lexer);
                    }

                    int token_start = lexer->position;
                    while (lexer->position < lexer->length &&
                        isalpha(NEXT(lexer, 0))) {
                        SHIFT(lexer, 1);
                    }
                    if (lexer->position > token_start) {
                        int length = lexer->position - token_start;
                        char* type_str = strndup(lexer->input + token_start, length);
                        if (is_valid_type(type_str)) add_token(lexer, TOKEN_TYPE, type_str, length);
                        else add_error(lexer, "Invalid type: %s", type_str);
                        free(type_str);
                    } else {
                        add_error(lexer, "Expected type after colon");
                        return;
                    }

                    if (lexer->position < lexer->length && NEXT(lexer, 0) == ':') {
                        add_token(lexer, TOKEN_COLON, ":", 1);
                        SHIFT(lexer, 1);
                        token_start = lexer->position;

                        while (lexer->position < lexer->length &&
                            isdigit(NEXT(lexer, 0))) {
                            SHIFT(lexer, 1);
                        }

                        if (lexer->position > token_start) {
                            int length = lexer->position - token_start;
                            add_token(lexer, TOKEN_VAR_SIZE, lexer->input + token_start, length);
                        } else {
                            add_error(lexer, "Expected bit size after ':'");
                            return;
                        }
                    }
                }
            }
            break;

        case '.':
            if (strncmp(lexer->input + lexer->position, "...", 3) == 0) {
                add_token(lexer, TOKEN_ELLIPSIS, "...", 3);
                SHIFT(lexer, 3);
    lexer->column++;
            } else if (strncmp(lexer->input + lexer->position, "..", 2) == 0) {
                add_token(lexer, TOKEN_DOUBLE_DOT, "..", 2);
                SHIFT(lexer, 2);
            } else if (strncmp(lexer->input + lexer->position, ".", 1) == 0) {
                add_token(lexer, TOKEN_DOT, ".", 1);
                SHIFT(lexer, 1);
            } else goto identifier;
            break;

        case 'r':
            if (strncmp(lexer->input + lexer->position, "return", 6) == 0) {
                add_token(lexer, TOKEN_RETURN, "return", 6);
                SHIFT(lexer, 6);
            } else if (strncmp(lexer->input + lexer->position, "ralloc", 6) == 0) {
                add_token(lexer, TOKEN_RALLOC, "ralloc", 6);
                SHIFT(lexer, 6);

            } else goto identifier;
            break;

        case '%':
            if (strncmp(lexer->input + lexer->position, "\%inclib", 7) == 0) {
                add_token(lexer, TOKEN_PREPROC_INCLIB, "inclib", 7);
                SHIFT(lexer, 7);
             } else if (strncmp(lexer->input + lexer->position, "\%incfile", 8) == 0) {
                add_token(lexer, TOKEN_PREPROC_INCFILE, "incfile", 8);
                SHIFT(lexer, 8);
            } else if (strncmp(lexer->input + lexer->position, "\%define", 7) == 0) {
                add_token(lexer, TOKEN_PREPROC_DEFINE, "define", 7);
                SHIFT(lexer, 7);
            } else if (strncmp(lexer->input + lexer->position, "\%assign", 7) == 0) {
                add_token(lexer, TOKEN_PREPROC_ASSIGN, "assign", 7);
                SHIFT(lexer, 7);
            } else if (strncmp(lexer->input + lexer->position, "\%undef", 6) == 0) {
                add_token(lexer, TOKEN_PREPROC_UNDEF, "undef", 6);
                SHIFT(lexer, 6);
            } else if (strncmp(lexer->input + lexer->position, "\%ifdef", 6) == 0) {
                add_token(lexer, TOKEN_PREPROC_IFDEF, "ifdef", 6);
                SHIFT(lexer, 6);
            } else if (strncmp(lexer->input + lexer->position, "\%ifndef", 6) == 0) {
                add_token(lexer, TOKEN_PREPROC_IFNDEF, "ifndef", 6);
                SHIFT(lexer, 6);
            } else if (strncmp(lexer->input + lexer->position, "\%endif", 6) == 0) {
                add_token(lexer, TOKEN_PREPROC_ENDIF, "endif", 6);
                SHIFT(lexer, 6);
            } else if (strncmp(lexer->input + lexer->position, "\%line", 5) == 0) {
                add_token(lexer, TOKEN_PREPROC_LINE, "line", 5);
                SHIFT(lexer, 5);
            } else if (strncmp(lexer->input + lexer->position, "\%error", 6) == 0) {
                add_token(lexer, TOKEN_PREPROC_ERROR, "error", 6);
                SHIFT(lexer, 6);
            } else if (strncmp(lexer->input + lexer->position, "\%pragma", 7) == 0) {
                add_token(lexer, TOKEN_PREPROC_PRAGMA, "pragma", 7);
                SHIFT(lexer, 7);
            } else if (strncmp(lexer->input + lexer->position, "\%macro", 6) == 0) {
                add_token(lexer, TOKEN_PREPROC_MACRO, "macro", 6);
                SHIFT(lexer, 6);
            } else {
                add_token(lexer, TOKEN_PERCENT, "%", 1);
                SHIFT(lexer, 1);
            }
            break;

        case '{':
            add_token(lexer, TOKEN_LCURLY, "{", 1);
            SHIFT(lexer, 1);
            break;

        case '}':
            add_token(lexer, TOKEN_RCURLY, "}", 1);
            SHIFT(lexer, 1);
            break;

        case '[':
            add_token(lexer, TOKEN_LBRACKET, "[", 1);
            SHIFT(lexer, 1);
            break;

        case ']':
            add_token(lexer, TOKEN_RBRACKET, "]", 1);
            SHIFT(lexer, 1);
            break;

        case '(':
            add_token(lexer, TOKEN_LPAREN, "(", 1);
            SHIFT(lexer, 1);
            break;

        case ')':
            add_token(lexer, TOKEN_RPAREN, ")", 1);
            SHIFT(lexer, 1);
            break;

        case '=':
            if (NEXT(lexer, 1) == '=') {
                add_token(lexer, TOKEN_DOUBLE_EQ, "==", 2);
                SHIFT(lexer, 2);
            } else {
                add_token(lexer, TOKEN_EQUAL, "=", 1);
                SHIFT(lexer, 1);
            }
            break;

        case ',':
            add_token(lexer, TOKEN_COMMA, ",", 1);
            SHIFT(lexer, 1);
            break;

        case ';':
            add_token(lexer, TOKEN_SEMICOLON, ";", 1);
            SHIFT(lexer, 1);
            break;

        case '\'':
            parse_char(lexer);
            break;

        case '"':
            parse_string(lexer);
            break;

        case '_':
            if (NEXT(lexer, 1) == '_') {
                    add_token(lexer, TOKEN_DOUBLE_UNDERSCORE, "__", 2);
                    SHIFT(lexer, 2);
                } else {
                    int start = lexer->position;
                    SHIFT(lexer, 1);
                    if (isalnum(NEXT(lexer, 0)) || 
                        NEXT(lexer, 0) == '_') {
                        while (lexer->position < lexer->length &&
                              (isalnum(NEXT(lexer, 0)) ||
                               NEXT(lexer, 0) == '_')) {
                            SHIFT(lexer, 1);
                        }
                        int length = lexer->position - start;
                        char* word = strndup(lexer->input + start, length);
                        if (!word) {
                            add_error(lexer, "Memory error");
                            return;
                        }
                        add_token(lexer, TOKEN_ID, word, length);
                        free(word);
                    } else add_token(lexer, TOKEN_UNDERSCORE, "_", 1);
                }
                break;

        case 'N':
            if (strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
                if (lexer->position + 4 < lexer->length) {
                    if (isalnum(NEXT(lexer, 4)) || 
                        NEXT(lexer, 4) == '_') {
                        goto identifier;
                    }
                }
                add_token(lexer, TOKEN_NONE, "NONE", 4);
                SHIFT(lexer, 4);
            } else goto identifier;
            break;

        default:
            if (isalpha(NEXT(lexer, 0)) || 
                NEXT(lexer, 0) == '_') {
            identifier:
                int start = lexer->position;
                while (lexer->position < lexer->length &&
                      (isalnum(NEXT(lexer, 0)) ||
                       NEXT(lexer, 0) == '_')) {
                    SHIFT(lexer, 1);
                }
                int length = lexer->position - start;
                char* word = strndup(lexer->input + start, length);
                if (!word) {
                    add_error(lexer, "Memory error");
                    return;
                }

                if (is_valid_type(word)) add_token(lexer, TOKEN_TYPE, word, length);
                else if (is_valid_modifier(word)) add_token(lexer, TOKEN_MODIFIER, word, length);
                else add_token(lexer, TOKEN_ID, word, length);
                free(word);
            } else if (isdigit(NEXT(lexer, 0)) || 
                        NEXT(lexer, 0) == '-' || 
                        NEXT(lexer, 0) == '+') 
                parse_number(lexer);
            else {
                add_error(lexer, "Unexpected character: '%c'", NEXT(lexer, 0));
                SHIFT(lexer, 1);
            }
            break;
        }
    }

    // Add EOF token after processing all input
    add_token(lexer, TOKEN_EOF, "EOF", 3);
}

// Read tokens from binary token file
Token* read_tokens_from_file(const char* filename, int* token_count) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open token file");
        return NULL;
    }

    // Read file header
    TokenFileHeader header;
    if (fread(&header, sizeof(header), 1, file) != 1) {
        fclose(file);
        return NULL;
    }

    // Validate file signature
    if (strncmp(header.signature, "PAXT", 4) != 0) {
        fprintf(stderr, "Invalid file format\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for tokens
    Token* tokens = malloc(header.token_count * sizeof(Token));
    if (!tokens) {
        fclose(file);
        return NULL;
    }

    // Read each token from file
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

// Free token array
void free_tokens(Token* tokens, int token_count) {
    for (int i = 0; i < token_count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
}

// Main function - entry point
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    // Open and read source file
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Couldn't open the file");
        return 1;
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate buffer and read file
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

    // Initialize lexer and tokenize
    Lexer* lexer = init_lexer(buffer_file);
    tokenize(lexer);

    // Find maximum line number for output organization
    int max_line = 0;
    for (int i = 0; i < lexer->token_count; i++) {
        if (lexer->tokens[i].line > max_line) {
            max_line = lexer->tokens[i].line;
        }
    }

    // Organize tokens by line
    Token*** lines = calloc(max_line + 1, sizeof(Token**));
    int* token_counts = calloc(max_line + 1, sizeof(int));
    int* capacities = calloc(max_line + 1, sizeof(int));

    for (int i = 0; i < lexer->token_count; i++) {
        Token* token = &lexer->tokens[i];
        if (token->type == TOKEN_EOF) continue;
        
        int line = token->line;
        if (line >= 0 && line <= max_line) {
            // Expand line storage if needed
            if (token_counts[line] >= capacities[line]) {
                capacities[line] = capacities[line] ? capacities[line] * 2 : 10;
                lines[line] = realloc(lines[line], capacities[line] * sizeof(Token*));
            }
            lines[line][token_counts[line]++] = token;
        }
    }

    // Print tokens grouped by line
    for (int line = 0; line <= max_line; line++) {
        if (token_counts[line] == 0) continue;
        
        printf("line %d: [", line);
        for (int i = 0; i < token_counts[line]; i++) {
            Token* token = lines[line][i];
            
            // Print token with value if applicable
            if (token->type == TOKEN_ID ||
                token->type == TOKEN_INT ||
                token->type == TOKEN_REAL ||
                token->type == TOKEN_STRING ||
                token->type == TOKEN_CHAR ||
                token->type == TOKEN_TYPE ||
                token->type == TOKEN_MODIFIER ||
                token->type == TOKEN_VAR_SIZE ||
                token->type == TOKEN_ERROR ||
                token->type == TOKEN_PREPROC_MACRO ||
                token->type == TOKEN_OUTSIDE_COMPILE ||
                token->type == TOKEN_OUTSIDE_CODE) {
                printf("%s:%s", token_names[token->type], token->value);
            } else printf("%s", token_names[token->type]);
            
            if (i < token_counts[line] - 1) printf(" ");
        }
        printf("]\n");
    }

    // Clean up resources
    for (int i = 0; i <= max_line; i++) {
        free(lines[i]);
    }
    free(lines);
    free(token_counts);
    free(capacities);
    free_lexer(lexer);
    free(buffer_file);
    return 0;
}
