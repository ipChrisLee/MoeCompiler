lexer grammar CommonLex;

// keyword
Int : 'int';
Void: 'void';
Const: 'const';
Return : 'return';
If : 'if';
Else : 'else';
For : 'for';
While : 'while';
Do : 'do';
Break : 'break';
Continue : 'continue'; 

// operator
Lparen : '(' ;
Rparen : ')' ;
Lbrkt : '[' ;
Rbrkt : ']' ;
Lbrace : '{' ;
Rbrace : '}' ;
Comma : ',' ;
Semicolon : ';';
Question : '?';
Colon : ':';

Minus : '-';
Exclamation : '!';
Tilde : '~';
Addition : '+';
Multiplication : '*';
Division : '/';
Modulo : '%';
LAND : '&&';
LOR : '||';
EQ : '==';
NEQ : '!=';
LT : '<';
LE : '<=';
GT : '>';
GE : '>=';

// integer, identifier
IntLiteral
    : [0-9]+
    | '0'[xX][0-9a-fA-F]+
    | '0'[0-7]+
    ;

Hexadecimal_Fractional_Constant
    : [0-9A-Fa-f]*'.'[0-9A-Fa-f]+
    | [0-9A-Fa-f]+'.'
    ;

Hexadecimal_Digit_Sequence : [0-9A-Fa-f]+ ;

Binary_Exponent_Part : [pP][+-]?[0-9]+ ;

Hexadecimal_Floating_Constant
    : '0'[xX] Hexadecimal_Fractional_Constant Binary_Exponent_Part
    | '0'[xX] Hexadecimal_Digit_Sequence Binary_Exponent_Part
    ;

Exponent_Part : [eE][+-]?[0-9]+ ;

Fractional_Constant
    : [0-9]*'.'[0-9]+
    | [0-9]+'.'
    ;

Decinal_Floating_Constant
    : Fractional_Constant (Exponent_Part)?
    | [0-9]+ Exponent_Part
    ;

FloatLiteral
    : Decinal_Floating_Constant
    | Hexadecimal_Floating_Constant
    ;

Identifier
    : [a-zA-Z_][a-zA-Z_0-9]*
    ;

STRING : '"'(ESC|.)*?'"';

fragment
ESC : '\\"'|'\\\\';

WS : 
    [ \t\r\n] -> skip
    ;

LINE_COMMENT : '//' .*? '\r'? '\n' -> skip;
COMMENT      :'/*'.*?'*/'-> skip ;
