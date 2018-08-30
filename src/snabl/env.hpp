#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include "snabl/call.hpp"
#include "snabl/lib.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/pos.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/types/bool.hpp"
#include "snabl/types/float.hpp"
#include "snabl/types/int.hpp"

namespace snabl {
	template <typename ValT>
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

		Ops ops;
		PC pc;

		const ScopePtr main;
		
		Env();
		size_t next_type_tag();
		Sym sym(const string &name);
		
		void parse(string_view in, Forms &out);
		bool parse(istream &in, Pos start_pos, char end, Forms &out);
		
		template <typename ImpT, typename... ArgsT>
		Op &emit(const OpType<ImpT> &type, ArgsT &&... args);

		void compile(string_view in);
		void compile(const Forms &forms);		

		void compile(const Forms::const_iterator &begin,
								 const Forms::const_iterator &end);
		
		void run(string_view in);
		void run(optional<PC> end_pc=nullopt);

		void begin_lib(Lib &lib);
		Lib &lib();
		Lib &end_lib();

		const ScopePtr &begin_scope();
		const ScopePtr &scope();
		ScopePtr end_scope();

		Call &begin_call(const CallTargetPtr &target, optional<PC> return_pc=nullopt);
		Call *call();
		Call end_call();
		
		template <typename ValT>
		void push(const TypePtr<ValT> &type, const ValT &val);

		void push(const Box &val);
		Box pop();
		const Stack &stack();
		void dump_stack(std::ostream &out) const;
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

	template <typename ImpT, typename... ArgsT>
	Op &Env::emit(const OpType<ImpT> &type, ArgsT &&... args) {
		ops.emplace_back(type, args...);
		return ops.back();
	}
	
	template <typename ValT>
	void Env::push(const TypePtr<ValT> &type, const ValT &val) {
		_stack.emplace_back(type, val);
	}
}

#endif
