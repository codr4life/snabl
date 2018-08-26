#ifndef SNABL_SYM_HPP
#define SNABL_SYM_HPP

#include <memory>
#include <string>

namespace snabl {
	class Sym;
	
	class SymImp {
	public:
		const std::string name;
		const std::size_t hash;
		
		SymImp(const std::string &name);
	};

	class Sym {
	public:
		friend std::hash<Sym>;
		friend bool operator ==(Sym x, Sym y);
		friend bool operator !=(Sym x, Sym y);
		friend bool operator <(Sym x, Sym y);
		
		Sym(const SymImp *imp);
		const std::string &name() const;
	private:
		const SymImp *_imp;
	};

	bool operator ==(Sym x, Sym y);
	bool operator !=(Sym x, Sym y);
}

namespace std {
	template<>
	struct hash<snabl::Sym> {
		std::size_t operator ()(const snabl::Sym &x) const { return x._imp->hash; }
	};
}

#endif
