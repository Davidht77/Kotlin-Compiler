#include "ast.h"
#include <iostream>

using namespace std;

Exp::~Exp() {}
Stm::~Stm() {}

string Exp::binopToChar(BinaryOp op) {
    switch(op) {
        case PLUS_OP: return "+";
        case MINUS_OP: return "-";
        case MUL_OP: return "*";
        case DIV_OP: return "/";
        case POW_OP: return "^";
        case MOD_OP: return "%";
        case LE_OP: return "<=";
        case LT_OP: return "<";
        case GT_OP: return ">";
        case GE_OP: return ">=";
        case EQ_OP: return "==";
        case NE_OP: return "!=";
        case AND_OP: return "&&";
        case OR_OP: return "||";
        default: return "?";
    }
}

BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op) : left(l), right(r), op(op) {}
BinaryExp::~BinaryExp() { delete left; delete right; }

NumberExp::NumberExp(int v) : value(v) {}
NumberExp::~NumberExp() {}

BoolExp::BoolExp(bool v) : value(v) {}
BoolExp::~BoolExp() {}

IdExp::IdExp(string v) : value(v) {}
IdExp::~IdExp() {}

VarDec::VarDec(string name, string type, Exp* init, bool isConst) 
    : name(name), type(type), init(init), isConst(isConst) {}
VarDec::~VarDec() { if(init) delete init; }

Block::Block() {}
Block::~Block() {
    for (Stm* s : stmts) delete s;
}

IfStmt::IfStmt(Exp* condition, Block* thenBlock, Block* elseBlock) 
    : condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {}

WhileStmt::WhileStmt(Exp* condition, Block* block) 
    : condition(condition), block(block) {}

ForStmt::ForStmt(string varName, Exp* rangeExp, Block* block)
    : varName(varName), rangeExp(rangeExp), block(block) {}

AssignExp::AssignExp(string id, Exp* e) : id(id), e(e) {}
AssignExp::~AssignExp() { delete e; }

PrintStm::PrintStm(Exp* e) : e(e) {}
PrintStm::~PrintStm() { delete e; }

ReturnStm::ReturnStm(Exp* e) : e(e) {}

FcallExp::FcallExp(string nombre, vector<Exp*> args) : nombre(nombre), argumentos(args) {}

FunDec::FunDec(string nombre, string tipo, vector<string> Ptipos, vector<string> Pnombres, Block* cuerpo)
    : nombre(nombre), tipo(tipo), Ptipos(Ptipos), Pnombres(Pnombres), cuerpo(cuerpo) {}

Program::Program() {}
Program::~Program() {
    for (VarDec* v : vdlist) delete v;
    for (FunDec* f : fdlist) delete f;
}
