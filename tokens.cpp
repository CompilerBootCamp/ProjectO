//
// Created by zvukm on 10.10.2019.
//
#include "tokens.h"

std::string convertTokenToString(int type) {
    switch (type) {
        case Class:
            return "class";
        case Extends:
            return "extends";
        case Is:
            return "is";
        case End:
            return "end";
        case Var:
            return "var";
        case Method:
            return "method";
        case This:
            return "this";
        case While:
            return "while";
        case Loop:
            return "loop";
        case If:
            return "if";
        case Then:
            return "then";
        case Else:
            return "else";
        case Return:
            return "return";
        case LeftRoundBracket:
            return "(";
        case RightRoundBracket:
            return ")";
        case LeftRectBracket:
            return "[";
        case RightRectBracket:
            return "]";
        case Dot:
            return ".";
        case Comma:
            return ",";
        case Colon:
            return ":";
        case Assignment:
            return ":=";
        case Literal:
            return "literal";
        case Identifier:
            return "identifier";
        default:
            return "";
    }
}