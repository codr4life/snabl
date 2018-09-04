#ifndef SNABL_FMT3_HPP
#define SNABL_FMT3_HPP

#include "snabl/std.hpp"

namespace snabl {	
	string fmt3_arg(const char* x);
	string fmt3_arg(const string &x);

	template <typename T>
	string fmt3_arg(const T &x) { return to_string(x); }

	struct fmt3_conv {
		template <typename T>
		fmt3_conv(T&& val);
		
		const string as_str;
	};

	template <typename T>
	fmt3_conv::fmt3_conv(T&& val): as_str(fmt3_arg(val)) { }
	
	string fmt3(string_view spec, const vector<fmt3_conv> &args);
}

#endif
