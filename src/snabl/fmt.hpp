#ifndef SNABL_FMT_HPP
#define SNABL_FMT_HPP

#include <string>
#include <vector>

#include "snabl/error.hpp"
#include "snabl/stdx/string_view.hpp"

namespace snabl {	
	std::string fmt_arg(const char* x);
	std::string fmt_arg(const std::string &x);

	template <typename ArgT>
	std::string fmt_arg(const ArgT &x) { return std::to_string(x); }

	struct fmt_conv {
		template<class T>
		fmt_conv(T&& val);
		const std::string as_str;
	};

	template<class T>
	fmt_conv::fmt_conv(T&& val): as_str(fmt_arg(val)) { }
	
	std::string fmt(stdx::string_view spec, const std::vector<fmt_conv> &args);
}

#endif
