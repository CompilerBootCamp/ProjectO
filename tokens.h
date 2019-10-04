//
// Created by user on 03.10.2019.
//

#ifndef COMPILER_TOKENS_H
#define COMPILER_TOKENS_H
#include <string>

enum Keywords { Class, Extends, Is, End, Var, Method, This, While, Loop, If, Then, Else, Return};

enum Delimiters {LeftRoundBracket, RightRoundBracket, LeftRectBracket, RightRectBracket, Dot, Comma, NotDelimiter};

struct Span {
    int lineIndex;
    int symbolIndex;
};

class Tokens {
public:
    std::string image;
    Span span;
    Tokens(const std::string &image, const Span &span) : image(image), span(span) {}
};

class Keyword:public Tokens {
    int id;
public:
    Keyword(const std::string &image, const Span &span, int id) : Tokens(image, span), id(id) {}
};

class Literal:public Tokens {
public:
    Literal(const std::string &image, const Span &span) : Tokens(image, span) {}
};

class IntegerLiteral:public Literal {
    int value;
public:
    IntegerLiteral(const std::string &image, const Span &span) : Literal(image, span) {
        value = std::stoi(image);
    }
};

class RealLiteral:public Literal {
    double value;
public:
    RealLiteral(const std::string &image, const Span &span) : Literal(image, span) {
        value = std::stod(image);
    }
};

class BoolLiteral:public Literal {
    bool value;
public:
    BoolLiteral(const std::string &image, const Span &span) : Literal(image, span) {
        value = image == "true" ? true : false;
    }
};


class Delimiter:public Tokens {
    int delimiterCode;
public:
    Delimiter(const std::string &image, const Span &span, int delimiterCode) : Tokens(image, span), delimiterCode(delimiterCode) {}
};

class Identificator:public Tokens {
public:
    Identificator(const std::string &image, const Span &span) : Tokens(image, span) {}
};

class Operator:public Tokens {
public:
    Operator(const std::string &image, const Span &span) : Tokens(image, span) {}
};
#endif //COMPILER_TOKENS_H

