#include "dshash.hpp"
#include <iostream>

void test_add() {
	struct number n1 = {0, 0, 21};
	struct number n2 = {0, 0, 21};

	//print_number(&n1);
	//std::cout << std::endl;
	//print_number(&n2);
	//std::cout << std::endl;
	
	add_to(&n1, &n2);
	
	//print_number(&n1);
	//std::cout << std::endl;
}

/*
int main(int argc, char* argv[]) {
	//test_add();
	test_add_larger();
	std::cout << std::endl;
	
	test_new00();
	std::cout << std::endl;
	test_new11();
	std::cout << std::endl;
	test_new1();
	std::cout << std::endl;
	test_new_first_overflow();
	std::cout << std::endl;
	
	test_new_overflow();
	std::cout << std::endl;
	
	test_first_modulo();
	std::cout << std::endl;
}
*/


int main() {
	test_add();
}
