#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"
#include "snabl/timer.hpp"

namespace snabl {
  Op::Op(Env &env, const string &type, Pos pos):
    type(env.sym(type)), pos(pos), next(nullptr) { }

  void Op::dump(ostream &out) const {
    out << type;
    dump_args(out);
    out << endl;
  }

  namespace ops {
    const string Async::type("async"), Await::type("await"), Bench::type("bench"),
      Call::type("call"), DDrop::type("ddrop"),
      Drop::type("drop"), Dup::type("dup"), Else::type("else"), Eqval::type("eqval"),
      Fimp::type("fimp"), Funcall::type("funcall"), Get::type("get"),
      If::type("if"), Is::type("is"), Jump::type("jump"), JumpIf::type("jump-if"),
      Lambda::type("lambda"), Let::type("let"), Nop::type("nop"), Push::type("push"),
      Recall::type("recall"), Return::type("return"), Rot::type("rot"),
      RSwap::type("rswap"), Scope::type("scope"), ScopeEnd::type("scope-end"),
      SDrop::type("sdrop"), Split::type("split"), SplitEnd::type("split-end"),
      Stack::type("stack"), Stop::type("stop"), Swap::type("swap"),
      Task::type("task"), Throw::type("throw"),
      Times::type("times"), Try::type("try"), TryEnd::type("try-end"),
      Yield::type("yield");

    Async::Async(Env &env, Pos pos, I64 delta): Op(env, type, pos), delta(delta) { }

    void Async::run(Env &env) {
      env.task->async_depth += delta;
      env.jump(next);
    }

    Await::Await(Env &env, Pos pos): Op(env, type, pos) { }

    void Await::run(Env &env) {
      auto &v(env.peek());

      if (v.type != &env.async_type) {
        throw RuntimeError(env, fmt("Expected Async, was:", {v.type->id}));
      }

      auto a(v.as<AsyncPtr>());
      if (!a->valid() && !env.yield()) { a->wait(); }
        
      env.pop();
      auto r(a->get());
      if (r) { env.push(*r); }
      env.jump(next);
    }

    Bench::Bench(Env &env, Pos pos): Op(env, type, pos), end_pc(-1) { }

    void Bench::run(Env &env) {
      const I64 reps(env.pop().as_i64);
        
      for (I64 i(0); i < reps; i++) {
        env.jump(next);
        env.run();
      }
        
      Timer t;
        
      for (int i(0); i < reps; i++) {
        env.jump(next);
        env.run();
      }
        
      env.push(env.time_type, t.ns());
      env.jump(end_pc);
    }
    
    Call::Call(Env &env, Pos pos): Op(env, type, pos) { }

    void Call::run(Env &env) {
      env.jump(next);
      env.pop().call(pos);
    }
    
    DDrop::DDrop(Env &env, Pos pos): Op(env, type, pos) { }
    
    void DDrop::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);
        
      if (I64(s.size()) <= t.stack_offs) {
        throw Error("Nothing to ddrop");
      }
        
