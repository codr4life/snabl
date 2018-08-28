#ifndef SNABL_FMT1_HPP
#define SNABL_FMT1_HPP

#include "snabl/error.hpp"
#include "snabl/std.hpp"

namespace snabl {
	string fmt1_arg(const char *x);	
	string fmt1_arg(const string &x);	

	template <typename ArgT>
	string fmt1_arg(const ArgT &x) { return to_string(x); }

	void _fmt1(string &spec, size_t i);

	template <typename Arg1, typename...ArgsT>
	void _fmt1(string &spec, size_t i, const Arg1 &arg1, ArgsT &&... args) {
		const string id {'%', static_cast<char>('0'+i)};
		const string arg(fmt1_arg(arg1));
		bool found(false);
		size_t j(0);

		while ((j = spec.find(id, j)) != string::npos) {
			if (j > 0 && spec[j-1] == '%') {
				spec.erase(j, 1);
				j += id.size();
			} else {
				spec.replace(j, id.size(), arg);
				j += arg.size();
				found = true;
			}
		}

		if (!found) { throw Error("Unused fmt1 arg"); }
		_fmt1(spec, i+1, forward<ArgsT>(args)...);
	}
	
	template <typename...ArgsT>
	string fmt1(string_view spec, ArgsT &&... args) {
		string out(spec);
		_fmt1(out, 0, forward<ArgsT>(args)...);
		return out;
	}
}

#endif
