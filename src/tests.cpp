#include <cassert>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fmt.hpp"
#include "snabl/func.hpp"
#include "snabl/type.hpp"

namespace snabl {
	void fmt_tests() {
		assert(fmt("%0 %1 %2", {"foo", "bar", 42}) == "foo bar 42");
		assert(fmt("%0 %%1 %1", {"foo", "bar"}) == "foo %1 bar");
		assert(fmt("%0 %0", {""}) == " ");
		assert(fmt("%%0", {}) == "%0");
	}

	void lang_tests() {
		using namespace snabl;

		Env env;

		env.run("42");
		assert(env.pop().as<Int>() == Int(42));

		env.run("t");
		assert(env.pop().as<bool>());

		env.run("f");
		assert(!env.pop().as<bool>());

		env.run("1 = 1");
		assert(env.pop().as<bool>());

		env.run("1 == 1");
		assert(env.pop().as<bool>());

		env.run("1 < 3");
		assert(env.pop().as<bool>());
	
		env.run("-3.14");
		assert(Int(env.pop().as<Float>()*100) == Int(-314));

		env.run("3.14 int");
		assert(env.pop().as<Int>() == Int(3));

		env.run("42 float");
		assert(env.pop().as<Float>() == Float(42));

		env.run("1 + 3");
		assert(env.pop().as<Int>() == Int(4));

		env.run("1 +, 3 * 2");
		assert(env.pop().as<Int>() == Int(7));

		env.run("1 + (3 * 2)");
		assert(env.pop().as<Int>() == Int(7));

		env.run("(1 +, 5 - 2)");
		assert(env.pop().as<Int>() == Int(4));

		env.run("1 3 5 drop +");
		assert(env.pop().as<Int>() == Int(4));

		env.run("(let: foo 42) @foo");
		assert(env.pop().as<Int>() == Int(42));

		env.run("{@foo} call");
		assert(env.pop().as<Int>() == Int(42));
		
		env.run("1 +<_ Int> 3");
		assert(env.pop().as<Int>() == Int(4));

		env.run("if: (1 < 3) 5 7");
		assert(env.pop().as<Int>() == Int(5));

		env.run("if: (3 < 1) 5 7");
		assert(env.pop().as<Int>() == Int(7));
		
		env.run("func: foo1<A> A (* 2) 21 foo1");
		assert(env.pop().as<Int>() == Int(42));
		
		env.run("func: fib<Int> Int,\n"
						"  let: n _\n"
						"  if: (@n < 2) @n, (fib, @n - 1) + (fib, @n - 2)");
		
		env.run("fib 10");
		assert(env.pop().as<Int>() == Int(55));

		env.run("ms, bench 3 {sleep, 10ms}");
		assert(env.pop().as<Int>() > Int(30));
	}

	void all_tests() {
		fmt_tests();
		lang_tests();
	}
}
