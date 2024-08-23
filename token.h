/*
 * token.h - Single character tokenizer for JAVASCRIPT-like language.
 *
 * Author: Philip R. Simonson
 * Date: 08/11/2024
 *
 */

// Token Types
typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_IDENTIFIER,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_EQUALS,
    TOKEN_ASSIGN,
    TOKEN_REM,
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_PRINT,
    TOKEN_INPUT,
    TOKEN_WHILE,
    TOKEN_EXIT,
    TOKEN_UNKNOWN
} TokenType;

// Token Structure
typedef struct {
    TokenType type;
    char *lexeme;
    int position;
    int line;
} Token;

Token *tokenize(const char *source);
void free_tokens(Token *tokens);

