#include "snabl/env.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

int main() {
	snabl::Env env;
	snabl::Type<int> int_type(env.get_sym("Int"));
	snabl::AFuncPtr add(std::make_shared<snabl::Func<2>>(env.get_sym("+")));
	env.push(int_type, 42);
	return 0;
}
