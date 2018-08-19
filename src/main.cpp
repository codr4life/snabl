#include <cassert>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

using namespace snabl;

int main() {
	Env env;
	
	env.lobby.add_fimp<2, 1>(env.get_sym("+"),
		{env.int_type, env.int_type},
		{env.int_type},
		[](Call &call) {
			Env &env(call.scope->env);
			
			Int
				y(call.scope->pop_stack().as<Int>()),
				x(call.scope->pop_stack().as<Int>());
			
			env.push_stack(env.int_type, x+y);
		});

	Forms forms;
	env.parse("1 + 2", forms);
	env.bin.compile(forms);
	env.bin.run();
	
	//"($n < 2) if-else 1 {(fib, $n --) + (fib, $n - 2)}";
	//env.bin.compile(add_int);
	
	auto s(env.begin(env.scope()));
	env.push_stack(env.int_type, Int(42));
	assert(s->pop_stack().as<Int>() == Int(42));
	auto foo(env.get_sym("foo"));
	env.main->set_var(foo, env.int_type, Int(42));
	assert(s->get_var(foo)->as<Int>() == Int(42));
	env.end();

	env.bin.emit_push(Box(env.int_type, Int(42)));
	return 0;
}
