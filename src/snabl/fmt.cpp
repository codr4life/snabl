#include <sstream>

#include "snabl/error.hpp"
#include "snabl/fmt.hpp"

namespace snabl {
	std::string fmt_arg(const char* x) { return std::string(x); }
	std::string fmt_arg(const std::string &x) { return x; }

	std::string fmt(stdx::string_view spec, const std::vector<fmt_conv> &args) {
		std::stringstream out;
		std::size_t i(0), j;
		
		while ((j = spec.find('%', i)) != stdx::string_view::npos) {
			if (j < spec.length()-1 && spec[j+1] == '%') {
				j++;
				out << spec.substr(i, j-i);
				i = j+1;
			} else {
				out << spec.substr(i, j-i);
				std::size_t len(0);
				const auto arg(std::stoul(std::string(spec.substr(j+1)), &len));

				if (arg >= args.size()) {
					throw Error("Invalid fmt arg: " + std::to_string(arg));
				}

				out << args[arg].as_str;
				i = j+len+1;
			}
		}

		if (i < spec.length()) { out << spec.substr(i); }
		return out.str();
	}
}
