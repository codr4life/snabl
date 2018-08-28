#include "snabl/fmt2.hpp"

namespace snabl {
	string fmt2_arg(const char* x) { return string(x); }
	string fmt2_arg(const string &x) { return x; }

	static void _fmt2(string &spec, size_t i, const fmt2_conv &arg1) {
		const string id {'%', static_cast<char>('0'+i)};
		bool found(false);
		size_t j(0);

		while ((j = spec.find(id, j)) != string::npos) {
			if (j > 0 && spec[j-1] == '%') {
				spec.erase(j, 1);
				j += id.size();
			} else {
				spec.replace(j, id.size(), arg1.as_str);
				j += arg1.as_str.size();
				found = true;
			}
		}

		if (!found) { throw Error("Unused fmt2 arg"); }
	}

	string fmt2(string_view spec, initializer_list<fmt2_conv> args) {
		string out(spec);
		int i(0);

		for (const auto &a : args){
			_fmt2(out, i, a);
			i++;
		}
	
		return out;
	}
}
