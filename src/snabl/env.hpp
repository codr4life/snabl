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
#include "snabl/types/lambda.hpp"
#include "snabl/types/nil.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
	template <typename ValT>
	class Type;
	
	class Env {
	private:
		unordered_map<string, unique_ptr<SymImp>> _syms;
		size_t _type_tag;
		vector<ScopePtr> _scopes;
		Stack _stack;
		vector<size_t> _stack_offs;
		unordered_map<Sym, Box> _vars;
	public:
		static const Pos home_pos;

		TraitPtr maybe_type, a_type, no_type, num_type;
		TypePtr<bool> bool_type;
		TypePtr<Float> float_type;
		TypePtr<Int> int_type;
		TypePtr<Lambda> lambda_type;
		TypePtr<Nil> nil_type;
		TypePtr<Time> time_type;
		
		libs::Home home;
		unordered_set<char> separators;

		Ops ops;
		PC pc;

		const ScopePtr main;
		
		Env();
		size_t next_type_tag();
		Sym sym(const string &name);
		
		void parse(string_view in, Forms &out);
		void parse(istream &in, Forms &out);
		bool parse(istream &in, Pos start_pos, char end, Forms &out);
		
		template <typename ImpT, typename... ArgsT>
		Op &emit(const OpType<ImpT> &type, ArgsT &&... args);

		void compile(string_view in);
		void compile(const Forms &forms);		
		void compile(Forms::const_iterator begin, Forms::const_iterator end);
		
		void run(string_view in);
		void run(istream &in);
		void run(optional<PC> end_pc=nullopt);

		void begin_lib(Lib &lib);
		Lib &lib();
		void end_lib();

		const ScopePtr &begin_scope(const ScopePtr &parent=nullptr);
		const ScopePtr &scope();
		void end_scope();
		
		template <typename... ArgsT>
		Call &begin_call(ArgsT &&... args);

		Call &call();
		void end_call();
		
		void begin_stack(size_t offs);
		size_t end_stack();		

		template <typename ValT, typename... ArgsT>
		void push(const TypePtr<ValT> &type, ArgsT &&...args);

		void push(const Box &val);
		Box pop();
		const Stack &stack();
		void dump_stack(std::ostream &out) const;
	private:
		Pos _pos;
		vector<Lib *> _libs;
		deque<Call> _calls;
		
		template <typename FormT>
		bool parse_body(istream &in, char end, Forms &out);

		void parse_id(istream &in, Forms &out);
		void parse_lambda(istream &in, Forms &out);
		void parse_num(istream &in, Forms &out);
		void parse_sexpr(istream &in, Forms &out);
		void parse_type_list(istream &in, Forms &out);
	};

	template <typename FormT>
	bool Env::parse_body(istream &in, char end, Forms &out) {
		auto start_pos(_pos);
		Forms body;
		if (!parse(in, start_pos, end, body) && end) { return false; }
		out.emplace_back(FormT::type, start_pos, body.cbegin(), body.cend());
		return true;
	}
	
	template <typename ImpT, typename... ArgsT>
	Op &Env::emit(const OpType<ImpT> &type, ArgsT &&... args) {
		ops.emplace_back(type, args...);
		return ops.back();
	}
	
	template <typename ValT, typename... ArgsT>
	void Env::push(const TypePtr<ValT> &type, ArgsT &&...args) {
		_stack.emplace_back(type, ValT(forward<ArgsT>(args)...));
	}

	template <typename ValT>
	const MacroPtr &Lib::add_macro(Sym id, const TypePtr<ValT> &type, const ValT &val) {
		return add_macro(id, [type, val](Forms::const_iterator &in,
																		 Forms::const_iterator end,
																		 FuncPtr &func, FimpPtr &fimp,
																		 Env &env) {
											 env.emit(ops::Push::type,
																(in++)->pos,
																Box(type, val));			
										 });
	}

	template <typename ImpT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id, const OpType<ImpT> &type, ArgsT &&... args) {
		return add_macro(id, [type, args...](Forms::const_iterator &in,
																				 Forms::const_iterator end,
																				 FuncPtr &func, FimpPtr &fimp,
																				 Env &env) {
											 env.emit(type, (in++)->pos, forward<ArgsT>(args)...);			
										 });
	}

	template <typename... ArgsT>
	Call &Env::begin_call(ArgsT &&... args) {
		_calls.emplace_back(scope(), forward<ArgsT>(args)...);
		return _calls.back();
	}
}

#endif
