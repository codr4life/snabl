#ifndef SNABL_FMT_HPP
#define SNABL_FMT_HPP

#include <string>
#include <utility>

#include "snabl/error.hpp"
#include "snabl/std/string_view.hpp"

namespace snabl {
	std::string fmt_arg(const char *x);	
	std::string fmt_arg(const std::string &x);	

	template <typename ArgT>
	std::string fmt_arg(const ArgT &x) { return std::to_string(x); }

	void _fmt(std::string &spec, std::size_t i);

	template <typename Arg1, typename...ArgsT>
	void _fmt(std::string &spec, std::size_t i, const Arg1 &arg1, ArgsT &&... args) {
		const std::string id {'%', static_cast<char>('0'+i)};
		const std::string arg(fmt_arg(arg1));
		bool found(false);
		std::size_t j(0);

		while ((j = spec.find(id, j)) != std::string::npos) {
			if (j > 0 && spec[j-1] == '%') {
				spec.erase(j, 1);
				j += id.size();
			} else {
				spec.replace(j, id.size(), arg);
				j += arg.size();
				found = true;
			}
		}

		if (!found) { throw Error("Unused fmt arg"); }
		_fmt(spec, i+1, std::forward<ArgsT>(args)...);
	}
	
	template <typename...ArgsT>
	std::string fmt(std::string_view spec, ArgsT &&... args) {
		std::string out(spec);
		_fmt(out, 0, std::forward<ArgsT>(args)...);
		return out;
	}
}

#endif
