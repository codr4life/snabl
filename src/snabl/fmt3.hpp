#ifndef SNABL_FMT_HPP
#define SNABL_FMT_HPP

#include "snabl/std.hpp"

namespace snabl {	
	string fmt_arg(const char* x);
	string fmt_arg(const string &x);

	template <typename T>
	string fmt_arg(const T &x) { return to_string(x); }

	struct fmt_conv {
		template <typename T>
		fmt_conv(T&& val);
		
		const string as_str;
	};

	template <typename T>
	fmt_conv::fmt_conv(T&& val): as_str(fmt_arg(val)) { }
	
	string fmt(string_view spec, const vector<fmt_conv> &args);
}

#endif
