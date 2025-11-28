#include <iostream>
#include "ast.h"
#include "visitor.h"
#include "semantic_types.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

// Helper to get register name based on size
string getReg(string baseReg, int size) {
    if (baseReg == "rax") {
        if (size == 1) return "%al";
        if (size == 2) return "%ax";
        if (size == 4) return "%eax";
        return "%rax";
    }
    if (baseReg == "rcx") {
        if (size == 1) return "%cl";
        if (size == 2) return "%cx";
        if (size == 4) return "%ecx";
        return "%rcx";
    }
    if (baseReg == "rdx") {
        if (size == 1) return "%dl";
        if (size == 2) return "%dx";
        if (size == 4) return "%edx";
        return "%rdx";
    }
    if (baseReg == "rbx") {
        if (size == 1) return "%bl";
        if (size == 2) return "%bx";
        if (size == 4) return "%ebx";
        return "%rbx";
    }
    // Add more if needed
    return "%" + baseReg;
}

// Helper to get instruction suffix
string getSuffix(int size) {
    if (size == 1) return "b";
    if (size == 2) return "w";
    if (size == 4) return "l";
    return "q";
}

int getTypeSize(Type* t) {
    if (!t) return 8; // Default to 64-bit if unknown
    if (t->ttype == Type::BYTE || t->ttype == Type::UBYTE) return 1;
    if (t->ttype == Type::SHORT || t->ttype == Type::USHORT) return 2;

    if (t->ttype == Type::INT || t->ttype == Type::UINT) return 4;
    if (t->ttype == Type::LONG || t->ttype == Type::ULONG) return 8;
    if (t->ttype == Type::BOOL) return 1; // Bool as byte
    return 8; // Default
}

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int StringExp::accept(Visitor* visitor) {
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
// Implementación de GenCodeVisitor

int GenCodeVisitor::generar(Program* program) {
    // Inicializar offset y estado de función antes de comenzar
    offset = -8; 
    entornoFuncion = false; 
    program->accept(this);
    return 0;
}

int GenCodeVisitor::visit(Program* program) {
    // 1. Sección de datos
    out << ".data\n";
    out << "print_fmt_num: .string \"%ld \\n\""<<endl;
    out << "print_fmt_str: .string \"%s\\n\""<<endl;

    // A. Recorrer VarDecs Globales para registrarlas y definirlas estáticamente.
    for (auto dec : program->vdlist){
        memoriaGlobal[dec->name] = true; // Registrar la variable como global

        // 1. Manejar StringExp para recolectar la literal (si es StringExp).
        if (dec->init) {
            StringExp* stringExp = dynamic_cast<StringExp*>(dec->init);
            if (stringExp) {
                if (!stringLiterals.count(stringExp->value)) {
                    string label = "str_" + to_string(stringCont++);
                    stringLiterals[stringExp->value] = label;
                }
            }
        }
        
        // 2. Generar la DEFINICIÓN ESTÁTICA (.quad).
        out << dec->name << ": .quad ";
        
        NumberExp* numExp = dynamic_cast<NumberExp*>(dec->init);
        BoolExp* boolExp = dynamic_cast<BoolExp*>(dec->init);
        
        if (numExp) {
            out << numExp->value << endl;
        } else if (boolExp) {
            out << (boolExp->value ? 1 : 0) << endl;
        } else {
            out << "0" << endl; 
        }
    }
    
    // 2. Sección de código
    out << ".text\n";
    out << ".global main\n";
    
    // Punto de entrada `main` por defecto (mantenemos su lógica)
    if (program->fdlist.empty()) {
        out << "main:\n";
        out << " pushq %rbp\n";
        out << " movq %rsp, %rbp\n";
        out << " movl $0, %eax\n";
        out << " popq %rbp\n";
        out << " ret\n";
    }

    // Funciones
    for (auto dec : program->fdlist){
        dec->accept(this);
    }

    // B. Imprimir las literales de cadena recolectadas (al final para incluir las de funciones)
    if (!stringLiterals.empty()) {
        out << ".data\n";
        for (auto& pair : stringLiterals) {
            out << pair.second << ": .string \"" << pair.first << "\""<<endl;
        }
    }

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
    return 0;
}

int GenCodeVisitor::visit(VarDec* stm) {
    string var = stm->name;
    
    if (!entornoFuncion) {
        memoriaGlobal[var] = true;
        if (stm->init) {
            StringExp* stringExp = dynamic_cast<StringExp*>(stm->init);
            if (stringExp) {
                if (!stringLiterals.count(stringExp->value)) {
                    string label = "str_" + to_string(stringCont++);
                    stringLiterals[stringExp->value] = label;
                }
            }
        }
        return 0; 
    } else { 
        // Use Environment for local variables
        env.add_var(var, offset);
        int varOffset = offset;
        offset -= 8;
        
        if (stm->init) {
            stm->init->accept(this); 
            // Store based on type size
            int size = getTypeSize(stm->init->inferredType);
            string reg = getReg("rax", size);
            out << " mov" << getSuffix(size) << " " << reg << ", " << varOffset << "(%rbp)"<<endl;
        }
    }
    return 0;
}

int GenCodeVisitor::visit(NumberExp* exp) {
    int size = getTypeSize(exp->inferredType);
    string reg = getReg("rax", size);
    out << " mov" << getSuffix(size) << " $" << exp->value << ", " << reg << endl;
    return 0;
}

int GenCodeVisitor::visit(BoolExp* exp) {
    int size = getTypeSize(exp->inferredType);
    string reg = getReg("rax", size);
    out << " mov" << getSuffix(size) << " $" << (exp->value ? 1 : 0) << ", " << reg << endl;
    return 0;
}

int GenCodeVisitor::visit(StringExp* exp) {
    string label;
    if (stringLiterals.count(exp->value)) {
        label = stringLiterals[exp->value];
    } else {
        label = "str_" + to_string(stringCont++);
        stringLiterals[exp->value] = label;
    }
    out << " leaq " << label << "(%rip), %rax\n"; 
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    int size = getTypeSize(exp->inferredType);
    string reg = getReg("rax", size);

    if (memoriaGlobal.count(exp->value))
        out << " mov" << getSuffix(size) << " " << exp->value << "(%rip), " << reg << endl;
    else {
        // Lookup in environment
        int varOffset = env.lookup(exp->value);
        out << " mov" << getSuffix(size) << " " << varOffset << "(%rbp), " << reg << endl;
    }
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << " pushq %rax\n";
    exp->right->accept(this);
    out << " movq %rax, %rcx\n popq %rax\n";

    int size = getTypeSize(exp->left->inferredType); // Assume left determines size for arithmetic
    string suffix = getSuffix(size);
    string regAx = getReg("rax", size);
    string regCx = getReg("rcx", size);

    switch (exp->op) {
        case PLUS_OP:  out << " add" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case MINUS_OP: out << " sub" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case MUL_OP:   out << " imul" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case DIV_OP:   
            if (size == 1) out << " cbw\n idivb " << regCx << "\n";
            else if (size == 2) out << " cwd\n idivw " << regCx << "\n";
            else if (size == 4) out << " cdq\n idivl " << regCx << "\n";
            else out << " cqo\n idivq " << regCx << "\n";
            break; 
        case MOD_OP:   
            if (size == 1) out << " cbw\n idivb " << regCx << "\n movzbq %ah, %rax\n"; // Remainder in AH
            else if (size == 2) out << " cwd\n idivw " << regCx << "\n movzwq %dx, %rax\n"; // Remainder in DX
            else if (size == 4) out << " cdq\n idivl " << regCx << "\n movslq %edx, %rax\n"; // Remainder in EDX
            else out << " cqo\n idivq " << regCx << "\n movq %rdx, %rax\n"; // Remainder in RDX
            break; 
        case POW_OP:   out << " # WARNING: Operador POW (**) no implementado\n"; break;

        // Operadores de Comparación
        case LE_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setle %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case LT_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setl %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GT_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setg %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GE_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setge %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case EQ_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " sete %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case NE_OP:
            out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"
                      << " movl $0, %eax\n"
                      << " setne %al\n"
                      << " movzbq %al, %rax\n";
            break;
        // Operadores Lógicos (simplificados)
        case AND_OP: out << " and" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        case OR_OP:  out << " or" << suffix << " " << regCx << ", " << regAx << "\n"; break;
        default: break;
    }
    return 0;
}

int GenCodeVisitor::visit(AssignExp* stm) {
    stm->e->accept(this);
    int size = getTypeSize(stm->e->inferredType);
    string reg = getReg("rax", size);

    if (memoriaGlobal.count(stm->id))
        out << " mov" << getSuffix(size) << " " << reg << ", " << stm->id << "(%rip)"<<endl;
    else {
        int varOffset = env.lookup(stm->id);
        out << " mov" << getSuffix(size) << " " << reg << ", " << varOffset << "(%rbp)"<<endl;
    }
    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this); 

    StringExp* stringExp = dynamic_cast<StringExp*>(stm->e); 

    if (stringExp) {
        out << " movq %rax, %rsi\n"; 
        out << " leaq print_fmt_str(%rip), %rdi\n"; 
    } else {
        // Ensure value is sign-extended to 64-bit for printf
        int size = getTypeSize(stm->e->inferredType);
        if (size == 1) out << " movsbq %al, %rsi\n";
        else if (size == 2) out << " movswq %ax, %rsi\n";
        else if (size == 4) out << " movslq %eax, %rsi\n";
        else out << " movq %rax, %rsi\n"; 
        
        out << " leaq print_fmt_num(%rip), %rdi\n"; 
    }
    
    out << " movl $0, %eax\n"
        << " call printf@PLT\n";
    return 0;
}

