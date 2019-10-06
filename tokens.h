//
// Created by user on 03.10.2019.
//

#ifndef COMPILER_TOKENS_H
#define COMPILER_TOKENS_H
#include <string>
#include <iostream>

enum TokenTypes {KeywordToken, LiteralToken, DelimiterToken, IdentificatorToken, OperatorToken};

enum Keywords { Class, Extends, Is, End, Var, Method, This, While, Loop, If, Then, Else, Return};

enum Delimiters {LeftRoundBracket, RightRoundBracket, LeftRectBracket, RightRectBracket, Dot, Comma, NotDelimiter};

struct Span {
    int lineIndex;
    int symbolIndex;
};

class Tokens {
    std::string image;
    Span span;
    int type;
public:
    Tokens(const std::string &image, const Span &span, int type) : image(image), span(span), type(type) {}

    virtual void printInformation() {
        std::cout << "(" << span.lineIndex << " " << span.symbolIndex << ")\t"  << image << std::endl;
    }

    bool checkType(int expectedType) {
        return type == expectedType;
    }

    const std::string &getImage() const {
        return image;
    }
};

class Keyword:public Tokens {
    int id;
public:
    Keyword(const std::string &image, const Span &span, int id) : Tokens(image, span, KeywordToken), id(id) {}
    bool checkId(int expectedId) {
        return id == expectedId;
    }
};

class Literal:public Tokens {
public:
    Literal(const std::string &image, const Span &span) : Tokens(image, span, LiteralToken) {}
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
    Delimiter(const std::string &image, const Span &span, int delimiterCode) : Tokens(image, span, DelimiterToken), delimiterCode(delimiterCode) {}
    bool checkCode(int expectedCode) {
        return delimiterCode == expectedCode;
    }
};

class Identificator:public Tokens {
public:
    Identificator(const std::string &image, const Span &span) : Tokens(image, span, IdentificatorToken) {}
};

class Operator:public Tokens {
public:
    Operator(const std::string &image, const Span &span) : Tokens(image, span, OperatorToken) {}
};
#endif //COMPILER_TOKENS_H

