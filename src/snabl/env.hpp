#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include <deque>
#include <unordered_map>

#include "snabl/box.hpp"
#include "snabl/int.hpp"
#include "snabl/lib.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	template <typename ValueT>
	class Type;
	
	class Env {
	private:
		std::unordered_map<std::string, std::unique_ptr<SymImp>> _syms;
	public:
		Lib lobby;
		const TypePtr<Int> int_type;
		
		Env();
		Sym get_sym(const std::string &name);
		
		template <typename ValueT>
		void push(const TypePtr<ValueT> &type, const ValueT &value);
	private:
		std::deque<Box> _stack;
	};

	template <typename ValueT>
	void Env::push(const TypePtr<ValueT> &type, const ValueT &value) {
		_stack.emplace_back(type, value);
	}
}

#endif
