#include "snabl/env.hpp"
#include "tests.hpp"

using namespace snabl;

int main(int argc, const char *argv[]) {
	Env env;
	
	if (argc == 1) {
		snabl::all_tests();
		//todo: start repl
	} else {
		ifstream f(argv[1]);
		env.run(f);
	}
	
	return 0;
}
