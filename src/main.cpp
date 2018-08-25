#include <cassert>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fmt.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

using namespace snabl;

int main() {
	Env env;

	std::cout << fmt("%0 %1 %2", "foo", std::string("bar"), 42) << std::endl;
	
	env.home.add_macro(env.get_sym("drop"),
										 [](Forms::const_iterator &in,
												const Forms::const_iterator &end,
												AFuncPtr &func, AFimpPtr &fimp,
												Bin &out) {
											 auto &form(*in++);
											 out.emplace_back(ops::Drop::type, form.pos);			
										 });

	env.home.add_macro(env.get_sym("let:"),
										 [](Forms::const_iterator &in,
												const Forms::const_iterator &end,
												AFuncPtr &func, AFimpPtr &fimp,
												Bin &out) {
											 auto &form(*in++);
											 auto &p(*in++);

											 if (&p.type != &forms::Id::type) {
												 throw SyntaxError(p.pos, "Invalid let: place");
											 }
												 
											 (*in).type.compile(in, end, func, fimp, out);
											 out.emplace_back(ops::PutVar::type,
																				form.pos,
																				p.as<forms::Id>().sym);
										 });

	
	env.home.add_fimp<2, 1>(env.get_sym("+"),
		{Box(env.int_type), Box(env.int_type)},
		{Box(env.int_type)},
		[](Call &call) {
			Env &env(call.scope->env);
			
			Int
				y(call.scope->pop_stack().as<Int>()),
				x(call.scope->pop_stack().as<Int>());
			
			env.push_stack(env.int_type, x+y);
		});

	env.home.add_fimp<2, 1>(env.get_sym("*"),
		{Box(env.int_type), Box(env.int_type)},
		{Box(env.int_type)},
		[](Call &call) {
			Env &env(call.scope->env);
			
			Int
				y(call.scope->pop_stack().as<Int>()),
				x(call.scope->pop_stack().as<Int>());
			
			env.push_stack(env.int_type, x*y);
		});

	env.home.add_fimp<2, 1>(env.get_sym("="),
		{Box(env.maybe_type), Box(env.maybe_type)},
		{Box(env.bool_type)},
		[](Call &call) {
			Env &env(call.scope->env);
			
			Box
				y(call.scope->pop_stack()),
				x(call.scope->pop_stack());
			
			env.push_stack(env.bool_type, x.is_eqval(y));
		});

	env.home.add_fimp<2, 1>(env.get_sym("=="),
		{Box(env.maybe_type), Box(env.maybe_type)},
		{Box(env.bool_type)},
		[](Call &call) {
			Env &env(call.scope->env);
			
			Box
				y(call.scope->pop_stack()),
				x(call.scope->pop_stack());
			
			env.push_stack(env.bool_type, x.is_equid(y));
		});
	
	auto s(env.begin(env.scope()));

	env.run("42");
	assert(s->pop_stack().as<Int>() == Int(42));

	env.run("-3.14");
	assert(Int(s->pop_stack().as<Float>()*100) == Int(-314));

	env.run("1 + 3");
	assert(s->pop_stack().as<Int>() == Int(4));

	env.run("1 +, 3 * 2");
	assert(s->pop_stack().as<Int>() == Int(7));

	env.run("1 + (3 * 2)");
	assert(s->pop_stack().as<Int>() == Int(7));

	env.run("1 = 1");
	assert(s->pop_stack().as<bool>());

	env.run("1 == 1");
	assert(s->pop_stack().as<bool>());

	env.run("1 3 5 drop +");
	assert(s->pop_stack().as<Int>() == Int(4));

	env.run("(let: foo 42) @foo");
	assert(s->pop_stack().as<Int>() == Int(42));

	env.run("1 +<_ Int> 3");
	assert(s->pop_stack().as<Int>() == Int(4));

	//"func: fib<Int> Int (let: n) if-else: ($n < 2) 1, (fib, $n --) + (fib, $n - 2)";
	
	auto foo(env.get_sym("foo"));
	env.main->put_var(foo, Box(env.int_type, Int(42)));
	assert(s->get_var(foo)->as<Int>() == Int(42));
	env.end();

	return 0;
}
