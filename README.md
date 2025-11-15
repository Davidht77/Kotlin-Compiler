Program         ::= VarDecList FunDecList

VarDecList      ::= (VarDec)*

FunDecList      ::= (FunDec)+

FunDec          ::= "fun" id "(" ParamListOpt ")" TypeAnnotationOpt Block

ParamListOpt    ::= (ParamDec ("," ParamDec)*) | ε

ParamDec        ::= VarSymbol id TypeAnnotationOpt

VarDec          ::= VarSymbol id TypeAnnotationOpt InitializerOpt StmtTerminator

VarSymbol       ::= ("const" | ε) ("val" | "var")

TypeAnnotationOpt ::= ":" Type | ε

Type            ::= id

InitializerOpt  ::= "=" Exp | ε

Block           ::= "{" StmtListOpt "}"

StmtList        ::= (Stmt StmtTerminator)*

StmtListOpt     ::= StmtList | ε

Stmt            ::= VarDec 
                 | Exp 
                 | PrintStmt 
                 | IfStmt 
                 | WhileStmt 
                 | ForStmt 
                 | ReturnStmt

PrintStmt       ::= ("println"|"print") "(" ExpOpt ")"

IfStmt          ::= "if" "(" Exp ")" Block ElseOpt

ElseOpt         ::= "else" Block | ε

WhileStmt       ::= "while" "(" Exp ")" Block

ForStmt         ::= "for" "(" id "in" Exp ")" Block

ReturnStmt      ::= "return" ExpOpt

ExpOpt          ::= Exp | ε

Exp             ::= Assignment

Assignment      ::= id "=" Assignment | LogicOr

LogicOr         ::= LogicAnd ("||" LogicAnd)*

LogicAnd        ::= Equality ("&&" Equality)*

Equality        ::= Relational (("=="|"!=") Relational)*

Relational      ::= Additive (("<"|">"|"<="|">=") Additive)*

Additive        ::= Multiplicative (("+"|"-") Multiplicative)*

Multiplicative  ::= Unary (("*"|"/"|"%") Unary)*

Unary           ::= ("+"|"-"|"!") Unary | Primary

Primary         ::= id
                 | Num
                 | Bool
                 | "(" Exp ")"
                 | FunctionCall

FunctionCall    ::= id "(" ArgListOpt ")"

ArgListOpt      ::= (Exp ("," Exp)*) | ε

StmtTerminator  ::= ";" | Newline
