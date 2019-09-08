#include <exception>
//Recursive descent parser demo
//syntax: n(lchildren,rchildren)
//example: n(n(0,0),n(n(0,0),0))

struct node {
	node* lchildren;
	node* rchildren;
};

node* parseImpl(char*& expression) {
	auto& currentChar = expression;
	if (currentChar == 0)
		return 0;

	if (*currentChar == 'n') {
		node* currentNode = new node;
		//eat 'n' and '('
		currentNode->lchildren = parseImpl(currentChar += 2);
		currentNode->rchildren = parseImpl(currentChar += 2);//eat ','
		expression++;//eat them!
		return currentNode;
	}
	else if (*currentChar == '0') {
		return 0;
	}
	throw std::exception();
}

node* parse(const char* expression) {
	auto length = strlen(expression);
	char* copy = new char[length+1];
	memcpy(copy, expression, length);
	copy[length] = 0;

	return parseImpl(copy);
}

int main() {
	auto look = parse("n(n(n(0,0),n(n(0,0),0)),n(0,n(0,0)))");
	std::cout << "";
}
