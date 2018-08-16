#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include <deque>
#include <map>

#include "snabl/box.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	template <typename ValueT>
	class Type;
	
	class Env {
	public:
		Env();
		Sym get_sym(const std::string &name);
		
		template <typename ValueT>
		void push(Type<ValueT> &type, const ValueT &value);
	private:
		std::map<std::string, std::shared_ptr<SymImp>> _syms;
		std::deque<Box> _stack;
	};

	template <typename ValueT>
	void Env::push(Type<ValueT> &type, const ValueT &value) {
		_stack.emplace_back(type, value);
	}
}

#endif
