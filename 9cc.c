#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Token kind
typedef enum {
	TK_RESERVED, // symbol
	TK_NUM, // Integer Token
	TK_EOF, // End-of-input Token
} TokenKind;

typedef struct Token Token;

// Token type
struct Token {
	TokenKind kind; // 
	Token *next; // Next input token
	int val; // If kind is TK_NUM, the number
	char *str; // Token string
};

// Token currently focused on
Token *token;

// Function for reporting errors
// Takes the same arguments as printf
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// When the next token is the expected symbol, read one token forward
// true is returned. Otherwise, return false
bool consume(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op)
		return false;
	token = token->next;
	return true;
}

// If the next token is the expected symbol, read one token.
// Otherwise, report an error
void expect(char op) {
	if (token->kind != TK_RESERVED || token->str[0] != op)
		error("'c' is not ", op);
	token = token->next;
}

// If the next token is a number, advaces one token and returns the number
// Otherwise, report an error
int expect_number() {
	if (token->kind != TK_NUM)
		error("Number is not");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

// Create a new token and connect it to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

// Tokenize the input string p and retrun it
Token *tokenize(char *p) {
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p) {
		// Skip whitespace characters
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p == '+' || *p == '-') {
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error("Cannot tokenize");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
	fprintf(stderr, " ");
	return 1;
    }
	
	// Tokenize
	token = tokenize(argv[1]);

	// Output first half of assembly
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");

	// The first part of the equation must be a number, so check it.
	// Out put the first mov instruction
	printf("    mov rax, %d\n", expect_number());

	while (!at_eof()) {
		if (consume('+')) {
			printf("	add rax, %d\n", expect_number());
			continue;
		}

		expect('-');
		printf("	sub rax, %d\n", expect_number());
	}
	printf("	ret\n");
	return 0;
}

