//
// Created by user on 03.10.2019.
//

#ifndef COMPILER_TOKENS_H
#define COMPILER_TOKENS_H
#include <string>
#include <iostream>


enum TokenTypes { Class, Extends, Is, End, Var, Method, This, While, Loop, If,
        Then, Else, Return, LeftRoundBracket, RightRoundBracket, LeftRectBracket,
        RightRectBracket, Dot, Comma, Colon, NotDelimiter, Assignment, IntegerLiteral, RealLiteral, BooleanLiteral, Identifier };

struct Span {
    int lineIndex;
    int symbolIndex;
};

std::string convertTokenToString(int type);

class Tokens {
    std::string image;
    Span span;
    int type;
public:
    Tokens(const std::string &image, const Span &span, int type) : image(image), span(span), type(type) {}

    int getType() {
        return type;
    }

    const std::string& getImage() const {
        return image;
    }

    std::string toString() const {
        return convertTokenToString(type);
    }
};

/*
value = std::stoi(image);
value = std::stod(image);
value = image == "true" ? true : false;
 */

#endif //COMPILER_TOKENS_H

