#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "snabl/bin.hpp"
#include "snabl/float.hpp"
#include "snabl/int.hpp"
#include "snabl/lib.hpp"
#include "snabl/pos.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	template <typename ValueT>
	class Type;
	
	class Env {
	private:
		std::vector<ScopePtr> _scopes;
		std::unordered_map<std::string, std::unique_ptr<SymImp>> _syms;
	public:
		static const Pos home_pos;

		Lib lobby;
		const TypePtr<Float> float_type;
		const TypePtr<Int> int_type;
		std::unordered_set<char> separators;
		Bin bin;
		const ScopePtr main;
		
		Env();
		void parse(const std::string &in, Forms &out);
		void parse(std::istream &in, Pos start_pos, Forms &out);
		Sym get_sym(const std::string &name);

		ScopePtr begin(const ScopePtr &parent=nullptr);
		ScopePtr scope();
		ScopePtr end();
		
		template <typename ValueT>
		void push_stack(const TypePtr<ValueT> &type, const ValueT &value);
		void push_stack(const Box &value);
		std::optional<Box> pop_stack();
		
		Stack::iterator stack_end();
	private:
		Pos _pos;
		Stack _stack;

		void parse_id(std::istream &in, Forms &out);
		void parse_num(std::istream &in, Forms &out);
	};

	template <typename ValueT>
	void Env::push_stack(const TypePtr<ValueT> &type, const ValueT &value) {
		_stack.emplace_back(type, value);
	}
}

#endif
