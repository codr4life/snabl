#include <cassert>

#include "snabl/fmt.hpp"
#include "snabl/ptr.hpp"

namespace snabl {
	void fmt_tests() {
		assert(fmt("%0 %1 %2", {"foo", "bar", 42}) == "foo bar 42");
		assert(fmt("%0 %%1 %1", {"foo", "bar"}) == "foo %1 bar");
		assert(fmt("%0 %0", {""}) == " ");
		assert(fmt("%%0", {}) == "%0");
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
		ptr_tests();
	}
}