int GenCodeVisitor::visit(Block* b) {
    env.add_level(); // New scope
    for (auto s : b->stmts){
        s->accept(this);
    }
    env.remove_level(); // End scope
    return 0;
}

int GenCodeVisitor::visit(IfStmt* stm) {
    int label = labelcont++;
    stm->condition->accept(this);
    out << " cmpq $0, %rax"<<endl;
    out << " je else_" << label << endl;
    stm->thenBlock->accept(this);
    out << " jmp endif_" << label << endl;
    out << "else_" << label << ":"<< endl;
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
    int label = labelcont++;
    int saved_offset = offset;
    
    env.add_level(); // Scope for loop variable

    Exp* range = stm->rangeExp;
    Exp* start = nullptr;
    Exp* end = nullptr;
    Exp* step = nullptr;
    bool isDownTo = false;
    
    BinaryExp* stepExp = dynamic_cast<BinaryExp*>(range);
    if (stepExp && stepExp->op == STEP_OP) {
        step = stepExp->right;
        range = stepExp->left; 
    }
    
    BinaryExp* rangeBin = dynamic_cast<BinaryExp*>(range);
    if (rangeBin) {
        if (rangeBin->op == RANGE_OP) {
            start = rangeBin->left;
            end = rangeBin->right;
            isDownTo = false;
        } else if (rangeBin->op == DOWNTO_OP) {
            start = rangeBin->left;
            end = rangeBin->right;
            isDownTo = true;
        }
    }
    
    if (!start || !end) {
        start = new NumberExp(0); 
    }

    // Determine type size for loop variable (assume Int or based on range start)
    // Ideally we should check inferredType of start/end.
    // For now, let's assume Int (4 bytes) as standard for loops unless specified otherwise.
    // Or better, check start->inferredType.
    int size = 4; 
    if (start->inferredType) size = getTypeSize(start->inferredType);

    string suffix = getSuffix(size);
    string regAx = getReg("rax", size);
    string regCx = getReg("rcx", size);

    start->accept(this);
    int start_offset = offset;
    offset -= 8;
    out << " mov" << suffix << " " << regAx << ", " << start_offset << "(%rbp)\n";
    
    end->accept(this);
    int end_offset = offset;
    offset -= 8;
    out << " mov" << suffix << " " << regAx << ", " << end_offset << "(%rbp)\n";
    
    int step_offset = offset;
    offset -= 8;
    if (step) {
        step->accept(this);
    } else {
        out << " mov" << suffix << " $1, " << regAx << "\n";
    }
    out << " mov" << suffix << " " << regAx << ", " << step_offset << "(%rbp)\n";

    string var = stm->varName;
    // Add loop variable to environment
    env.add_var(var, offset);
    int varOffset = offset;
    offset -= 8;
    
    out << " mov" << suffix << " " << start_offset << "(%rbp), " << regAx << "\n";
    out << " mov" << suffix << " " << regAx << ", " << varOffset << "(%rbp)\n";
    
    out << "loop_" << label << ":\n";
    
    out << " mov" << suffix << " " << varOffset << "(%rbp), " << regAx << "\n"; 
    out << " mov" << suffix << " " << end_offset << "(%rbp), " << regCx << "\n";   
    out << " cmp" << suffix << " " << regCx << ", " << regAx << "\n"; 
    
    if (isDownTo) {
        out << " jl endloop_" << label << "\n"; 
    } else {
        out << " jg endloop_" << label << "\n";
    }
    
    stm->block->accept(this);
    
    out << " mov" << suffix << " " << varOffset << "(%rbp), " << regAx << "\n";
    out << " mov" << suffix << " " << step_offset << "(%rbp), " << regCx << "\n";
    if (isDownTo) {
        out << " sub" << suffix << " " << regCx << ", " << regAx << "\n"; 
    } else {
        out << " add" << suffix << " " << regCx << ", " << regAx << "\n"; 
    }
    out << " mov" << suffix << " " << regAx << ", " << varOffset << "(%rbp)\n";
    
    out << " jmp loop_" << label << "\n";
    
    out << "endloop_" << label << ":\n";
    
    env.remove_level(); // End loop scope
    offset = saved_offset;
    return 0;
}

