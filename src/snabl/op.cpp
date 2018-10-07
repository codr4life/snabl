#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"
#include "snabl/timer.hpp"

namespace snabl {
  namespace ops {
    const OpType Bench::type("bench"), Call::type("call"), DDrop::type("ddrop"),
      Drop::type("drop"), Dup::type("dup"), Else::type("else"), Eqval::type("eqval"),
      Fimp::type("fimp"), Funcall::type("funcall"), Get::type("get"),
      Isa::type("isa"), Jump::type("jump"), JumpIf::type("jump-if"),
      Lambda::type("lambda"), Let::type("let"), Nop::type("nop"), Push::type("push"),
      Recall::type("recall"), Return::type("return"), Rot::type("rot"),
      RSwap::type("rswap"), Scope::type("scope"), ScopeEnd::type("scope-end"),
      SDrop::type("sdrop"), Split::type("split"), SplitEnd::type("split-end"),
      Stack::type("stack"), Stop::type("stop"), Swap::type("swap"),
      Sync::type("sync"), Task::type("task"), Throw::type("throw"),
      Times::type("times"), Try::type("try"), TryEnd::type("try-end"),
      Yield::type("yield");

    Bench::Bench(Env &env, Pos pos): Op(type, pos, make_imp(env)), end_pc(-1) { }

    OpImp Bench::make_imp(Env &env) {
      return [this, &env]() {
        const Int reps(env.pop().as<Int>());
        
        for (int i(0); i < reps; i++) {
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
      };
    }
    
    Call::Call(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Call::make_imp(Env &env) {
      return [this, &env]() {
        env.jump(next);
        env.pop().call(pos);
      };
    }
    
    DDrop::DDrop(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }
    
    OpImp DDrop::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);
        
        if (Int(s.size()) <= t.stack_offs) {
          throw Error("Nothing to ddrop");
        }
        
        s.pop_back();
        s.pop_back();
        env.jump(next);
      };
    }
    
    Drop::Drop(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Drop::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);
        
        if (Int(s.size()) <= t.stack_offs) {
          throw RuntimeError(env, pos,
                             fmt("Nothing to drop: %0/%1", {s.size(), t.stack_offs}));
        }
        
