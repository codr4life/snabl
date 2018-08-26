#include <cassert>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fmt.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

namespace snabl {
	void fmt_tests() {
		assert(fmt("%0 %1 %2", "foo", "bar", 42) == "foo bar 42");
		assert(fmt("%0 %0", "") == " ");
	}

	void lang_tests() {
		using namespace snabl;

		Env env;
		auto s(env.scope());

		env.run("42");
		assert(s->pop_stack().as<Int>() == Int(42));

		env.run("t");
		assert(s->pop_stack().as<bool>());

		env.run("f");
		assert(!s->pop_stack().as<bool>());

		env.run("1 = 1");
		assert(s->pop_stack().as<bool>());

		env.run("1 == 1");
		assert(s->pop_stack().as<bool>());

		env.run("1 < 3");
		assert(s->pop_stack().as<bool>());
	
		env.run("-3.14");
		assert(Int(s->pop_stack().as<Float>()*100) == Int(-314));

		env.run("3.14 int");
		assert(s->pop_stack().as<Int>() == Int(3));

		env.run("42 float");
		assert(s->pop_stack().as<Float>() == Float(42));

		env.run("1 + 3");
		assert(s->pop_stack().as<Int>() == Int(4));

		env.run("1 +, 3 * 2");
		assert(s->pop_stack().as<Int>() == Int(7));

		env.run("1 + (3 * 2)");
		assert(s->pop_stack().as<Int>() == Int(7));

		env.run("1 3 5 drop +");
		assert(s->pop_stack().as<Int>() == Int(4));

		env.run("(let: foo 42) @foo");
		assert(s->pop_stack().as<Int>() == Int(42));

		env.run("1 +<_ Int> 3");
		assert(s->pop_stack().as<Int>() == Int(4));

		env.run("if: (1 < 3) 5 7");
		assert(s->pop_stack().as<Int>() == Int(5));

		env.run("if: (3 < 1) 5 7");
		assert(s->pop_stack().as<Int>() == Int(7));

		//"func: fib<Int> Int (let: n) if: ($n < 2) 1, (fib, $n --) + (fib, $n - 2)";
	}

	void all_tests() {
		fmt_tests();
		lang_tests();
	}
}
