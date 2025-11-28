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
    rangeType = new Type(Type::RANGE); // Added
    currentVarCount = 0;
}

// ===========================================================
//   Registrar funciones globales
// ===========================================================

// Helper to infer return type from function body
Type* TypeChecker::inferReturnType(Stm* s) {
    if (!s) return nullptr;

    if (ReturnStm* ret = dynamic_cast<ReturnStm*>(s)) {
        if (ret->e) {
            return ret->e->accept(this);
        } else {
            return voidType;
        }
    }

    if (Block* b = dynamic_cast<Block*>(s)) {
        for (Stm* stmt : b->stmts) {
            Type* t = inferReturnType(stmt);
            if (t) return t;
        }
    }

    if (IfStmt* ifStmt = dynamic_cast<IfStmt*>(s)) {
        Type* t = inferReturnType(ifStmt->thenBlock);
        if (t) return t;
        if (ifStmt->elseBlock) {
            return inferReturnType(ifStmt->elseBlock);
        }
    }

    if (WhileStmt* whileStmt = dynamic_cast<WhileStmt*>(s)) {
        return inferReturnType(whileStmt->block);
    }

    if (ForStmt* forStmt = dynamic_cast<ForStmt*>(s)) {
        return inferReturnType(forStmt->block);
    }

    return nullptr;
}

void TypeChecker::add_function(FunDec* fd) {
    if (functions.find(fd->nombre) != functions.end()) {
        cerr << "Error: función '" << fd->nombre << "' ya fue declarada." << endl;
        exit(0);
    }

    Type* returnType = new Type();
    if (fd->tipo.empty()) {
        // Inference logic
        // Temporarily add parameters to env to allow expression type checking
        env.add_level();
        for (size_t i = 0; i < fd->Pnombres.size(); ++i) {
            Type* pt = new Type();
             // Assuming parameters must have explicit types for now
             if (fd->Ptipos[i].empty()) {
                 cerr << "Error: parámetros deben tener tipo explícito en función '" << fd->nombre << "'." << endl;
                 exit(0);
             }
            if (!pt->set_basic_type(fd->Ptipos[i])) {
                cerr << "Error: tipo de parámetro inválido en función '" << fd->nombre << "'." << endl;
                exit(0);
            }
            env.add_var(fd->Pnombres[i], pt);
        }

        Type* inferred = inferReturnType(fd->cuerpo);
        env.remove_level();

        if (inferred) {
            returnType = inferred;
        } else {
            returnType->ttype = Type::VOID;
        }
    } else if (!returnType->set_basic_type(fd->tipo)) {
        cerr << "Error: tipo de retorno no válido en función '" << fd->nombre << "'." << endl;
        exit(0);
    }

    functions[fd->nombre] = returnType;
}

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
    if (v->type.empty()) {
        // Inference from initializer
        if (v->init) {
             t = v->init->accept(this);
        } else {
            cerr << "Error: variable '" << v->name << "' sin tipo ni inicializador." << endl;
            exit(0);
        }
    } else if (!t->set_basic_type(v->type)) {
        cerr << "Error: tipo de variable no válido: '" << v->type << "'" << endl;
        // Debug: print ascii values
        cerr << "Debug: ";
        for (char c : v->type) cerr << (int)c << " ";
        cerr << endl;
        exit(0);
    }

    if (!v->type.empty() && v->init) {
        Type* initType = v->init->accept(this);
        if (!initType->canAssignTo(t)) {
             cerr << "Error: tipo de inicializador incompatible con variable '" << v->name << "'." << endl;
             exit(0);
        }
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
        
        currentVarCount++;
    }
    functionVarCounts[currentFunction] = currentVarCount;

    functionVarCounts[currentFunction] = currentVarCount;

    // Use the type already registered in add_function
    Type* returnType = functions[f->nombre];
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

    if (!expType->canAssignTo(varType)) {
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
        if (!(t->canAssignTo(retornodefuncion))) {
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
    
    env.add_level();
    env.add_var(stm->varName, intType);
    
    // Count the loop variable
    if (!currentFunction.empty()) {
        currentVarCount++;
        functionVarCounts[currentFunction] = currentVarCount;
    }

    Type* rangeT = stm->rangeExp->accept(this); // Visit range to check types there
    if (!rangeT->match(rangeType)) {
        cerr << "Error: for loop range must be a range type." << endl;
        exit(0);
    }
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
            // Allow all numeric types
            if (!((left->ttype >= Type::INT && left->ttype <= Type::ULONG) && 
                  (right->ttype >= Type::INT && right->ttype <= Type::ULONG))) {
                cerr << "Error: operación aritmética requiere operandos numéricos." << endl;
                exit(0);
            }
            // Simplified: return intType for everything or left type.
            return left; 
        case LE_OP:
        case LT_OP:
        case GT_OP:
        case GE_OP:
        case EQ_OP:
        case NE_OP:
            if (!((left->ttype >= Type::INT && left->ttype <= Type::ULONG) && 
                  (right->ttype >= Type::INT && right->ttype <= Type::ULONG)) &&
                !((left->match(boolType) && right->match(boolType)))) {
                 // Allow comparison of numbers or bools
                 if (!left->match(right)) {
                    // Relaxed check for numbers?
                    if (!((left->ttype >= Type::INT && left->ttype <= Type::ULONG) && 
                          (right->ttype >= Type::INT && right->ttype <= Type::ULONG))) {
                        cerr << "Error: tipos incompatibles en comparación." << endl;
                        exit(0);
                    }
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
             if (!(left->isNumeric() && right->isNumeric())) {
                cerr << "Error: rango requiere operandos numéricos." << endl;
                exit(0);
             }
             return rangeType;

        case STEP_OP:
             if (!((left->match(rangeType) || left->isNumeric()) && right->isNumeric())) {
                cerr << "Error: step requiere un rango (o número) y un paso numérico." << endl;
                exit(0);
             }
             return rangeType;

        default:
            cerr << "Error: operador binario no soportado." << endl;
            exit(0);
    }
    e->inferredType = left; // Assign inferred type (simplified)
    return left;
}

Type* TypeChecker::visit(NumberExp* e) { 
    e->inferredType = intType; 
    return intType; 
}

Type* TypeChecker::visit(BoolExp* e) { 
    e->inferredType = boolType;
    return boolType; 
}

Type* TypeChecker::visit(StringExp* e) { 
    e->inferredType = stringType;
    return stringType; 
} // Added

Type* TypeChecker::visit(IdExp* e) {
    if (!env.check(e->value)) {
        cerr << "Error: variable '" << e->value << "' no declarada." << endl;
        exit(0);
    }
    Type* t = env.lookup(e->value);
    e->inferredType = t;
    return t;
}

Type* TypeChecker::visit(FcallExp* e) {
    auto it = functions.find(e->nombre);
    if (it == functions.end()) {
        cerr << "Error: llamada a función no declarada '" << e->nombre << "'." << endl;
        exit(0);
    }

    Type* t = it->second;
    e->inferredType = t;
    return t;
}