        s.pop_back();
        env.jump(next);
      };
    }
    
    Dup::Dup(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Dup::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);
        if (Int(s.size()) <= t.stack_offs) { throw Error("Nothing to dup"); }
        env.push(s.back());
        env.jump(next);
      };
    }
    
    Else::Else(Env &env, Pos pos, Int skip_pc):
      Op(type, pos, make_imp(env)), skip_pc(skip_pc) { }

    OpImp Else::make_imp(Env &env) {
      return [this, &env]() {
        const auto &v(env.peek());

        if (v.type != env.bool_type) {
          throw RuntimeError(env, pos, fmt("Invalid else cond: %0", {v}));
        }

        if (v.as<bool>()) {
          env.jump(next);
        } else {
          env.jump(skip_pc);
        }

        env.pop();
      };
    }
    
    Eqval::Eqval(Env &env, Pos pos, const optional<const Box> rhs):
      Op(type, pos, make_imp(env)), rhs(rhs) { }

    OpImp Eqval::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        if (Int(s.size()) <= t.stack_offs+(rhs ? 0 : 1)) {
          throw Error("Nothing to eqval");
        }
        
        const auto lhs(env.pop());
        const auto _rhs(rhs ? *rhs : s.back());
        env.push(env.bool_type, lhs.eqval(_rhs));
        env.jump(next);
      };
    }
    
    void Eqval::dump_args(ostream &out) const {
      if (rhs) {
        out << ' ';
        rhs->dump(out);
      }
    }

    Fimp::Fimp(Env &env, Pos pos, const FimpPtr &ptr, bool is_scope):
      Op(type, pos, make_imp(env)), ptr(ptr), is_scope(is_scope) { }

    OpImp Fimp::make_imp(Env &env) {
      return [this, &env]() {
        if (is_scope) { ptr->parent_scope = env.scope(); }
        env.jump(ptr->end_pc);
      };
    }
    
    void Fimp::dump_args(ostream &out) const { out << ' ' << ptr->id; }
    
    Funcall::Funcall(Env &env, Pos pos, Func &func):
      Op(type, pos, make_imp(env)), func(func) { }
    Funcall::Funcall(Env &env, Pos pos, const FimpPtr &fimp):
      Op(type, pos, make_imp(env)), func(fimp->func), fimp(fimp) { }

    OpImp Funcall::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        const FimpPtr *f(nullptr);

        if (Int(s.size()) >= t.stack_offs+func.nargs) {
          if (fimp) { f = &fimp; }
          if (!f && prev_fimp) { f = &prev_fimp; }

          if (f) {
            if (func.nargs &&
                (*f)->score(s.begin()+(s.size()-func.nargs),
                            s.end()) == -1) { f = nullptr; }
          }

          if (!fimp && !f) {
            f = func.get_best_fimp(s.begin()+(s.size()-func.nargs), s.end());
          }
        } 
      
        if (!f) {
          throw RuntimeError(env, pos, fmt("Func not applicable: %0", {func.id}));
        }
      
        if (!fimp) { prev_fimp = *f; }
        env.jump(next);
        snabl::Fimp::call(*f, pos);
      };
    }
    
    void Funcall::dump_args(ostream &out) const {
      out << ' ' << (fimp ? fimp->id : func.id);
      if (prev_fimp) { out << " (" << prev_fimp->id << ')'; }
    }

    Get::Get(Env &env, Pos pos, Sym id): Op(type, pos, make_imp(env)), id(id) { }

    OpImp Get::make_imp(Env &env) {
      return [this, &env]() {
        auto v(env.scope()->get(id));
        if (!v) { throw RuntimeError(env, pos, fmt("Unknown var: %0", {id})); }
        env.push(*v);
        env.jump(next);
      };
    }
      
    Isa::Isa(Env &env, Pos pos, const ATypePtr &rhs):
      Op(type, pos, make_imp(env)), rhs(rhs) { }
    
    OpImp Isa::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        if (Int(s.size()) <= t.stack_offs) {
          throw RuntimeError(env, pos, "Nothing to isa");
        }
          
        const bool ok(s.back().isa(rhs));
        s.pop_back();
        env.push(env.bool_type, ok);
        env.jump(next);
      };
    }
    
    void Isa::dump_args(ostream &out) const { out << ' ' << rhs->id; }

    Jump::Jump(Env &env, Pos pos, Int end_pc):
      Op(type, pos, make_imp(env)), end_pc(end_pc) { }

    OpImp Jump::make_imp(Env &env) {
      return [this, &env]() { env.jump(end_pc); };
    }
    
    JumpIf::JumpIf(Env &env, Pos pos, function<bool ()> &&cond):
      Op(type, pos, make_imp(env)), cond(move(cond)), end_pc(-1) { } 
    
    OpImp JumpIf::make_imp(Env &env) {
      return [this, &env]() {
        if (cond()) {
          env.jump(end_pc);
        } else {
          env.jump(next);
        }
      };
    }
    
    Lambda::Lambda(Env &env, Pos pos, bool is_scope):
      Op(type, pos, make_imp(env)), is_scope(is_scope), end_pc(-1) { }
    
    OpImp Lambda::make_imp(Env &env) {
      return [this, &env]() {
        env.push(env.lambda_type,
                 make_shared<snabl::Lambda>(is_scope ? env.scope() : nullptr,
                                            start_pc, end_pc));
        env.jump(end_pc);
      };
    }
    
    Let::Let(Env &env, Pos pos, Sym id): Op(type, pos, make_imp(env)), id(id) { }

    OpImp Let::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);
        if (Int(s.size()) <= t.stack_offs) { throw Error("Nothing to let"); }
        auto &v(s.back());
        env.scope()->let(id, v);
        s.pop_back();
        env.jump(next);
      };
    }
    
    void Let::dump_args(ostream &out) const { out << ' ' << id; }

    Nop::Nop(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Nop::make_imp(Env &env) {
      return [this, &env]() { env.jump(next); };
    }
    
    Push::Push(Env &env, Pos pos, const Box &val):
      Op(type, pos, make_imp(env)), val(val) { }
    
    OpImp Push::make_imp(Env &env) {
      return [this, &env]() {
        env.push(val);
        env.jump(next);
      };
    }

    void Push::dump_args(ostream &out) const {
      out << ' ';
      val.dump(out);
    }

    Recall::Recall(Env &env, Pos pos): Op(type, pos, make_imp(env)) { } 

    OpImp Recall::make_imp(Env &env) {
      return [this, &env]() { env.recall(pos); };
    }
    
    Return::Return(Env &env, Pos pos): Op(type, pos, make_imp(env)) { } 

    OpImp Return::make_imp(Env &env) {
      return [this, &env]() { env._return(pos); };
    }
    
    Rot::Rot(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Rot::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        if (Int(s.size()) <= t.stack_offs+2) {
          throw RuntimeError(env, pos, "Nothing to rot");
        }
        
        auto i(s.size()-1);
        swap(s[i], s[i-2]);
        swap(s[i], s[i-1]);
        env.jump(next);
      };
    }
    
    RSwap::RSwap(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp RSwap::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        if (Int(s.size()) <= t.stack_offs+2) {
          throw RuntimeError(env, pos, "Nothing to rswap");
        }

        const auto i(s.size()-1);
        swap(s[i], s[i-2]);
        env.jump(next);
      };
    }
    
    Scope::Scope(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Scope::make_imp(Env &env) {
      return [this, &env]() {
        env.begin_scope(env.scope());
        env.jump(next);
      };
    }
    
    ScopeEnd::ScopeEnd(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp ScopeEnd::make_imp(Env &env) {
      return [this, &env]() {
        env.end_scope();
        env.jump(next);
      };
    }
    
    SDrop::SDrop(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp SDrop::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        if (Int(s.size()) <= t.stack_offs+1) {
          throw RuntimeError(env, pos, "Nothing to sdrop");
        }
        
        const auto i(s.size()-1);
        s[i-1] = s[i];  
        s.pop_back();
        env.jump(next);
      };
    }
    
    Split::Split(Env &env, Pos pos, Int offs):
      Op(type, pos, make_imp(env)), offs(offs) { }

    OpImp Split::make_imp(Env &env) {
      return [this, &env]() {
        env.begin_split(offs);
        env.jump(next);
      };
    }
    
    SplitEnd::SplitEnd(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp SplitEnd::make_imp(Env &env) {
      return [this, &env]() {
        env.end_split();
        env.jump(next);
      };
    }
    
    Stack::Stack(Env &env, Pos pos, bool end_split):
      Op(type, pos, make_imp(env)), end_split(end_split) { }

    OpImp Stack::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        const Int offs(t.stack_offs);
        if (end_split) { env.end_split(); }
        auto ss(make_shared<snabl::Stack>());
        
        if (Int(s.size()) > offs) {
          const auto i(s.begin()+offs), j(s.end());
          move(i, j, back_inserter(*ss));
          s.erase(i, j);
        }
        
        env.push(env.stack_type, ss);
        env.jump(next);
      };
    }
    
    Stop::Stop(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Stop::make_imp(Env &env) {
      return [this, &env]() { env.jump(nullptr); };
    }
    
    Swap::Swap(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Swap::make_imp(Env &env) {
      return [this, &env]() {
        auto &t(*env.task);
        auto &s(t.stack);

        if (Int(s.size()) <= t.stack_offs+1) {
          throw RuntimeError(env, pos, "Nothing to swap");
        }
        
        const auto i(s.size()-1);
        swap(s[i], s[i-1]);
        env.jump(next);
      };
    }
    
    Sync::Sync(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Sync::make_imp(Env &env) {
      return [this, &env]() {
        auto &v(env.peek());

        if (v.type != env.async_type) {
          throw RuntimeError(env, pos, fmt("Expected Async, was:", {v.type->id}));
        }

        auto a(v.as<AsyncPtr>());
        if (!a->valid() && !env.yield()) { a->wait(); }
        
        env.pop();
        auto r(a->get());
        if (r) { env.push(*r); }
        env.jump(next);
      };
    }
    
    Task::Task(Env &env, Pos pos, Int start_pc):
      Op(type, pos, make_imp(env)), start_pc(start_pc), end_pc(-1) { }

    OpImp Task::make_imp(Env &env) {
      return [this, &env]() {
        auto start(&env.ops[start_pc]->imp);
        env.push(env.task_type, env.start_task(start, env.task->scope));
        env.jump(end_pc);
      };
    }
    
    Throw::Throw(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Throw::make_imp(Env &env) {
      return [this, &env]() {
        auto &v(env.peek());
        auto e((v.type == env.error_type)
               ? *v.as<ErrorPtr>()
               : UserError(env, pos, v));
        env.task->stack.pop_back();
        throw e;
      };
    }
    
    Times::Times(Env &env, Pos pos, Int i_reg):
      Op(type, pos, make_imp(env)), i_reg(i_reg) { }

    OpImp Times::make_imp(Env &env) {
      return [this, &env]() {
        env.let_reg(i_reg, env.pop().as<Int>());
        env.jump(next);
      };
    }
    
    Try::Try(Env &env, Pos pos, Int state_reg):
      Op(type, pos, make_imp(env)), state_reg(state_reg), end_pc(-1) { }

    OpImp Try::make_imp(Env &env) {
      return [this, &env]() {
        env.let_reg(state_reg, State(env));
        env.begin_try(*this);
        env.jump(next);
      };
    }
    
    TryEnd::TryEnd(Env &env, Pos pos, Int state_reg):
      Op(type, pos, make_imp(env)), state_reg(state_reg) { }

    OpImp TryEnd::make_imp(Env &env) {
      return [this, &env]() {
        env.clear_reg(state_reg);
        env.end_try();
        env.jump(next);
      };
    }
    
    Yield::Yield(Env &env, Pos pos): Op(type, pos, make_imp(env)) { }

    OpImp Yield::make_imp(Env &env) {
      return [this, &env]() {
        env.jump(next);
        env.yield();
      };
    }
  }
}
