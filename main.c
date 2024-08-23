/*
 * main.c - Source for tokenizing and parsing JavaScript-like language
 *          and generating BASIC code.
 *
 * Author: Philip R. Simonson
 * Date: 08/10/2024
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "parse.h"

// Load a source file
char *load_file(const char *filename, long int *outsize)
{
	char name[512];
	long int nbytes;
	long int size;
	char *source;
	char *dot;
	FILE *fp;

	dot = strrchr(filename, '.');
	if(dot != NULL) {
		*dot = '\0';
	}
	strncpy(name, filename, sizeof(name)-1);
	strncat(name, ".js", sizeof(name)-1);

	if((fp = fopen(name, "rt")) == NULL) {
		fprintf(stderr, "Error: Cannot open file '%s'.\n", name);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	source = (char*)malloc((size+1) * sizeof(char));
	if(source == NULL) {
		fprintf(stderr, "Error: Out of memory.\n");
		fclose(fp);
		return NULL;
	}

	nbytes = fread(source, sizeof(char), size, fp);
	source[nbytes] = '\0';
	fclose(fp);

	if(nbytes != size) {
		fprintf(stderr, "Error: Total bytes does NOT match size.\n");
		free(source);
		source = NULL;
	}

	if(outsize != NULL) {
		*outsize = size;
	}

	return source;
}

// Main Function
int main(int argc, char *argv[]) {
    char *source;
    Token *tokens;
    
    if (argc != 2 && argc != 3) {
	    fprintf(stderr, "Usage: %s <filename.js>\n", argv[0]);
	    return 1;
    }

    char *filename = NULL;
    int mode = 0;
    int found = 0;
    for (int i = 0; i < argc; ++i) {
	    if (!found && argv[i][0] == '-') {
		    int j = 1;
		    while (!found && argv[i][j] != '\0') {
			    if (argv[i][j] == 'b') {
				    mode = 0;
				    break;
			    } else {
				    fprintf(stderr, "Unknown option '%c'.\n", argv[i][j]);
				    return 1;
			    }
			    ++j;
		    }
		    found = 1;
	    } else {
		    filename = argv[i];
	    }
    }

    source = load_file(filename, NULL);
    if(source == NULL) {
	    return 1;
    }
    
    tokens = tokenize(source);
    free(source);

    Token *currentToken = tokens;
    while(currentToken->type != TOKEN_EOF) {
	    ASTNode *ast = parse_statement(&currentToken);
	    if (ast == NULL) {
	        fprintf(stderr, "Error in parsing the source code.\n");
	        free(tokens);  // Free tokens before exiting
	        return 1;
	    }

	    if(!mode) {
		    generate_gwbasic_code(ast, 0);
	    }
	    free_ast(ast);
	    putchar('\n');
    }

    // Free tokens and AST
    free_tokens(tokens);

    return 0;
}

