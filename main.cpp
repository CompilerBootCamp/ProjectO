#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <algorithm>
using namespace std;
// база ключевых слов
std::set <std::string> all_keywords = {"class", "Program", "var", "Array", "is", "true", "while", "end"};
std::set <char> all_delimiters = {':', '{', '}', '(', ')', '=', '[', ']',',','.'};
// classes Tokens

class Tokens {
	std::string name;
};

class Keyword:public Tokens {
	int id;
};
class Literal:public Tokens {
};
// int vs double ?????
class Lit_int:public Literal {
	int value;
};
class Lit_real:public Literal {
	double value;
};
class Lit_bool:public Literal {
	bool value;
};

class Variable:public Tokens{

};
class Whitespaces:public Tokens {
	
};
class Delimiter:public Tokens {
	// сразу определить перечень
	enum {
		LeftRectBracket,
		RightrRectBracket
	};
};
class Identificator:public Tokens {};

// класс лексическогог анализатора
std::vector<Tokens> all_tokens;

bool read_token(ifstream& stream) {
	static char symbol = ' ';
	string s;
	while(isspace(symbol)) symbol = stream.get();
	if (isalpha(symbol)) {
		s = {symbol};
		while(isalnum(symbol=stream.get())) s += (symbol);
	}
	else 
		if ( isdigit(symbol) ) {
			s = {symbol};
			while (isdigit(symbol=stream.get())) s += (symbol);
		}
		else {
			if (*all_delimiters.lower_bound(symbol)==symbol) {
				s = {symbol};
				symbol = stream.get();
			}
		}
	while(isspace(symbol)) symbol = stream.get();	
	cout << "\ttokens: " << s << endl;
	return symbol != EOF;
}

int main() {
	ifstream stream;
	stream.open("test.txt");
	while (read_token(stream)) ;
	stream.close();
}