      s.pop_back();
      s.pop_back();
      env.jump(next);
    }
    
    Drop::Drop(Env &env, Pos pos): Op(env, type, pos) { }

    void Drop::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);
        
      if (I64(s.size()) <= t.stack_offs) {
        throw RuntimeError(env,
                           fmt("Nothing to drop: %0/%1", {s.size(), t.stack_offs}));
      }
        
      s.pop_back();
      env.jump(next);
    }
    
    Dup::Dup(Env &env, Pos pos): Op(env, type, pos) { }

    void Dup::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);
      if (I64(s.size()) <= t.stack_offs) { throw Error("Nothing to dup"); }
      env.push(s.back());
      env.jump(next);
    }
    
    Else::Else(Env &env, Pos pos, I64 skip_pc):
      Op(env, type, pos), skip_pc(skip_pc) { }

    void Else::run(Env &env) {
      if (env.peek()) {
        env.jump(next);
      } else {
        env.jump(skip_pc);
      }

      env.pop();
    }
    
    Eqval::Eqval(Env &env, Pos pos, const optional<const Box> rhs):
      Op(env, type, pos), rhs(rhs) { }

    void Eqval::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      if (I64(s.size()) <= t.stack_offs+(rhs ? 0 : 1)) {
        throw Error("Nothing to eqval");
      }
        
      const auto lhs(env.pop());
      const auto _rhs(rhs ? *rhs : s.back());
      env.push(env.bool_type, lhs.eqval(_rhs));
      env.jump(next);

    }
    
    void Eqval::dump_args(ostream &out) const {
      if (rhs) {
        out << ' ';
        rhs->dump(out);
      }
    }

    Fimp::Fimp(Env &env, Pos pos, snabl::Fimp &fimp, bool is_scope):
      Op(env, type, pos), fimp(fimp), is_scope(is_scope) { }

    void Fimp::run(Env &env) {
      if (is_scope) { fimp.parent_scope = env.scope(); }
      env.jump(fimp.end_pc);
    }
    
    void Fimp::dump_args(ostream &out) const { out << ' ' << fimp.id; }
    
    Funcall::Funcall(Env &env, Pos pos, Func &func):
      Op(env, type, pos), func(func), fimp(nullptr), prev_fimp(nullptr) { }

    Funcall::Funcall(Env &env, Pos pos, snabl::Fimp &fimp):
      Op(env, type, pos),
      func(fimp.func), fimp(&fimp), prev_fimp(nullptr) { }

    void Funcall::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      snabl::Fimp *f(nullptr);

      if (I64(s.size()) >= t.stack_offs+func.nargs) {
        if (fimp) { f = fimp; }
        if (!f && prev_fimp) { f = prev_fimp; }

        if (f) {
          if (func.nargs &&
              f->score(s.begin()+(s.size()-func.nargs),
                       s.end()) == -1) { f = nullptr; }
        }

        if (!fimp && !f) {
          f = func.get_best_fimp(s.begin()+(s.size()-func.nargs), s.end());
        }
      } 
      
      if (!f) {
        throw RuntimeError(env, fmt("Func not applicable: %0", {func.id}));
      }
      
      if (!fimp) { prev_fimp = f; }
      env.jump(next);
      f->call(pos);
    }
    
    void Funcall::dump_args(ostream &out) const {
      out << ' ' << (fimp ? fimp->id : func.id);
      if (prev_fimp) { out << " (" << prev_fimp->id << ')'; }
    }

    Get::Get(Env &env, Pos pos, Sym id): Op(env, type, pos), id(id) { }

    void Get::run(Env &env) {
      auto v(env.scope()->get(id));
      if (!v) { throw RuntimeError(env, fmt("Unknown var: %0", {id})); }
      env.push(*v);
      env.jump(next);
    }

    If::If(Env &env, Pos pos, I64 skip_pc):
      Op(env, type, pos), skip_pc(skip_pc) { }

    void If::run(Env &env) {
      if (env.peek()) {
        env.jump(skip_pc);
      } else {
        env.jump(next);
      }

      env.pop();
    }
    
    Is::Is(Env &env, Pos pos, const AType &rhs):
      Op(env, type, pos), rhs(rhs) { }
    
    void Is::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      if (I64(s.size()) <= t.stack_offs) {
        throw RuntimeError(env, "Nothing to is");
      }
          
      const bool ok(s.back().is(rhs));
      s.pop_back();
      env.push(env.bool_type, ok);
      env.jump(next);
    }
    
    void Is::dump_args(ostream &out) const { out << ' ' << rhs.id; }

    Jump::Jump(Env &env, Pos pos, I64 end_pc):
      Op(env, type, pos), end_pc(end_pc) { }

    void Jump::run(Env &env) { env.jump(end_pc); }
    
    JumpIf::JumpIf(Env &env, Pos pos, function<bool ()> &&cond):
      Op(env, type, pos), cond(move(cond)), end_pc(-1) { } 
    
    void JumpIf::run(Env &env) {
      if (cond()) {
        env.jump(end_pc);
      } else {
        env.jump(next);
      }
    }
    
    Lambda::Lambda(Env &env, Pos pos, bool is_scope):
      Op(env, type, pos),
      is_scope(is_scope), start_pc(nullptr), end_pc(-1) { }
    
    void Lambda::run(Env &env) {
      env.push(env.lambda_type,
               snabl::Lambda(is_scope ? env.scope() : nullptr, start_pc, end_pc));
      env.jump(end_pc);
    }
    
    Let::Let(Env &env, Pos pos, Sym id): Op(env, type, pos), id(id) { }

    void Let::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);
      if (I64(s.size()) <= t.stack_offs) { throw Error("Nothing to let"); }
      auto &v(s.back());
      env.scope()->let(id, v);
      s.pop_back();
      env.jump(next);
    }
    
    void Let::dump_args(ostream &out) const { out << ' ' << id; }

    Nop::Nop(Env &env, Pos pos): Op(env, type, pos) { }

    void Nop::run(Env &env) { env.jump(next); }
    
    Push::Push(Env &env, Pos pos, const Box &val):
      Op(env, type, pos), val(val) { }
    
    void Push::run(Env &env) {
      env.push(val);
      env.jump(next);
    }

    void Push::dump_args(ostream &out) const {
      out << ' ';
      val.dump(out);
    }

    Recall::Recall(Env &env, Pos pos): Op(env, type, pos) { } 

    void Recall::run(Env &env) { env.recall(pos); }
    
    Return::Return(Env &env, Pos pos): Op(env, type, pos) { } 

    void Return::run(Env &env) { env._return(pos); }
    
    Rot::Rot(Env &env, Pos pos): Op(env, type, pos) { }

    void Rot::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      if (I64(s.size()) <= t.stack_offs+2) {
        throw RuntimeError(env, "Nothing to rot");
      }
        
      auto i(s.size()-1);
      swap(s[i], s[i-2]);
      swap(s[i], s[i-1]);
      env.jump(next);
    }
    
    RSwap::RSwap(Env &env, Pos pos): Op(env, type, pos) { }

    void RSwap::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      if (I64(s.size()) <= t.stack_offs+2) {
        throw RuntimeError(env, "Nothing to rswap");
      }

      const auto i(s.size()-1);
      swap(s[i], s[i-2]);
      env.jump(next);
    }
    
    Scope::Scope(Env &env, Pos pos): Op(env, type, pos) { }

    void Scope::run(Env &env) {
      env.begin_scope(env.scope());
      env.jump(next);
    }
    
    ScopeEnd::ScopeEnd(Env &env, Pos pos): Op(env, type, pos) { }

    void ScopeEnd::run(Env &env) {
      env.end_scope();
      env.jump(next);
    }
    
    SDrop::SDrop(Env &env, Pos pos): Op(env, type, pos) { }

    void SDrop::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      if (I64(s.size()) <= t.stack_offs+1) {
        throw RuntimeError(env, "Nothing to sdrop");
      }
        
      const auto i(s.size()-1);
      s[i-1] = s[i];  
      s.pop_back();
      env.jump(next);
    }
    
    Split::Split(Env &env, Pos pos, I64 offs):
      Op(env, type, pos), offs(offs) { }

    void Split::run(Env &env) {
      env.begin_split(offs);
      env.jump(next);
    }
    
    SplitEnd::SplitEnd(Env &env, Pos pos): Op(env, type, pos) { }

    void SplitEnd::run(Env &env) {
      env.end_split();
      env.jump(next);
    }
    
    Stack::Stack(Env &env, Pos pos, bool end_split):
      Op(env, type, pos), end_split(end_split) { }

    void Stack::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      const I64 offs(t.stack_offs);
      if (end_split) { env.end_split(); }
      auto ss(StackPtr::make(&env.stack_type.pool));
        
      if (I64(s.size()) > offs) {
        const auto i(s.begin()+offs), j(s.end());
        move(i, j, back_inserter(*ss));
        s.erase(i, j);
      }
        
      env.push(env.stack_type, ss);
      env.jump(next);
    }
    
    Stop::Stop(Env &env, Pos pos): Op(env, type, pos) { }

    void Stop::run(Env &env) { env.jump(nullptr); }
    
    Swap::Swap(Env &env, Pos pos): Op(env, type, pos) { }

    void Swap::run(Env &env) {
      auto &t(*env.task);
      auto &s(t.stack);

      if (I64(s.size()) <= t.stack_offs+1) {
        throw RuntimeError(env, "Nothing to swap");
      }
        
      const auto i(s.size()-1);
      swap(s[i], s[i-1]);
      env.jump(next);
    }
        
    Task::Task(Env &env, Pos pos):
      Op(env, type, pos), start_pc(nullptr), end_pc(-1) { }

    void Task::run(Env &env) {
      env.start_task(next, env.task->scope);
      env.jump(end_pc);
    }
    
    Throw::Throw(Env &env, Pos pos): Op(env, type, pos) { }

    void Throw::run(Env &env) {
      auto &v(env.peek());
      auto e((v.type == &env.error_type)
             ? v.as<UserError>()
             : UserError(env, v));
      env.pop();
      throw e;
    }
    
    Times::Times(Env &env, Pos pos, I64 i_reg):
      Op(env, type, pos), i_reg(i_reg) { }

    void Times::run(Env &env) {
      env.let_reg(i_reg, env.pop().as_i64);
      env.jump(next);
    }
    
    Try::Try(Env &env, Pos pos):
      Op(env, type, pos), end_pc(-1) { }

    void Try::run(Env &env) {
      env.begin_try(*this);
      env.jump(next);
    }
    
    TryEnd::TryEnd(Env &env, Pos pos): Op(env, type, pos) { }

    void TryEnd::run(Env &env) {
      env.end_try();
      env.jump(next);
    }
    
    Yield::Yield(Env &env, Pos pos): Op(env, type, pos) { }

    void Yield::run(Env &env) {
      env.jump(next);
      env.yield();
    }
  }
}
