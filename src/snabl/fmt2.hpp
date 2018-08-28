#ifndef SNABL_FMT2_HPP
#define SNABL_FMT2_HPP

#include "snabl/error.hpp"
#include "snabl/std.hpp"
#include "snabl/std/string_view.hpp"

namespace snabl {	
	string fmt2_arg(const char* x);
	string fmt2_arg(const string &x);

	template <typename ArgT>
	string fmt2_arg(const ArgT &x) { return to_string(x); }

	struct fmt2_conv {
    template<class T>
    fmt2_conv(T&& val);

		const string as_str;
	};

	template<class T>
	fmt2_conv::fmt2_conv(T&& val): as_str(fmt2_arg(val)) { }
	
	string fmt2(string_view spec, initializer_list<fmt2_conv> args);
}

#endif
