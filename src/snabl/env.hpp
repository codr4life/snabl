#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include "snabl/call.hpp"
#include "snabl/lib.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/pos.hpp"
#include "snabl/run.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/types.hpp"
#include "snabl/types/bool.hpp"
#include "snabl/types/error.hpp"
#include "snabl/types/float.hpp"
#include "snabl/types/int.hpp"
#include "snabl/types/iter.hpp"
#include "snabl/types/lambda.hpp"
#include "snabl/types/meta.hpp"
#include "snabl/types/nil.hpp"
#include "snabl/types/stack.hpp"
#include "snabl/types/str.hpp"
#include "snabl/types/sym.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
	template <typename ValT>
	class Type;
	
	class Env {
	public:
	private:
		list<SymImp> _syms;
		unordered_map<string, Sym> _sym_table;
		size_t _type_tag;
		vector<ScopePtr> _scopes;
		Stack _stack;
	public:
		TraitPtr root_type, maybe_type, no_type, num_type, seq_type, sink_type, 
			source_type;
		
		TypePtr<ATypePtr> meta_type;
		TypePtr<bool> bool_type;
		TypePtr<ErrorPtr> error_type;
		TypePtr<Float> float_type;
		TypePtr<Int> int_type;
		TypePtr<IterPtr> iter_type;
		TypePtr<LambdaPtr> lambda_type;
		TypePtr<Nil> nil_type;
		TypePtr<StackPtr> stack_type;
		TypePtr<StrPtr> str_type;
		TypePtr<Sym> sym_type;
		TypePtr<Time> time_type;
		
		libs::Home home;
		unordered_set<char> separators;

		Ops ops;
		PC pc;
		
		const ScopePtr &main;
		
		Env():
			_type_tag(1),
			home(*this),
			separators({
					' ', '\t', '\n', ',', ';', '?', '.', '|',
						'<', '>', '(', ')', '{', '}', '[', ']'
						}),
			pc(nullptr),
			main(begin_scope()),
			_stack_offs(0) { begin_lib(home); }

		Env(const Env &) = delete;
		const Env &operator=(const Env &) = delete;

		size_t next_type_tag() { return _type_tag++; }

		Sym sym(const string &name) {
			const auto found(_sym_table.find(name));
			if (found != _sym_table.end()) { return found->second; }
			_syms.emplace_back(name);
			const auto imp(&_syms.back());
			return _sym_table.emplace(name, Sym(imp)).first->second;
		}

		template <typename ImpT, typename... ArgsT>
		Op &emit(const OpType<ImpT> &type, ArgsT &&... args) {
		  Op *prev(ops.empty() ? nullptr : &ops.back());
			ops.emplace_back(*this, type, args...);
			auto &op(ops.back());
			if (prev) { prev->next = &op.imp; }
			return op;
		}

		void emit(Pos pos, FuncPtr &func, FimpPtr &fimp);

		void compile(string_view in);
		void compile(istream &in);
		void compile(const Forms &forms);
		void compile(const Form &form);
		void compile(const Form &form, FuncPtr &func, FimpPtr &fimp);
		void compile(Forms::const_iterator begin, Forms::const_iterator end);
		void compile(Forms::const_iterator begin, Forms::const_iterator end,
								 FuncPtr &func, FimpPtr &fimp);
		
		void run(string_view in);
		void run(istream &in);
		void run();

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
			_scopes.push_back(make_shared<Scope>(*this, parent));
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
		Call &begin_call(ArgsT &&... args) {
			_calls.emplace_back(*this, forward<ArgsT>(args)...);
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

		Box &peek() {
			if (_stack.size() <= _stack_offs) { throw Error("Nothing to peek"); }
			return _stack.back();
		}

		Box pop() {
			if (_stack.size() <= _stack_offs) { throw Error("Nothing to pop"); }
			Box v(_stack.back());
			_stack.pop_back();
			return v;
		}

		const Stack &stack() { return _stack; }
		
		void split(size_t offs=0) {
			_stack_offs = _stack.size()-offs;
			_splits.push_back(_stack_offs);
		}

		void unsplit() {
			_splits.pop_back();
			_stack_offs = _splits.empty() ? 0 : _splits.back();
		}

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
		deque<Call> _calls;
		vector<ops::Try *> _tries;
		vector<size_t> _splits;
		size_t _stack_offs;
		
		friend State;
		friend RuntimeError;
		friend ops::DDrop::Type;
		friend ops::Drop::Type;
		friend ops::Dup::Type;
		friend ops::Eqval::Type;
		friend ops::Fimp::Type;
		friend ops::Funcall::Type;
		friend ops::Isa::Type;
		friend ops::Let::Type;
		friend ops::Rot::Type;
		friend ops::RSwap::Type;
		friend ops::SDrop::Type;
		friend ops::Stack::Type;
		friend ops::Swap::Type;
		friend ops::Try::Type;
		friend ops::TryEnd::Type;
	};

	inline bool Box::isa(const ATypePtr &rhs) const {
		auto &lhs((_type == _type->lib.env.meta_type) ? as<ATypePtr>() : _type);
		return lhs->isa(rhs);
	}

	inline void Call::recall(Env &env) const {
		_state->restore_libs(env);
		_state->restore_scopes(env);
		if (target->opts() & Target::Opts::Vars) { env.scope()->clear_vars(); }
		env.pc = target->start_pc();
	}

	template <typename ValT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id,
																 const TypePtr<ValT> &type,
																 ArgsT &&... args) {
		return add_macro(id, [type, args...](Forms::const_iterator &in,
																					Forms::const_iterator end,
																					FuncPtr &func, FimpPtr &fimp,
																					Env &env) {
											 env.emit(ops::Push::type, (in++)->pos, type, args...);			
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
