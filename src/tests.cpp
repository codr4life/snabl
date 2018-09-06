#include <cassert>

#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fmt.hpp"
#include "snabl/func.hpp"
#include "snabl/ptr.hpp"
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

		env.run("1 +, 3 * 2");
		assert(env.pop().as<Int>() == Int(7));

		env.run("1 + (3 * 2)");
		assert(env.pop().as<Int>() == Int(7));

		env.run("(1 +, 5 - 2)");
		assert(env.pop().as<Int>() == Int(4));

		env.run("1 3 5 drop +");
		assert(env.pop().as<Int>() == Int(4));

		env.run("21 dup +");
		assert(env.pop().as<Int>() == Int(42));

		env.run("1 3 swap -");
		assert(env.pop().as<Int>() == Int(2));

		env.run("1 2 3 rot -; -");
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
		assert(env.pop().as<Int>() > Int(15));
	}

	void ptr_ref_tests() {
		auto foo(Ptr<int>::make(42));
		assert(foo.nrefs() == 0);
		assert(*foo == 42);

		foo = foo;
		assert(foo.nrefs() == 0);

		{
			Ptr<int> bar(foo);
			assert(foo.nrefs() == 1);
			assert(bar.nrefs() == 1);
			*bar = 7;
		}
		
		assert(foo.nrefs() == 0);
		assert(*foo == 7);

		{
			auto bar(Ptr<int>::make(42));
			auto baz(bar);

			baz = foo;
			assert(foo.nrefs() == 1);
			assert(bar.nrefs() == 0);
		}
	}

	struct Foo {
		int v;
		Foo(int v): v(v) { }
	};
		
	void ptr_const_tests() {
		const auto p(Ptr<Foo>::make(41));
		p->v++;
		assert(p->v == 42);
	}
	
	void ptr_tests() {
		ptr_ref_tests();
		ptr_const_tests();
	}
	
	void all_tests() {
		fmt_tests();
		lang_tests();
		ptr_tests();
	}
}
