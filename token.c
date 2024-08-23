/*
 * token.c - Single character tokenizer for JAVASCRIPT-like language.
 *
 * Author: Philip R. Simonson
 * Date: 08/11/2024
 *
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

// Function to Tokenize Source Code
Token *tokenize(const char *source) {
    Token *tokens = malloc(sizeof(Token) * 4096);  // Dynamic array of tokens
    int tokenIndex = 0;
    int position = 0;
    int line = 1;

    while (tokenIndex < 4096 && *source) {
        while (*source == ' ' || *source == '\t' || *source == '\r' || *source == '\n') {
	    if (*source == '\n' || *source == '\r') {
		    line++;
	    }
            source++;
            position++;
        }

        if (isdigit(*source)) {
            const char *start = source;
            while (isdigit(*source)) source++;
            tokens[tokenIndex].type = TOKEN_NUMBER;
            tokens[tokenIndex].lexeme = strndup(start, source - start);
            tokens[tokenIndex].position = position;
	    tokens[tokenIndex].line = line;
            position += source - start;
            tokenIndex++;
        } else if (isalpha(*source) || *source == '_') {
            const char *start = source;
            while (isalnum(*source) || *source == '_') source++;
            tokens[tokenIndex].lexeme = strndup(start, source - start);
            tokens[tokenIndex].position = position;
	    tokens[tokenIndex].line = line;
            position += source - start;

            if (strcmp(tokens[tokenIndex].lexeme, "if") == 0) {
                tokens[tokenIndex].type = TOKEN_IF;
            } else if (strcmp(tokens[tokenIndex].lexeme, "else") == 0) {
                tokens[tokenIndex].type = TOKEN_ELSE;
            } else if (strcmp(tokens[tokenIndex].lexeme, "print") == 0) {
                tokens[tokenIndex].type = TOKEN_PRINT;
            } else if (strcmp(tokens[tokenIndex].lexeme, "input") == 0) {
                tokens[tokenIndex].type = TOKEN_INPUT;
	    } else if (strcmp(tokens[tokenIndex].lexeme, "while") == 0) {
                tokens[tokenIndex].type = TOKEN_WHILE;
	    } else if (strcmp(tokens[tokenIndex].lexeme, "exit") == 0) {
                tokens[tokenIndex].type = TOKEN_EXIT;
	    } else if (strcmp(tokens[tokenIndex].lexeme, "var") == 0) {
                tokens[tokenIndex].type = TOKEN_ASSIGN;
            } else {
                tokens[tokenIndex].type = TOKEN_IDENTIFIER;
            }
            tokenIndex++;
	} else if (*source == '=' && *(source+1) == '=') {
            tokens[tokenIndex].type = TOKEN_OPERATOR;
            tokens[tokenIndex].lexeme = strndup(source, 2);
            tokens[tokenIndex].position = position;
	    tokens[tokenIndex].line = line;
	    position += 2;
	    tokenIndex++;
	    source += 2;
	} else if (*source == '/' && *(source+1) == '/') {
	    source += 2;
	    const char *start = source;
            while (*source && *source != '\n' && *source != '\r') source++;
	    const int length = source - start;
            tokens[tokenIndex].type = TOKEN_REM;
            tokens[tokenIndex].lexeme = strndup(start, length);
            tokens[tokenIndex].position = position;
	    tokens[tokenIndex].line = line;
	    position += length;
	    tokenIndex++;
	} else if (*source == '"') {
            const char *start = ++source;
            while (*source && *source != '"') source++;
	    tokens[tokenIndex].type = TOKEN_STRING;
            tokens[tokenIndex].lexeme = strndup(start, source - start);
            tokens[tokenIndex].position = position;
	    tokens[tokenIndex].line = line;
            position += source - start;
	    tokenIndex++;
	    source++;
        } else {
            switch (*source) {
                case '+': case '-': case '*': case '/':
                    tokens[tokenIndex].type = TOKEN_OPERATOR;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                case '>':
                    tokens[tokenIndex].type = TOKEN_OPERATOR;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
		case '<':
                    tokens[tokenIndex].type = TOKEN_OPERATOR;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
		    break;
                case '=':
                    tokens[tokenIndex].type = TOKEN_EQUALS;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
		    break;
                case ';':
                    tokens[tokenIndex].type = TOKEN_SEMICOLON;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                case '(':
                    tokens[tokenIndex].type = TOKEN_LPAREN;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                case ')':
                    tokens[tokenIndex].type = TOKEN_RPAREN;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                case '{':
                    tokens[tokenIndex].type = TOKEN_LBRACE;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                case '}':
                    tokens[tokenIndex].type = TOKEN_RBRACE;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                case ',':
                    tokens[tokenIndex].type = TOKEN_OPERATOR;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                case '\0':
                    tokens[tokenIndex].type = TOKEN_EOF;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
                    break;
                default:
                    tokens[tokenIndex].type = TOKEN_UNKNOWN;
                    tokens[tokenIndex].lexeme = strndup(source, 1);
            }
            tokens[tokenIndex].position = position;
            tokens[tokenIndex].line = line;
            position++;
            tokenIndex++;
            source++;
        }
    }
    tokens[tokenIndex].type = TOKEN_EOF;
    tokens[tokenIndex].lexeme = NULL;
    tokens[tokenIndex].position = position;
    tokens[tokenIndex].line = line;
    return tokens;
}

// Free tokens
void free_tokens(Token *tokens) {
	for (int i = 0; tokens[i].type != TOKEN_EOF; ++i) {
		free(tokens[i].lexeme); // Free each lexeme
	}
	free(tokens); // Free token array of pointers
}


