#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/scope.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"
#include "snabl/target.hpp"
#include "snabl/types.hpp"

namespace snabl {
  struct Op;
  
  using Ops = deque<Op>;
  
  struct AOpType {
    const string id;
    AOpType(const string &id): id(id) { }
    AOpType(const AOpType &) = delete;
    const AOpType &operator=(const AOpType &) = delete;
    virtual OpImp make_imp(Env &env, Op &op) const=0;
    virtual void dump(const Op &op, ostream &out) const { }
  };

  template <typename DataT>
  struct OpType: AOpType {
    OpType(const string &id): AOpType(id) { }   
    void dump(const Op &op, ostream &out) const override;
    virtual void dump_data(const DataT &op, ostream &out) const { }
  };
    
  struct Op {
    const AOpType &type;
    const Pos pos;
    any data;
    const OpImp imp;
    OpImp next;

    Op(const Op &src)=delete;
    
    template <typename DataT, typename... ArgsT>
    Op(Env &env, const OpType<DataT> &type, Pos pos, ArgsT &&... args):
      type(type),
      pos(pos),
      data(DataT(forward<ArgsT>(args)...)),
      imp(type.make_imp(env, *this)) { }

    template <typename DataT>
    const DataT &as() const { return any_cast<const DataT &>(data); }

    template <typename DataT>
    DataT &as() { return any_cast<DataT &>(data); }

    void dump(ostream &out) const {
      out << type.id;
      type.dump(*this, out);
      out << endl;
    }
  };
  
  template <typename DataT>
  void OpType<DataT>::dump(const Op &op, ostream &out) const {
    dump_data(op.as<DataT>(), out);
  }

  namespace ops {
    struct Bench {        
      struct Type: OpType<Bench> {
        Type(const string &id): OpType<Bench>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      Int end_pc;
      Bench(): end_pc(-1) { }
    };

