#include "typechecker.h"
#include <iostream>
#include <stdexcept>
using namespace std;


Type* NumberExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BoolExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* IdExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* BinaryExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* FcallExp::accept(TypeVisitor* v) { return v->visit(this); }
Type* StringExp::accept(TypeVisitor* v) { return v->visit(this); } // Added

Type* AssignExp::accept(TypeVisitor* v) { return v->visit(this); } // Changed from AssignStm
Type* PrintStm::accept(TypeVisitor* v) { return v->visit(this); }
Type* ReturnStm::accept(TypeVisitor* v) { return v->visit(this); }
Type* WhileStmt::accept(TypeVisitor* v) { return v->visit(this); } // Added
Type* IfStmt::accept(TypeVisitor* v) { return v->visit(this); }    // Added
Type* ForStmt::accept(TypeVisitor* v) { return v->visit(this); }   // Added

Type* VarDec::accept(TypeVisitor* v) { return v->visit(this); }
Type* FunDec::accept(TypeVisitor* v) { return v->visit(this); }
Type* Block::accept(TypeVisitor* v) { return v->visit(this); } // Changed from Body
Type* Program::accept(TypeVisitor* v) { return v->visit(this); }

// ===========================================================
//   Constructor del TypeChecker
// ===========================================================

TypeChecker::TypeChecker() {
    intType = new Type(Type::INT);
    boolType = new Type(Type::BOOL);
    voidType = new Type(Type::VOID);
    stringType = new Type(Type::STRING); // Added
    currentVarCount = 0;
}

// ===========================================================
//   Registrar funciones globales
// ===========================================================

void TypeChecker::add_function(FunDec* fd) {
    if (functions.find(fd->nombre) != functions.end()) {
        cerr << "Error: función '" << fd->nombre << "' ya fue declarada." << endl;
        exit(0);
    }

    Type* returnType = new Type();
    if (!returnType->set_basic_type(fd->tipo)) {
        cerr << "Error: tipo de retorno no válido en función '" << fd->nombre << "'." << endl;
        exit(0);
    }

    functions[fd->nombre] = returnType;
}

// ===========================================================
//   Método principal de verificación
// ===========================================================

void TypeChecker::typecheck(Program* program) {
    if (program) program->accept(this);
    // cout << "Revisión exitosa" << endl; // Optional: Comment out to reduce noise
}

// ===========================================================
//   Nivel superior: Programa y Bloque
// ===========================================================

Type* TypeChecker::visit(Program* p) {
    // Primero registrar funciones
    for (auto f : p->fdlist)
        add_function(f);

    env.add_level();
    for (auto v : p->vdlist)
        v->accept(this);  
    for (auto f : p->fdlist)
        f->accept(this);  
    env.remove_level();
    return voidType;
}

Type* TypeChecker::visit(Block* b) {
    env.add_level();
    for (auto s : b->stmts)
        s->accept(this); 
    env.remove_level();
    return voidType;
}

// ===========================================================
//   Declaraciones
// ===========================================================

Type* TypeChecker::visit(VarDec* v) {
    Type* t = new Type();
    if (!t->set_basic_type(v->type)) { // Changed from v->tipo to v->type
        cerr << "Error: tipo de variable no válido." << endl;
        exit(0);
    }

    if (env.check(v->name)) { // Changed from v->variables loop to v->name
        cerr << "Error: variable '" << v->name << "' ya declarada." << endl;
        exit(0);
    }
    env.add_var(v->name, t);

    // Variable counting logic
    if (!currentFunction.empty()) {
        currentVarCount++;
        functionVarCounts[currentFunction] = currentVarCount;
    }
    return voidType;
}

Type* TypeChecker::visit(FunDec* f) {
    currentFunction = f->nombre;
    currentVarCount = 0;
    functionVarCounts[currentFunction] = 0; // Initialize

    env.add_level();
    for (size_t i = 0; i < f->Pnombres.size(); ++i) {
        Type* pt = new Type();
        if (!pt->set_basic_type(f->Ptipos[i])) { // Changed from Tparametros to Ptipos
            cerr << "Error: tipo de parámetro inválido en función '" << f->nombre << "'." << endl;
            exit(0);
        }
        env.add_var(f->Pnombres[i], pt); // Changed from Nparametros to Pnombres
        
        // Parameters also count as local variables for stack offset purposes in some architectures,
        // but usually they are passed in registers or stack above RBP. 
        // For GenCodeVisitor logic, they are assigned offsets if they spill or if we want to be safe.
        // Let's check GenCodeVisitor::visit(FunDec). 
        // It assigns offsets to parameters too! So we should count them.
        currentVarCount++;
    }
    functionVarCounts[currentFunction] = currentVarCount;

    Type* returnType = new Type();
    returnType->set_basic_type(f->tipo);
    retornodefuncion = returnType;
    f->cuerpo->accept(this);

    env.remove_level();
    currentFunction = "";
    return voidType;
}

// ===========================================================
//   Sentencias
// ===========================================================

Type* TypeChecker::visit(PrintStm* stm) {
    Type* t = stm->e->accept(this);
    if (!(t->match(intType) || t->match(boolType) || t->match(stringType))) { // Added stringType support
        cerr << "Error: tipo inválido en print (solo int, bool o string)." << endl;
        exit(0);
    }
    return voidType;
}

