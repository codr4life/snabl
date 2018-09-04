#ifndef SNABL_FMT_HPP
#define SNABL_FMT_HPP

#include "snabl/std.hpp"

namespace snabl {	
	template <typename T>
	void fmt_arg(const T &x, ostream &out) { out << x; }

	struct fmt_conv {
		template <typename T>
		fmt_conv(T&& val): print([val](ostream &out) { fmt_arg(val, out); }) { }
		const function<void (ostream &)> print;
	};
	
	string fmt(string_view spec, const vector<fmt_conv> &args);
}

#endif
