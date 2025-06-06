#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークン種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数
    TK_EOF,      // 入力の終わり
    TK_KEYWORD,   // return, if, while, for
} TokenKind;

typedef struct Token Token;
// トークン型
struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // TK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;        // トークンの長さ
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

// 着目トークン
extern Token *token;
// 入力プログラム
extern char *user_input;

typedef struct LVar LVar;
// ローカル変数の型
struct LVar {
    LVar *next;  // 次の変数かNULL
    char *name;  // 変数名
    int len;     // 名前の長さ
    int offset;  // rbpからのオフセット 
};

LVar *find_lvar(Token *tok);

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
    ND_LVAR, // local variable
    ND_NUM, // Integer
    ND_RETURN, // "return"
    ND_IF,        // "if"
    ND_WHILE,     // "while"
    ND_FOR,       // "for"
    ND_BLOCK,     // { ... }
    ND_FUNCALL,   // Function call
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind; // ノードの型
    Node *next;    // 次のノード
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // ND_NUMの場合使用
    int offset;    // ND_LVARの場合使用

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
};

extern Node *code[100];

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
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
    LVar *locals;
    int stack_size;
};