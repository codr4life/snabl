#include "snabl/env.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

int main() {
	snabl::Env env;
	auto add(env.lobby.add_func(env.get_sym("+")));
	env.push(env.int_type, 42L);
	return 0;
}