    struct Call {       
      struct Type: OpType<Call> {
        Type(const string &id): OpType<Call>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct DDrop {
      struct Type: OpType<DDrop> {
        Type(const string &id): OpType<DDrop>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Drop {
      struct Type: OpType<Drop> {
        Type(const string &id): OpType<Drop>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Dup {
      struct Type: OpType<Dup> {
        Type(const string &id): OpType<Dup>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Else {
      struct Type: OpType<Else> {
        Type(const string &id): OpType<Else>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      Int skip_pc;
      
      Else(Int skip_pc=-1): skip_pc(skip_pc) { }
    };
    
    struct Eqval {
      struct Type: OpType<Eqval> {
        Type(const string &id): OpType<Eqval>(id) { }
        void dump_data(const Eqval &op, ostream &out) const override;
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const optional<const Box> rhs;
      Eqval(const optional<const Box> rhs=nullopt): rhs(rhs) { }
    };

    struct Fimp {
      struct Type: OpType<Fimp> {
        Type(const string &id): OpType<Fimp>(id) { }
        void dump_data(const Fimp &op, ostream &out) const override;
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const FimpPtr ptr;
      const bool is_scope;
      
      Fimp(const FimpPtr &ptr, bool is_scope): ptr(ptr), is_scope(is_scope) { }
    };

    struct Funcall {
      struct Type: OpType<Funcall> {
        Type(const string &id): OpType<Funcall>(id) { }
        void dump_data(const Funcall &op, ostream &out) const override;
        OpImp make_imp(Env &env, Op &op) const override;
      };
      
      static const Type type;
      const FuncPtr func;
      const FimpPtr fimp;
      
      FimpPtr prev_fimp;
      Funcall(const FuncPtr &func);
      Funcall(const FimpPtr &fimp);
    };
    
    struct Get {
      struct Type: OpType<Get> {
        Type(const string &id): OpType<Get>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const Sym id;
      Get(Sym id): id(id) { }
    };

    struct Isa {
      struct Type: OpType<Isa> {
        Type(const string &id): OpType<Isa>(id) { }
        void dump_data(const Isa &op, ostream &out) const override;
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const ATypePtr rhs;
      Isa(const ATypePtr &rhs): rhs(rhs) { }
    };

    struct Jump {
      struct Type: OpType<Jump> {
        Type(const string &id): OpType<Jump>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      Int end_pc;

      Jump(Int end_pc=-1): end_pc(end_pc) { }
    };

    struct JumpIf {
      struct Type: OpType<JumpIf> {
        Type(const string &id): OpType<JumpIf>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      function<bool ()> cond;
      Int end_pc;
      
      JumpIf(function<bool ()> &&cond): cond(move(cond)), end_pc(-1) { } 
    };


    struct Lambda {
      struct Type: OpType<Lambda> {
        Type(const string &id): OpType<Lambda>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const bool is_scope;
      OpImp start_pc;
      Int end_pc;
      
      Lambda(bool is_scope): is_scope(is_scope), end_pc(-1) { }
    };

    struct Let {
      struct Type: OpType<Let> {
        Type(const string &id): OpType<Let>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const Sym id;
      Let(Sym id): id(id) { }
    };

    struct Nop {
      struct Type: OpType<Nop> {
        Type(const string &id): OpType<Nop>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Push {
      struct Type: OpType<Push> {
        Type(const string &id): OpType<Push>(id) { }
        void dump_data(const Push &op, ostream &out) const override;
        OpImp make_imp(Env &env, Op &op) const override;
      };
        
      static const Type type;     
      const Box val;
      Push(const Box &val): val(val) { }
      template <typename ValT, typename... ArgsT>
      Push(const TypePtr<ValT> &type, ArgsT &&...args):
        val(type, forward<ArgsT>(args)...) { }
    };

    struct Recall {
      struct Type: OpType<Recall> {
        Type(const string &id): OpType<Recall>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Return {
      struct Type: OpType<Return> {
        Type(const string &id): OpType<Return>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Rot {
      struct Type: OpType<Rot> {
        Type(const string &id): OpType<Rot>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct RSwap {
      struct Type: OpType<RSwap> {
        Type(const string &id): OpType<RSwap>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Scope {
      struct Type: OpType<Scope> {
        Type(const string &id): OpType<Scope>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      };

    struct ScopeEnd {
      struct Type: OpType<ScopeEnd> {
        Type(const string &id): OpType<ScopeEnd>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct SDrop {
      struct Type: OpType<SDrop> {
        Type(const string &id): OpType<SDrop>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Split {
      struct Type: OpType<Split> {
        Type(const string &id): OpType<Split>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const Int offs;
      Split(Int offs=0): offs(offs) { }
    };

    struct SplitEnd {
      struct Type: OpType<SplitEnd> {
        Type(const string &id): OpType<SplitEnd>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Stack {
      struct Type: OpType<Stack> {
        Type(const string &id): OpType<Stack>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const bool end_split;
      Stack(bool end_split): end_split(end_split) { }
    };

    struct Stop {
      struct Type: OpType<Stop> {
        Type(const string &id): OpType<Stop>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Swap {
      struct Type: OpType<Swap> {
        Type(const string &id): OpType<Swap>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };

    struct Task {
      struct Type: OpType<Task> {
        Type(const string &id): OpType<Task>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const Int start_pc;
      Int end_pc;

      Task(Int start_pc): start_pc(start_pc), end_pc(-1) { }
    };

    struct Throw {
      struct Type: OpType<Throw> {
        Type(const string &id): OpType<Throw>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };
    
    struct Times {
      struct Type: OpType<Times> {
        Type(const string &id): OpType<Times>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const Int i_reg;
      
      Times(Int i_reg): i_reg(i_reg) { }
    };

    struct Try {
      struct Type: OpType<Try> {
        Type(const string &id): OpType<Try>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
      const Int state_reg;
      Int end_pc;

      Try(Int state_reg): state_reg(state_reg), end_pc(-1) { }
    };

    struct TryEnd {
      struct Type: OpType<TryEnd> {
        Type(const string &id): OpType<TryEnd>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };
      
      static const Type type;
      const Int state_reg;
      TryEnd(Int state_reg): state_reg(state_reg) { }
    };

    struct Yield {
      struct Type: OpType<Yield> {
        Type(const string &id): OpType<Yield>(id) { }
        OpImp make_imp(Env &env, Op &op) const override;
      };

      static const Type type;
    };
  } 
}

#endif
