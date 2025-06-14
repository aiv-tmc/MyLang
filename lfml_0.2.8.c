#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// Переименованы токены для лучшей читаемости
typedef enum {
    // Основные токены
    TOKEN_DOLLAR,
    TOKEN_NUM_SYSTEM,
    TOKEN_TYPE,
    TOKEN_COLON,
    TOKEN_BYTE_SIZE,
    TOKEN_IDENTIFIER,
    TOKEN_SEMICOLON,
    TOKEN_EQUAL,
    
    // Управляющие конструкции
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_DO,
    TOKEN_LCURLY,
    TOKEN_RCURLY,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    
    // Функции
    TOKEN_FUNCTION,
    TOKEN_START_FUNCTION,
    TOKEN_RETURN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    
    // Литералы
    TOKEN_INT,
    TOKEN_REAL,
    TOKEN_CHAR,
    TOKEN_BOOL,
    TOKEN_STRING,        // Добавлен строковый литерал
    TOKEN_NONE,          // Добавлено значение NONE для bool
    
    // Препроцессорные директивы (переименованы)
    TOKEN_PREPROC_MACRO,
    TOKEN_DOUBLE_COLON,
    TOKEN_ELLIPSIS,
    TOKEN_PREPROC_INCLIB,
    TOKEN_PREPROC_INCFILE,
    TOKEN_MODIFIER,
    
    // Операторы
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_TILDE,
    TOKEN_PIPE,
    TOKEN_AMPERSAND,
    TOKEN_BANG,
    TOKEN_CARET,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_SHR,
    TOKEN_SHL,
    TOKEN_SAR,
    TOKEN_SAL,
    TOKEN_ROR,
    TOKEN_ROL,
    TOKEN_GE,
    TOKEN_LE,
    TOKEN_EQ_EQ,
    TOKEN_NE,
    TOKEN_PLUS_EQ,
    TOKEN_MINUS_EQ,
    TOKEN_STAR_EQ,      // *=
    TOKEN_SLASH_EQ,     // /=
    TOKEN_PERCENT_EQ,   // %=
    TOKEN_PIPE_EQ,      // |=
    TOKEN_AMPERSAND_EQ, // &=
    TOKEN_CARET_EQ,     // ^=
    TOKEN_TILDE_EQ,     // ~=
    TOKEN_PLUS_PLUS,
    TOKEN_MINUS_MINUS,
    TOKEN_AND_AND,
    TOKEN_PIPE_PIPE,
    TOKEN_STAR_STAR,
    
    // Управляющие конструкции
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_DEFAULT,
    
    // Препроцессорные директивы (переименованы)
    TOKEN_PREPROC_DEFINE,
    TOKEN_PREPROC_UNDEF,
    TOKEN_PREPROC_IFDEF,
    TOKEN_PREPROC_IFNDEF,
    TOKEN_PREPROC_ENDIF,
    TOKEN_PREPROC_LINE,
    TOKEN_PREPROC_ERROR,
    TOKEN_PREPROC_PRAGMA,
    
    // Ключевые слова
    TOKEN_BREAK,        // Добавлен break
    TOKEN_CONTINUE,     // Добавлен continue
    TOKEN_QUIT,         // Добавлен quit
    
    // Символы
    TOKEN_QUESTION,
    
    // Специальные
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

// Обновленный массив имен токенов с новыми и переименованными
const char *token_names[] = {
    [TOKEN_DOUBLE_COLON] = "DOUBLE_COLON",
    [TOKEN_ELLIPSIS] = "ELLIPSIS",
    [TOKEN_PREPROC_INCLIB] = "PREPROC_INCLIB",
    [TOKEN_PREPROC_INCFILE] = "PREPROC_INCFILE",
    [TOKEN_MODIFIER] = "MODIFIER",
    [TOKEN_DOLLAR] = "DOLLAR",
    [TOKEN_NUM_SYSTEM] = "NUM_SYSTEM",
    [TOKEN_TYPE] = "TYPE",
    [TOKEN_COLON] = "COLON",
    [TOKEN_BYTE_SIZE] = "BYTE_SIZE",
    [TOKEN_IDENTIFIER] = "IDENTIFIER",
    [TOKEN_SEMICOLON] = "SEMICOLON",
    [TOKEN_EQUAL] = "EQUAL",
    [TOKEN_IF] = "IF",
    [TOKEN_ELIF] = "ELIF",
    [TOKEN_ELSE] = "ELSE",
    [TOKEN_DO] = "DO",
    [TOKEN_LCURLY] = "LCURLY",
    [TOKEN_RCURLY] = "RCURLY",
    [TOKEN_LBRACKET] = "LBRACKET",
    [TOKEN_RBRACKET] = "RBRACKET",
    [TOKEN_LBRACE] = "LBRACE",
    [TOKEN_RBRACE] = "RBRACE",
    [TOKEN_FUNCTION] = "FUNCTION",
    [TOKEN_START_FUNCTION] = "START_FUNCTION",
    [TOKEN_RETURN] = "RETURN",
    [TOKEN_LPAREN] = "LPAREN",
    [TOKEN_RPAREN] = "RPAREN",
    [TOKEN_COMMA] = "COMMA",
    [TOKEN_INT] = "INT",
    [TOKEN_REAL] = "REAL",
    [TOKEN_CHAR] = "CHAR",
    [TOKEN_BOOL] = "BOOL",
    [TOKEN_STRING] = "STRING",
    [TOKEN_NONE] = "NONE",
    [TOKEN_PREPROC_MACRO] = "PREPROC_MACRO",
    [TOKEN_PLUS] = "PLUS",
    [TOKEN_MINUS] = "MINUS",
    [TOKEN_STAR] = "STAR",
    [TOKEN_SLASH] = "SLASH",
    [TOKEN_PERCENT] = "PERCENT",
    [TOKEN_TILDE] = "TILDE",
    [TOKEN_PIPE] = "PIPE",
    [TOKEN_AMPERSAND] = "AMPERSAND",
    [TOKEN_BANG] = "BANG",
    [TOKEN_CARET] = "CARET",
    [TOKEN_GT] = "GT",
    [TOKEN_LT] = "LT",
    [TOKEN_SHR] = "SHR",
    [TOKEN_SHL] = "SHL",
    [TOKEN_SAR] = "SAR",
    [TOKEN_SAL] = "SAL",
    [TOKEN_ROR] = "ROR",
    [TOKEN_ROL] = "ROL",
    [TOKEN_GE] = "GE",
    [TOKEN_LE] = "LE",
    [TOKEN_EQ_EQ] = "EQ_EQ",
    [TOKEN_NE] = "NE",
    [TOKEN_PLUS_EQ] = "PLUS_EQ",
    [TOKEN_MINUS_EQ] = "MINUS_EQ",
    [TOKEN_STAR_EQ] = "STAR_EQ",
    [TOKEN_SLASH_EQ] = "SLASH_EQ",
    [TOKEN_PERCENT_EQ] = "PERCENT_EQ",
    [TOKEN_PIPE_EQ] = "PIPE_EQ",
    [TOKEN_AMPERSAND_EQ] = "AMPERSAND_EQ",
    [TOKEN_CARET_EQ] = "CARET_EQ",
    [TOKEN_TILDE_EQ] = "TILDE_EQ",
    [TOKEN_PLUS_PLUS] = "PLUS_PLUS",
    [TOKEN_MINUS_MINUS] = "MINUS_MINUS",
    [TOKEN_AND_AND] = "AND_AND",
    [TOKEN_PIPE_PIPE] = "PIPE_PIPE",
    [TOKEN_STAR_STAR] = "STAR_STAR",
    [TOKEN_SWITCH] = "SWITCH",
    [TOKEN_CASE] = "CASE",
    [TOKEN_DEFAULT] = "DEFAULT",
    [TOKEN_PREPROC_DEFINE] = "PREPROC_DEFINE",
    [TOKEN_PREPROC_UNDEF] = "PREPROC_UNDEF",
    [TOKEN_PREPROC_IFDEF] = "PREPROC_IFDEF",
    [TOKEN_PREPROC_IFNDEF] = "PREPROC_IFNDEF",
    [TOKEN_PREPROC_ENDIF] = "PREPROC_ENDIF",
    [TOKEN_PREPROC_LINE] = "PREPROC_LINE",
    [TOKEN_PREPROC_ERROR] = "PREPROC_ERROR",
    [TOKEN_PREPROC_PRAGMA] = "PREPROC_PRAGMA",
    [TOKEN_BREAK] = "BREAK",
    [TOKEN_CONTINUE] = "CONTINUE",
    [TOKEN_QUIT] = "QUIT",
    [TOKEN_QUESTION] = "QUESTION",
    [TOKEN_EOF] = "EOF",
    [TOKEN_ERROR] = "ERROR"
};

// Структура токена
typedef struct {
    TokenType type;
    char *value;
    int line;
    int column;
    int length;
} Token;

// Структура лексера
typedef struct {
    const char *input;
    int length;
    int position;
    int line;
    int column;
    Token *tokens;
    int token_count;
    int token_capacity;
} Lexer;

// Инициализация лексера
Lexer* init_lexer(const char *input) {
    Lexer *lexer = malloc(sizeof(Lexer));
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

// Освобождение памяти лексера
void free_lexer(Lexer *lexer) {
    for (int i = 0; i < lexer->token_count; i++) {
        free(lexer->tokens[i].value);
    }
    free(lexer->tokens);
    free(lexer);
}

// Добавление токена
void add_token(Lexer *lexer, TokenType type, const char *value, int length) {
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

// Добавление токена ошибки
void add_error(Lexer *lexer, const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    add_token(lexer, TOKEN_ERROR, buffer, strlen(buffer));
}

// Пропуск пробельных символов
void skip_whitespace(Lexer *lexer) {
    while (lexer->position < lexer->length) {
        char c = lexer->input[lexer->position];
        
        if (c == ' ' || c == '\t') {
            lexer->position++;
            lexer->column++;
        } else if (c == '\n') {
            lexer->position++;
            lexer->line++;
            lexer->column = 1;
        } else {
            break;
        }
    }
}

// Пропуск комментариев
void skip_comments(Lexer *lexer) {
    if (lexer->position < lexer->length - 1) {
        // Однострочный комментарий
        if (lexer->input[lexer->position] == '/' && 
            lexer->input[lexer->position + 1] == '/') {
            while (lexer->position < lexer->length && 
                   lexer->input[lexer->position] != '\n') {
                lexer->position++;
            }
            lexer->column = 1;
        }
        // Многострочный комментарий
        else if (lexer->input[lexer->position] == '/' && 
                 lexer->input[lexer->position + 1] == '*') {
            lexer->position += 2;
            lexer->column += 2;
            
            while (lexer->position < lexer->length - 1) {
                if (lexer->input[lexer->position] == '*' && 
                    lexer->input[lexer->position + 1] == '/') {
                    lexer->position += 2;
                    lexer->column += 2;
                    return;
                }
                if (lexer->input[lexer->position] == '\n') {
                    lexer->line++;
                    lexer->column = 1;
                } else {
                    lexer->column++;
                }
                lexer->position++;
            }
            add_error(lexer, "Unclosed comment");
        }
    }
}

// Разбор чисел с поддержкой экспоненциальной записи
void parse_number(Lexer *lexer) {
    int start = lexer->position;
    bool is_real = false;
    bool has_exponent = false;
    
    while (lexer->position < lexer->length) {
        char c = lexer->input[lexer->position];
        
        if (c == '.') {
            if (is_real) {
                add_error(lexer, "Invalid real number format");
                return;
            }
            is_real = true;
            lexer->position++;
            lexer->column++;
        } 
        // Обработка экспоненты
        else if (c == 'e') {
            is_real = true;
            lexer->position++;
            lexer->column++;
            has_exponent = true;
            
            // Обработка знака экспоненты
            if (lexer->position < lexer->length && 
                (lexer->input[lexer->position] == '+' || 
                 lexer->input[lexer->position] == '-')) {
                lexer->position++;
                lexer->column++;
            }
            
            // Проверка цифр после экспоненты
            if (lexer->position >= lexer->length || 
                !isdigit(lexer->input[lexer->position])) {
                add_error(lexer, "Exponent has no digits");
                return;
            }
        } 
        else if (isdigit(c)) {
            lexer->position++;
            lexer->column++;
        } else {
            break;
        }
    }
    
    int length = lexer->position - start;
    TokenType type = is_real ? TOKEN_REAL : TOKEN_INT;
    add_token(lexer, type, lexer->input + start, length);
}

// Разбор символьных литералов
void parse_char(Lexer *lexer) {
    lexer->position++; // Пропускаем начальную '
    lexer->column++;
    
    int start = lexer->position;
    
    // Обработка экранированных символов
    if (lexer->position < lexer->length && lexer->input[lexer->position] == '\\') {
        lexer->position++;
        lexer->column++;
        if (lexer->position >= lexer->length) {
            add_error(lexer, "Unclosed character literal");
            return;
        }
    }
    
    if (lexer->position >= lexer->length) {
        add_error(lexer, "Unclosed character literal");
        return;
    }
    
    lexer->position++;
    lexer->column++;
    
    if (lexer->position >= lexer->length || lexer->input[lexer->position] != '\'') {
        add_error(lexer, "Unclosed character literal");
        return;
    }
    
    int length = lexer->position - start;
    add_token(lexer, TOKEN_CHAR, lexer->input + start, length);
    
    lexer->position++; // Пропускаем завершающую '
    lexer->column++;
}

// Разбор строковых литералов
void parse_string(Lexer *lexer) {
    lexer->position++; // Пропускаем начальную "
    lexer->column++;
    
    int start = lexer->position;
    
    while (lexer->position < lexer->length) {
        if (lexer->input[lexer->position] == '\\') {
            // Пропускаем экранированный символ
            lexer->position++;
            lexer->column++;
            if (lexer->position < lexer->length) {
                lexer->position++;
                lexer->column++;
            }
        } else if (lexer->input[lexer->position] == '"') {
            break;
        } else {
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
    
    lexer->position++; // Пропускаем завершающую "
    lexer->column++;
}

// Разбор булевых значений с поддержкой NONE
void parse_bool(Lexer *lexer) {
    if (strncmp(lexer->input + lexer->position, "TRUE", 4) == 0) {
        add_token(lexer, TOKEN_BOOL, "TRUE", 4);
        lexer->position += 4;
        lexer->column += 4;
    } else if (strncmp(lexer->input + lexer->position, "FALSE", 5) == 0) {
        add_token(lexer, TOKEN_BOOL, "FALSE", 5);
        lexer->position += 5;
        lexer->column += 5;
    } else if (strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
        add_token(lexer, TOKEN_NONE, "NONE", 4);
        lexer->position += 4;
        lexer->column += 4;
    } else {
        add_error(lexer, "Invalid boolean value");
    }
}

// Проверка валидности модификатора
bool is_valid_modifier(const char *modifier) {
    const char *valid_modifiers[] = {"const", "unsig", "public", "private", "dynam"};
    for (int i = 0; i < 5; i++) {
        if (strcmp(modifier, valid_modifiers[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Проверка валидности типа
bool is_valid_type(const char *type) {
    const char *valid_types[] = {"int", "real", "char", "bool"};
    for (int i = 0; i < 4; i++) {
        if (strcmp(type, valid_types[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Разбор использования переменной (вызова)
static void parse_variable_access(Lexer *lexer) {
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
    } else {
        add_error(lexer, "Expected identifier after '$' in variable access");
    }
}

// Разбор объявления переменной
static void parse_variable_declaration(Lexer *lexer) {
    add_token(lexer, TOKEN_DOLLAR, "$", 1);
    lexer->position++;
    lexer->column++;
    
    skip_whitespace(lexer);
    
    // Обработка модификаторов
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
                char *modifier = strndup(lexer->input + mod_start, length);
                
                if (is_valid_modifier(modifier)) {
                    add_token(lexer, TOKEN_MODIFIER, modifier, length);
                } else {
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
            } else if (lexer->input[lexer->position] == ']') {
                break;
            } else {
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
    
    // Обработка одиночного модификатора
    if (isalpha(lexer->input[lexer->position])) {
        int mod_start = lexer->position;
        while (lexer->position < lexer->length && 
               isalpha(lexer->input[lexer->position])) {
            lexer->position++;
            lexer->column++;
        }
        
        if (lexer->position > mod_start) {
            int length = lexer->position - mod_start;
            char *modifier = strndup(lexer->input + mod_start, length);
            
            if (is_valid_modifier(modifier)) {
                add_token(lexer, TOKEN_MODIFIER, modifier, length);
            } else {
                add_error(lexer, "Invalid modifier: %s", modifier);
            }
            free(modifier);
        }
        
        skip_whitespace(lexer);
    }
    
    // Проверка двойного двоеточия после модификаторов
    if (lexer->position < lexer->length - 1 && 
        lexer->input[lexer->position] == ':' && 
        lexer->input[lexer->position + 1] == ':') {
        add_token(lexer, TOKEN_DOUBLE_COLON, "::", 2);
        lexer->position += 2;
        lexer->column += 2;
        skip_whitespace(lexer);
    }
    
    // Обработка системы счисления
    int token_start = lexer->position;
    while (lexer->position < lexer->length && 
           isdigit(lexer->input[lexer->position])) {
        lexer->position++;
        lexer->column++;
    }
    
    // Если есть цифры перед типом
    if (lexer->position > token_start) {
        int length = lexer->position - token_start;
        add_token(lexer, TOKEN_NUM_SYSTEM, lexer->input + token_start, length);
        token_start = lexer->position;
    }
    
    // Обработка типа данных
    while (lexer->position < lexer->length && 
           isalpha(lexer->input[lexer->position])) {
        lexer->position++;
        lexer->column++;
    }
    
    if (lexer->position > token_start) {
        int length = lexer->position - token_start;
        char *type_str = strndup(lexer->input + token_start, length);
        if (is_valid_type(type_str)) {
            add_token(lexer, TOKEN_TYPE, type_str, length);
        } else {
            add_error(lexer, "Invalid type: %s", type_str);
        }
        free(type_str);
    } else {
        add_error(lexer, "Expected type after '$'");
        return;
    }
    
    // Обработка размера в битах
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
            add_token(lexer, TOKEN_BYTE_SIZE, lexer->input + token_start, length);
        } else {
            add_error(lexer, "Expected byte size after ':'");
            return;
        }
    }
    
    skip_whitespace(lexer);
    
    // Обработка идентификатора
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
    } else {
        add_error(lexer, "Expected identifier after type");
        return;
    }
    
    // Обработка массива
    while (lexer->position < lexer->length && lexer->input[lexer->position] == '[') {
        add_token(lexer, TOKEN_LBRACKET, "[", 1);
        lexer->position++;
        lexer->column++;
        skip_whitespace(lexer);
        
        // Обработка содержимого скобок
        while (lexer->position < lexer->length && lexer->input[lexer->position] != ']') {
            if (lexer->input[lexer->position] == '?') {
                add_token(lexer, TOKEN_QUESTION, "?", 1);
                lexer->position++;
                lexer->column++;
            } else if (lexer->position < lexer->length - 2 && 
                     lexer->input[lexer->position] == '.' &&
                     lexer->input[lexer->position + 1] == '.' &&
                     lexer->input[lexer->position + 2] == '.') {
                add_token(lexer, TOKEN_ELLIPSIS, "...", 3);
                lexer->position += 3;
                lexer->column += 3;
            } else if (lexer->input[lexer->position] == ':') {
                add_token(lexer, TOKEN_COLON, ":", 1);
                lexer->position++;
                lexer->column++;
            } else if (isdigit(lexer->input[lexer->position]) || 
                     lexer->input[lexer->position] == '-' ||
                     lexer->input[lexer->position] == '+') {
                parse_number(lexer);
            } else if (isalpha(lexer->input[lexer->position])) {
                int id_start = lexer->position;
                while (lexer->position < lexer->length && 
                       (isalnum(lexer->input[lexer->position]) || 
                        lexer->input[lexer->position] == '_')) {
                    lexer->position++;
                    lexer->column++;
                }
                int id_length = lexer->position - id_start;
                add_token(lexer, TOKEN_IDENTIFIER, lexer->input + id_start, id_length);
            } else {
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
    
    // Обработка инициализации
    if (lexer->position < lexer->length && lexer->input[lexer->position] == '=') {
        add_token(lexer, TOKEN_EQUAL, "=", 1);
        lexer->position++;
        lexer->column++;
        
        skip_whitespace(lexer);
        
        // Инициализация массива
        if (lexer->position < lexer->length && lexer->input[lexer->position] == '{') {
            add_token(lexer, TOKEN_LBRACE, "{", 1);
            lexer->position++;
            lexer->column++;
            
            while (lexer->position < lexer->length) {
                skip_whitespace(lexer);
                skip_comments(lexer);
                
                if (lexer->input[lexer->position] == '}') break;
                
                // Разбор элемента инициализации
                if (lexer->input[lexer->position] == '\'') {
                    parse_char(lexer);
                } 
                else if (lexer->input[lexer->position] == '"') {
                    parse_string(lexer);
                }
                else if (isdigit(lexer->input[lexer->position]) || 
                         lexer->input[lexer->position] == '-' ||
                         lexer->input[lexer->position] == '+') {
                    parse_number(lexer);
                } 
                else if (isalpha(lexer->input[lexer->position])) {
                    if (strncmp(lexer->input + lexer->position, "TRUE", 4) == 0 ||
                        strncmp(lexer->input + lexer->position, "FALSE", 5) == 0 ||
                        strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
                        parse_bool(lexer);
                    } else {
                        // Обработка вызова функции
                        int func_start = lexer->position;
                        while (lexer->position < lexer->length && 
                               (isalnum(lexer->input[lexer->position]) || 
                                lexer->input[lexer->position] == '_')) {
                            lexer->position++;
                            lexer->column++;
                        }
                        
                        int func_length = lexer->position - func_start;
                        char *func_name = strndup(lexer->input + func_start, func_length);
                        
                        skip_whitespace(lexer);
                        if (lexer->input[lexer->position] == '(') {
                            add_token(lexer, TOKEN_FUNCTION, func_name, func_length);
                            // Пропускаем аргументы функции
                            lexer->position++;
                            lexer->column++;
                            int depth = 1;
                            while (lexer->position < lexer->length && depth > 0) {
                                if (lexer->input[lexer->position] == '(') depth++;
                                else if (lexer->input[lexer->position] == ')') depth--;
                                lexer->position++;
                                lexer->column++;
                            }
                        } else {
                            add_token(lexer, TOKEN_IDENTIFIER, func_name, func_length);
                        }
                        free(func_name);
                    }
                } else {
                    add_error(lexer, "Unexpected character in initialization");
                    return;
                }
                
                skip_whitespace(lexer);
                
                // Обработка разделителя
                if (lexer->input[lexer->position] == ',') {
                    add_token(lexer, TOKEN_COMMA, ",", 1);
                    lexer->position++;
                    lexer->column++;
                } else if (lexer->input[lexer->position] != '}') {
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
        // Скалярная инициализация
        else {
            if (lexer->input[lexer->position] == '\'') {
                parse_char(lexer);
            } 
            else if (lexer->input[lexer->position] == '"') {
                parse_string(lexer);
            }
            else if (isdigit(lexer->input[lexer->position]) || 
                     lexer->input[lexer->position] == '-' ||
                     lexer->input[lexer->position] == '+') {
                parse_number(lexer);
            } 
            else if (isalpha(lexer->input[lexer->position])) {
                if (strncmp(lexer->input + lexer->position, "TRUE", 4) == 0 ||
                    strncmp(lexer->input + lexer->position, "FALSE", 5) == 0 ||
                    strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
                    parse_bool(lexer);
                } else {
                    // Обработка вызова функции
                    int func_start = lexer->position;
                    while (lexer->position < lexer->length && 
                           (isalnum(lexer->input[lexer->position]) || 
                            lexer->input[lexer->position] == '_')) {
                        lexer->position++;
                        lexer->column++;
                    }
                    
                    int func_length = lexer->position - func_start;
                    char *func_name = strndup(lexer->input + func_start, func_length);
                    
                    skip_whitespace(lexer);
                    if (lexer->input[lexer->position] == '(') {
                        add_token(lexer, TOKEN_FUNCTION, func_name, func_length);
                        // Пропускаем аргументы функции
                        lexer->position++;
                        lexer->column++;
                        int depth = 1;
                        while (lexer->position < lexer->length && depth > 0) {
                            if (lexer->input[lexer->position] == '(') depth++;
                            else if (lexer->input[lexer->position] == ')') depth--;
                            lexer->position++;
                            lexer->column++;
                        }
                    } else {
                        add_token(lexer, TOKEN_IDENTIFIER, func_name, func_length);
                    }
                    free(func_name);
                }
            } else {
                add_error(lexer, "Unexpected character in initialization");
                return;
            }
        }
    }
}

// Разбор аргументов функции
void parse_arguments(Lexer *lexer) {
    add_token(lexer, TOKEN_LPAREN, "(", 1);
    lexer->position++;
    lexer->column++;
    
    while (lexer->position < lexer->length && 
           lexer->input[lexer->position] != ')') {
        skip_whitespace(lexer);
        
        if (lexer->input[lexer->position] == '\'') {
            parse_char(lexer);
        } else if (lexer->input[lexer->position] == '"') {
            parse_string(lexer);
        } else if (isdigit(lexer->input[lexer->position]) || 
                 lexer->input[lexer->position] == '-' ||
                 lexer->input[lexer->position] == '+') {
            parse_number(lexer);
        } else if (isalpha(lexer->input[lexer->position])) {
            if (strncmp(lexer->input + lexer->position, "TRUE", 4) == 0 ||
                strncmp(lexer->input + lexer->position, "FALSE", 5) == 0 ||
                strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
                parse_bool(lexer);
            } else {
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
        } else if (lexer->input[lexer->position] == '$') {
            // Для вложенных переменных внутри аргументов
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
                parse_variable_access(lexer);
                break;
            }

            char c = lexer->input[temp_pos];
            bool is_declaration = false;

            if (c == '[') {
                is_declaration = true;
            } else if (isdigit(c)) {
                is_declaration = true;
            } else if (isalpha(c)) {
                int start_word = temp_pos;
                while (temp_pos < lexer->length && 
                       (isalnum(lexer->input[temp_pos]) || lexer->input[temp_pos] == '_')) {
                    temp_pos++;
                    temp_col++;
                }
                int len_word = temp_pos - start_word;
                char *word = strndup(lexer->input + start_word, len_word);

                if (is_valid_modifier(word)) {
                    is_declaration = true;
                } else if (is_valid_type(word)) {
                    is_declaration = true;
                }
                free(word);
            }

            if (is_declaration) {
                parse_variable_declaration(lexer);
            } else {
                parse_variable_access(lexer);
            }
        }
        
        skip_whitespace(lexer);
        
        if (lexer->position < lexer->length && 
            lexer->input[lexer->position] == ',') {
            add_token(lexer, TOKEN_COMMA, ",", 1);
            lexer->position++;
            lexer->column++;
        }
        
        skip_whitespace(lexer);
    }
    
    if (lexer->position < lexer->length && 
        lexer->input[lexer->position] == ')') {
        add_token(lexer, TOKEN_RPAREN, ")", 1);
        lexer->position++;
        lexer->column++;
    } else {
        add_error(lexer, "Expected ')' after arguments");
    }
}

// Разбор препроцессорных директив
void parse_macro(Lexer *lexer) {
    lexer->position++; // Пропускаем '#'
    lexer->column++;

    // Проверяем известные директивы
    if (strncmp(lexer->input + lexer->position, "inclib", 6) == 0) {
        lexer->position += 6;
        lexer->column += 6;

        skip_whitespace(lexer);
        
        if (lexer->input[lexer->position] != '(') {
            add_error(lexer, "Expected '(' after #inclib");
            return;
        }
        
        lexer->position++; // Пропускаем '('
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
        
        lexer->position++; // Пропускаем ')' 
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
        
        lexer->position++; // Пропускаем '('
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
        
        lexer->position++; // Пропускаем ')' 
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
        // Обработка других макросов
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

// Основная функция лексического анализа
void tokenize(Lexer *lexer) {
    while (lexer->position < lexer->length) {
        skip_whitespace(lexer);
        skip_comments(lexer);
        
        if (lexer->position >= lexer->length) break;
        
        char c = lexer->input[lexer->position];
        char next = (lexer->position < lexer->length - 1) ? lexer->input[lexer->position + 1] : '\0';
        char next_next = (lexer->position < lexer->length - 2) ? lexer->input[lexer->position + 2] : '\0';

        // Обработка составных операторов
        if (c == '+' && next == '+') {
            add_token(lexer, TOKEN_PLUS_PLUS, "++", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '+' && next == '=') {
            add_token(lexer, TOKEN_PLUS_EQ, "+=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '-' && next == '-') {
            add_token(lexer, TOKEN_MINUS_MINUS, "--", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '-' && next == '=') {
            add_token(lexer, TOKEN_MINUS_EQ, "-=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '=' && next == '=') {
            add_token(lexer, TOKEN_EQ_EQ, "==", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '!' && next == '=') {
            add_token(lexer, TOKEN_NE, "!=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '=' && next == '=' && next_next == '>') {
            add_token(lexer, TOKEN_ROR, "==>", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        } else if (c == '>' && next == '>' && next_next == '>') {
            add_token(lexer, TOKEN_SAR, ">>>", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        } else if (c == '>' && next == '>') {
            add_token(lexer, TOKEN_SHR, ">>", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '>' && next == '=') {
            add_token(lexer, TOKEN_GE, ">=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '<' && next == '<' && next_next == '<') {
            add_token(lexer, TOKEN_SAL, "<<<", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        } else if (c == '<' && next == '=' && next_next == '=') {
            add_token(lexer, TOKEN_SAL, "<==", 3);
            lexer->position += 3;
            lexer->column += 3;
            continue;
        } else if (c == '<' && next == '<') {
            add_token(lexer, TOKEN_SHL, "<<", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '<' && next == '=') {
            add_token(lexer, TOKEN_LE, "<=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '&' && next == '&') {
            add_token(lexer, TOKEN_AND_AND, "&&", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '|' && next == '|') {
            add_token(lexer, TOKEN_PIPE_PIPE, "||", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '*' && next == '*') {
            add_token(lexer, TOKEN_STAR_STAR, "**", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '*' && next == '=') {
            add_token(lexer, TOKEN_STAR_EQ, "*=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '/' && next == '=') {
            add_token(lexer, TOKEN_SLASH_EQ, "/=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '%' && next == '=') {
            add_token(lexer, TOKEN_PERCENT_EQ, "%=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '|' && next == '=') {
            add_token(lexer, TOKEN_PIPE_EQ, "|=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '&' && next == '=') {
            add_token(lexer, TOKEN_AMPERSAND_EQ, "&=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '^' && next == '=') {
            add_token(lexer, TOKEN_CARET_EQ, "^=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        } else if (c == '~' && next == '=') {
            add_token(lexer, TOKEN_TILDE_EQ, "~=", 2);
            lexer->position += 2;
            lexer->column += 2;
            continue;
        }
        
        switch (c) {
            case '$': {
                int save_pos = lexer->position;
                int save_line = lexer->line;
                int save_col = lexer->column;
                
                int temp_pos = save_pos + 1;
                int temp_col = save_col + 1;
                int temp_line = save_line;
                
                // Пропуск пробелов после '$'
                while (temp_pos < lexer->length && 
                       (lexer->input[temp_pos] == ' ' || lexer->input[temp_pos] == '\t')) {
                    temp_pos++;
                    temp_col++;
                }
                
                if (temp_pos >= lexer->length) {
                    // Только '$' в конце файла
                    add_token(lexer, TOKEN_DOLLAR, "$", 1);
                    lexer->position++;
                    lexer->column++;
                    break;
                }
                
                char first_char = lexer->input[temp_pos];
                bool is_declaration = false;
                
                if (first_char == '[') {
                    is_declaration = true;
                } else if (isdigit(first_char)) {
                    is_declaration = true;
                } else if (isalpha(first_char)) {
                    int word_start = temp_pos;
                    while (temp_pos < lexer->length && 
                           (isalnum(lexer->input[temp_pos]) || lexer->input[temp_pos] == '_')) {
                        temp_pos++;
                        temp_col++;
                    }
                    int word_len = temp_pos - word_start;
                    char *word = strndup(lexer->input + word_start, word_len);
                    
                    if (is_valid_modifier(word)) {
                        is_declaration = true;
                    } else if (is_valid_type(word)) {
                        is_declaration = true;
                    }
                    free(word);
                }
                
                if (is_declaration) {
                    parse_variable_declaration(lexer);
                } else {
                    parse_variable_access(lexer);
                }
                break;
            }
                
            case 'i':
                if (strncmp(lexer->input + lexer->position, "if", 2) == 0) {
                    add_token(lexer, TOKEN_IF, "if", 2);
                    lexer->position += 2;
                    lexer->column += 2;
                } else {
                    goto identifier;
                }
                break;
                
            case 'e':
                if (strncmp(lexer->input + lexer->position, "elif", 4) == 0) {
                    add_token(lexer, TOKEN_ELIF, "elif", 4);
                    lexer->position += 4;
                    lexer->column += 4;
                } else if (strncmp(lexer->input + lexer->position, "else", 4) == 0) {
                    add_token(lexer, TOKEN_ELSE, "else", 4);
                    lexer->position += 4;
                    lexer->column += 4;
                } else {
                    goto identifier;
                }
                break;
                
            case 'd':
                if (strncmp(lexer->input + lexer->position, "do", 2) == 0) {
                    add_token(lexer, TOKEN_DO, "do", 2);
                    lexer->position += 2;
                    lexer->column += 2;
                } 
                else if (strncmp(lexer->input + lexer->position, "default", 7) == 0) {
                    add_token(lexer, TOKEN_DEFAULT, "default", 7);
                    lexer->position += 7;
                    lexer->column += 7;
                } 
                else {
                    goto identifier;
                } 
               break;
 
            case 's':
                if (strncmp(lexer->input + lexer->position, "switch", 6) == 0) {
                    add_token(lexer, TOKEN_SWITCH, "switch", 6);
                    lexer->position += 6;
                    lexer->column += 6;
                } else {
                    goto identifier;
                }
                break;
                
            case 'c':
                if (strncmp(lexer->input + lexer->position, "case", 4) == 0) {
                    add_token(lexer, TOKEN_CASE, "case", 4);
                    lexer->position += 4;
                    lexer->column += 4;
                    skip_whitespace(lexer);
                    if (lexer->input[lexer->position] == '(') {
                        parse_arguments(lexer);
                    }
                } else if (strncmp(lexer->input + lexer->position, "continue", 8) == 0) {
                    add_token(lexer, TOKEN_CONTINUE, "continue", 8);
                    lexer->position += 8;
                    lexer->column += 8;
                } 
                else {
                    goto identifier;
                }
                break;
                
            case 'b':
                if (strncmp(lexer->input + lexer->position, "break", 5) == 0) {
                    add_token(lexer, TOKEN_BREAK, "break", 5);
                    lexer->position += 5;
                    lexer->column += 5;
                } else {
                    goto identifier;
                }
                break;
                
            case 'q':
                if (strncmp(lexer->input + lexer->position, "quit", 4) == 0) {
                    add_token(lexer, TOKEN_QUIT, "quit", 4);
                    lexer->position += 4;
                    lexer->column += 4;
                } else {
                    goto identifier;
                }
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
                add_token(lexer, TOKEN_AMPERSAND, "&", 1);
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
                
            case '_':
                // Обработка функции
                lexer->position++;
                lexer->column++;
                int start_pos = lexer->position;
                while (lexer->position < lexer->length && 
                       (isalnum(lexer->input[lexer->position]) || 
                        lexer->input[lexer->position] == '_')) {
                    lexer->position++;
                    lexer->column++;
                }
                int len = lexer->position - start_pos;
                if (len > 0) {
                    add_token(lexer, TOKEN_FUNCTION, lexer->input + start_pos, len);
                } else {
                    add_error(lexer, "Expected function name after '_'");
                }
                break;
                
            case '~':
                lexer->position++;
                lexer->column++;
                start_pos = lexer->position;
                while (lexer->position < lexer->length && 
                       (isalnum(lexer->input[lexer->position]) || 
                        lexer->input[lexer->position] == '_')) {
                    lexer->position++;
                    lexer->column++;
                }
                len = lexer->position - start_pos;
                if (len > 0) {
                    add_token(lexer, TOKEN_START_FUNCTION, lexer->input + start_pos, len);
                } else {
                    add_token(lexer, TOKEN_TILDE, "~", 1);
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
                else {
                    add_error(lexer, "Unexpected dot");
                    lexer->position++;
                    lexer->column++;
                }
                break;

            case 'r':
                if (strncmp(lexer->input + lexer->position, "return", 6) == 0) {
                    add_token(lexer, TOKEN_RETURN, "return", 6);
                    lexer->position += 6;
                    lexer->column += 6;
                } else {
                    goto identifier;
                }
                break;
                
            case '#':
                parse_macro(lexer);
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
                
            case 'T':
            case 'F':
            case 'N':
                if (strncmp(lexer->input + lexer->position, "TRUE", 4) == 0 ||
                    strncmp(lexer->input + lexer->position, "FALSE", 5) == 0 ||
                    strncmp(lexer->input + lexer->position, "NONE", 4) == 0) {
                    parse_bool(lexer);
                } else {
                    goto identifier;
                }
                break;
                
            default:
                if (isalpha(c) || c == '_') {
identifier:
                    int start = lexer->position;
                    while (lexer->position < lexer->length && 
                           (isalnum(lexer->input[lexer->position]) || 
                            lexer->input[lexer->position] == '_')) {
                        lexer->position++;
                        lexer->column++;
                    }
                    int length = lexer->position - start;
                    add_token(lexer, TOKEN_IDENTIFIER, lexer->input + start, length);
                } else {
                    add_error(lexer, "Unexpected character: '%c'", c);
                    lexer->position++;
                    lexer->column++;
                }
                break;
        }
    }
    
    add_token(lexer, TOKEN_EOF, "EOF", 3);
}

// Главная функция
int main() {
    FILE *file = fopen("file.txt", "r");
    if (file == NULL) {
        perror("![FR100A]ERROR: couldn`t open the file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *buffer_file = (char *)malloc(file_size + 1);
    if (buffer_file == NULL) {
        perror("![A1F02C]ERROR: insufficient memory");
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(buffer_file, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        perror("![F1E03D]ERROR: failed to read file");
        free(buffer_file);
        fclose(file);
        return 1;
    }
    buffer_file[file_size] = '\0';
    fclose(file);

    Lexer *lexer = init_lexer(buffer_file);
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
    return 0;
}
