#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include "snabl/call.hpp"
#include "snabl/lib.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/pos.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/types/bool.hpp"
#include "snabl/types/error.hpp"
#include "snabl/types/float.hpp"
#include "snabl/types/int.hpp"
#include "snabl/types/lambda.hpp"
#include "snabl/types/meta.hpp"
#include "snabl/types/nil.hpp"
#include "snabl/types/str.hpp"
#include "snabl/types/sym.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
	template <typename ValT>
	class Type;

	class Env {
	public:
		Alloc<Box> stack_alloc;
		Alloc<Call> calls_alloc;
		Alloc<pair<string, SymImp>> syms_alloc;
	private:
		map<string, SymImp, less<string>, Alloc<pair<string, SymImp>>> _syms;
		size_t _type_tag;
		vector<ScopePtr> _scopes;
		Stack _stack;
	public:
		TraitPtr a_type, maybe_type, no_type, num_type;
		TypePtr<ATypePtr> meta_type;
		TypePtr<bool> bool_type;
		TypePtr<ErrorPtr> error_type;
		TypePtr<Float> float_type;
		TypePtr<Int> int_type;
		TypePtr<LambdaPtr> lambda_type;
		TypePtr<Nil> nil_type;
		TypePtr<StrPtr> str_type;
		TypePtr<Sym> sym_type;
		TypePtr<Time> time_type;
		
		libs::Home home;
		unordered_set<char> separators;

		Ops ops;
		PC pc;

		const ScopePtr &main;
		
		Env():
			_syms(syms_alloc),
			_type_tag(1),
			_stack(stack_alloc),
			home(*this),
			separators({' ', '\t', '\n', ',', ';', '?', '<', '>', '(', ')', '{', '}'}),
			pc(ops.begin()),
			main(begin_scope()),
		  _calls(calls_alloc) { begin_lib(home); }

		Env(const Env &) = delete;
		const Env &operator=(const Env &) = delete;

		size_t next_type_tag() { return _type_tag++; }

		Sym sym(const string &name) {
			auto found(_syms.find(name));
			
			return Sym((found == _syms.end())
								 ? &_syms.emplace(name, SymImp(name)).first->second
								 : &found->second);
		}

		template <typename ImpT, typename... ArgsT>
		Op &emit(const OpType<ImpT> &type, ArgsT &&... args) {
			auto prev_offs(pc-ops.begin());
			ops.emplace_back(type, args...);
			pc = ops.begin()+prev_offs;
			return ops.back();
		}

		void emit(Pos pos, FuncPtr &func, FimpPtr &fimp);

		void compile(string_view in);
		void compile(istream &in);
		void compile(const Forms &forms);		
		void compile(Forms::const_iterator begin, Forms::const_iterator end);
		void compile(Forms::const_iterator begin, Forms::const_iterator end,
								 FuncPtr &func, FimpPtr &fimp);
		
		void run(string_view in);
		void run(istream &in);
		void run(optional<PC> end_pc=nullopt);

		void begin_lib(Lib &lib) { _libs.push_back(&lib); }
	
		Lib &lib() {
			if (_libs.empty()) { throw Error("No libs"); }
			return *_libs.back();
		}

		void end_lib() {
			if (_libs.empty()) { throw Error("No libs"); }
			_libs.pop_back();
		}

		const ScopePtr &begin_scope(const ScopePtr &parent=nullptr) {
			_scopes.push_back(ScopePtr::make(*this, parent));
			return _scopes.back();
		}

		const ScopePtr &scope() const {
			if (_scopes.empty()) { throw Error("No open scopes"); }
			return _scopes.back();
		}

		void end_scope() {
			if (_scopes.empty()) { throw Error("No open scopes"); }
			_scopes.pop_back();
		}

		template <typename... ArgsT>
		Call &begin_call(Scope &scope, ArgsT &&... args) {
			_calls.emplace_back(scope, forward<ArgsT>(args)...);
			return _calls.back();
		}
		
		Call &call() {
			if (_calls.empty()) { throw Error("No calls"); }
			return _calls.back();
		}

		void end_call() {
			if (_calls.empty()) { throw Error("No active calls"); }
			_calls.pop_back();
		}

		void push(const Box &val) { _stack.push_back(val); }

		template <typename ValT, typename... ArgsT>
		void push(const TypePtr<ValT> &type, ArgsT &&...args) {
			_stack.emplace_back(type, ValT(forward<ArgsT>(args)...));
		}

		Box pop() {
			if (_stack.empty()) { throw Error("Nothing to pop"); }
			Box v(_stack.back());
			_stack.pop_back();
			return v;
		}

		const Stack &stack() { return _stack; }

		template <typename... ArgsT>
		void note(Pos pos, const string &msg, ArgsT &&... args) {
			cerr << fmt("Note in row %0, col %1: ", {pos.row, pos.col})
					 << fmt(msg, {args...}) << endl;
		}

		template <typename... ArgsT>
		void warn(Pos pos, const string &msg, ArgsT &&... args) {
			cerr << fmt("Warning in row %0, col %1: ", {pos.row, pos.col})
					 << fmt(msg, {args...}) << endl;
		}

	private:
		vector<Lib *> _libs;
		deque<Call, Alloc<Call>> _calls;
		
		friend struct State;
		friend struct RuntimeError;
	};
		
	template <typename ValT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id,
																 const TypePtr<ValT> &type,
																 ArgsT &&... args) {
		return add_macro(id, [type, args...](Forms::const_iterator &in,
																					Forms::const_iterator end,
																					FuncPtr &func, FimpPtr &fimp,
																					Env &env) {
											 env.emit(ops::Push::type,
																(in++)->pos,
																type,
																args...);			
										 });
	}

	template <typename ImpT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id, const OpType<ImpT> &type, ArgsT &&... args) {
		return add_macro(id, [&type, args...](Forms::const_iterator &in,
																					 Forms::const_iterator end,
																					 FuncPtr &func, FimpPtr &fimp,
																					 Env &env) {
											 env.emit(type, (in++)->pos, args...);			
										 });
	}
}

#endif
