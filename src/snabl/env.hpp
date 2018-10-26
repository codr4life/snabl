#ifndef SNABL_ENV_HPP
#define SNABL_ENV_HPP

#include "snabl/lib.hpp"
#include "snabl/libs/abc.hpp"
#include "snabl/mpool.hpp"
#include "snabl/pos.hpp"
#include "snabl/scope.hpp"
#include "snabl/stack.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/task.hpp"
#include "snabl/types.hpp"
#include "snabl/types/async.hpp"
#include "snabl/types/bin.hpp"
#include "snabl/types/bool.hpp"
#include "snabl/types/byte.hpp"
#include "snabl/types/char.hpp"
#include "snabl/types/enum.hpp"
#include "snabl/types/error.hpp"
#include "snabl/types/file.hpp"
#include "snabl/types/float.hpp"
#include "snabl/types/i64.hpp"
#include "snabl/types/iter.hpp"
#include "snabl/types/lambda.hpp"
#include "snabl/types/meta.hpp"
#include "snabl/types/nil.hpp"
#include "snabl/types/stack.hpp"
#include "snabl/types/str.hpp"
#include "snabl/types/sym.hpp"
#include "snabl/types/time.hpp"
#include "snabl/user_error.hpp"

namespace snabl {
  template <typename ValT>
  struct Type;

  const array<int, 3> version {0, 4, 1};
  
  struct Env {
    list<SymImp> syms;
    unordered_map<string, Sym> sym_table;
    I64 type_tag;
    set<char> separators;
    MPool<Call> call_pool;
    MPool<Task> task_pool;
    MPool<Try> try_pool;
    MPool<Scope> scope_pool;
    Task main_task;
    Task *task;
    unordered_map<Sym, unique_ptr<Lib>> libs;
    Lib &home_lib, &s_lib;
    libs::Abc &abc_lib;

    Trait &no_type, &maybe_type, &root_type, &cmp_type, &num_type, &seq_type,
      &source_type, &sink_type;
    
    Type<AType *> &meta_type;
    Type<Nil> &nil_type;

    AsyncType &async_type;
    BinType &bin_type;
    BoolType &bool_type;
    ByteType &byte_type;
    CharType &char_type;
    ErrorType &error_type;
    FileType &file_type;
    FloatType &float_type;
    I64Type &i64_type;
    IterType &iter_type;
    LambdaType &lambda_type;
    FileType &rfile_type;
    StackType &stack_type;
    StrType &str_type;
    SymType &sym_type;
    TimeType &time_type;

    EnumType &enum_type, &io_type;

    Scope root_scope;

    map<char, Char> special_chars;
    map<Char, char> char_specials;
    vector<I64> nregs;
    Ops ops;

    istream *stdin;
    ostream *stdout, *stderr;
    
    Env():
      type_tag(1),
      separators {
        ' ', '\t', '\n',
        ',', '?', '&', '|',
        '<', '>', '(', ')', '{', '}', '[', ']'
      },
      main_task(*this),
      task(&main_task),
      home_lib(add_lib<Lib>(sym(""))),
      s_lib(add_lib<Lib>(sym("s"))),
      abc_lib(add_lib<libs::Abc>()),
      no_type(abc_lib.add_type<Trait>(sym("No"))),
      maybe_type(abc_lib.add_type<Trait>(sym("Maybe"))),
      root_type(abc_lib.add_type<Trait>(sym("T"), {&maybe_type})),
      cmp_type(abc_lib.add_type<Trait>(sym("Cmp"), {&root_type})),
      num_type(abc_lib.add_type<Trait>(sym("Num"), {&cmp_type})),
      seq_type(abc_lib.add_type<Trait>(sym("Seq"), {&root_type})),
      source_type(abc_lib.add_type<Trait>(sym("Source"), {&root_type})),
      sink_type(abc_lib.add_type<Trait>(sym("Sink"), {&root_type})),

      meta_type(abc_lib.add_type<MetaType>(sym("Type"), {&root_type})),
      nil_type(abc_lib.add_type<NilType>(sym("Nil"), {&maybe_type})),

