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

namespace snabl {
	struct Op;
	
	using Ops = deque<Op>;
	using OpImp = function<void (Ops::const_iterator end_pc)>;
	
	struct AOpType {
		const string id;
		const size_t label_offs;
		AOpType(const string &id): id(id), label_offs(next_label_offs++) { }
		AOpType(const AOpType &) = delete;
		const AOpType &operator=(const AOpType &) = delete;
		virtual OpImp make_imp(Env &env, Op &op) const;
		virtual void dump(const Op &op, ostream &out) const { }
	private:
		static size_t next_label_offs;
	};

	template <typename DataT>
	struct OpType: public AOpType {
		OpType(const string &id): AOpType(id) { }		
		void dump(const Op &op, ostream &out) const override;
		virtual void dump_data(const DataT &op, ostream &out) const { }
	};
	
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
			optional<size_t> nops;
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

		struct FimpEnd {
			struct Type: public OpType<FimpEnd> {
				Type(const string &id): OpType<FimpEnd>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const bool end_scope;
			FimpEnd(bool end_scope): end_scope(end_scope) { }
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

		struct Lambda {
			struct Type: public OpType<Lambda> {
				Type(const string &id): OpType<Lambda>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			optional<size_t> start_pc, nops;
			Target::Opts opts;
			Lambda(): opts(Target::Opts::None) { }
		};

		struct LambdaEnd {
			struct Type: public OpType<LambdaEnd> {
				Type(const string &id): OpType<LambdaEnd>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
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
			static const OpType<Nop> type;
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

		struct Skip {
			struct Type: public OpType<Skip> {
				Type(const string &id): OpType<Skip>(id) { }

				void dump_data(const Skip &op, ostream &out) const override {
					out << ' ' << *op.nops;
				}

				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			optional<size_t> nops;

			Skip(optional<size_t> nops=nullopt): nops(nops) { }
		};

		struct Split {
			static const OpType<Split> type;
		};

		struct SplitEnd {
			static const OpType<SplitEnd> type;
		};

		struct Stack {
			static const OpType<Stack> type;
			const bool unsplit;
			Stack(bool unsplit): unsplit(unsplit) { }
		};

		struct Swap {
			static const OpType<Swap> type;
		};

		struct Try {
			struct Type: public OpType<Try> {
				Type(const string &id): OpType<Try>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			optional<size_t> handler_pc;
			optional<State> state;
		};

		struct TryEnd {
			struct Type: public OpType<TryEnd> {
				Type(const string &id): OpType<TryEnd>(id) { }
				OpImp make_imp(Env &env, Op &op) const override;
			};
			
			static const Type type;
		};
	}
	
	struct Op {
	private:		
		variant<ops::Call, ops::DDrop, ops::Drop, ops::Dup, ops::Else, ops::Eqval,
					  ops::Fimp, ops::FimpEnd, ops::Funcall, ops::Get, ops::Isa,
						ops::Lambda, ops::LambdaEnd, ops::Let, ops::Nop, ops::Push, ops::Recall,
						ops::Rot, ops::RSwap, ops::SDrop, ops::Skip, ops::Split, ops::SplitEnd,
						ops::Stack, ops::Swap, ops::Try, ops::TryEnd> _data;
	public:
		const AOpType &type;
		const Pos pos;
		const OpImp imp;
		
		template <typename DataT, typename... ArgsT>
		Op(Env &env, const OpType<DataT> &type, Pos pos, ArgsT &&... args):
			_data(DataT(forward<ArgsT>(args)...)),
			type(type),
			pos(pos),
			imp(type.make_imp(env, *this)) { }

		template <typename DataT>
		const DataT &as() const { return get<DataT>(_data); }

		template <typename DataT>
		DataT &as() { return get<DataT>(_data); }

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
}

#endif
