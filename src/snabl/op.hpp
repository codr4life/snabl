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
	struct OpType: public AOpType {
		OpType(const string &id): AOpType(id) { }		
		void dump(const Op &op, ostream &out) const override;
		virtual void dump_data(const DataT &op, ostream &out) const { }
	};
		
	struct Op {
	private:		
		any _data;
	public:
		const AOpType &type;
		const Pos pos;
		const OpImp imp;
		PC next;

		Op(const Op &src):
			_data(src._data), type(src.type), pos(src.pos), imp(src.imp), next(nullptr) { }
		
		template <typename DataT, typename... ArgsT>
		Op(Env &env, const OpType<DataT> &type, Pos pos, ArgsT &&... args):
			_data(DataT(forward<ArgsT>(args)...)),
			type(type),
			pos(pos),
			imp(type.make_imp(env, *this)),
			next(nullptr) { }

		template <typename DataT>
		const DataT &as() const { return any_cast<const DataT &>(_data); }

		template <typename DataT>
		DataT &as() { return any_cast<DataT &>(_data); }

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
		struct Call {				
			struct Type: public OpType<Call> {
				Type(const string &id): OpType<Call>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct DDrop {
			struct Type: public OpType<DDrop> {
				Type(const string &id): OpType<DDrop>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Drop {
			struct Type: public OpType<Drop> {
				Type(const string &id): OpType<Drop>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Dup {
			struct Type: public OpType<Dup> {
				Type(const string &id): OpType<Dup>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Else {
			struct Type: public OpType<Else> {
				Type(const string &id): OpType<Else>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			Int skip_pc;
			
			Else(Int skip_pc=-1): skip_pc(skip_pc) { }
		};
		
		struct Eqval {
			struct Type: public OpType<Eqval> {
				Type(const string &id): OpType<Eqval>(id) { }
				void dump_data(const Eqval &op, ostream &out) const override;
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const optional<const Box> rhs;
			Eqval(const optional<const Box> rhs=nullopt): rhs(rhs) { }
		};

		struct Fimp {
			struct Type: public OpType<Fimp> {
				Type(const string &id): OpType<Fimp>(id) { }
				void dump_data(const Fimp &op, ostream &out) const override;
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const FimpPtr ptr;
			
			Fimp(const FimpPtr &ptr): ptr(ptr) { }
		};

		struct Funcall {
			struct Type: public OpType<Funcall> {
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
			struct Type: public OpType<Get> {
				Type(const string &id): OpType<Get>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const Sym id;
			Get(Sym id): id(id) { }
		};

		struct Isa {
			struct Type: public OpType<Isa> {
				Type(const string &id): OpType<Isa>(id) { }
				void dump_data(const Isa &op, ostream &out) const override;
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const ATypePtr rhs;
			Isa(const ATypePtr &rhs): rhs(rhs) { }
		};

		struct Jump {
			struct Type: public OpType<Jump> {
				Type(const string &id): OpType<Jump>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			Int end_pc;

			Jump(Int end_pc=-1): end_pc(end_pc) { }
		};

		struct JumpIf {
			struct Type: public OpType<JumpIf> {
				Type(const string &id): OpType<JumpIf>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			function<bool ()> cond;
			Int end_pc;
			
			JumpIf(function<bool ()> &&cond): cond(move(cond)), end_pc(-1) { } 
		};


		struct Lambda {
			struct Type: public OpType<Lambda> {
				Type(const string &id): OpType<Lambda>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			PC start_pc;
			Int end_pc;
			Target::Opts opts;
			
			Lambda():
				start_pc(nullptr), end_pc(-1), opts(Target::Opts::None) { }
		};

		struct Let {
			struct Type: public OpType<Let> {
				Type(const string &id): OpType<Let>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const Sym id;
			Let(Sym id): id(id) { }
		};

		struct Nop {
			struct Type: public OpType<Nop> {
				Type(const string &id): OpType<Nop>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Push {
			struct Type: public OpType<Push> {
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
			struct Type: public OpType<Recall> {
				Type(const string &id): OpType<Recall>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Return {
			struct Type: public OpType<Return> {
				Type(const string &id): OpType<Return>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Rot {
			struct Type: public OpType<Rot> {
				Type(const string &id): OpType<Rot>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct RSwap {
			struct Type: public OpType<RSwap> {
				Type(const string &id): OpType<RSwap>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct SDrop {
			struct Type: public OpType<SDrop> {
				Type(const string &id): OpType<SDrop>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Split {
			struct Type: public OpType<Split> {
				Type(const string &id): OpType<Split>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct SplitEnd {
			struct Type: public OpType<SplitEnd> {
				Type(const string &id): OpType<SplitEnd>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Stack {
			struct Type: public OpType<Stack> {
				Type(const string &id): OpType<Stack>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const bool end_split;
			Stack(bool end_split): end_split(end_split) { }
		};

		struct Swap {
			struct Type: public OpType<Swap> {
				Type(const string &id): OpType<Swap>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
		};

		struct Times {
			struct Type: public OpType<Times> {
				Type(const string &id): OpType<Times>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const Int i_reg;
			
			Times(Int i_reg): i_reg(i_reg) { }
		};

		struct Try {
			struct Type: public OpType<Try> {
				Type(const string &id): OpType<Try>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const Int state_reg;
			Int handler_pc;

			Try(Int state_reg): state_reg(state_reg), handler_pc(-1) { }
		};

		struct TryEnd {
			struct Type: public OpType<TryEnd> {
				Type(const string &id): OpType<TryEnd>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};
			
			static const Type type;
			const Int state_reg;
			TryEnd(Int state_reg): state_reg(state_reg) { }
		};
	}	
}

#endif
