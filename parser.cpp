#include <iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* p = new Program();
    // VarDecList ::= (VarDec)*
    // We check for starts of VarDec: const, val, var
    while (check(Token::CONST) || check(Token::VAL) || check(Token::VAR)) {
        p->vdlist.push_back(parseVarDec());
    }
    
    // FunDecList ::= (FunDec)+
    // Must have at least one function
    if (check(Token::FUN)) {
        p->fdlist.push_back(parseFunDec());
        while (check(Token::FUN)) {
            p->fdlist.push_back(parseFunDec());
        }
    } else {
        if (!isAtEnd()) {
             throw runtime_error("Expected function declaration");
        }
    }
    
    cout << "Parser exitoso" << endl;
    return p;
}

VarDec* Parser::parseVarDec() {
    // VarDec ::= VarSymbol id TypeAnnotationOpt InitializerOpt StmtTerminator
    // VarSymbol ::= ("const" | ε) ("val" | "var")
    
    bool isConst = false;
    if (match(Token::CONST)) {
        isConst = true;
    }
    
    if (match(Token::VAL)) {
        // val implies const usually, but grammar has separate const.
        // Let's say val is immutable by default, var is mutable.
        // If const is present, it's definitely const.
        if (!isConst) isConst = true; 
    } else if (match(Token::VAR)) {
        // var is mutable
    } else {
        throw runtime_error("Expected 'val' or 'var'");
    }
    
    if (!match(Token::ID)) throw runtime_error("Expected variable name");
    string name = previous->text;
    
    string type = "";
    // TypeAnnotationOpt ::= ":" Type | ε
    if (match(Token::COLON)) {
        if (!match(Token::ID)) throw runtime_error("Expected type");
        type = previous->text;
    }
    
    Exp* init = nullptr;
    // InitializerOpt ::= "=" Exp | ε
    if (match(Token::ASSIGN)) {
        init = parseExp();
    }
    
    // StmtTerminator ::= ";" | Newline (we use SEMICOL)
    if (!match(Token::SEMICOL)) throw runtime_error("Expected ';'");
    
    return new VarDec(name, type, init, isConst);
}

FunDec* Parser::parseFunDec() {
    // FunDec ::= "fun" id "(" ParamListOpt ")" TypeAnnotationOpt Block
    match(Token::FUN);
    
    if (!match(Token::ID)) throw runtime_error("Expected function name");
    string name = previous->text;
    
    match(Token::LPAREN);
    
    vector<string> pNames;
    vector<string> pTypes;
    
    // ParamListOpt ::= (ParamDec ("," ParamDec)*) | ε
    // ParamDec ::= VarSymbol id TypeAnnotationOpt
    if (!check(Token::RPAREN)) {
        while (match(Token::COMA)){
             // VarSymbol
             bool paramConst = false;
             if (match(Token::CONST)) paramConst = true;
             if (match(Token::VAL)) { /* val */ }
             else if (match(Token::VAR)) { /* var */ }
             else {
                throw runtime_error("Expected 'val' or 'var' in parameter");
             }
             
             if (!match(Token::ID)) throw runtime_error("Expected parameter name");
             pNames.push_back(previous->text);
             
             string pType = "";
             if (match(Token::COLON)) {
                 if (!match(Token::ID)) throw runtime_error("Expected parameter type");
                 pType = previous->text;
             }
             pTypes.push_back(pType);
             
        }
    }
    
    match(Token::RPAREN);
    
    string returnType = "";
    if (match(Token::COLON)) {
        if (!match(Token::ID)) throw runtime_error("Expected return type");
        returnType = previous->text;
    }
    
    Block* body = parseBlock();
    
    return new FunDec(name, returnType, pTypes, pNames, body);
}

Block* Parser::parseBlock() {
    // Block ::= "{" StmtListOpt "}"
    match(Token::LKEY);
    Block* b = new Block();
    
    // StmtListOpt ::= StmtList | ε
    // StmtList ::= (Stmt StmtTerminator)*
    // Stmt ::= VarDec | Exp | PrintStmt | IfStmt | WhileStmt | ForStmt | ReturnStmt
    
    while (!check(Token::RKEY) && !isAtEnd()) {
        b->stmts.push_back(parseStmt());
     }
    match(Token::RKEY);
    return b;
}

Stm* Parser::parseStmt() {
    Stm* s = nullptr;
    
    if (check(Token::CONST) || check(Token::VAL) || check(Token::VAR)) {
        s = parseVarDec();
    }
    else if (match(Token::PRINT) || match(Token::PRINTLN)) {
        match(Token::LPAREN);
        Exp* e = nullptr;
        if (!check(Token::RPAREN)) {
            e = parseExp();
        }
        match(Token::RPAREN);
        match(Token::SEMICOL);
        s = new PrintStm(e);
    }
    else if (match(Token::IF)) {
        match(Token::LPAREN);
        Exp* cond = parseExp();
        match(Token::RPAREN);
        Block* thenB = parseBlock();
        Block* elseB = nullptr;
        if (match(Token::ELSE)) {
            elseB = parseBlock();
        }
        s = new IfStmt(cond, thenB, elseB);
    }
    else if (match(Token::WHILE)) {
        match(Token::LPAREN);
        Exp* cond = parseExp();
        match(Token::RPAREN);
        Block* b = parseBlock();
        s = new WhileStmt(cond, b);
    }
    else if (match(Token::FOR)) {
        match(Token::LPAREN);
        if (!match(Token::ID)) throw runtime_error("Expected variable in for");
        string varName = previous->text;
        if (!match(Token::IN)) throw runtime_error("Expected 'in'");
        Exp* range = parseExp();
        match(Token::RPAREN);
        Block* b = parseBlock();
        s = new ForStmt(varName, range, b);
    }
    else if (match(Token::RETURN)) {
        Exp* e = nullptr;
        // ExpOpt
        if (!check(Token::SEMICOL)) {
             e = parseExp();
        }
        match(Token::SEMICOL);
        s = new ReturnStm(e);
    }
    else {
        // Exp
        Exp* e = parseExp();
        match(Token::SEMICOL);
        s = e; // Exp inherits Stm now
    }
    
    return s;
}