Type* TypeChecker::visit(AssignExp* stm) { // Changed from AssignStm
    if (!env.check(stm->id)) {
        cerr << "Error: variable '" << stm->id << "' no declarada." << endl;
        exit(0);
    }

    Type* varType = env.lookup(stm->id);
    Type* expType = stm->e->accept(this);

    if (!varType->match(expType)) {
        cerr << "Error: tipos incompatibles en asignación a '" << stm->id << "'." << endl;
        exit(0);
    }
    return voidType;
}

Type* TypeChecker::visit(ReturnStm* stm) {
    if (stm->e) {
        Type* t = stm->e->accept(this);
        if (!(t->match(intType) || t->match(boolType) || t->match(voidType) || t->match(stringType))) {
            cerr << "Error: tipo inválido en return." << endl;
            exit(0);
        }
        // Note: Strict return type checking against function signature can be added here
        // but keeping it simple as per original code structure, just checking validity of type itself.
        // Actually original code checked against 'retornodefuncion'.
        if (!(t->match(retornodefuncion))) {
             cerr << "Error: retorno distinto al declarado en la función." << endl;
             exit(0);
        }
    } else {
        if (!retornodefuncion->match(voidType)) {
            cerr << "Error: retorno vacío en función no void." << endl;
            exit(0);
        }
    }
    return voidType;
}

Type* TypeChecker::visit(WhileStmt* stm) {
    Type* t = stm->condition->accept(this);
    if (!t->match(boolType)) {
        cerr << "Error: condición de while debe ser bool." << endl;
        exit(0);
    }
    stm->block->accept(this);
    return voidType;
}

Type* TypeChecker::visit(IfStmt* stm) {
    Type* t = stm->condition->accept(this);
    if (!t->match(boolType)) {
        cerr << "Error: condición de if debe ser bool." << endl;
        exit(0);
    }
    stm->thenBlock->accept(this);
    if (stm->elseBlock) {
        stm->elseBlock->accept(this);
    }
    return voidType;
}

Type* TypeChecker::visit(ForStmt* stm) {
    // Simplified For check: assuming range is int
    // In a full implementation, we'd check if rangeExp is a Range/DownTo expression
    // and if the variable is declared or needs to be added to env.
    // For now, let's assume the loop variable is a new local int.
    
    env.add_level();
    env.add_var(stm->varName, intType);
    
    // Count the loop variable
    if (!currentFunction.empty()) {
        currentVarCount++;
        functionVarCounts[currentFunction] = currentVarCount;
    }

    stm->rangeExp->accept(this); // Visit range to check types there
    stm->block->accept(this);
    env.remove_level();
    return voidType;
}

// ===========================================================
//   Expresiones
// ===========================================================

Type* TypeChecker::visit(BinaryExp* e) {
    Type* left = e->left->accept(this);
    Type* right = e->right->accept(this);

    switch (e->op) {
        case PLUS_OP: 
        case MINUS_OP: 
        case MUL_OP: 
        case DIV_OP: 
        case POW_OP:
        case MOD_OP: // Added MOD_OP
            if (!(left->match(intType) && right->match(intType))) {
                cerr << "Error: operación aritmética requiere operandos int." << endl;
                exit(0);
            }
            return intType;
        case LE_OP:
        case LT_OP:
        case GT_OP:
        case GE_OP:
        case EQ_OP:
        case NE_OP:
            if (!((left->match(intType) && right->match(intType)) || (left->match(boolType) && right->match(boolType)))) {
                 // Allow comparison of ints or bools
                 // For stricter typing, maybe only ints for <, >, etc.
                 // But == and != should work for both.
                 // Keeping it simple: if types match, it's ok.
                 if (!left->match(right)) {
                    cerr << "Error: tipos incompatibles en comparación." << endl;
                    exit(0);
                 }
            }
            return boolType;

        case AND_OP:
        case OR_OP:
            if (!(left->match(boolType) && right->match(boolType))) {
                cerr << "Error: operación lógicas requiere operandos bool." << endl;
                exit(0);
            }
            return boolType;
            
        case RANGE_OP:
        case DOWNTO_OP:
        case STEP_OP:
             // These return a Range type ideally, but for now let's say they are valid if operands are int
             if (!(left->match(intType) && right->match(intType))) {
                cerr << "Error: rango requiere operandos int." << endl;
                exit(0);
             }
             return intType; // Returning int as a placeholder for Range

        default:
            cerr << "Error: operador binario no soportado." << endl;
            exit(0);
    }
}

Type* TypeChecker::visit(NumberExp* e) { return intType; }

Type* TypeChecker::visit(BoolExp* e) { return boolType; }

Type* TypeChecker::visit(StringExp* e) { return stringType; } // Added

Type* TypeChecker::visit(IdExp* e) {
    if (!env.check(e->value)) {
        cerr << "Error: variable '" << e->value << "' no declarada." << endl;
        exit(0);
    }
    return env.lookup(e->value);
}

Type* TypeChecker::visit(FcallExp* e) {
    auto it = functions.find(e->nombre);
    if (it == functions.end()) {
        cerr << "Error: llamada a función no declarada '" << e->nombre << "'." << endl;
        exit(0);
    }
    // Check arguments?
    // For full type checking, we should verify argument types match parameter types.
    return it->second;
}
