#include "snabl/error.hpp"
#include "snabl/fmt.hpp"

namespace snabl {
	string fmt(string_view spec, const vector<fmt_conv> &args) {
		stringstream out;
		string::size_type i(0), j;
		
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
					throw Error("Invalid fmt arg: " + to_string(arg));
				}

				args[arg].print(out);
				i = j+len+1;
			}
		}

		if (i < spec.length()) { out << spec.substr(i); }
		return out.str();
	}
}