// Exp ::= Assignment
Exp* Parser::parseExp() {
    if (check(Token::ID)) {
        Token* savedId = current;
        advance();
        if (match(Token::ASSIGN)) {
            string name = savedId->text;
            Exp* val = parseExp(); // Right recursive Assignment
            return new AssignExp(name, val); // Returns Exp*
        } else {
            Exp* l = parseLogicOr();
            if (match(Token::ASSIGN)) {
                // Check if l is lvalue (IdExp)
                IdExp* idExp = dynamic_cast<IdExp*>(l);
                if (!idExp) throw runtime_error("Invalid assignment target");
                string name = idExp->value;
                delete l; // Replace IdExp with AssignExp
                Exp* r = parseExp();
                return new AssignExp(name, r); // Returns Exp*
            }
            return l;
        }
    }
    return parseLogicOr();
}

// LogicOr ::= LogicAnd ("||" LogicAnd)*
Exp* Parser::parseLogicOr() {
    Exp* l = parseLogicAnd();
    while (match(Token::DISJ)) {
        Exp* r = parseLogicAnd();
        l = new BinaryExp(l, r, OR_OP);
    }
    return l;
}

// LogicAnd ::= Equality ("&&" Equality)*
Exp* Parser::parseLogicAnd() {
    Exp* l = parseEquality();
    while (match(Token::CONJ)) {
        Exp* r = parseEquality();
        l = new BinaryExp(l, r, AND_OP);
    }
    return l;
}

// Equality ::= Relational (("=="|"!=") Relational)*
Exp* Parser::parseEquality() {
    Exp* l = parseRelational();
    while (check(Token::EQ) || check(Token::NE)) {
        BinaryOp op = check(Token::EQ) ? EQ_OP : NE_OP;
        advance();
        Exp* r = parseRelational();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Relational ::= Additive (("<"|">"|"<="|">=") Additive)*
Exp* Parser::parseRelational() {
    Exp* l = parseAdditive();
    while (check(Token::LT) || check(Token::GT) || check(Token::LE) || check(Token::GE)) {
        BinaryOp op;
        if (check(Token::LT)) op = LT_OP;
        else if (check(Token::GT)) op = GT_OP;
        else if (check(Token::LE)) op = LE_OP;
        else op = GE_OP;
        advance();
        Exp* r = parseAdditive();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Additive ::= Multiplicative (("+"|"-") Multiplicative)*
Exp* Parser::parseAdditive() {
    Exp* l = parseMultiplicative();
    while (check(Token::PLUS) || check(Token::MINUS)) {
        BinaryOp op = check(Token::PLUS) ? PLUS_OP : MINUS_OP;
        advance();
        Exp* r = parseMultiplicative();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Multiplicative ::= Unary (("*"|"/"|"%") Unary)*
Exp* Parser::parseMultiplicative() {
    Exp* l = parseUnary();
    while (check(Token::MUL) || check(Token::DIV) || check(Token::MOD)) {
        BinaryOp op;
        if (check(Token::MUL)) op = MUL_OP;
        else if (check(Token::DIV)) op = DIV_OP;
        else op = MOD_OP;
        advance();
        Exp* r = parseUnary();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

// Unary ::= ("+"|"-"|"!") Unary | Primary
Exp* Parser::parseUnary() {
    if (match(Token::PLUS)) {
        return parseUnary(); // Unary + is no-op
    } else if (match(Token::MINUS)) {
        Exp* e = parseUnary();
        return new BinaryExp(new NumberExp(0), e, MINUS_OP); // 0 - e
    } else if (match(Token::NOT)) {
        Exp* e = parseUnary();
        return new BinaryExp(e, new BoolExp(false), EQ_OP);
    }
    return parsePrimary();
}

// Primary ::= id | Num | Bool | "(" Exp ")" | FunctionCall
Exp* Parser::parsePrimary() {
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::TRUE)) {
        return new BoolExp(true);
    }
    else if (match(Token::FALSE)) {
        return new BoolExp(false);
    }
    else if (match(Token::LPAREN)) {
        Exp* e = parseExp();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::ID)) {
        string name = previous->text;
        if (check(Token::LPAREN)) {
            // FunctionCall
            match(Token::LPAREN);
            vector<Exp*> args;
            if (!check(Token::RPAREN)) {
                do {
                    args.push_back(parseExp());
                } while (match(Token::COMA));
            }
            match(Token::RPAREN);
            return new FcallExp(name, args);
        } else {
            return new IdExp(name);
        }
    }
    throw runtime_error("Expected expression");
}
