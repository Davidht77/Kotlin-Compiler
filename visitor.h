#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class BinaryExp;
class NumberExp;
class BoolExp; // Added
class Program;
class PrintStm;
class WhileStmt; // Renamed from WhileStm
class IfStmt;    // Renamed from IfStm
class AssignExp; // Renamed from AssignStm
class Block;     // Renamed from Body
class VarDec;
class FcallExp;
class ReturnStm;
class FunDec;
class ForStmt;   // Added
class IdExp;

class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(BoolExp* exp) = 0; // Added
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(Program* p) = 0;
    virtual int visit(PrintStm* stm) = 0;
    virtual int visit(WhileStmt* stm) = 0;
    virtual int visit(IfStmt* stm) = 0;
    virtual int visit(AssignExp* stm) = 0; // Renamed
    virtual int visit(Block* block) = 0; // Changed Body to Block
    virtual int visit(VarDec* vd) = 0;
    virtual int visit(FcallExp* fcall) = 0;
    virtual int visit(ReturnStm* r) = 0;
    virtual int visit(FunDec* fd) = 0;
    virtual int visit(ForStmt* stm) = 0; // Added
};


class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;
public:
    GenCodeVisitor(std::ostream& out) : out(out) {}
    int generar(Program* program);
    unordered_map<string, int> memoria;
    unordered_map<string, bool> memoriaGlobal;
    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    string nombreFuncion;
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(BoolExp* exp) override; // Added
    int visit(IdExp* exp) override;
    int visit(Program* p) override ;
    int visit(PrintStm* stm) override;
    int visit(AssignExp* stm) override; // Renamed
    int visit(WhileStmt* stm) override;
    int visit(IfStmt* stm) override;
    int visit(Block* block) override; // Changed Body to Block
    int visit(VarDec* vd) override;
    int visit(FcallExp* fcall) override;
    int visit(ReturnStm* r) override;
    int visit(FunDec* fd) override;
    int visit(ForStmt* stm) override; // Added
};

#endif // VISITOR_H