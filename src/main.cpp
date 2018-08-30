#include "snabl/env.hpp"
#include "tests.hpp"

using namespace snabl;

int main(int argc, const char *argv[]) {
	Env env;
	
	if (argc == 1) {
		snabl::all_tests();
		//todo: start repl
	} else {
		const string fn(argv[1]);
		ifstream f(fn);
		if (f.fail()) { throw Error(fmt("File not found: %0", {fn})); } 
		env.run(f);
	}
	
	return 0;
}
