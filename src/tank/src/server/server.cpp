#include <cstdlib>
#include <iostream>
#include "core/ConfigVar.h"

int main (int argc, char *argv[]) {
	ConfigVar v("test", "0");
	std::cout << "YEAH" << std::endl;
	return EXIT_SUCCESS;
}
