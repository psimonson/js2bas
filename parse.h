/*
 * parse.h - Simple javascript-like language parser.
 *
 * Author: Philip R. Simonson
 * Date: 08/11/2024
 *
 */

// AST Node Types
typedef enum {
    AST_NUMBER,
    AST_STRING,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_IF,
    AST_REM,
    AST_PRINT,
    AST_ASSIGN,
    AST_INPUT,
    AST_WHILE,
    AST_EXIT,
    AST_EQUALS
} ASTNodeType;

// AST Node Structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            char *value;
        } number;
	struct {
            char *value;
	} string;
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            char *op;
        } binary_op;
        struct {
            struct ASTNode *condition;
            struct ASTNode **then_branch;
            struct ASTNode **else_branch;
	    int then_count;
	    int else_count;
        } if_stmt;
	struct {
	    struct ASTNode *identifier;
	    struct ASTNode *expression;
	} assign_stmt;
        struct {
            struct ASTNode *expression;
        } print_stmt;
	struct {
	    struct ASTNode *condition;
	    struct ASTNode **body;
	    int body_count;
	} while_stmt;
	struct {
	    struct ASTNode *string;
	    struct ASTNode *identifier;
	} input_stmt;
    } as;
} ASTNode;

void generate_gwbasic_code(ASTNode *node, int depth);
ASTNode *parse_statement(Token **tokens);
void free_ast(ASTNode *node);

