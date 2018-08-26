#ifndef SNABL_FMT_HPP
#define SNABL_FMT_HPP

#include <string>
#include <utility>

#include "snabl/error.hpp"

namespace snabl {
	std::string fmt_arg(const char *x);	
	std::string fmt_arg(const std::string &x);	

	template <typename ArgT>
	std::string fmt_arg(const ArgT &x) { return std::to_string(x); }

	void _fmt(std::string &spec, int i);

	template <typename Arg1, typename...ArgsT>
	void _fmt(std::string &spec, int i, const Arg1 &arg1, ArgsT &&... args) {
		const std::string id {'%', static_cast<char>('0'+i)};
		const std::string arg(fmt_arg(arg1));
		bool found = false;
		size_t j(0);

		while ((j = spec.find(id, j)) != std::string::npos) {
			spec.replace(j, id.size(), arg);
			j += arg.size()-id.size();
			found = true;
		}

		if (!found) { throw Error("Unused fmt arg"); }
		_fmt(spec, i+1, std::forward<ArgsT>(args)...);
	}
	
	template <typename...ArgsT>
	std::string &&fmt(std::string spec, ArgsT &&... args) {
		_fmt(spec, 0, std::forward<ArgsT>(args)...);
		return std::move(spec);
	}
}

#endif
