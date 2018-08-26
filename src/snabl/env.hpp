#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "snabl/bin.hpp"
#include "snabl/bool.hpp"
#include "snabl/call.hpp"
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
		std::unordered_map<std::string, std::unique_ptr<SymImp>> _syms;
		size_t _type_tag;
		std::vector<ScopePtr> _scopes;
		Stack _stack;
		std::unordered_map<Sym, Box> _vars;
	public:
		static const Pos home_pos;

		Lib home;
		const TraitPtr maybe_type, a_type, no_type, num_type;
		const TypePtr<bool> bool_type;
		const TypePtr<Float> float_type;
		const TypePtr<Int> int_type;
		std::unordered_set<char> separators;
		Bin bin;
		const ScopePtr main;
		
		Env();
		size_t next_type_tag();
		Sym get_sym(const std::string &name);
		
		void parse(const std::string &in, Forms &out);
		bool parse(std::istream &in, Pos start_pos, char end, Forms &out);
		
		void compile(const std::string &in);
		void run(const std::string &in);
		
		void push_lib(Lib &lib);
		Lib &lib();
		Lib &pop_lib();

		ScopePtr begin();
		ScopePtr scope();
		ScopePtr end();

		Call &push_call(const TargetPtr &target, ssize_t return_pc);
		Call *peek_call();
		void pop_call();
		
		template <typename ValueT>
		void push_stack(const TypePtr<ValueT> &type, const ValueT &value);
		void push_stack(const Box &value);
		std::optional<Box> pop_stack();
		const Stack &stack();

		Box const* get_var(const Sym &id);
		std::optional<Box> put_var(const Sym &id, const Box &value);
	private:
		Pos _pos;
		std::vector<Lib *> _libs;
		std::deque<Call> _calls;
		
		void parse_id(std::istream &in, Forms &out);
		void parse_num(std::istream &in, Forms &out);
		bool parse_rest(std::istream &in, char end, Forms &out);
		void parse_sexpr(std::istream &in, Forms &out);
		void parse_type_list(std::istream &in, Forms &out);
	};

	template <typename ValueT>
	void Env::push_stack(const TypePtr<ValueT> &type, const ValueT &value) {
		_stack.emplace_back(type, value);
	}
}

#endif
