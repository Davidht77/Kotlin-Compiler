#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <unordered_map>
#include <string>
#include "ast.h"
#include "environment.h"
#include "semantic_types.h"

using namespace std;

class BinaryExp;
class NumberExp;
class DoubleExp; // Added
class Program;
class PrintStm;
class AssignExp; // Changed from AssignStm
class FunDec;
class ReturnStm;
class Block; // Changed from Body
class VarDec;
class FcallExp;
class BoolExp;
class WhileStmt; // Added
class IfStmt;    // Added
class ForStmt;   // Added
class StringExp; // Added

class TypeVisitor {
public:

    // --- Nodos de nivel superior ---
    virtual Type* visit(Program* p) = 0;
    virtual Type* visit(Block* b) = 0; // Changed from Body
    virtual Type* visit(VarDec* v) = 0;
    virtual Type* visit(FunDec* f) = 0;

    // --- Sentencias ---
    virtual Type* visit(PrintStm* stm) = 0;
    virtual Type* visit(AssignExp* stm) = 0; // Changed from AssignStm
    virtual Type* visit(ReturnStm* stm) = 0;
    virtual Type* visit(WhileStmt* stm) = 0; // Added
    virtual Type* visit(IfStmt* stm) = 0;    // Added
    virtual Type* visit(ForStmt* stm) = 0;   // Added

    // --- Expresiones ---
    virtual Type* visit(BinaryExp* e) = 0;
    virtual Type* visit(NumberExp* e) = 0;
    virtual Type* visit(DoubleExp* e) = 0; // Added
    virtual Type* visit(IdExp* e) = 0;
    virtual Type* visit(BoolExp* e) = 0;
    virtual Type* visit(FcallExp* e) = 0;
    virtual Type* visit(StringExp* e) = 0; // Added
};



// ──────────────────────────────────────────────
//   CLASE TYPECHECKER
// ──────────────────────────────────────────────

class TypeChecker : public TypeVisitor {
private:
    Environment<Type*> env;                 // Entorno de variables y sus tipos
    unordered_map<string, Type*> functions; // Entorno de funciones

    // Tipos básicos
    Type* intType;
    Type* boolType;
    Type* voidType;
    Type* stringType; // Added
    Type* rangeType; // Added
    Type* retornodefuncion;
    // Registro de funciones
    void add_function(FunDec* fd);
    
    // Helper for return type inference
    Type* inferReturnType(Stm* s);

    // Variable counting
    string currentFunction;
    int currentVarCount;

public:
    unordered_map<string, int> functionVarCounts; // Public to access from main

    TypeChecker();

    // Método principal de verificación
    void typecheck(Program* program);

    // --- Visitas de alto nivel ---
    Type* visit(Program* p) override;
    Type* visit(Block* b) override; // Changed from Body
    Type* visit(VarDec* v) override;
    Type* visit(FunDec* f) override;

    // --- Sentencias ---
    Type* visit(PrintStm* stm) override;
    Type* visit(AssignExp* stm) override; // Changed from AssignStm
    Type* visit(ReturnStm* stm) override;
    Type* visit(WhileStmt* stm) override; // Added
    Type* visit(IfStmt* stm) override;    // Added
    Type* visit(ForStmt* stm) override;   // Added

    // --- Expresiones ---
    Type* visit(BinaryExp* e) override;
    Type* visit(NumberExp* e) override;
    Type* visit(DoubleExp* e) override; // Added
    Type* visit(IdExp* e) override;
    Type* visit(BoolExp* e) override;
    Type* visit(FcallExp* e) override;
    Type* visit(StringExp* e) override; // Added
};

#endif // TYPECHECKER_H
