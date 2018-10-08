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
#include "snabl/types/async.hpp"
#include "snabl/types/bool.hpp"
#include "snabl/types/char.hpp"
#include "snabl/types/error.hpp"
#include "snabl/types/file.hpp"
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
    libs::Home home_lib;

    Trait &no_type, &maybe_type, &root_type, &num_type, &seq_type, &source_type,
      &sink_type;
    
    Type<AType *> &meta_type;
    Type<Nil> &nil_type;

    Type<AsyncPtr> &async_type;
    Type<bool> &bool_type;
    Type<Char> &char_type;
    Type<ErrorPtr> &error_type;
    Type<Float> &float_type;
    Type<Int> &int_type;
    Type<IterPtr> &iter_type;
    Type<Lambda> &lambda_type;
    Type<FilePtr> &rfile_type;
    Type<StackPtr> &stack_type;
    Type<StrPtr> &str_type;
    Type<Sym> &sym_type;
    Type<TaskPtr> &task_type;
    Type<Time> &time_type;
    
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
      ' ', '\t', '\n', ',', '?', '&', '.', '|',
        '<', '>', '(', ')', '{', '}', '[', ']'
        },
      home_lib(*this),

      no_type(home_lib.add_type<Trait>(sym("_"))),
      maybe_type(home_lib.add_type<Trait>(sym("Maybe"))),
      root_type(home_lib.add_type<Trait>(sym("T"), {&maybe_type})),
      num_type(home_lib.add_type<Trait>(sym("Num"), {&root_type})),
      seq_type(home_lib.add_type<Trait>(sym("Seq"), {&root_type})),
      source_type(home_lib.add_type<Trait>(sym("Source"), {&root_type})),
      sink_type(home_lib.add_type<Trait>(sym("Sink"), {&root_type})),

      meta_type(home_lib.add_type<MetaType>(sym("Type"), {&root_type})),
      nil_type(home_lib.add_type<NilType>(sym("Nil"), {&maybe_type})),

      async_type(home_lib.add_type<AsyncType>(sym("Async"), {&root_type})),
      bool_type(home_lib.add_type<BoolType>(sym("Bool"), {&root_type})),
      char_type(home_lib.add_type<CharType>(sym("Char"), {&root_type})),
      error_type(home_lib.add_type<ErrorType>(sym("Error"), {&root_type})),
      float_type(home_lib.add_type<FloatType>(sym("Float"), {&num_type})),
      int_type(home_lib.add_type<IntType>(sym("Int"), {
            &num_type, &seq_type})),
      iter_type(home_lib.add_type<IterType>(sym("Iter"), {
            &seq_type, &source_type})),
      lambda_type(home_lib.add_type<LambdaType>(sym("Lambda"), {&root_type})),
      rfile_type(home_lib.add_type<RFileType>(sym("RFile"), {&root_type})),
      stack_type(home_lib.add_type<StackType>(sym("Stack"), {
            &seq_type, &sink_type, &source_type})),
      str_type(home_lib.add_type<StrType>(sym("Str"), {
            &seq_type, &sink_type, &source_type})),
      sym_type(home_lib.add_type<SymType>(sym("Sym"), {&root_type})),
      task_type(home_lib.add_type<TaskType>(sym("Task"), {&root_type})),
      time_type(home_lib.add_type<TimeType>(sym("Time"), {&root_type})),
      
      main_task((task = start_task())),
      root_scope(begin_scope()) {
        add_special_char('t', 8);
        add_special_char('n', 10);
        add_special_char('r', 13);
        add_special_char('e', 27);
        add_special_char('s', 32);
        begin_regs();
        home_lib.init();
      }

    TaskPtr start_task(PC start_pc=nullptr, const ScopePtr &parent_scope=nullptr) {
      auto t(TaskPtr::make(*this, start_pc, parent_scope));

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
    
    template <typename OpT, typename...ArgsT>
    OpT &emit(ArgsT &&...args) {
      Op *prev(ops.empty() ? nullptr : ops.back().get());
      auto op(new OpT(*this, forward<ArgsT>(args)...));
      ops.emplace_back(op);
      if (prev) { prev->next = &op->imp; }
      return *op;
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

    void jump(PC pc) { task->pc = pc; }

    void jump(Int pc) {
      task->pc = (pc == Int(ops.size())) ? nullptr : &ops[pc]->imp;
    }

    void begin_call(Fimp &target, Pos pos, PC return_pc=nullptr) {
      task->calls.emplace_back(*this, target, pos, return_pc);
    }

    void begin_call(const Lambda &target, Pos pos, PC return_pc) {
      task->calls.emplace_back(*this, target, pos, return_pc);
    }

    const Call &call() const { return task->calls.back(); }
    void end_call() { task->calls.pop_back(); }
    
    void recall(Pos pos) {
      auto &calls(task->calls);
      if (!calls.size) { throw RuntimeError(*this, pos, "Nothing to recall"); }

      const auto &c(calls.back());
      const auto &t(c.get_target());
      const auto &s(c.state);
      
      s.restore_lib(*this);
      s.restore_scope(*this);
      s.restore_tries(*this);
      s.restore_splits(*this);
      
      if (t.parent_scope) { task->scope->clear_vars(); }
      jump(t.start_pc);
    }

    void _return(Pos pos) {
      auto &calls(task->calls);
      if (!calls.size) { throw RuntimeError(*this, pos, "Nothing to return from"); }
      auto &c(calls.back());
      auto &t(c.get_target());
      if (t.parent_scope) { end_scope(); }
      task->pc = c.return_pc;
      auto fi(dynamic_cast<const Fimp *>(&t));
      if (fi && !fi->imp) { end_split(); }
      end_call();
    }
    
    void begin_try(ops::Try &op) { task->tries.push_back(&op); }
    ops::Try *current_try() { return task->tries.back(); }
    void end_try() { task->tries.pop_back(); }

    void push(const Box &val) { task->stack.push_back(val); }

    template <typename ValT, typename...ArgsT>
    void push(Type<ValT> &type, ArgsT &&...args) {
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

    template <typename...ArgsT>
    void note(Pos pos, const string &msg, ArgsT &&...args) {
      cerr << fmt("Note in row %0, col %1: ", {pos.row, pos.col})
           << fmt(msg, {args...}) << endl;
    }

    template <typename...ArgsT>
    void warn(Pos pos, const string &msg, ArgsT &&...args) {
      cerr << fmt("Warning in row %0, col %1: ", {pos.row, pos.col})
           << fmt(msg, {args...}) << endl;
    }        
  };

  struct UserError: RuntimeError {
    const Box val;
    UserError(Env &env, Pos pos, const Box &_val);
  };

  inline bool Box::isa(const AType &rhs) const {
    auto &lhs((type == &type->lib.env.meta_type) ? *as<AType *>() : *type);
    return lhs.isa(rhs);
  }

  template <typename ValT, typename...ArgsT>
  const MacroPtr &Lib::add_macro(Sym id,
                                 Type<ValT> &type,
                                 ArgsT &&...args) {
    return add_macro(id, [&type, args...](Forms::const_iterator &in,
                                         Forms::const_iterator end,
                                         Env &env) {
                       env.emit<ops::Push>((in++)->pos, type, args...);     
                     });
  }

  template <typename OpT, typename...ArgsT>
  const MacroPtr &Lib::add_macro(Sym id, ArgsT &&...args) {
    return add_macro(id, [args...](Forms::const_iterator &in,
                                   Forms::const_iterator end,
                                   Env &env) {
                       env.emit<OpT>((in++)->pos, args...);
                     });
  }
}

#endif
