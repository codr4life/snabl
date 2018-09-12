#include "snabl/fmt.hpp"
#include "snabl/std.hpp"

namespace snabl {
	void fmt_tests() {
		assert(fmt("%0 %1 %2", {"foo", "bar", 42}) == "foo bar 42");
		assert(fmt("%0 %%1 %1", {"foo", "bar"}) == "foo %1 bar");
		assert(fmt("%0 %0", {""}) == " ");
		assert(fmt("%%0", {}) == "%0");
	}

	void all_tests() {
		fmt_tests();
	}
}
