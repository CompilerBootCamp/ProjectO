#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
using namespace std;
// база ключевых слов
std::set <std::string> all_keywords = {"class", "Program", "var", "Array", "is", "true", "while", "end"};
std::set <char> all_delimiters = {':', '{', '}', '(', ')', '='};
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

void read_tokens(ifstream& stream) {
	char ch;
	std::string s;
	while (stream >> s) {
		//cout << "new tokens: " << s << endl;
		std::string buf;
		for(auto ch: s) {
			if (all_delimiters.find(ch)!=all_delimiters.end()) {
				cout << "\tfind token: " << buf << endl;
				cout << "\tfind token: " << ch << endl;
				buf.clear();

			}
			else {
				buf += ch;
				if ( all_keywords.find(buf)!=all_keywords.end()) {
					cout << "\tfind token: " << buf << endl;
					buf.clear();
				}
			}
			
		}
	}
}

int main() {
	cout << "start" <<endl;
	ifstream stream;
	stream.open("test.txt");
	read_tokens(stream);
	stream.close();
}
