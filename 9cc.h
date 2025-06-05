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
    TK_RETURN,   // return 
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
bool consume_return(); // returnのconsume
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
    ND_LVAR, // ローカル変数
    ND_NUM, // Integer
    ND_RETURN, // return
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺
    Node *rhs;     // 右辺
    int val;       // ND_NUMの場合使用
    int offset;    // ND_LVARの場合使用
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

typedef struct LVar LVar;
// ローカル変数の型
struct LVar {
    LVar *next;  // 次の変数かNULL
    char *name;  // 変数名
    int len;     // 名前の長さ
    int offset;  // rbpからのオフセット 
};

// 変数名で検索し、見つからない場合はNULLを返す
LVar *find_lvar(Token *tok);