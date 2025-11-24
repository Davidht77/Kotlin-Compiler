#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;
class VarDec;

// Operadores binarios soportados
enum BinaryOp { 
    PLUS_OP, 
    MINUS_OP, 
    MUL_OP, 
    DIV_OP,
    POW_OP,
    MOD_OP, // Added MOD
    LE_OP,
    LT_OP,  // Added LT
    GT_OP,  // Added GT
    GE_OP,  // Added GE
    EQ_OP,  // Added EQ
    NE_OP,  // Added NE
    AND_OP, // Added AND
    OR_OP   // Added OR
};

class Stm{
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

// Clase abstracta Exp
class Exp : public Stm { // Exp inherits from Stm
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();

};

// Expresión numérica
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    NumberExp(int v);
    ~NumberExp();
};

class BoolExp : public Exp { // Added BoolExp
public:
    bool value;
    int accept(Visitor* visitor);
    BoolExp(bool v);
    ~BoolExp();
};

// Expresión numérica
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    IdExp(string v);
    ~IdExp();
};


class VarDec : public Stm { // Inherit from Stm to allow VarDec in StmtList
public:
    string type;
    string name; // Changed from list<string> vars to single name per grammar
    Exp* init;   // Added initializer
    bool isConst; // Added const/val distinction
    VarDec(string name, string type, Exp* init, bool isConst);
    int accept(Visitor* visitor);
    ~VarDec();
};

// Replaced Body with Block to match grammar
class Block : public Stm {
public:
    list<Stm*> stmts;
    int accept(Visitor* visitor);
    Block();
    ~Block();
};

class IfStmt: public Stm {
public:
    Exp* condition;
    Block* thenBlock;
    Block* elseBlock; // Can be nullptr
    IfStmt(Exp* condition, Block* thenBlock, Block* elseBlock);
    int accept(Visitor* visitor);
    ~IfStmt(){};
};

class WhileStmt: public Stm {
public:
    Exp* condition;
    Block* block;
    WhileStmt(Exp* condition, Block* block);
    int accept(Visitor* visitor);
    ~WhileStmt(){};
};

class ForStmt: public Stm { // Added ForStmt
public:
    string varName;
    Exp* rangeExp; // "in Exp"
    Block* block;
    ForStmt(string varName, Exp* rangeExp, Block* block);
    int accept(Visitor* visitor);
    ~ForStmt(){};
};

class AssignExp: public Exp { // Renamed from AssignStm and inherits Exp
public:
    string id;
    Exp* e;
    AssignExp(string, Exp*);
    ~AssignExp();
    int accept(Visitor* visitor);
};

class PrintStm: public Stm {
public:
    Exp* e;
    PrintStm(Exp*);
    ~PrintStm();
    int accept(Visitor* visitor);
};

class ReturnStm: public Stm {
public:
    Exp* e;
    ReturnStm(Exp* e);
    ~ReturnStm(){};
    int accept(Visitor* visitor);
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    FcallExp(string nombre, vector<Exp*> args);
    ~FcallExp(){};
};

class FunDec{
public:
    string nombre;
    string tipo;
    Block* cuerpo;
    vector<string> Ptipos;
    vector<string> Pnombres;
    int accept(Visitor* visitor);
    FunDec(string nombre, string tipo, vector<string> Ptipos, vector<string> Pnombres, Block* cuerpo);
    ~FunDec(){};
};

class Program{
public:
    list<VarDec*> vdlist;
    list<FunDec*> fdlist;
    Program();
    ~Program();
    int accept(Visitor* visitor);
};

#endif // AST_H
