#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include "snabl/def.hpp"
#include "snabl/form.hpp"
#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/stack.hpp"
#include "snabl/target.hpp"

namespace snabl {
  struct Fimp: Def, Target {
    using Args = vector<Box>;
    using Imp = function<void (Fimp &)>;
    
    static Sym get_id(const Func &func, const Args &args);

    Func &func;
    const Args args;
    const optional<Form> form;
    const Imp imp;

    Fimp(const Fimp &)=delete;
    const Fimp &operator =(const Fimp &)=delete;
    
    Fimp(Func &func, const Args &args, Imp imp);
    Fimp(Func &func, const Args &args, const Form &form);
    string target_id() const override { return id.name(); }   
    Int score(Stack::const_iterator begin, Stack::const_iterator end) const;
    bool compile(Pos pos);
    void call(Pos pos);
  };
}

#endif
