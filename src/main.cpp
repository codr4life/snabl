#include "snabl/env.hpp"
#include "snabl/type.hpp"

int main() {
	snabl::Env env;
	snabl::Type<int> int_type(env.get_sym("Int"));
	env.push(int_type, 42);
	return 0;
}
