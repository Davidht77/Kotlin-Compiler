# Kotlin-Compiler

## Gramatica inicial de Kotlin
Program         ::= VarDecList FunDecList

VarDecList      ::= (VarDec)*

FunDecList      ::= (FunDec)+

FunDec          ::= "fun" id "(" ParamDecList? ")" (":" Type)? Block

ParamDecList    ::= ParamDec ("," ParamDec)*
ParamDec        ::= ("val"|"var") id ":" Type

VarDec          ::= ("val"|"var") id (":" Type)? ( "=" Exp )? ";"

Block           ::= "{" StmtList? "}"

StmtList        ::= (Stmt)*

Stmt            ::= VarDec
                 | Exp ";"
                 | PrintStmt
                 | IfStmt
                 | WhileStmt
                 | ForStmt
                 | ReturnStmt

PrintStmt       ::= ("println"|"print") "(" Exp? ")" ";"

IfStmt          ::= "if" "(" Exp ")" Block ("else" Block)?

WhileStmt       ::= "while" "(" Exp ")" Block

ForStmt         ::= "for" "(" id "in" Exp ")" Block

ReturnStmt      ::= "return" (Exp)? ";"

Exp             ::= Assignment

Assignment      ::= id "=" Assignment   // asignación derecha a izquierda
                 | LogicOr

LogicOr         ::= LogicAnd ("||" LogicAnd)*

LogicAnd        ::= Equality ("&&" Equality)*

Equality        ::= Relational (("=="|"!=") Relational)*

Relational      ::= Additive (("<"|">"|"<="|">=") Additive)*

Additive        ::= Multiplicative (("+"|"-") Multiplicative)*

Multiplicative  ::= Unary (("*"|"/"|"%") Unary)*

Unary           ::= ("+"|"-"|"!") Unary
                 | Primary

Primary         ::= id
                 | Num
                 | Bool
                 | "(" Exp ")"
                 | FunctionCall

FunctionCall    ::= id "(" ArgList? ")"
ArgList         ::= Exp ("," Exp)*

Type            ::= id   // puedes ampliar para incluir tipos básicos: Int, Boolean, etc.

Num             ::= [0-9]+
Bool            ::= "true" | "false"
id              ::= [a-zA-Z_][a-zA-Z0-9_]*
