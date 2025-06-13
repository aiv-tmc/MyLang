#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

// Перечисление типов токенов
typedef enum {
    TOKEN_INT, TOKEN_REAL, TOKEN_CHAR, TOKEN_BOOL, TOKEN_STRING,
    TOKEN_IF, TOKEN_ELIF, TOKEN_ELSE, TOKEN_DO, TOKEN_FUNCTION,
    TOKEN_START_FUNCTION, TOKEN_RETURN, TOKEN_SWITCH, TOKEN_CASE,
    TOKEN_DEFAULT, TOKEN_BREAK, TOKEN_CONTINUE, TOKEN_QUIT,
    TOKEN_PREPROC_MACRO, TOKEN_PREPROC_INCLIB, TOKEN_PREPROC_INCFILE,
    TOKEN_PREPROC_DEFINE, TOKEN_PREPROC_UNDEF, TOKEN_PREPROC_IFDEF,
    TOKEN_PREPROC_IFNDEF, TOKEN_PREPROC_ENDIF, TOKEN_PREPROC_LINE,
    TOKEN_PREPROC_ERROR, TOKEN_PREPROC_PRAGMA,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT,
    TOKEN_DOUBLE_STAR, TOKEN_DOUBLE_AND, TOKEN_DOUBLE_PIPE, TOKEN_BANG,
    TOKEN_TILDE, TOKEN_PIPE, TOKEN_AMPERSAND, TOKEN_CARET, TOKEN_SHR,
    TOKEN_SHL, TOKEN_SAR, TOKEN_SAL, TOKEN_ROR, TOKEN_ROL, TOKEN_GT,
    TOKEN_LT, TOKEN_GE, TOKEN_LE, TOKEN_DOUBLE_EQ, TOKEN_NE, TOKEN_EQUAL,
    TOKEN_PLUS_EQ, TOKEN_MINUS_EQ, TOKEN_STAR_EQ, TOKEN_SLASH_EQ,
    TOKEN_PERCENT_EQ, TOKEN_PIPE_EQ, TOKEN_AMPERSAND_EQ, TOKEN_CARET_EQ,
    TOKEN_TILDE_EQ, TOKEN_DOUBLE_PLUS, TOKEN_DOUBLE_MINUS, TOKEN_DOLLAR,
    TOKEN_COLON, TOKEN_DOUBLE_COLON, TOKEN_COMMA, TOKEN_SEMICOLON,
    TOKEN_QUESTION, TOKEN_ELLIPSIS, TOKEN_LCURLY, TOKEN_RCURLY,
    TOKEN_LBRACKET, TOKEN_RBRACKET, TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_MODIFIER, TOKEN_NUMSYS, TOKEN_TYPE, TOKEN_VAR_SIZE,
    TOKEN_IDENTIFIER, TOKEN_EOF, TOKEN_ERROR
} TokenType;

// Структура для хранения ключевых слов
typedef struct {
    const char* keyword;
    TokenType type;
} KeywordEntry;

// Хеш-таблица ключевых слов
static const KeywordEntry keywords[] = {
    {"if", TOKEN_IF}, {"elif", TOKEN_ELIF}, {"else", TOKEN_ELSE},
    {"do", TOKEN_DO}, {"def", TOKEN_DEFAULT}, {"switch", TOKEN_SWITCH},
    {"case", TOKEN_CASE}, {"break", TOKEN_BREAK}, {"continue", TOKEN_CONTINUE},
    {"quit", TOKEN_QUIT}, {"return", TOKEN_RETURN}, {"TRUE", TOKEN_BOOL},
    {"FALSE", TOKEN_BOOL}, {"NONE", TOKEN_BOOL}, {"const", TOKEN_MODIFIER},
    {"unsig", TOKEN_MODIFIER}, {"public", TOKEN_MODIFIER}, {"private", TOKEN_MODIFIER},
    {"dynam", TOKEN_MODIFIER}, {"int", TOKEN_TYPE}, {"real", TOKEN_TYPE},
    {"char", TOKEN_TYPE}, {"bool", TOKEN_TYPE}, {NULL, TOKEN_ERROR}
};

// Структура токена
typedef struct {
    TokenType type;
    const char* value;  // Указатель на исходную строку
    int line;
    int column;
    int length;
} Token;

// Структура лексера
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

