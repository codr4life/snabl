#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include "snabl/lib.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/pos.hpp"
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
  struct Type;

  const array<Int, 3> version {0, 2, 1};

  struct Env {
    list<SymImp> syms;
    unordered_map<string, Sym> sym_table;
    Int type_tag;
    TaskPtr task;
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

    map<char, Char> special_chars;
    map<Char, char> char_specials;
    vector<Int> nregs;
    Ops ops;
    
    Env():
      type_tag(1),
      task(nullptr),
      separators {
      ' ', '\t', '\n', ',', ';', '?', '&', '.', '|',
        '<', '>', '(', ')', '{', '}', '[', ']'
        },
      home_lib(*this),
      main_task((task = start_task())),
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

      if (task) {
        t->next = task;
        t->prev = task->prev;
        t->prev->next = t;
        task->prev = t;
      } else {
        t->prev = t->next = t;
      }
      
      return t;
    }

    bool yield() { 
      if (task->next == task) { return false; }
      task = task->next; 
      return true;
    }

    Env(const Env &) = delete;
    const Env &operator=(const Env &) = delete;

    Int next_type_tag() { return type_tag++; }

    void add_special_char(char in, Char out) {
      special_chars.emplace(in, out);
      char_specials.emplace(out, in);
    }

    optional<Char> find_special_char(char in) {
      auto found(special_chars.find(in));
      return (found == special_chars.end()) ? nullopt : make_optional(found->second);
    }

    optional<char> find_char_special(Char in) {
      auto found(char_specials.find(in));
      return (found == char_specials.end()) ? nullopt : make_optional(found->second);
    }
    
    Sym sym(const string &name) {
      const auto found(sym_table.find(name));
      if (found != sym_table.end()) { return found->second; }
      syms.emplace_back(name);
      const auto imp(&syms.back());
      return sym_table.emplace(name, Sym(imp)).first->second;
    }

    void begin_regs() { nregs.push_back(0); }
    
    Int end_regs() {
      const Int n(nregs.back());
      nregs.pop_back();
      return n;
    }

    Int next_reg(Pos pos) {
      auto &n(nregs.back());

      if (n == Scope::MaxRegs) {
        throw CompileError(pos, "No more regs, consider adding additional scopes");
      }

      return n++;
    }
    
    template <typename T>
    T &get_reg(Int idx) { return any_cast<T &>(task->scope->regs[idx]); }

    template <typename T>
    const T &get_reg(Int idx) const {
      return any_cast<const T &>(task->scope->regs[idx]);
    }

    void let_reg(Int idx, any &&val) { task->scope->regs[idx] = move(val); }
    void clear_reg(Int idx) const { task->scope->regs[idx].reset(); }
    
    template <typename ImpT, typename... ArgsT>
    Op &emit(const OpType<ImpT> &type, ArgsT &&... args) {
      Op *prev(ops.empty() ? nullptr : &ops.back());
      ops.emplace_back(*this, type, args...);
      auto &op(ops.back());
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

    Lib &lib() const { return *task->lib; }
    PC pc() const { return task->pc; }
    
    const ScopePtr &begin_scope(const ScopePtr &parent=nullptr) {
      return task->begin_scope(parent);
    }

    const ScopePtr &scope() const { return task->scope; }

    void end_scope() { task->end_scope(); }

    void jump(const OpImp &pc) { task->pc = pc ? &pc : nullptr; }

    void jump(PC pc) { task->pc = pc; }

    void jump(Int pc) {
      task->pc = (pc == Int(ops.size())) ? nullptr : &(ops.begin()+pc)->imp;
    }

    void begin_call(const TargetPtr &target, Pos pos, PC return_pc) {
      task->calls.emplace_back(*this, target, pos, return_pc);
    }
    
    const Call &call() const { return task->calls.back(); }
    void end_call() { task->calls.pop_back(); }
    
    void recall(Pos pos) {
      auto &calls(task->calls);
      if (!calls.size) { throw RuntimeError(*this, pos, "Nothing to recall"); }

      const auto &c(calls.back());
      const auto &t(*c.target);
      const auto &s(c.state);
      
      s.restore_lib(*this);
      s.restore_scope(*this);
      s.restore_tries(*this);
      s.restore_splits(*this);
      
      if (t.parent_scope) { task->scope->clear_vars(); }
      jump(&t.start_pc);
    }

    void _return(Pos pos) {
      auto &calls(task->calls);
      if (!calls.size) { throw RuntimeError(*this, pos, "Nothing to return from"); }
      auto &c(calls.back());
      auto &t(*c.target);
      if (t.parent_scope) { end_scope(); }
      task->pc = c.return_pc;
      auto fi(dynamic_cast<Fimp *>(&t));
      if (fi && !fi->imp) { end_split(); }
      end_call();
    }
    
    void begin_try(ops::Try &op) { task->tries.push_back(&op); }
    ops::Try *current_try() { return task->tries.back(); }
    void end_try() { task->tries.pop_back(); }

    void push(const Box &val) { task->stack.push_back(val); }

    template <typename ValT, typename... ArgsT>
    void push(const TypePtr<ValT> &type, ArgsT &&...args) {
      task->stack.emplace_back(type, ValT(forward<ArgsT>(args)...));
    }

    Box &peek() {
      if (Int(task->stack.size()) <= task->stack_offs) {
        throw Error("Nothing to peek");
      }
      
      return task->stack.back();
    }

    Box pop() {
      if (Int(task->stack.size()) <= task->stack_offs) {
        throw Error("Nothing to pop");
      }
      
      Box v(task->stack.back());
      task->stack.pop_back();
      return v;
    }

    const Stack &stack() { return task->stack; }

    void begin_split(Int offs=0) {
      task->stack_offs = task->stack.size()-offs;
      task->splits.push_back(task->stack_offs);
    }

    void end_split() {
      task->splits.pop_back();
      task->stack_offs = task->splits.size ? task->splits.back() : 0;
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
  };

  struct UserError: RuntimeError {
    const Box val;
    UserError(Env &env, Pos pos, const Box &_val);
  };

  inline bool Box::isa(const ATypePtr &rhs) const {
    auto &lhs((type == type->lib.env.meta_type) ? as<ATypePtr>() : type);
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
