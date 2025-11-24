#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Program::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}


int PrintStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IfStmt::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int WhileStmt::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int ForStmt::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Block::accept(Visitor* visitor){
    return visitor->visit(this);
}

int VarDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int FcallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int FunDec::accept(Visitor* visitor){
    return visitor->visit(this);
}

int ReturnStm::accept(Visitor* visitor){
    return visitor->visit(this);
}


///////////////////////////////////////////////////////////////////////////////////

int GenCodeVisitor::generar(Program* program) {
    program->accept(this);
        return 0;
}

int GenCodeVisitor::visit(Program* program) {
    out << ".data\nprint_fmt: .string \"%ld \\n\""<<endl;

    for (auto dec : program->vdlist){
        dec->accept(this);
    }

    for (auto& [var, _] : memoriaGlobal) {
        out << var << ": .quad 0"<<endl;
    }

    out << ".text\n";
    
    for (auto dec : program->fdlist){
        dec->accept(this);
    }

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
        return 0;
}

int GenCodeVisitor::visit(VarDec* stm) {
    // Updated to match new VarDec structure
    string var = stm->name;
    if (!entornoFuncion) {
        memoriaGlobal[var] = true;
    } else {
        memoria[var] = offset;
        offset -= 8;
    }
    // Handle initializer if present
    if (stm->init) {
        stm->init->accept(this);
        if (memoriaGlobal.count(var))
            out << " movq %rax, " << var << "(%rip)"<<endl;
        else
            out << " movq %rax, " << memoria[var] << "(%rbp)"<<endl;
    }
    return 0;
}



int GenCodeVisitor::visit(NumberExp* exp) {
    out << " movq $" << exp->value << ", %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    out << " movq $" << (exp->value ? 1 : 0) << ", %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    if (memoriaGlobal.count(exp->value))
        out << " movq " << exp->value << "(%rip), %rax"<<endl;
    else
        out << " movq " << memoria[exp->value] << "(%rbp), %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << " pushq %rax\n";
    exp->right->accept(this);
    out << " movq %rax, %rcx\n popq %rax\n";

    switch (exp->op) {
        case PLUS_OP:  out << " addq %rcx, %rax\n"; break;
        case MINUS_OP: out << " subq %rcx, %rax\n"; break;
        case MUL_OP:   out << " imulq %rcx, %rax\n"; break;
        // Added other ops support or placeholders
        case DIV_OP:   out << " cqo\n idivq %rcx\n"; break; // Basic div
        case MOD_OP:   out << " cqo\n idivq %rcx\n movq %rdx, %rax\n"; break; // Mod
        case LE_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setle %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case LT_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setl %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GT_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setg %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GE_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setge %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case EQ_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " sete %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case NE_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setne %al\n"
                      << " movzbq %al, %rax\n";
            break;
        // AND/OR need short-circuit or bitwise? Using bitwise for now for simplicity
        case AND_OP: out << " andq %rcx, %rax\n"; break;
        case OR_OP:  out << " orq %rcx, %rax\n"; break;
        default: break;
    }
    return 0;
}


int GenCodeVisitor::visit(AssignExp* stm) {
    stm->e->accept(this);
    if (memoriaGlobal.count(stm->id))
        out << " movq %rax, " << stm->id << "(%rip)"<<endl;
    else
        out << " movq %rax, " << memoria[stm->id] << "(%rbp)"<<endl;
            return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);
    out <<
        " movq %rax, %rsi\n"
        " leaq print_fmt(%rip), %rdi\n"
        " movl $0, %eax\n"
        " call printf@PLT\n";
            return 0;
}



int GenCodeVisitor::visit(Block* b) {
    for (auto s : b->stmts){
        s->accept(this);
    }
        return 0;
}

int GenCodeVisitor::visit(IfStmt* stm) {
    int label = labelcont++;
    stm->condition->accept(this);
    out << " cmpq $0, %rax"<<endl;
    out << " je else_" << label << endl;
    stm->thenBlock->accept(this);
    out << " jmp endif_" << label << endl;
    out << " else_" << label << ":"<< endl;
    if (stm->elseBlock) stm->elseBlock->accept(this);
    out << "endif_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(WhileStmt* stm) {
    int label = labelcont++;
    out << "while_" << label << ":"<<endl;
    stm->condition->accept(this);
    out << " cmpq $0, %rax" << endl;
    out << " je endwhile_" << label << endl;
    stm->block->accept(this);
    out << " jmp while_" << label << endl;
    out << "endwhile_" << label << ":"<< endl;
    return 0;
}

int GenCodeVisitor::visit(ForStmt* stm) {
    // Placeholder implementation for ForStmt
    // Needs loop logic
    return 0;
}


int GenCodeVisitor::visit(ReturnStm* stm) {
    if (stm->e) stm->e->accept(this);
    out << " jmp .end_"<<nombreFuncion << endl;
    return 0;
}

int GenCodeVisitor::visit(FunDec* f) {
    entornoFuncion = true;
    memoria.clear();
    offset = -8;
    nombreFuncion = f->nombre;
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    out << ".globl " << f->nombre << endl;
    out << f->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    int size = f->Pnombres.size();
    for (int i = 0; i < size; i++) {
        memoria[f->Pnombres[i]]=offset;
        out << " movq " << argRegs[i] << "," << offset << "(%rbp)" << endl;
        offset -= 8;
    }
    
    // Declarations are now inside Block (cuerpo), but we might need to pre-allocate stack?
    // Visitor visits block which visits statements including VarDecs.
    // We need to calculate stack size.
    // Simple approach: visit body to count vars? Or just dynamic push?
    // Or just reserve a big chunk.
    // Existing code did: `for (auto i: f->cuerpo->declarations)`
    // New Block has `stmts` which mixes VarDec and others.
    // We'll just reserve some space or rely on push/pop if we were stack based, but we use RBP offsets.
    // We need to know how many locals.
    // For now, let's just reserve 128 bytes.
    int reserva = 128; 
    out << " subq $" << reserva << ", %rsp" << endl;
    
    f->cuerpo->accept(this);
    
    out << ".end_"<< f->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int size = exp->argumentos.size();
    for (int i = 0; i < size; i++) {
        exp->argumentos[i]->accept(this);
        out << " mov %rax, " << argRegs[i] <<endl;
    }
    out << "call " << exp->nombre << endl;
    return 0;
}
