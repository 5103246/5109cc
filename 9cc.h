#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// tokenize.c
//

// Token 
typedef enum {
    TK_RESERVED, // Keywords or punctuators
    TK_IDENT,    // Identifiers
    TK_NUM,      // Integer literals
    TK_EOF,      // End-of-file markers
    TK_KEYWORD,   // return, if, while, for
} TokenKind;

// Token type
typedef struct Token Token;
struct Token {
    TokenKind kind; // Token kind
    Token *next;    // Next Token
    int val;        // If kind is TK_NUM, use val
    char *str;      // Token string
    int len;        // Token length
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
bool consume_keyword(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
bool startswith(char *p, char *q);
bool is_alpha(char c);
bool is_alnum(char c);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);

extern Token *token;
extern char *user_input;

//
// parse.c
//

// Variable
typedef struct Var Var;
struct Var {
    Var *next;   // Next variable or NULL
    char *name;  // Variable name
    int len;     // Name length
    int offset;  // Offset from RBP 
};

Var *find_var(Token *tok);

// Node
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_LT,  // <
    ND_LE,  // <=
    ND_ASSIGN, // =
    ND_VAR, // Variable
    ND_NUM, // Integer
    ND_RETURN, // "return"
    ND_IF,        // "if"
    ND_WHILE,     // "while"
    ND_FOR,       // "for"
    ND_BLOCK,     // { ... }
    ND_FUNCALL,   // Function call
} NodeKind;

// Node type
typedef struct Node Node;
struct Node {
    NodeKind kind; // Node kind
    Node *next;    // Next node
    Token *tok;    // Representative token
    Node *lhs;     // Left-hand side
    Node *rhs;     // Right-hand side
    
    int val;     
    Var *var;

    // "if", "while" or "for"
    Node *cond;
    Node *then;
    Node *els;  
    Node *init;
    Node *inc;

    // Block 
    Node *body;

    // Fuction call
    char *funcname;
    Node *args;
};

extern Node *code[100];

Node *new_node(NodeKind kind, Token *tok);
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);

typedef struct Function Fuction;
struct Fuction {
    Fuction *next;
    char *name;
    Node *node;
    Var *locals;
    int stack_size;
};