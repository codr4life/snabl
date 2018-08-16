#include <cassert>

#include "snabl/env.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

int main() {
	snabl::Env env;
	auto add(env.lobby.add_func(env.get_sym("+")));
	auto s(env.begin(env.scope()));
	env.push_stack(env.int_type, snabl::Int(42));
	assert(s->pop_stack().as<snabl::Int>() == snabl::Int(42));
	auto foo(env.get_sym("foo"));
	env.main->set_var(foo, env.int_type, snabl::Int(42));
	assert(s->get_var(foo)->as<snabl::Int>() == snabl::Int(42));
	env.end();
	return 0;
}
