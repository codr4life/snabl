#ifndef SNABL_SYM_HPP
#define SNABL_SYM_HPP

#include <string>

namespace snabl {
	struct Sym {
	public:
		Sym(const std::string &name);
	private:
		const std::string _name;
	};

	bool operator==(const Sym &x, const Sym &y);
	bool operator!=(const Sym &x, const Sym &y);
}

#endif
