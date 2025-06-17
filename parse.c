#include "9cc.h"

Var *locals;

Var *find_var(Token *tok) {
    for (Var *var = locals; var; var = var->next)
        if (var->len == tok->len && !strncmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

char *copy_token_str(Token *tok) {
    char *buf = calloc(tok->len + 1, 1);
    memcpy(buf, tok->str, tok->len);
    buf[tok->len] = '\0';
    return buf;
}

Node *new_node(NodeKind kind, Token *tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->tok = tok;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok) {
    Node *node = new_node(kind, tok);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_unary(NodeKind kind, Node *expr, Token *tok) {
    Node *node = new_node(kind, tok);
    node->lhs = expr;
    return node;
}

Node *new_num(int val, Token *tok) {
    Node *node = new_node(ND_NUM, tok);
    node->val = val;
    return node;
}

Node *new_var(Var *var, Token *tok) {
    Node *node = new_node(ND_VAR, tok);
    node->var = var;
    return node;
}

Node *code[100];

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

void program() {
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt() {
    Node *node;
    Token *tok;

    if (consume_keyword("return")) {
        node = new_unary(ND_RETURN, expr(), tok);
        expect(";");
        return node;
    }

    if (consume_keyword("if")) {
        node = new_node(ND_IF, tok);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume_keyword("else")) 
            node->els = stmt();
        return node;
    }

    if (consume_keyword("while")) {
        node = new_node(ND_WHILE, tok);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }

    if (consume_keyword("for")) {
        node = new_node(ND_FOR, tok);
        expect("(");

        if (!consume(";")) {
            node->init = expr();
            expect(";");
        }
        if (!consume(";")) {
            node->cond = expr();
            expect(";");
        }
        if(!consume(")")) {
            node->inc = expr();
            expect(")");
        }
        node->then = stmt();
        return node;
    }

    if (consume("{")) {
        Node head;
        head.next = NULL;
        Node *cur = &head;

        while (!consume("}")) {
            cur->next = stmt();
            cur = cur->next;
        }
        node = new_node(ND_BLOCK, tok);
        node->body = head.next;
        return node;
    }

    node = expr();
    expect(";");
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    Token *tok;
    if (consume("=")) 
        node = new_binary(ND_ASSIGN, node, assign(), tok);
    return node;
}

Node *equality() {
    Node *node = relational();
    Token *tok;

    for(;;) {
        if (consume("=="))
            node = new_binary(ND_EQ, node, relational(), tok);
        else if(consume("!="))
            node = new_binary(ND_NE, node, relational(), tok);
        else   
            return node;
    }
}

Node *relational() {
    Node *node = add();
    Token *tok;

    for(;;) {
        if (consume("<"))
            node = new_binary(ND_LT, node, add(), tok);
        else if(consume("<="))
            node = new_binary(ND_LE, node, add(), tok);
        else if(consume(">"))
            node = new_binary(ND_LT, add(), node, tok);
        else if(consume(">="))
            node = new_binary(ND_LE, add(), node, tok);
        else   
            return node;
    }
}

Node *add() {
    Node *node = mul();
    Token *tok;

    for(;;) {
        if (consume("+"))
            node = new_binary(ND_ADD, node, mul(), tok);
        else if(consume("-"))
            node = new_binary(ND_SUB, node, mul(), tok);
        else   
            return node;
    }
}

Node *mul() {
    Node *node = unary();
    Token *tok;

    for(;;) {
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary(), tok);
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary(), tok);
        else
            return node;
    }
}

Node *unary() {
    Token *tok;
    if (consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_num(0, tok), unary(), tok);
    return primary();
}

// func-args = "(" (assign ("," assign)*)? ")"
Node *func_args() {
    if (consume(")"))
        return NULL;

    Node *head = assign();
    Node *cur = head;
    while (consume(",")) {
        cur->next = assign();
        cur = cur->next;
    }
    expect(")");
    return head;
}

Node *primary() {
    Token *tok; 

    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    tok = consume_ident();
    if (tok) {
        if (consume("(")) {
            Node *node = new_node(ND_FUNCALL, tok);
            node->funcname = copy_token_str(tok);
            node->args = func_args();
            return node;
        }

        Var *var = find_var(tok);
        if (var) {
            return new_var(var, tok);
        } else {
            var = calloc(1, sizeof(Var));
            var->next = locals;
            var->name = tok->str;
            var->len = tok->len;
            if (locals == NULL) {
                var->offset = 8;
            } else {
                var->offset = locals->offset + 8;
            }
            locals = var;
            return new_var(var, tok);
        }
    }

    return new_num(expect_number(), tok);
}
