/*
 * parse.c - Simple javascript-like language token parser.
 *
 * Author: Philip R. Simonson
 * Date: 08/11/2024
 *
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parse.h"

// Error Handling
void error(const char *message, Token *token) {
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, "Error: %s at end of input.\n", message);
    } else {
        fprintf(stderr, "Error: %s at '%s' (line %d).\n", message, token->lexeme, token->line);
    }
}

// Parse Expressions
ASTNode *parse_expression(Token **tokens) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) exit(1);  // Memory allocation check

    if ((*tokens)->type == TOKEN_NUMBER) {
        node->type = AST_NUMBER;
        node->as.number.value = (*tokens)->lexeme;
        (*tokens)++;
    } else if ((*tokens)->type == TOKEN_STRING) {
	node->type = AST_STRING;
	node->as.string.value = (*tokens)->lexeme;
	(*tokens)++;
    } else if ((*tokens)->type == TOKEN_IDENTIFIER) {
	node->type = AST_IDENTIFIER;
	node->as.string.value = (*tokens)->lexeme;
	(*tokens)++;
    } else {
        error("Expected a number or identifier or string", *tokens);
        free(node);
        return NULL;  // Return null if expression is not valid
    }

    if ((*tokens)->type == TOKEN_OPERATOR) {
        char *op = (*tokens)->lexeme;
        (*tokens)++;
        ASTNode *right = parse_expression(tokens);
        if (right == NULL) {
            free(node);
            return NULL;
        }
        ASTNode *binary_op_node = malloc(sizeof(ASTNode));
        if (binary_op_node == NULL) exit(1);  // Memory allocation check
        binary_op_node->type = AST_BINARY_OP;
        binary_op_node->as.binary_op.left = node;
        binary_op_node->as.binary_op.right = right;
        binary_op_node->as.binary_op.op = op;
        return binary_op_node;
    }

    // Check for statement terminator
    if ((*tokens)->type == TOKEN_SEMICOLON) {
	(*tokens)++; // Skip ';'
    }

    return node;
}

// Parse If Statements
ASTNode *parse_if_statement(Token **tokens) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node == NULL) exit(1);  // Memory allocation check
    node->type = AST_IF;

    (*tokens)++;  // Skip 'if'
    if ((*tokens)->type == TOKEN_LPAREN) {
        (*tokens)++;  // Skip '('
        node->as.if_stmt.condition = parse_expression(tokens);
        if (node->as.if_stmt.condition == NULL) {
            error("Invalid condition in if statement", *tokens);
            free(node);
            return NULL;  // Error in parsing condition
        }
        if ((*tokens)->type == TOKEN_RPAREN) {
            (*tokens)++;  // Skip ')'
        } else {
            error("Expected ')' after if condition", *tokens);
            free(node);
            return NULL;  // Error: expected closing parenthesis
        }
    } else {
        error("Expected '(' after 'if'", *tokens);
        free(node);
        return NULL;  // Error: expected opening parenthesis
    }

    node->as.if_stmt.then_count = 0;
    node->as.if_stmt.then_branch = NULL;

    if ((*tokens)->type == TOKEN_LBRACE) {
        (*tokens)++;

	while((*tokens)->type != TOKEN_RBRACE && (*tokens)->type != TOKEN_EOF) {
		ASTNode **tmp = (ASTNode**)realloc(node->as.if_stmt.then_branch, sizeof(ASTNode*) * (node->as.if_stmt.then_count+2));
	        if (tmp == NULL) {
	            fprintf(stderr, "Out of memory!\n");
	            free(node);
	            return NULL;  // Error out of memory
	        }
		node->as.if_stmt.then_branch = tmp;
	        node->as.if_stmt.then_branch[node->as.if_stmt.then_count] = parse_statement(tokens);
	        if (node->as.if_stmt.then_branch[node->as.if_stmt.then_count] == NULL) {
	            error("Invalid statement in then branch", *tokens);
	            free(node);
	            return NULL;  // Error in parsing then branch
	        }
		node->as.if_stmt.then_count++;
	        node->as.if_stmt.then_branch[node->as.if_stmt.then_count] = NULL;

		// Check for statement termination
		if ((*tokens)->type == TOKEN_SEMICOLON) {
			(*tokens)++; // Skip ';'
		}
	}

	if ((*tokens)->type == TOKEN_RBRACE) {
	       (*tokens)++;
	} else {
	       error("Expected '}' after then branch", *tokens);
	       free(node);
	       return NULL;  // Error: expected closing brace
	}
    } else {
        error("Expected '{' after if condition", *tokens);
        free(node);
        return NULL;  // Error: expected opening brace
    }

    node->as.if_stmt.else_count = 0;
    node->as.if_stmt.else_branch = NULL;

    if ((*tokens)->type == TOKEN_ELSE) {
        (*tokens)++; // Skip 'else'

	if ((*tokens)->type == TOKEN_LBRACE) {
                (*tokens)++; // Skip '{'

		while((*tokens)->type != TOKEN_RBRACE && (*tokens)->type != TOKEN_EOF) {
			ASTNode **tmp = (ASTNode**)realloc(node->as.if_stmt.else_branch, sizeof(ASTNode*) * (node->as.if_stmt.else_count+2));
		        if (tmp == NULL) {
		            fprintf(stderr, "Out of memory!\n");
		            free(node);
		            return NULL;  // Error out of memory
		        }
			node->as.if_stmt.else_branch = tmp;
		        node->as.if_stmt.else_branch[node->as.if_stmt.else_count] = parse_statement(tokens);
			if (node->as.if_stmt.else_branch[node->as.if_stmt.else_count] == NULL) {
	                	error("Invalid statement in else branch", *tokens);
	                	free(node);
	                	return NULL;  // Error in parsing else branch
	            	}
			node->as.if_stmt.else_count++;
			node->as.if_stmt.else_branch[node->as.if_stmt.else_count] = NULL;

			// Check for statement termination
			if ((*tokens)->type == TOKEN_SEMICOLON) {
				(*tokens)++; // Skip ';'
			}
		}

	        if ((*tokens)->type == TOKEN_RBRACE) {
	        	(*tokens)++; // Skip '}'
	        } else {
	               	error("Expected '}' after else branch", *tokens);
	               	free(node);
	               	return NULL;  // Error: expected closing brace
	        }
        } else {
            error("Expected '{' after else keyword", *tokens);
            free(node);
            return NULL;  // Error: expected opening brace
        }
    }

    // Check for statement terminator
    if ((*tokens)->type == TOKEN_SEMICOLON) {
        (*tokens)++; // Skip ';'
    }

    return node;
}

// Forward Declarations
ASTNode *parse_expression(Token **tokens);
ASTNode *parse_statement(Token **tokens);
ASTNode *parse_while_statement(Token **tokens);
ASTNode *parse_input_statement(Token **tokens);
ASTNode *parse_variable_statement(Token **tokens);

// Parse Statements
ASTNode *parse_statement(Token **tokens) {
    if ((*tokens)->type == TOKEN_IF) {
        return parse_if_statement(tokens);
    } else if ((*tokens)->type == TOKEN_ASSIGN) {
	return parse_variable_statement(tokens);
    } else if ((*tokens)->type == TOKEN_WHILE) {
	return parse_while_statement(tokens);
    } else if ((*tokens)->type == TOKEN_REM) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (node == NULL) exit(1); // Memory allocation check
	node->type = AST_REM;
	node->as.string.value = (*tokens)->lexeme;
	(*tokens)++; // Skip REM
	return node;
    } else if ((*tokens)->type == TOKEN_EXIT) {
	ASTNode *node = malloc(sizeof(ASTNode));
	if (node == NULL) exit(1); // Memory allocation check
	node->type = AST_EXIT;
	node->as.string.value = NULL;
	(*tokens)++; // Skip 'break'
	
	// Check for statement terminator
	if ((*tokens)->type == TOKEN_SEMICOLON) {
	    (*tokens)++; // Skip ';'
	}

	return node;
    } else if ((*tokens)->type == TOKEN_IDENTIFIER) {
	return parse_input_statement(tokens);
    } else if ((*tokens)->type == TOKEN_PRINT) {
        ASTNode *node = malloc(sizeof(ASTNode));
        if (node == NULL) exit(1);  // Memory allocation check
        node->type = AST_PRINT;
        (*tokens)++;  // Skip 'print'
        node->as.print_stmt.expression = parse_expression(tokens);
        if (node->as.print_stmt.expression == NULL) {
            error("Invalid expression in print statement", *tokens);
            free(node);
            return NULL;  // Error in parsing print statement
        }

	// Check for statement terminator
	if ((*tokens)->type == TOKEN_SEMICOLON) {
	    (*tokens)++; // Skip ';'
	}

        return node;
    } else {
        return parse_expression(tokens);
    }
}

// Parse while statement
ASTNode *parse_while_statement(Token **tokens)
{
	ASTNode *node = malloc(sizeof(ASTNode));
	if (node == NULL) exit(1); // Memory allocation check
	node->type = AST_WHILE;
	node->as.while_stmt.body = NULL;

	(*tokens)++; // Skip 'while'
	if ((*tokens)->type == TOKEN_LPAREN) {
		(*tokens)++; // Skip '('
		node->as.while_stmt.condition = parse_expression(tokens);
		if (node->as.while_stmt.condition == NULL) {
			error("Invalid condition in while statement", *tokens);
			free(node);
			return NULL; // Error in parsing condition
		}
		if ((*tokens)->type == TOKEN_RPAREN) {
			(*tokens)++; // Skip ')'
		} else {
			error("Expected ')' after while condition", *tokens);
			free(node);
			return NULL;
		}
	} else {
		error("Expected '(' after 'while'", *tokens);
		free(node);
		return NULL; // Error: expected opening parenthesis
	}

	node->as.while_stmt.body_count = 0;
	node->as.while_stmt.body = NULL;

	if ((*tokens)->type == TOKEN_LBRACE) {
		(*tokens)++;

		while((*tokens)->type != TOKEN_RBRACE && (*tokens)->type != TOKEN_EOF) {
			ASTNode **tmp = (ASTNode**)realloc(node->as.while_stmt.body, sizeof(ASTNode*) * (node->as.while_stmt.body_count + 2));
			if(tmp == NULL) {
				fprintf(stderr, "Out of memory!\n");
				free(node);
				return NULL;
			}
			node->as.while_stmt.body = tmp;
			node->as.while_stmt.body[node->as.while_stmt.body_count] = parse_statement(tokens);
			if (node->as.while_stmt.body[node->as.while_stmt.body_count] == NULL) {
				error("Invalid statement in while body", *tokens);
				free(node);
				return NULL; // Error in parsing while body
			}
			node->as.while_stmt.body_count++;
			node->as.while_stmt.body[node->as.while_stmt.body_count] = NULL;
			// Check for statement terminator
			if ((*tokens)->type == TOKEN_SEMICOLON) {
			    (*tokens)++; // Skip ';'
			}

		}

		if ((*tokens)->type == TOKEN_RBRACE) {
			(*tokens)++;
		} else {
			printf("%s\n", (*tokens)->lexeme);
			error("Expected '}' after while body", *tokens);
			free(node);
			return NULL; // Error: expected closing brace
		}
	} else {
		error("Expected '{' after while condition", *tokens);
		free(node);
		return NULL; // Error: expected opening brace
	}

	// Check for statement terminator
	if ((*tokens)->type == TOKEN_SEMICOLON) {
	    (*tokens)++; // Skip ';'
	}

	return node;
}

// Parse input statement
ASTNode *parse_input_statement(Token **tokens)
{
	ASTNode *node = malloc(sizeof(ASTNode));
	if (node == NULL) exit(1); // Memory allocation check
	
	ASTNode *tmp = NULL;

	if((*tokens)->type == TOKEN_IDENTIFIER) {
		tmp = parse_expression(tokens);
		if (tmp == NULL) {
			error("Expected 'identifier'", *tokens);
			free(node);
			return NULL;
		}
	} else {
		error("Expected identifier", *tokens);
		free(node);
		return NULL;
	}

	if((*tokens)->type == TOKEN_EQUALS) {
		(*tokens)++; // Skip '='
	} else {
		error("Expected '=' after identifier", *tokens);
		free(node);
		return NULL;
	}

	if((*tokens)->type == TOKEN_INPUT) {
		node->type = AST_INPUT;
		node->as.input_stmt.identifier = tmp;
		(*tokens)++; // Skip 'input'

		if((*tokens)->type == TOKEN_LPAREN) {
			(*tokens)++; // Skip '('

			if((*tokens)->type == TOKEN_STRING) {
				node->as.input_stmt.string = parse_expression(tokens);
			} else {
				error("Expected string", *tokens);
				free(node);
				return NULL;
			}

			if((*tokens)->type == TOKEN_RPAREN) {
				(*tokens)++; // Skip ')'
			} else {
				error("Expected ')'", *tokens);
				free(node);
				return NULL;
			}
		} else {
			error("Expected '(' after '='", *tokens);
			free(node);
			return NULL;
		}
	} else {
		node->type = AST_EQUALS;
		node->as.assign_stmt.identifier = tmp;
		node->as.assign_stmt.expression = parse_expression(tokens);
		if (node->as.assign_stmt.expression == NULL) {
			error("Expected number or string or identifier", *tokens);
			free(node);
			return NULL;
		}
	}

	// Check for statement terminator
	if ((*tokens)->type == TOKEN_SEMICOLON) {
		(*tokens)++; // Skip ';'
	}

	return node;
}

// Parse variables
ASTNode *parse_variable_statement(Token **tokens)
{
	ASTNode *node = malloc(sizeof(ASTNode));
	if (node == NULL) exit(1);  // Memory allocation check
	node->type = AST_ASSIGN;
	(*tokens)++; // Skip 'var'

	if((*tokens)->type == TOKEN_IDENTIFIER) {
  		node->as.assign_stmt.identifier = parse_expression(tokens);
	} else {
		error("Expected identifier", *tokens);
		free(node);
		return NULL;
	}

	if((*tokens)->type == TOKEN_EQUALS) {
		(*tokens)++; // Skip '='
	}

	if((*tokens)->type == TOKEN_NUMBER || (*tokens)->type == TOKEN_STRING) {
		node->as.assign_stmt.expression = parse_expression(tokens);
	} else {
	    error("Invalid expression in variable statement", *tokens);
	    free(node);
	    return NULL;  // Error in parsing print statement
	}

	return node;
}

// Generate GW-BASIC Code
void generate_gwbasic_code(ASTNode *node, int depth) {
    if (node == NULL) return;
    if (depth < 0) return;

    switch (node->type) {
        case AST_NUMBER:
            printf("%s", node->as.number.value);
            break;
	case AST_STRING:
	    printf("\"%s\"", node->as.string.value);
	    break;
	case AST_IDENTIFIER:
	    printf("%s", node->as.string.value);
	    break;
        case AST_BINARY_OP:
            generate_gwbasic_code(node->as.binary_op.left, depth);
	    if(strncmp(node->as.binary_op.op, "==", 2) == 0) {
		    printf(" = ");
	    } else {
	            printf(" %s ", node->as.binary_op.op);
	    }
            generate_gwbasic_code(node->as.binary_op.right, depth);
            break;
        case AST_IF:
            printf("IF ");
            generate_gwbasic_code(node->as.if_stmt.condition, depth);
            printf(" THEN");
	    for(int i = 0; i < (depth + 1); ++i) {
	    	printf("\t");
	    }
	    for(int i = 0; i < node->as.if_stmt.then_count; ++i) {
		if (i < node->as.if_stmt.then_count) {
			printf("\n");
		}
		for(int i = 0; i < (depth + 1); ++i) {
			printf("\t");
		}
            	generate_gwbasic_code(node->as.if_stmt.then_branch[i], depth + 1);
	    }
            if (node->as.if_stmt.else_branch) {
		printf("\n");
		for(int i = 0; i < depth; ++i) {
			printf("\t");
		}
                printf("ELSE");
		for(int i = 0; i < node->as.if_stmt.else_count; ++i) {
			if (i < node->as.if_stmt.else_count) {
				printf("\n");
			}
			for(int i = 0; i < (depth + 1); ++i) {
				printf("\t");
			}
                	generate_gwbasic_code(node->as.if_stmt.else_branch[i], depth + 1);
		}
            }
	    printf("\n");
	    for(int i = 0; i < depth; ++i) {
		    printf("\t");
	    }
	    printf("END IF");
            break;
	case AST_WHILE:
	    printf("WHILE ");
	    generate_gwbasic_code(node->as.while_stmt.condition, depth);
	    for(int i = 0; i < depth; ++i) {
	    	printf("\t");
	    }
	    for(int i = 0; i < node->as.while_stmt.body_count; ++i) {
		if (i < node->as.while_stmt.body_count) {
			printf("\n");
		}
		for(int i = 0; i < (depth + 1); ++i) {
			printf("\t");
		}
	    	generate_gwbasic_code(node->as.while_stmt.body[i], depth + 1);
	    }
	    printf("\n");
	    for(int i = 0; i < depth; ++i) {
		    printf("\t");
	    }
	    printf("WEND");
	    break;
	case AST_EXIT:
	    printf("END");
	    break;
	case AST_INPUT:
	    printf("INPUT ");
	    generate_gwbasic_code(node->as.input_stmt.string, depth);
	    printf(" ; ");
	    generate_gwbasic_code(node->as.input_stmt.identifier, depth);
	    break;
	case AST_ASSIGN:
	    printf("DIM ");
	    generate_gwbasic_code(node->as.assign_stmt.identifier, depth);
	    printf(" AS ");
	    if (node->as.assign_stmt.expression->type == AST_NUMBER) {
		    printf("INTEGER");
	    } else if(node->as.assign_stmt.expression->type == AST_STRING) {
		    printf("STRING");
	    } else if(node->as.assign_stmt.expression->type == AST_IDENTIFIER) {
		    printf("STRING");
	    }
	    break;
	case AST_EQUALS:
	    generate_gwbasic_code(node->as.assign_stmt.identifier, depth);
	    printf(" = ");
	    generate_gwbasic_code(node->as.assign_stmt.expression, depth);
	    break;
	case AST_REM:
	    printf("REM %s", node->as.string.value);
	    break;
        case AST_PRINT:
            printf("PRINT ");
            generate_gwbasic_code(node->as.print_stmt.expression, depth);
            break;
    }
}

// Free AST recursively
void free_ast(ASTNode *node) {
	if (node == NULL) return;

	switch (node->type) {
		case AST_NUMBER:
			// No need to free
			break;
		case AST_STRING:
			// No need to free
			break;
		case AST_IDENTIFIER:
			// No need to free
			break;
		case AST_BINARY_OP:
			free_ast(node->as.binary_op.left);
			free_ast(node->as.binary_op.right);
			break;
		case AST_IF:
			free_ast(node->as.if_stmt.condition);
			for (int i = 0; i < node->as.if_stmt.then_count; ++i) {
				free_ast(node->as.if_stmt.then_branch[i]);
			}
			free(node->as.if_stmt.then_branch);
			for (int i = 0; i < node->as.if_stmt.else_count; ++i) {
				free_ast(node->as.if_stmt.else_branch[i]);
			}
			free(node->as.if_stmt.else_branch);
			break;
		case AST_ASSIGN:
			free_ast(node->as.assign_stmt.identifier);
			free_ast(node->as.assign_stmt.expression);
			break;
		case AST_PRINT:
			free_ast(node->as.print_stmt.expression);
			break;
		case AST_WHILE:
			free_ast(node->as.while_stmt.condition);
			for (int i = 0; i < node->as.while_stmt.body_count; ++i) {
				free_ast(node->as.while_stmt.body[i]);
			}
			free(node->as.while_stmt.body);
			break;
		case AST_REM:
			// No need to free as it's pointing to lexeme in tokens
			break;
		case AST_INPUT:
			free_ast(node->as.input_stmt.identifier);
			free_ast(node->as.input_stmt.string);
			break;
		case AST_EXIT:
			// No associated memory to free for EXIT
			break;
		case AST_EQUALS:
			// This is typically handled under BINARY_OP
			break;
	}

	free(node); // Finally, free the node itself
}


