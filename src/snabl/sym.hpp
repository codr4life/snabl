#ifndef SNABL_SYM_HPP
#define SNABL_SYM_HPP

#include <memory>
#include <string>

namespace snabl {
	struct Sym;
	
	struct SymImp {
	public:
		friend std::hash<Sym>;
		SymImp(const std::string &name);
	private:
		const std::string _name;
		const size_t _hash;
	};

	using SymPtr = std::shared_ptr<SymImp>;
	
	struct Sym {
	public:
		friend std::hash<Sym>;
		friend bool operator==(const Sym &x, const Sym &y);
		friend bool operator!=(const Sym &x, const Sym &y);
		
		Sym(const SymPtr &imp);
	private:
		const SymPtr _imp;
	};

	bool operator==(const Sym &x, const Sym &y);
	bool operator!=(const Sym &x, const Sym &y);
}

namespace std {
	template<>
	struct hash<snabl::Sym> {
		size_t operator()(const snabl::Sym &x) const {
			return x._imp->_hash;
    }
	};
}

#endif