      async_type(abc_lib.add_type<AsyncType>(sym("Async"), {&root_type})),
      bin_type(abc_lib.add_type<BinType>(sym("Bin"),
        {&seq_type, &sink_type, &source_type})),
      bool_type(abc_lib.add_type<BoolType>(sym("Bool"), {&cmp_type})),
      byte_type(abc_lib.add_type<ByteType>(sym("Byte"), {&num_type})),
      char_type(abc_lib.add_type<CharType>(sym("Char"), {&cmp_type})),
      error_type(abc_lib.add_type<ErrorType>(sym("Error"), {&root_type})),
      file_type(abc_lib.add_type<FileType>(sym("File"), {&root_type})),
      float_type(abc_lib.add_type<FloatType>(sym("Float"), {&num_type})),
      i64_type(abc_lib.add_type<I64Type>(sym("I64"),
        {&num_type, &seq_type})),
      iter_type(abc_lib.add_type<IterType>(sym("Iter"),
        {&seq_type, &source_type})),
      lambda_type(abc_lib.add_type<LambdaType>(sym("Lambda"), {&root_type})),
      rfile_type(abc_lib.add_type<FileType>(sym("RFile"), {&file_type})),
      stack_type(abc_lib.add_type<StackType>(sym("Stack"),
        {&seq_type, &sink_type, &source_type})),
      str_type(abc_lib.add_type<StrType>(sym("Str"),
        {&cmp_type, &seq_type, &sink_type, &source_type})),
      sym_type(abc_lib.add_type<SymType>(sym("Sym"), {&cmp_type})),
      time_type(abc_lib.add_type<TimeType>(sym("Time"), {&cmp_type})),

      enum_type(abc_lib.add_type<EnumType>(sym("Enum"), {&cmp_type})),
      io_type(abc_lib.add_enum_type(sym("IO"), {sym("r"), sym("w"), sym("rw")})),

      root_scope(nullptr, {}),
      stdin(&cin),
      stdout(&cout),
      stderr(&cerr) {
        add_special_char('t', 8);
        add_special_char('n', 10);
        add_special_char('r', 13);
        add_special_char('e', 27);
        add_special_char('s', 32);
        begin_regs();
        task->lib = &home_lib;
        task->scope = &root_scope;
        abc_lib.init();
      }

    template <typename LibT, typename...ArgsT>
    LibT &add_lib(ArgsT &&...args) {
      auto l(new LibT(*this, forward<ArgsT>(args)...));
      libs.emplace(l->qid, l);
      return *l;
    }

    Lib *get_lib(Sym id) const {
      const auto found(libs.find(id));
      return (found == libs.end()) ? nullptr : found->second.get();
    }

    void use(Sym lib_id, const vector<Sym> def_ids={});
    
    Task &start_task(PC start_pc=nullptr, Scope *scope=nullptr) {
      auto t(task_pool.acquire(*this, start_pc, scope));
      t->next = task;
      t->prev = task->prev;
      t->prev->next = t;
      task->prev = t;
      return *t;
    }

    bool yield() {
      if (task->next == task) { return false; }
      task = task->next; 
      return true;
    }

    Env(const Env &) = delete;
    const Env &operator=(const Env &) = delete;

    I64 next_type_tag() { return type_tag++; }

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
    
    I64 end_regs() {
      const I64 n(nregs.back());
      nregs.pop_back();
      return n;
    }

    I64 next_reg(Pos pos) {
      auto &n(nregs.back());

      if (n == Scope::MaxRegs) {
        throw CompileError(pos, "No more regs, consider adding additional scopes");
      }

      return n++;
    }
    
    template <typename T>
    T &get_reg(I64 idx) { return any_cast<T &>(task->scope->regs[idx]); }

    template <typename T>
    const T &get_reg(I64 idx) const {
      return any_cast<const T &>(task->scope->regs[idx]);
    }

    void let_reg(I64 idx, any &&val) { task->scope->regs[idx] = move(val); }
    void clear_reg(I64 idx) const { task->scope->regs[idx].reset(); }
    
