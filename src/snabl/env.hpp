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
		vector<size_t> _stacks;
		unordered_map<Sym, Box> _vars;
	public:
		static const Pos home_pos;

		TraitPtr maybe_type, a_type, no_type, num_type;
		TypePtr<bool> bool_type;
		TypePtr<Float> float_type;
		TypePtr<Int> int_type;
		TypePtr<LambdaPtr> lambda_type;
		TypePtr<Nil> nil_type;
		TypePtr<Time> time_type;
		
		libs::Home home;
		unordered_set<char> separators;

		Ops ops;
		PC pc;

		const ScopePtr &main;
		
		Env():
			_type_tag(1),
			home(*this),
			separators({' ', '\t', '\n', ',', ';', '<', '>', '(', ')', '{', '}'}),
			pc(ops.begin()),
			main(begin_scope()),
			_pos(home_pos),
			_is_safe(true) { begin_lib(home); }

		size_t next_type_tag() { return _type_tag++; }

		Sym sym(const string &name) {
			auto found(_syms.find(name));
			
			return Sym((found == _syms.end())
								 ? _syms.emplace(name, make_unique<SymImp>(name))
								 .first->second.get()
								 : found->second.get());
		}

		void parse(string_view in, Forms &out);
		void parse(istream &in, Forms &out);
		bool parse(istream &in, Pos start_pos, char end, Forms &out);

		template <typename ImpT, typename... ArgsT>
		Op &emit(const OpType<ImpT> &type, ArgsT &&... args) {
			ops.emplace_back(type, args...);
			return ops.back();
		}

		void emit(Pos pos, FuncPtr &func, FimpPtr &fimp);

		void compile(string_view in);
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

		void begin_stack(size_t offs) {
			_stacks.push_back(_stacks.empty()
												? offs
												: max(offs, _stacks.back()));
		}

		size_t end_stack() {
			if (_stacks.empty()) { throw Error("No stacks"); }
			auto offs(_stacks.back());
			_stacks.pop_back();
			return offs;
		}

		void push(const Box &val) { _stack.push_back(val); }

		template <typename ValT, typename... ArgsT>
		void push(const TypePtr<ValT> &type, ArgsT &&...args) {
			_stack.emplace_back(type, ValT(forward<ArgsT>(args)...));
		}

		Box pop() {
			if (_is_safe &&
					(_stack.empty() ||
					 (!_stacks.empty() &&
						_stack.size() <= _stacks.back()))) { throw Error("Nothing to pop"); }
				
			Box v(_stack.back());
			_stack.pop_back();
			return v;
		}

		const Stack &stack() { return _stack; }

		void dump_stack(std::ostream &out) const {
			out << '[';
			char sep(0);
		
			for (auto &v: _stack) {
				if (sep) { out << sep; }
				v.dump(out);
				sep = ' ';
			}

			out << ']' << endl;
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

		bool is_safe() const { return _is_safe; }
	private:
		Pos _pos;
		vector<Lib *> _libs;
		deque<Lambda> _lambdas;
		deque<Call> _calls;
		bool _is_safe;
		
		template <typename FormT>
		bool parse_body(istream &in, char end, Forms &out) {
			auto start_pos(_pos);
			Forms body;
			if (!parse(in, start_pos, end, body) && end) { return false; }
			out.emplace_back(FormT::type, start_pos, body.cbegin(), body.cend());
			return true;
		}

		void parse_id(istream &in, Forms &out);
		void parse_lambda(istream &in, Forms &out);
		void parse_num(istream &in, Forms &out);
		void parse_sexpr(istream &in, Forms &out);
		void parse_type_list(istream &in, Forms &out);

		friend struct State;
	};
		
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
		return add_macro(id, [&type, args...](Forms::const_iterator &in,
																					Forms::const_iterator end,
																					FuncPtr &func, FimpPtr &fimp,
																					Env &env) {
											 env.emit(type, (in++)->pos, args...);			
										 });
	}
}

#endif
