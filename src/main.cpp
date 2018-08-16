#include <cassert>

#include "snabl/env.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

int main() {
	snabl::Env env;
	auto add(env.lobby.add_func(env.get_sym("+")));
	env.push(env.int_type, snabl::Int(42));
	auto s(env.begin());
	auto foo(env.get_sym("foo"));
	s->set(foo, env.int_type, snabl::Int(42));
	snabl::Box v(*s->get(foo));
	assert(v.value<snabl::Int>() == snabl::Int(42));
	env.end();
	return 0;
}
