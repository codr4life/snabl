#include "snabl/fmt2.hpp"

namespace snabl {
	std::string fmt2_arg(const char* x) { return std::string(x); }
	std::string fmt2_arg(const std::string &x) { return x; }

	static void _fmt2(std::string &spec, std::size_t i, const fmt2_conv &arg1) {
		const std::string id {'%', static_cast<char>('0'+i)};
		bool found(false);
		std::size_t j(0);

		while ((j = spec.find(id, j)) != std::string::npos) {
			if (j > 0 && spec[j-1] == '%') {
				spec.erase(j, 1);
				j += id.size();
			} else {
				spec.replace(j, id.size(), arg1.as_str);
				j += arg1.as_str.size();
				found = true;
			}
		}

		if (!found) { throw "Unused fmt2 arg"; }
	}

	std::string fmt2(std::string_view spec, std::initializer_list<fmt2_conv> args) {
		std::string out(spec);
		int i(0);

		for (const auto &a : args){
			_fmt2(out, i, a);
			i++;
		}
	
		return out;
	}
}
