#include "snabl/fmt1.hpp"

namespace snabl {
	string fmt1_arg(const char *x) { return string(x); }
	
	string fmt1_arg(const string &x) { return x; }

	void _fmt1(string &spec, size_t i) { }
}