    template <typename OpT, typename...ArgsT>
    OpT &emit(ArgsT &&...args) {
      Op *prev(ops.empty() ? nullptr : ops.back().get());
      auto op(new OpT(*this, forward<ArgsT>(args)...));
      ops.emplace_back(op);
      if (prev) { prev->next = op; }
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

    void load(const string &path) {
      ifstream f(path);
      if (f.fail()) { throw Error(fmt("File not found: %0", {path})); }
      compile(f);
    }
    
    Lib &lib() const { return *task->lib; }
    PC pc() const { return task->pc; }
    
    void begin_scope(const vector<Var> &vars) {
      task->scope = scope_pool.acquire(task->scope, vars);
    }

    Scope &scope() const { return *task->scope; }

    void end_scope() {
      auto prev(task->scope->prev);
      scope_pool.release(task->scope);
      task->scope = prev;
    }

    void jump(PC pc) { task->pc = pc; }

    void jump(I64 pc) {
      task->pc = (pc == I64(ops.size())) ? nullptr : ops[pc].get();
    }

    Pos pos() const { return task->pc ? task->pc->pos : Pos(-1, -1); }
    
    template <typename TargetT>
    void begin_call(TargetT &target, Pos pos, PC return_pc=nullptr) {
      task->call = call_pool.acquire(*this, target, pos, return_pc);
    }

    Call *call() const { return task->call; }

    void end_call() {
      auto prev(task->call->prev);
      call_pool.release(task->call);
      task->call = prev;
    }
    
    void recall(Pos pos) {
      if (!task->call) { throw RuntimeError(*this, "Nothing to recall"); }
      const auto &c(*task->call);
      const auto &t(c.get_target());
      const auto &s(c.state);
      s.restore_env(*this);
      s.restore_try(*this);
      if (t.vars) { task->scope->vars.clear(); }
      jump(t.start_pc);
    }

    void _return(Pos pos) {
      if (!task->call) { throw RuntimeError(*this, "Nothing to return from"); }
      auto &c(*task->call);
      auto &t(c.get_target());
      if (t.vars) { end_scope(); }
      task->pc = c.return_pc;
      auto fi(dynamic_cast<const Fimp *>(&t));
      if (fi && !fi->imp) { end_split(); }
      end_call();
    }
    
    void begin_try(ops::Try &op) { task->_try = try_pool.acquire(*this, op); }
    Try *_try() { return task->_try; }

    void end_try() {
      auto prev(task->_try->prev);
      try_pool.release(task->_try);
      task->_try = prev;
    }

    void push(const Box &val) { task->stack.push_back(val); }

    template <typename ValT, typename...ArgsT>
    void push(Type<ValT> &type, ArgsT &&...args) {
      task->stack.emplace_back(type, ValT(forward<ArgsT>(args)...));
    }

    Box &peek() {
      if (I64(task->stack.size()) <= task->stack_offs) {
        throw Error("Nothing to peek");
      }
      
      return task->stack.back();
    }

    Box pop() {
      if (I64(task->stack.size()) <= task->stack_offs) {
        throw Error("Nothing to pop");
      }
      
      Box v(task->stack.back());
      task->stack.pop_back();
      return v;
    }

    const Stack &stack() { return task->stack; }

    void begin_split(I64 offs=0) {
      task->stack_offs = task->stack.size()-offs;
      task->splits.push_back(task->stack_offs);
    }

    void end_split() {
      task->splits.pop_back();
      task->stack_offs = task->splits.empty() ? 0 : task->splits.back();
    }

    const Box *get(Sym id) const {
      const auto found(task->scope->vars.find(id));
      return found ? &found->val : nullptr;
    }
    
    template <typename...ArgsT>
    void let(Sym id, ArgsT &&...args) {
      if (!task->scope->vars.emplace(id, id, forward<ArgsT>(args)...)) {
        throw RuntimeError(*this, "Duplicate var: " + id.name());
      }
    }

    optional<Box> async(const function<optional<Box> ()> &fn);
    void push_async(const function<optional<Box> ()> &fn);
    bool is_async() const { return task->async_depth > 0; }
    void fopen(const string &name, fstream::openmode mode);
  };

  inline bool Box::is(const AType &rhs) const {
    auto &lhs((type == &type->lib.env.meta_type) ? *as<AType *>() : *type);
    return lhs.is(rhs);
  }

  template <typename ValT, typename...ArgsT>
  Macro &Lib::add_macro(Sym id,
                                 Type<ValT> &type,
                                 ArgsT &&...args) {
    return add_macro(id, [&type, args...](Forms::const_iterator &in,
                                         Forms::const_iterator end,
                                         Env &env) {
                       env.emit<ops::Push>((in++)->pos, type, args...);     
                     });
  }

  template <typename OpT, typename...ArgsT>
  Macro &Lib::add_macro(Sym id, ArgsT &&...args) {
    return add_macro(id, [args...](Forms::const_iterator &in,
                                   Forms::const_iterator end,
                                   Env &env) {
                       env.emit<OpT>((in++)->pos, args...);
                     });
  }
}

#endif
