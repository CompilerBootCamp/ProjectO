#include <iostream>
#include <string>
#include <unordered_map>
#include "lexAnalizer.h"


static const std::unordered_map<std::string, int> keywords = {
        {"class", Class},
        {"extends", Extends},
        {"is", Is},
        {"end", End},
        {"var", Var},
        {"method", Method},
        {"this", This},
        {"while", While},
        {"loop", Loop},
        {"if", If},
        {"then", Then},
        {"else", Else},
        {"return", Return}
};

enum FsmStates {
    StartSymbol,
    FindColon,
    NumericLiteral,
    StringLexeme
};

static int getDelimiterCode(char c) {
    switch (c) {
        case '(':
            return LeftRoundBracket;
        case ')':
            return RightRoundBracket;
        case '[':
            return LeftRectBracket;
        case ']':
            return RightRectBracket;
        case ',':
            return Comma;
        case '.':
            return Dot;
        default:
            return  NotDelimiter;
    }
}

std::vector<Tokens*> makeLexAnalysis(const char* str, const char* endStr) {
    std::vector<Tokens*> all_tokens;
    const char* lexemeStart;
    const char* stringStart = str;
    int delimiterCode;
    int fsmState = StartSymbol;
    int lineIndex = 0;
   
    while (str != endStr) {
        switch(fsmState) {
            case StartSymbol:
                delimiterCode = getDelimiterCode(*str);
                if (delimiterCode != NotDelimiter){
                    all_tokens.push_back(new Tokens(std::string(1, *str), {lineIndex, str-stringStart}, delimiterCode));
		    str++;
		}
                else if (isspace(*str)){
		   if (*str == '\n'){
			lineIndex++;
			stringStart = str+1;
		   }
                   str++;
		}
                else if (*str == ':') {
                    str++;
                    fsmState = FindColon;
                }
                else if (isalpha(*str)) {
                    lexemeStart = str;
                    str++;
                    fsmState = StringLexeme;
                }
                else if (isdigit(*str)) {
                    lexemeStart = str;
                    str++;
                    fsmState = NumericLiteral;
                }
                break;
                
            case FindColon:
                if (*str == '=') {
                	all_tokens.push_back(new Tokens(":=", {lineIndex, str-stringStart-1}, Assignment));
                	str++;
                }
                else
                    all_tokens.push_back(new Tokens(":", {lineIndex, str-stringStart-1}, Colon));
                fsmState = StartSymbol;
                break;

            case NumericLiteral:
                if (isdigit(*str) || (*str == '.'))
                    str++;
                else {
                    std::string lexeme = std::string(lexemeStart, str);
                    if (lexeme.find('.') != std::string::npos)
                        all_tokens.push_back(new Tokens(lexeme, {lineIndex, lexemeStart-stringStart}, Literal));
                    else
                        all_tokens.push_back(new Tokens(lexeme, {lineIndex, lexemeStart-stringStart}, Literal));
                    fsmState = StartSymbol;
                }
                break;
            
            case StringLexeme:
            	if(isalnum(*str))
    			str++;
            	else {
            	    std::string lexeme = std::string(lexemeStart, str);
            	    auto keyIt = keywords.find(lexeme);
            	    if (keyIt != keywords.end()) // найдено ключевое слово
                        all_tokens.push_back(new Tokens(lexeme, {lineIndex, lexemeStart-stringStart}, (*keyIt).second));
            	    else if (lexeme == "true" || lexeme == "false") // найден литерал Boolean
                        all_tokens.push_back(new Tokens(lexeme, {lineIndex, lexemeStart-stringStart}, Literal));
            	    else
                        all_tokens.push_back(new Tokens(lexeme, {lineIndex, lexemeStart-stringStart}, Identifier));
                    fsmState = StartSymbol;
            	}
                break;
        }
    }
    return all_tokens;
}

void deleteVector(std::vector<Tokens*> & v) {
    for (auto it = v.begin(); it != v.end(); ++it)
        delete *it;
}
