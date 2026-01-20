#include <iostream>

#include "add.h"

int main() {
	std::cout << "Hello, World!" << std::endl;
	int a = 1, b = 2;
	std::cout << "( " << a << " + " << b << " ) = " << add(a, b) << std::endl;
	system("pause");
	return 0;
}


