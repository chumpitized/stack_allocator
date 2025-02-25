#include <assert.h>
#include <cstring>
#include <iostream>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT sizeof(void *)
#endif

int main() {
	std::cout << "Default alignment is " << DEFAULT_ALIGNMENT << std::endl;
	return 1;
}