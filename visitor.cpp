#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
// Implementación de los métodos 'accept' para la interfaz Visitor (en AST)

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

// *** CORRECCIÓN: Método accept para StringExp (Necesario para dynamic_cast) ***
int StringExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
// ******************************************************************************

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

// MODIFICADO: Añade formatos de string y la definición de las literales
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
            // Ejemplo: var a: Int = 5; -> a: .quad 5
            out << numExp->value << endl;
        } else if (boolExp) {
            // Ejemplo: var b: Bool = true; -> b: .quad 1
            out << (boolExp->value ? 1 : 0) << endl;
        } else {
            // Default a 0 para StringExp o Expresiones más complejas
            out << "0" << endl; 
        }
    }
    
    // B. Imprimir las literales de cadena recolectadas
    for (auto& pair : stringLiterals) {
        out << pair.second << ": .string \"" << pair.first << "\""<<endl;
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

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
    return 0;
}

// MODIFICADO: Manejo de registro de strings globales
// En visitor.cpp

int GenCodeVisitor::visit(VarDec* stm) {
    string var = stm->name;
    
    if (!entornoFuncion) {
        // Entorno Global: La inicialización ya es estática (en Program::visit).
        // Solo mantenemos la lógica de registro por seguridad (aunque ya debe estar en Program::visit).
        memoriaGlobal[var] = true;
        
        // Manejar StringExp para registrar la literal (mantenido por seguridad)
        if (stm->init) {
            StringExp* stringExp = dynamic_cast<StringExp*>(stm->init);
            if (stringExp) {
                if (!stringLiterals.count(stringExp->value)) {
                    string label = "str_" + to_string(stringCont++);
                    stringLiterals[stringExp->value] = label;
                }
            }
        }
        
        return 0; // <<--- CORRECCIÓN CLAVE: NO GENERAR CÓDIGO EJECUTABLE AQUÍ
        
    } else { // Entorno Local (dentro de una función)
        memoria[var] = offset;
        offset -= 8;
        
        // Handle initializer if present (Asignación dinámica para LOCALES)
        if (stm->init) {
            stm->init->accept(this); // Evalúa la expresión en %rax
            out << " movq %rax, " << memoria[var] << "(%rbp)"<<endl;
        }
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

// NUEVO: Visitar StringExp
int GenCodeVisitor::visit(StringExp* exp) {
    string label;
    
    // 1. Reutilizar etiqueta o crear una nueva
    if (stringLiterals.count(exp->value)) {
        label = stringLiterals[exp->value];
    } else {
        label = "str_" + to_string(stringCont++);
        stringLiterals[exp->value] = label;
    }
    
    // 2. Cargar la dirección de la literal de cadena en %rax
    out << " leaq " << label << "(%rip), %rax\n"; 
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
        case DIV_OP:   out << " cqo\n idivq %rcx\n"; break; 
        case MOD_OP:   out << " cqo\n idivq %rcx\n movq %rdx, %rax\n"; break; 
        case POW_OP:   out << " # WARNING: Operador POW (**) no implementado\n"; break;

        // Operadores de Comparación
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
        // Operadores Lógicos (simplificados)
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

// MODIFICADO: Soporte para impresión de números y cadenas
int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this); // Evalúa la expresión -> resultado en %rax

    // Determinar si la expresión es una literal de cadena (Requiere dynamic_cast)
    StringExp* stringExp = dynamic_cast<StringExp*>(stm->e); 

    if (stringExp) {
        // Caso 1: Cadena. %rax contiene la dirección (leaq).
        out << " movq %rax, %rsi\n"; // Dirección de la cadena a RSI
        out << " leaq print_fmt_str(%rip), %rdi\n"; // Formato de string a RDI
    } else {
        // Caso 2: Número/Booleano/ID/Binario. %rax contiene el valor.
        out << " movq %rax, %rsi\n"; // Valor a RSI
        out << " leaq print_fmt_num(%rip), %rdi\n"; // Formato de número a RDI
    }
    
    out << " movl $0, %eax\n"
        << " call printf@PLT\n";
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
    // Implementación es solo un placeholder
    out << " # WARNING: ForStmt no implementado\n";
    stm->block->accept(this);
    return 0;
}


int GenCodeVisitor::visit(ReturnStm* stm) {
    if (stm->e) stm->e->accept(this); // Deja el valor de retorno en %rax
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
    out <<f->nombre<<":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    
    int size = f->Pnombres.size();
    for (int i = 0; i < size; i++) {
        memoria[f->Pnombres[i]]=offset;
        if (i < argRegs.size()) {
            // Parámetros en registros (hasta 6)
            out << " movq " << argRegs[i] << "," << offset << "(%rbp)" << endl;
        } else {
            // Parámetros en la pila (offset a RBP+16, RBP+24, ...)
            int arg_stack_pos = 16 + (i - (int)argRegs.size()) * 8; 
            out << " movq " << arg_stack_pos << "(%rbp), %rax\n";
            out << " movq %rax, " << offset << "(%rbp)" << endl;
        }
        offset -= 8;
    }
    
    // Reserva de espacio. 
    // Nota: Esta reserva es estática y simple. En un compilador real se calcularía el tamaño exacto.
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
    
    // Argumentos > 6: pasar por pila en orden inverso
    int num_stack_args = max(0, size - (int)argRegs.size());
    for (int i = size - 1; i >= (int)argRegs.size(); i--) {
        exp->argumentos[i]->accept(this);
        out << " pushq %rax\n";
    }

    // Argumentos <= 6: pasar por registro
    for (int i = 0; i < min(size, (int)argRegs.size()); i++) {
        exp->argumentos[i]->accept(this);
        out << " mov %rax, " << argRegs[i] <<endl;
    }

    // Llamada a función
    out << " movl $0, %eax\n"; // Número de registros vectoriales (0)
    out << "call " << exp->nombre << endl;

    // Limpiar argumentos de la pila (si hubo)
    if (num_stack_args > 0) {
        out << " addq $" << num_stack_args * 8 << ", %rsp\n";
    }
    
    // Resultado en %rax
    return 0;
}