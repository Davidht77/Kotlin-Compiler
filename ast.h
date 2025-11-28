#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;
class VarDec;
class Type; // Forward declaration for Type
class TypeVisitor; // Forward declaration for TypeVisitor

// Operadores binarios soportados
enum BinaryOp { 
    PLUS_OP, 
    MINUS_OP, 
    MUL_OP, 
    DIV_OP,
    POW_OP,
    MOD_OP,
    LE_OP,
    LT_OP,
    GT_OP,
    GE_OP,
    EQ_OP,
    NE_OP,
    AND_OP,
    OR_OP,
    RANGE_OP,
    DOWNTO_OP,
    STEP_OP
};

class Stm{
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual Type* accept(TypeVisitor* visitor) = 0; // Added
    virtual ~Stm() = 0;
};

// Clase abstracta Exp
class Exp : public Stm { // Exp inherits from Stm
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string
    virtual Type* accept(TypeVisitor* visitor) = 0; // Para verificador de tipos
    Type* inferredType = nullptr; // Para guardar el tipo inferido
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    ~BinaryExp();

};

// Expresión numérica
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    NumberExp(int v);
    ~NumberExp();
};

// Expresión numérica (Double)
class DoubleExp : public Exp {
public:
    double value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor);
    DoubleExp(double v);
    ~DoubleExp();
};

class BoolExp : public Exp {
public:
    bool value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    BoolExp(bool v);
    ~BoolExp();
};

// Expresión de cadena de texto (String Literal)
class StringExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    StringExp(string v);
    ~StringExp();
};

// Expresión ID
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    IdExp(string v);
    ~IdExp();
};


class VarDec : public Stm { // Inherit from Stm to allow VarDec in StmtList
public:
    string type;
    string name; 
    Exp* init;   
    bool isConst; 
    VarDec(string name, string type, Exp* init, bool isConst);
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // Changed to Type*
    ~VarDec();
};

// Replaced Body with Block to match grammar
class Block : public Stm {
public:
    list<Stm*> stmts;
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // Changed to Type*
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
    Type* accept(TypeVisitor* visitor); // Changed to Type*
    ~IfStmt(){};
};

class WhileStmt: public Stm {
public:
    Exp* condition;
    Block* block;
    WhileStmt(Exp* condition, Block* block);
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // Changed to Type*
    ~WhileStmt(){};
};

class ForStmt: public Stm { 
public:
    string varName;
    Exp* rangeExp; // "in Exp"
    Block* block;
    ForStmt(string varName, Exp* rangeExp, Block* block);
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // Changed to Type*
    ~ForStmt(){};
};

class AssignExp: public Exp { // Renamed from AssignStm and inherits Exp
public:
    string id;
    Exp* e;
    AssignExp(string, Exp*);
    Type* accept(TypeVisitor* visitor); // nuevo
    ~AssignExp();
    int accept(Visitor* visitor);
};

class PrintStm: public Stm {
public:
    Exp* e;
    PrintStm(Exp*);
    ~PrintStm();
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // Changed to Type*
};

class ReturnStm: public Stm {
public:
    Exp* e;
    ReturnStm(Exp* e);
    ~ReturnStm(){};
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // Changed to Type*
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    Exp* receiver; // Optional receiver for method-style calls (e.g., 100.toByte())
    int accept(Visitor* visitor);
    Type* accept(TypeVisitor* visitor); // nuevo
    FcallExp(string nombre, vector<Exp*> args, Exp* receiver = nullptr);
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
    Type* accept(TypeVisitor* visitor); // Changed to Type*
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
    Type* accept(TypeVisitor* visitor); // Changed to Type*
};

#endif // AST_H
