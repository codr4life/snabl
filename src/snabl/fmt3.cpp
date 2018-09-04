#include "snabl/error.hpp"
#include "snabl/fmt3.hpp"

namespace snabl {
	string fmt3_arg(const char* x) { return string(x); }
	string fmt3_arg(const string &x) { return x; }

	string fmt3(string_view spec, const vector<fmt3_conv> &args) {
		stringstream out;
		size_t i(0), j;
		
		while ((j = spec.find('%', i)) != string_view::npos) {
			if (j < spec.length()-1 && spec[j+1] == '%') {
				j++;
				out << spec.substr(i, j-i);
				i = j+1;
			} else {
				out << spec.substr(i, j-i);
				size_t len(0);
				const auto arg(stoul(string(spec.substr(j+1)), &len));

				if (arg >= args.size()) {
					throw Error("Invalid fmt3 arg: " + to_string(arg));
				}

				out << args[arg].as_str;
				i = j+len+1;
			}
		}

		if (i < spec.length()) { out << spec.substr(i); }
		return out.str();
	}
}
