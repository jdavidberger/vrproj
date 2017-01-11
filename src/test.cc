#include <iostream>

#include "shared/Matrices.h"

int main(int argc, char *argv[]) {
	{
		Matrix5 f;
		f.get(0, 0) = 2;
		f.get(0, 1) = 2;

		auto val = f * Vector5(1, 2, 3, 4, 5);
		std::cerr << val << std::endl;
	}
	{
		Matrix5 f;
		f.get(0, 0) = 2;
		f.get(1, 0) = 2;

		auto val = f * Vector5(1, 2, 3, 4, 5);
		std::cerr << val << std::endl;
	}
	return 0;
}
