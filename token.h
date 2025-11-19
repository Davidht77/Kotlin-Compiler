#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        PLUS,    // +
        MINUS,   // -
        MUL,     // *
        DIV,     // /
        POW,     // **
        LPAREN,  // (
        RPAREN,  // )
        LKEY,    // {
        RKEY,    // }
        SQRT,    // sqrt
        NUM,     // Número
        ERR,     // Error
        ID,      // ID
        LE,      // <=
        GE,      // >=
        EQ,      // ==
        NE,      // !=
        LT,      // <
        GT,      // >
        FUN,     // fun
        ENDFUN,  // endfun
        RETURN,  // return
        SEMICOL, // ;
        COLON,   // :
        ASSIGN,  // =
        PRINT,   // print
        IF,      // if
        WHILE,   // while
        DO,      // do
        THEN,    // then
        ENDIF,   // endif
        ENDWHILE, // endwhile
        ELSE,    // else
        END,      // Fin de entrada
        VAR,     // var
        COMA,    // ,
        TRUE,    // true
        FALSE    // false
    };

    // Atributos
    Type type;
    string text;

    // Constructores
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    // Sobrecarga de operadores de salida
    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H