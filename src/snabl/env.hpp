#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include <deque>
#include <unordered_map>
#include <unordered_set>

#include "snabl/bin.hpp"
#include "snabl/bool.hpp"
#include "snabl/call.hpp"
#include "snabl/float.hpp"
#include "snabl/int.hpp"
#include "snabl/lib.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/pos.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	template <typename ValueT>
	class Type;
	
	class Env {
	private:
		unordered_map<string, unique_ptr<SymImp>> _syms;
		size_t _type_tag;
		vector<ScopePtr> _scopes;
		Stack _stack;
		unordered_map<Sym, Box> _vars;
	public:
		static const Pos home_pos;

		TraitPtr maybe_type, a_type, no_type, num_type;
		TypePtr<bool> bool_type;
		TypePtr<Float> float_type;
		TypePtr<Int> int_type;

		libs::Home home;

		unordered_set<char> separators;
		Bin bin;
		const ScopePtr main;
		
		Env();
		size_t next_type_tag();
		Sym sym(const string &name);
		
		void parse(const string &in, Forms &out);
		bool parse(istream &in, Pos start_pos, char end, Forms &out);
		
		void compile(const string &in);
		void run(const string &in);
		
		void push_lib(Lib &lib);
		Lib &lib();
		Lib &pop_lib();

		ScopePtr begin();
		ScopePtr scope();
		ScopePtr end();

		Call &push_call(const TargetPtr &target,
										optional<PC> return_pc=nullopt);
		Call *peek_call();
		Call pop_call();
		
		template <typename ValueT>
		void push_stack(const TypePtr<ValueT> &type, const ValueT &value);
		void push_stack(const Box &value);
		Box pop_stack();
		const Stack &stack();

		Box const* get_var(Sym id);
		optional<Box> unsafe_put_var(Sym id, const Box &value);
	private:
		Pos _pos;
		vector<Lib *> _libs;
		deque<Call> _calls;
		
		void parse_id(istream &in, Forms &out);
		void parse_num(istream &in, Forms &out);
		bool parse_rest(istream &in, char end, Forms &out);
		void parse_sexpr(istream &in, Forms &out);
		void parse_type_list(istream &in, Forms &out);
	};

	template <typename ValueT>
	void Env::push_stack(const TypePtr<ValueT> &type, const ValueT &value) {
		_stack.emplace_back(type, value);
	}
}

#endif