// Инициализация лексера
Lexer* init_lexer(const char* input) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->input = input;
    lexer->length = strlen(input);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->token_count = 0;
    lexer->token_capacity = 256;  // Начальная емкость увеличена
    lexer->tokens = malloc(lexer->token_capacity * sizeof(Token));
    return lexer;
}

// Освобождение памяти лексера
void free_lexer(Lexer* lexer) {
    free(lexer->tokens);
    free(lexer);
}

// Добавление токена
void add_token(Lexer* lexer, TokenType type, const char* value, int length) {
    if (lexer->token_count >= lexer->token_capacity) {
        lexer->token_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, lexer->token_capacity * sizeof(Token));
    }

    Token token = {
        .type = type,
        .value = value,
        .line = lexer->line,
        .column = lexer->column - length,
        .length = length
    };
    lexer->tokens[lexer->token_count++] = token;
}

// Пропуск пробелов
void skip_whitespace(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        const char* current = lexer->input + lexer->position;
        size_t space_count = strspn(current, " \t");

        if (space_count > 0) {
            lexer->position += space_count;
            lexer->column += space_count;
        }

        if (lexer->position < lexer->length && lexer->input[lexer->position] == '\n') {
            lexer->position++;
            lexer->line++;
            lexer->column = 1;
        }
        else {
            break;
        }
    }
}

// Пропуск комментариев
void skip_comments(Lexer* lexer) {
    if (lexer->position < lexer->length - 1) {
        if (lexer->input[lexer->position] == '/' &&
            lexer->input[lexer->position + 1] == '/') {
            // Однострочный комментарий
            while (lexer->position < lexer->length &&
                lexer->input[lexer->position] != '\n') {
                lexer->position++;
            }
            lexer->column = 1;
        }
        else if (lexer->input[lexer->position] == '/' &&
            lexer->input[lexer->position + 1] == '*') {
            // Многострочный комментарий
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
                    lexer->column = 0;
                }
                lexer->position++;
                lexer->column++;
            }
            add_token(lexer, TOKEN_ERROR, "Unclosed comment", 16);
        }
    }
}

// Разбор чисел
void parse_number(Lexer* lexer) {
    int start = lexer->position;
    bool is_real = false;

    while (lexer->position < lexer->length) {
        char character = lexer->input[lexer->position];

        if (character == '.') {
            if (is_real) break;
            is_real = true;
        }
        else if (character == 'e') {
            is_real = true;
            if (lexer->position + 1 < lexer->length &&
                (lexer->input[lexer->position + 1] == '+' ||
                    lexer->input[lexer->position + 1] == '-')) {
                lexer->position++;
            }
        }
        else if (!isdigit(character)) {
            break;
        }
        lexer->position++;
        lexer->column++;
    }

    int length = lexer->position - start;
    TokenType type = is_real ? TOKEN_REAL : TOKEN_INT;
    add_token(lexer, type, lexer->input + start, length);
}

// Разбор строковых литералов
void parse_string(Lexer* lexer, char quote) {
    int start = ++lexer->position;
    lexer->column++;

    while (lexer->position < lexer->length) {
        if (lexer->input[lexer->position] == '\\' &&
            lexer->position + 1 < lexer->length) {
            lexer->position += 2;
            lexer->column += 2;
        }
        else if (lexer->input[lexer->position] == quote) {
            break;
        }
        else {
            lexer->position++;
            lexer->column++;
        }
    }

    int length = lexer->position - start;
    add_token(lexer, TOKEN_STRING, lexer->input + start, length);

    if (lexer->position < lexer->length && lexer->input[lexer->position] == quote) {
        lexer->position++;
        lexer->column++;
    }
    else {
        add_token(lexer, TOKEN_ERROR, "Unclosed string", 15);
    }
}

// Проверка ключевых слов
TokenType check_keyword(const char* str, int len) {
    for (const KeywordEntry* keyword_entry = keywords; keyword_entry->keyword; keyword_entry++) {
        if (strlen(keyword_entry->keyword) == len &&
            memcmp(str, keyword_entry->keyword, len) == 0) {
            return keyword_entry->type;
        }
    }
    return TOKEN_IDENTIFIER;
}

