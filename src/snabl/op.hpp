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
		virtual void dump(const DataT &op, ostream &out) const { }
	};
	
	namespace ops {
		struct Call {				
			static const OpType<Call> type;
		};

		struct DDrop {
			static const OpType<DDrop> type;
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
				void dump(const Eqval &op, ostream &out) const override;
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const optional<const Box> rhs;
			Eqval(const optional<const Box> rhs=nullopt): rhs(rhs) { }
		};

		struct Fimp {
			struct Type: public OpType<Fimp> {
				Type(const string &id): OpType<Fimp>(id) { }
				void dump(const Fimp &op, ostream &out) const override;
				OpImp make_imp(Env &env, Op &op) const override;
			};

			static const Type type;
			const FimpPtr ptr;
			
			Fimp(const FimpPtr &ptr): ptr(ptr) { }
		};

		struct FimpEnd {
			static const OpType<FimpEnd> type;
			const bool end_scope;
			FimpEnd(bool end_scope): end_scope(end_scope) { }
		};

		struct Funcall {
			struct Type: public OpType<Funcall> {
				Type(const string &id): OpType<Funcall>(id) { }
				void dump(const Funcall &op, ostream &out) const override;
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
			static const OpType<Get> type;
			const Sym id;
			Get(Sym id): id(id) { }
		};

		struct Isa {
			struct Type: public OpType<Isa> {
				Type(const string &id): OpType<Isa>(id) { }
				void dump(const Isa &op, ostream &out) const override;
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
			static const OpType<Let> type;
			const Sym id;
			Let(Sym id): id(id) { }
		};

		struct Nop {
			static const OpType<Nop> type;
		};

		struct Push {
			struct Type: public OpType<Push> {
				Type(const string &id): OpType<Push>(id) { }
				void dump(const Push &op, ostream &out) const override;
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
			static const OpType<Recall> type;
		};

		struct Rot {
			static const OpType<Rot> type;
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

				void dump(const Skip &op, ostream &out) const override {
					out << ' ' << *op.nops;
				}
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
		const AOpType &type;
		const Pos pos;
		const OpImp imp;
		
		template <typename DataT, typename... ArgsT>
		Op(Env &env, const OpType<DataT> &type, Pos pos, ArgsT &&... args):
			type(type),
			pos(pos),
			imp(type.make_imp(env, *this)),
			_data(DataT(forward<ArgsT>(args)...)) { }

		virtual ~Op() { }

		template <typename DataT>
		const DataT &as() const { return get<DataT>(_data); }

		template <typename DataT>
		DataT &as() { return get<DataT>(_data); }

		void dump(ostream &out) const {
			out << type.id;
			type.dump(*this, out);
			out << endl;
		}
	private:		
		variant<ops::Call, ops::DDrop, ops::Drop, ops::Dup, ops::Else, ops::Eqval,
					  ops::Fimp, ops::FimpEnd, ops::Funcall, ops::Get, ops::Isa,
						ops::Lambda, ops::LambdaEnd, ops::Let, ops::Nop, ops::Push, ops::Recall,
						ops::Rot, ops::RSwap, ops::SDrop, ops::Skip, ops::Split, ops::SplitEnd,
						ops::Stack, ops::Swap, ops::Try, ops::TryEnd> _data;
	};
	
	template <typename DataT>
	void OpType<DataT>::dump(const Op &op, ostream &out) const {
		dump(op.as<DataT>(), out);
	}
}

#endif
