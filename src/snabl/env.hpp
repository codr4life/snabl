#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include "snabl/lib.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/pos.hpp"
#include "snabl/run.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/task.hpp"
#include "snabl/types.hpp"
#include "snabl/types/bool.hpp"
#include "snabl/types/char.hpp"
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
#include "snabl/types/task.hpp"
#include "snabl/types/time.hpp"

namespace snabl {
	template <typename ValT>
	class Type;

	const array<Int, 3> version {0, 2, 1};

	class Env {
	public:
	private:
		list<SymImp> _syms;
		unordered_map<string, Sym> _sym_table;
		Int _type_tag;
		TaskPtr _task;
	public:
		set<char> separators;

		TraitPtr root_type, maybe_type, no_type, num_type, seq_type, sink_type, 
			source_type;
		
		TypePtr<ATypePtr> meta_type;
		TypePtr<bool> bool_type;
		TypePtr<Char> char_type;
		TypePtr<ErrorPtr> error_type;
		TypePtr<Float> float_type;
		TypePtr<Int> int_type;
		TypePtr<IterPtr> iter_type;
		TypePtr<LambdaPtr> lambda_type;
		TypePtr<Nil> nil_type;
		TypePtr<StackPtr> stack_type;
		TypePtr<StrPtr> str_type;
		TypePtr<Sym> sym_type;
		TypePtr<TaskPtr> task_type;
		TypePtr<Time> time_type;
		
		libs::Home home_lib;
		const TaskPtr main_task;
		const ScopePtr &root_scope;

		Env():
			_type_tag(1),
			_task(nullptr),
			separators {
			  ' ', '\t', '\n', ',', ';', '?', '&', '.', '|',
				'<', '>', '(', ')', '{', '}', '[', ']'
			},
			home_lib(*this),
		  main_task((_task = start_task())),
			root_scope(begin_scope()) {
			add_special_char('t', 8);
			add_special_char('n', 10);
			add_special_char('r', 13);
			add_special_char('e', 27);
			add_special_char('s', 32);
			begin_regs();
		}

		TaskPtr start_task(PC start_pc=nullptr, const ScopePtr &parent_scope=nullptr) {
			auto t(make_shared<Task>(*this, start_pc, parent_scope));

			if (_task) {
				t->_next = _task;
				t->_prev = _task->_prev;
				t->_prev->_next = t;
				_task->_prev = t;
			} else {
				t->_prev = t->_next = t;
			}
			
			return t;
		}

		bool yield() { 
			if (_task->_next == _task) { return false; }
			_task = _task->_next; 
			return true;
		}

		Env(const Env &) = delete;
		const Env &operator=(const Env &) = delete;

		Int next_type_tag() { return _type_tag++; }

		void add_special_char(char in, Char out) {
			_special_chars.emplace(in, out);
			_char_specials.emplace(out, in);
		}

		optional<Char> find_special_char(char in) {
			auto found(_special_chars.find(in));
			return (found == _special_chars.end()) ? nullopt : make_optional(found->second);
		}

		optional<char> find_char_special(Char in) {
			auto found(_char_specials.find(in));
			return (found == _char_specials.end()) ? nullopt : make_optional(found->second);
		}
		
		Sym sym(const string &name) {
			const auto found(_sym_table.find(name));
			if (found != _sym_table.end()) { return found->second; }
			_syms.emplace_back(name);
			const auto imp(&_syms.back());
			return _sym_table.emplace(name, Sym(imp)).first->second;
		}

		void begin_regs() { _nregs.push_back(0); }
		
		Int end_regs() {
			const Int n(_nregs.back());
			_nregs.pop_back();
			return n;
		}

		Int next_reg(Pos pos) {
			auto &n(_nregs.back());

			if (n == Scope::MaxRegs) {
				throw CompileError(pos, "No more regs, consider adding additional scopes");
			}

			return n++;
		}
		
		template <typename T>
		T &get_reg(Int idx) { return any_cast<T &>(_task->_scope->_regs[idx]); }

		template <typename T>
		const T &get_reg(Int idx) const {
			return any_cast<const T &>(_task->_scope->_regs[idx]);
		}

		void let_reg(Int idx, any &&val) { _task->_scope->_regs[idx] = move(val); }
		void clear_reg(Int idx) const { _task->_scope->_regs[idx].reset(); }
		
		template <typename ImpT, typename... ArgsT>
		Op &emit(const OpType<ImpT> &type, ArgsT &&... args) {
		  Op *prev(_ops.empty() ? nullptr : &_ops.back());
			_ops.emplace_back(*this, type, args...);
			auto &op(_ops.back());
			if (prev) { prev->next = op.imp; }
			return op;
		}

		void compile(string_view in);
		void compile(istream &in);
		void compile(const Forms &forms);
		void compile(const Form &form);
		void compile(Forms::const_iterator begin, Forms::const_iterator end);
		
		void run(string_view in);
		void run(istream &in);
		void run();

