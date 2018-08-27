#ifndef SNABL_FMT2_HPP
#define SNABL_FMT2_HPP

#include <string>

#include "snabl/error.hpp"
#include "snabl/std/string_view.hpp"

namespace snabl {	
	std::string fmt2_arg(const char* x);
	std::string fmt2_arg(const std::string &x);

	template <typename ArgT>
	std::string fmt2_arg(const ArgT &x) { return std::to_string(x); }

	struct fmt2_conv {
    template<class T>
    fmt2_conv(T&& val);

		const std::string as_str;
	};

	template<class T>
	fmt2_conv::fmt2_conv(T&& val): as_str(fmt2_arg(val)) { }
	
	std::string fmt2(std::string_view spec, std::initializer_list<fmt2_conv> args);
}

#endif
