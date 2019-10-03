//
// Created by user on 03.10.2019.
//

#ifndef COMPILER_TOKENS_H
#define COMPILER_TOKENS_H
#include <string>

enum Keywords { Class, Extends, Is, End, Var, Method, This, While, Loop, If, Then, Else, Return};

enum Delimiters {LeftRoundBracket, RightRoundBracket, LeftRectBracket, RightRectBracket, Dot, Comma, NotDelimiter};


class Tokens {
public:
    std::string image;
    Tokens(const std::string &image) : image(image) {}
};

class Keyword:public Tokens {
    int id;
public:
    Keyword(const std::string &image, int id) : Tokens(image), id(id) {}
};

class Literal:public Tokens {
public:
    Literal(const std::string &image) : Tokens(image) {}
};

class IntegerLiteral:public Literal {
    int value;
public:
    IntegerLiteral(const std::string &image) : Literal(image) {
        value = std::stoi(image);
    }
};

class RealLiteral:public Literal {
    double value;
public:
    RealLiteral(const std::string &image) : Literal(image) {
        value = std::stod(image);
    }
};

class BoolLiteral:public Literal {
    bool value;
public:
    BoolLiteral(const std::string &image) : Literal(image) {
        value = image == "true" ? true : false;
    }
};


class Delimiter:public Tokens {
    int delimiterCode;
public:
    Delimiter(const std::string &image, int delimiterCode) : Tokens(image), delimiterCode(delimiterCode) {}
};

class Identificator:public Tokens {
public:
    Identificator(const std::string &image) : Tokens(image) {}
};

class Operator:public Tokens {
public:
    Operator(const std::string &image) : Tokens(image) {}
};
#endif //COMPILER_TOKENS_H
