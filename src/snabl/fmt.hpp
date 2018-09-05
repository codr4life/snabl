#ifndef SNABL_FMT_HPP
#define SNABL_FMT_HPP

#include "snabl/std.hpp"

namespace snabl {	
	struct fmt_conv {
		template <typename T>
		fmt_conv(const T& v): print([&v](ostream &out) { out << v; }) { }
		const function<void (ostream &)> print;
	};
	
	string fmt(string_view spec, const vector<fmt_conv> &args);
}

#endif