int GenCodeVisitor::visit(FunDec* f) {
    offset = -8;
    nombreFuncion = f->nombre;
    vector<std::string> argRegs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; // Base names
    out << ".globl " << f->nombre << endl;
    out <<f->nombre<<":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    
    env.add_level(); // Scope for function arguments and local variables
    
    int size = f->Pnombres.size();
    for (int i = 0; i < size; i++) {
        env.add_var(f->Pnombres[i], offset);
        
        Type* t = new Type();
        t->set_basic_type(f->Ptipos[i]);
        int argSize = getTypeSize(t);
        string reg = getReg(argRegs[i], argSize);
        string suffix = getSuffix(argSize);

        if (i < argRegs.size()) {
            out << " mov" << suffix << " " << reg << "," << offset << "(%rbp)" << endl;
        } else {
            // Stack arguments are always 8 bytes on x86-64
            int arg_stack_pos = 16 + (i - (int)argRegs.size()) * 8; 
            out << " movq " << arg_stack_pos << "(%rbp), %rax\n"; // Read 8 bytes from stack
            // Store to local variable, potentially smaller size
            string regAx = getReg("rax", argSize);
            out << " mov" << suffix << " " << regAx << ", " << offset << "(%rbp)" << endl;
        }
        offset -= 8;
    }
    
    // Calculate reservation based on variable count
    int numVars = 0;
    if (functionVarCounts.count(f->nombre)) {
        numVars = functionVarCounts[f->nombre];
    } else {
        numVars = 16; // Default fallback
    }

    int reserva = (numVars * 8 + 15) / 16 * 16; // Round up to multiple of 16
    
    out << " subq $" << reserva << ", %rsp" << endl;
    
    f->cuerpo->accept(this);
    
    out << ".end_"<< f->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;
    env.remove_level(); // Remove argument scope
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    if (stm->e) {
        stm->e->accept(this); 
    }
    out << " leave\n";
    out << " ret\n";
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    vector<std::string> argRegs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; // Base names
    int size = exp->argumentos.size();
    
    int num_stack_args = max(0, size - (int)argRegs.size());
    for (int i = size - 1; i >= (int)argRegs.size(); i--) {
        exp->argumentos[i]->accept(this);
        // Push is always 64-bit, so we must ensure RAX has the value.
        // If accept returned a byte in AL, we should probably zero-extend it if we want to be safe,
        // but pushq %rax pushes whatever is in RAX.
        // For stack arguments, the callee expects them at specific offsets.
        // If callee expects Byte, it reads 1 byte.
        out << " pushq %rax\n";
    }

    for (int i = 0; i < min(size, (int)argRegs.size()); i++) {
        exp->argumentos[i]->accept(this);
        int argSize = getTypeSize(exp->argumentos[i]->inferredType);
        string reg = getReg(argRegs[i], argSize);
        string regAx = getReg("rax", argSize);
        out << " mov" << getSuffix(argSize) << " " << regAx << ", " << reg <<endl;
    }

    out << " movl $0, %eax\n"; 
    out << "call " << exp->nombre << endl;

    if (num_stack_args > 0) {
        out << " addq $" << num_stack_args * 8 << ", %rsp\n";
    }
    
    return 0;
}