		Lib &lib() const { return *_task->_lib; }
		const Ops &ops() const { return _ops; }
		PC pc() const { return _task->_pc; }
		
		const ScopePtr &begin_scope(const ScopePtr &parent=nullptr) {
			return _task->begin_scope(parent);
		}

		const ScopePtr &scope() const { return _task->_scope; }

		void end_scope() { _task->end_scope(); }

		void jump(const OpImp &pc) { _task->_pc = pc ? &pc : nullptr; }

		void jump(PC pc) { _task->_pc = pc; }

		void jump(Int pc) {
			_task->_pc = (pc == Int(_ops.size())) ? nullptr : &(_ops.begin()+pc)->imp;
		}

		void begin_call(const TargetPtr &target, Pos pos, PC return_pc) {
			_task->_calls.emplace_back(*this, target, pos, return_pc);
		}
		
		const Call &call() const { return _task->_calls.back(); }
		void end_call() { _task->_calls.pop_back(); }
		
		void recall(Pos pos) {
			auto &calls(_task->_calls);
			if (!calls.size()) { throw RuntimeError(*this, pos, "Nothing to recall"); }

			const auto &c(calls.back());
			const auto &t(*c.target);
			const auto &s(c.state);
			
			s.restore_lib(*this);
			s.restore_scope(*this);
			s.restore_tries(*this);
			s.restore_splits(*this);
			
			if (t._parent_scope) { _task->_scope->clear_vars(); }
			jump(&t._start_pc);
		}

		void _return(Pos pos) {
			auto &calls(_task->_calls);
			if (!calls.size()) { throw RuntimeError(*this, pos, "Nothing to return from"); }
			auto &c(calls.back());
			auto &t(*c.target);
			if (t._parent_scope) { end_scope(); }
			_task->_pc = c.return_pc;
			auto fi(dynamic_cast<Fimp *>(&t));
			if (fi && !fi->imp) { end_split(); }
			end_call();
		}
		
		void begin_try(ops::Try &op) { _task->_tries.push_back(&op); }
		ops::Try *current_try() { return _task->_tries.back(); }
		void end_try() { _task->_tries.pop_back(); }

		void push(const Box &val) { _task->_stack.push_back(val); }

		template <typename ValT, typename... ArgsT>
		void push(const TypePtr<ValT> &type, ArgsT &&...args) {
			_task->_stack.emplace_back(type, ValT(forward<ArgsT>(args)...));
		}

		Box &peek() {
			if (Int(_task->_stack.size()) <= _task->_stack_offs) {
				throw Error("Nothing to peek");
			}
			
			return _task->_stack.back();
		}

		Box pop() {
			if (Int(_task->_stack.size()) <= _task->_stack_offs) {
				throw Error("Nothing to pop");
			}
			
			Box v(_task->_stack.back());
			_task->_stack.pop_back();
			return v;
		}

		const Stack &stack() { return _task->_stack; }

		void begin_split(Int offs=0) {
			_task->_stack_offs = _task->_stack.size()-offs;
			_task->_splits.push_back(_task->_stack_offs);
		}

		void end_split() {
			_task->_splits.pop_back();
			_task->_stack_offs = _task->_splits.size() ? _task->_splits.back() : 0;
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
		map<char, Char> _special_chars;
		map<Char, char> _char_specials;
		vector<Int> _nregs;
		Ops _ops;
				
		friend RuntimeError;
		friend State;
		friend Target;

		friend ops::DDrop::Type;
		friend ops::Drop::Type;
		friend ops::Dup::Type;
		friend ops::Eqval::Type;
		friend ops::Fimp::Type;
		friend ops::Funcall::Type;
		friend ops::Isa::Type;
		friend ops::Let::Type;
		friend ops::Recall::Type;
		friend ops::Return::Type;
		friend ops::Rot::Type;
		friend ops::RSwap::Type;
		friend ops::SDrop::Type;
		friend ops::Stack::Type;
		friend ops::Swap::Type;
		friend ops::Task::Type;
		friend ops::Try::Type;
		friend ops::TryEnd::Type;		
	};

	inline bool Box::isa(const ATypePtr &rhs) const {
		auto &lhs((_type == _type->lib.env.meta_type) ? as<ATypePtr>() : _type);
		return lhs->isa(rhs);
	}

	template <typename ValT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id,
																 const TypePtr<ValT> &type,
																 ArgsT &&... args) {
		return add_macro(id, [type, args...](Forms::const_iterator &in,
																				 Forms::const_iterator end,
																				 Env &env) {
											 env.emit(ops::Push::type, (in++)->pos, type, args...);			
										 });
	}

	template <typename ImpT, typename... ArgsT>
	const MacroPtr &Lib::add_macro(Sym id, const OpType<ImpT> &type, ArgsT &&... args) {
		return add_macro(id, [&type, args...](Forms::const_iterator &in,
																					Forms::const_iterator end,
																					Env &env) {
											 env.emit(type, (in++)->pos, args...);			
										 });
	}
}

#endif
