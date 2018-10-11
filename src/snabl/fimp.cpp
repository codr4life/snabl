#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/lib.hpp"

namespace snabl {
  Sym Fimp::get_id(const Func &func, const Args &args) {
    stringstream buf;
    buf << func.id.name() << '<';
    char sep = 0;

    for (auto &a: args) {
      if (sep) {
        buf << sep;
      } else {
        sep = ' ';
      }

      if (a.is_valid) {
        a.write(buf);
      } else {
        buf << a.type->id.name();
      }
    }

    buf << '>';
    return func.lib.env.sym(buf.str());
  }

  Fimp::Fimp(Func &func, const Args &args, Imp imp):
    Def(get_id(func, args)), func(func), args(args), imp(imp) { }

  Fimp::Fimp(Func &func, const Args &args, const Form &form):
    Def(get_id(func, args)), func(func), args(args), form(form) { }

  I64 Fimp::score(Stack::const_iterator begin, Stack::const_iterator end) const {
    auto &env(func.lib.env);
    auto i(begin), j(args.begin());
    I64 score(0);

    for (; j != args.end(); i++, j++) {
      if (i == end) { return -1; }
      const Box &iv(*i), &jv(*j);
      const AType *it(iv.type), &jt(*jv.type);
      if (it == &env.no_type) { continue; }

      if (jv.is_valid) {
        if (!iv.is_valid || !iv.eqval(jv)) { return -1; }
      } else if (!it->isa(jt)) {
        return -1;
      }
      
      score += abs(static_cast<I64>(it->tag-jt.tag));
    }

    return score;
  }

  bool Fimp::compile(Pos pos) {
    if (start_pc) { return false; }
    auto &env(func.lib.env);
    bool is_scope(&form->type == &forms::Scope::type);
    auto &start_op(env.emit<ops::Fimp>(pos, *this, is_scope));      

    if (is_scope) {
      env.begin_regs();
      auto &b(form->as<forms::Scope>().body);
      env.compile(b);
      env.end_regs();
    } else {
      env.compile(*form);
    }
    
    env.emit<ops::Return>(pos);
    start_pc = start_op.next;
    end_pc = env.ops.size();
    return true;
  }

  void Fimp::call(Pos pos) {
    auto &env(func.lib.env);
    
    if (imp) {
      env.begin_call(*this, pos, env.pc());
      imp(*this);
      env.end_call();
    } else {
      compile(pos);
      if (parent_scope) { env.begin_scope(parent_scope); }
      env.begin_split(func.nargs);    
      env.begin_call(*this, pos, env.pc());
      env.jump(start_pc);
    }
  }
}
