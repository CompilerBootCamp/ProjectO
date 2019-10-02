#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;
// база ключевых слов
std::unordered_map<std::string, int> all_keywords = {
	{"class", 1},
	{"Program", 2}
};

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

void read_tokens() {
	char ch;
	std::string s;
	while (std::cin>>s) {
		cout << "new tokens: " <<s << std::endl;
		std::string buf;
		for(const auto& ch: s) {
			buf += ch;
			if ( /* buf можем определить к какому-либо классу*/ ) {
				all.tokens.push_back(buf);
				buf.clear();
			}
		}
	}
}

int main() {
	read_tokens();
}
