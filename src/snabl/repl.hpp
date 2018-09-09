#ifndef SNABL_REPL_HPP
#define SNABL_REPL_HPP

#include "snabl/std.hpp"

namespace snabl {	
	const vector version {0, 1};

	class Env;

	void repl(Env &env, istream &in, ostream &out);
}

#endif