// Разбор переменных и операторов
void parse_operator(Lexer* lexer) {
    char character = lexer->input[lexer->position];
    char next = (lexer->position < lexer->length - 1) ?
        lexer->input[lexer->position + 1] : '\0';
    char double_next = (lexer->position < lexer->length - 2) ?
        lexer->input[lexer->position + 2] : '\0';

    // Обработка составных операторов
    switch (character) {
    case '+':
        if (next == '+') {
            add_token(lexer, TOKEN_DOUBLE_PLUS, "++", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        else if (next == '=') {
            add_token(lexer, TOKEN_PLUS_EQ, "+=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '-':
        if (next == '-') {
            add_token(lexer, TOKEN_DOUBLE_MINUS, "--", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        else if (next == '=') {
            add_token(lexer, TOKEN_MINUS_EQ, "-=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '=':
        if (next == '=') {
            if (double_next == '>') {
                add_token(lexer, TOKEN_ROR, "==>", 3);
                lexer->position += 3; lexer->column += 3; return;
            }
            else {
                add_token(lexer, TOKEN_DOUBLE_EQ, "==", 2);
                lexer->position += 2; lexer->column += 2; return;
            }
        }
        break;
    case '!':
        if (next == '=') {
            add_token(lexer, TOKEN_NE, "!=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '>':
        if (next == '>') {
            if (double_next == '>') {
                add_token(lexer, TOKEN_SAR, ">>>", 3);
                lexer->position += 3; lexer->column += 3; return;
            }
            else {
                add_token(lexer, TOKEN_SHR, ">>", 2);
                lexer->position += 2; lexer->column += 2; return;
            }
        }
        else if (next == '=') {
            add_token(lexer, TOKEN_GE, ">=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '<':
        if (next == '<') {
            if (double_next == '<') {
                add_token(lexer, TOKEN_SAL, "<<<", 3);
                lexer->position += 3; lexer->column += 3; return;
            }
            else {
                add_token(lexer, TOKEN_SHL, "<<", 2);
                lexer->position += 2; lexer->column += 2; return;
            }
        }
        else if (next == '=') {
            if (double_next == '=') {
                add_token(lexer, TOKEN_ROL, "<==", 3);
                lexer->position += 3; lexer->column += 3; return;
            }
            else {
                add_token(lexer, TOKEN_LE, "<=", 2);
                lexer->position += 2; lexer->column += 2; return;
            }
        }
        break;
    case '&':
        if (next == '&') {
            add_token(lexer, TOKEN_DOUBLE_AND, "&&", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        else if (next == '=') {
            add_token(lexer, TOKEN_AMPERSAND_EQ, "&=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '|':
        if (next == '|') {
            add_token(lexer, TOKEN_DOUBLE_PIPE, "||", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        else if (next == '=') {
            add_token(lexer, TOKEN_PIPE_EQ, "|=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '*':
        if (next == '*') {
            add_token(lexer, TOKEN_DOUBLE_STAR, "**", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        else if (next == '=') {
            add_token(lexer, TOKEN_STAR_EQ, "*=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '/':
        if (next == '=') {
            add_token(lexer, TOKEN_SLASH_EQ, "/=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '%':
        if (next == '=') {
            add_token(lexer, TOKEN_PERCENT_EQ, "%=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '^':
        if (next == '=') {
            add_token(lexer, TOKEN_CARET_EQ, "^=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    case '~':
        if (next == '=') {
            add_token(lexer, TOKEN_TILDE_EQ, "~=", 2);
            lexer->position += 2; lexer->column += 2; return;
        }
        break;
    }

    // Одиночные символы
    switch (character) {
    case '$': add_token(lexer, TOKEN_DOLLAR, "$", 1); break;
    case ':':
        if (next == ':') {
            add_token(lexer, TOKEN_DOUBLE_COLON, "::", 2);
            lexer->position++; lexer->column++;
        }
        else {
            add_token(lexer, TOKEN_COLON, ":", 1);
        }
        break;
    case '.':
        if (next == '.' && double_next == '.') {
            add_token(lexer, TOKEN_ELLIPSIS, "...", 3);
            lexer->position += 2; lexer->column += 2;
        }
        else {
            add_token(lexer, TOKEN_ERROR, ".", 1);
        }
        break;
    case '{': add_token(lexer, TOKEN_LCURLY, "{", 1); break;
    case '}': add_token(lexer, TOKEN_RCURLY, "}", 1); break;
    case '[': add_token(lexer, TOKEN_LBRACKET, "[", 1); break;
    case ']': add_token(lexer, TOKEN_RBRACKET, "]", 1); break;
    case '(': add_token(lexer, TOKEN_LPAREN, "(", 1); break;
    case ')': add_token(lexer, TOKEN_RPAREN, ")", 1); break;
    case '=': add_token(lexer, TOKEN_EQUAL, "=", 1); break;
    case ',': add_token(lexer, TOKEN_COMMA, ",", 1); break;
    case ';': add_token(lexer, TOKEN_SEMICOLON, ";", 1); break;
    case '?': add_token(lexer, TOKEN_QUESTION, "?", 1); break;
    case '~': add_token(lexer, TOKEN_TILDE, "~", 1); break;
    case '|': add_token(lexer, TOKEN_PIPE, "|", 1); break;
    case '&': add_token(lexer, TOKEN_AMPERSAND, "&", 1); break;
    case '!': add_token(lexer, TOKEN_BANG, "!", 1); break;
    case '^': add_token(lexer, TOKEN_CARET, "^", 1); break;
    case '>': add_token(lexer, TOKEN_GT, ">", 1); break;
    case '<': add_token(lexer, TOKEN_LT, "<", 1); break;
    case '+': add_token(lexer, TOKEN_PLUS, "+", 1); break;
    case '-': add_token(lexer, TOKEN_MINUS, "-", 1); break;
    case '*': add_token(lexer, TOKEN_STAR, "*", 1); break;
    case '/': add_token(lexer, TOKEN_SLASH, "/", 1); break;
    case '%': add_token(lexer, TOKEN_PERCENT, "%", 1); break;
    default: add_token(lexer, TOKEN_ERROR, &character, 1); break;
    }
    lexer->position++;
    lexer->column++;
}

// Основная функция токенизации
void tokenize(Lexer* lexer) {
    while (lexer->position < lexer->length) {
        skip_whitespace(lexer);
        skip_comments(lexer);
        if (lexer->position >= lexer->length) break;

        char character = lexer->input[lexer->position];
        char next = (lexer->position < lexer->length - 1) ?
            lexer->input[lexer->position + 1] : '\0';

        // Обработка строк
        if (character == '"' || character == '\'') {
            parse_string(lexer, character);
            continue;
        }

        // Обработка чисел
        if (isdigit(character) || (character == '.' && isdigit(next))) {
            parse_number(lexer);
            continue;
        }

        // Обработка идентификаторов и ключевых слов
        if (isalpha(character) || character == '_' || character == '$') {
            int start = lexer->position;

            // Обработка специальных случаев для $
            if (character == '$') {
                lexer->position++;
                lexer->column++;

                // Пропуск пробелов после $
                while (lexer->position < lexer->length &&
                    (lexer->input[lexer->position] == ' ' ||
                        lexer->input[lexer->position] == '\t')) {
                    lexer->position++;
                    lexer->column++;
                }
            }

            while (lexer->position < lexer->length &&
                (isalnum(lexer->input[lexer->position]) ||
                    lexer->input[lexer->position] == '_' ||
                    lexer->input[lexer->position] == ':') {
                lexer->position++;
                lexer->column++;
            }

            int len = lexer->position - start;
            TokenType type = check_keyword(lexer->input + start, len);

            // Специальная обработка для функций
            if (type == TOKEN_IDENTIFIER) {
                if (lexer->input[start] == '_') {
                    type = TOKEN_FUNCTION;
                }
                else if (lexer->input[start] == '~') {
                    type = TOKEN_START_FUNCTION;
                }
            }

            add_token(lexer, type, lexer->input + start, len);
            continue;
        }

        // Обработка операторов и символов
        parse_operator(lexer);
    }
    add_token(lexer, TOKEN_EOF, "EOF", 3);
}

int main() {
    FILE* file = fopen("file.txt", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    Lexer* lexer = init_lexer(buffer);
    tokenize(lexer);

    // Вывод результатов
    printf("Line  Col  Type\n");
    for (int i = 0; i < lexer->token_count; i++) {
        Token token = lexer->tokens[i];
        printf("%-4d  %-4d %d\n", token.line, token.column, token.type);
    }

    free_lexer(lexer);
    free(buffer);
    return 0;
}
