#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/scope.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	struct Op;

	struct AOpType {
		const string id;
		const size_t label_offs;
		AOpType(const string &id): id(id), label_offs(next_label_offs++) { }
		AOpType(const AOpType &) = delete;
		const AOpType &operator=(const AOpType &) = delete;

		virtual void dump(const Op &op, ostream &out) const { }
	private:
		static size_t next_label_offs;
	};

	template <typename ImpT>
	struct OpType: public AOpType {
		OpType(const string &id): AOpType(id) { }
		void dump(const Op &op, ostream &out) const override;
		virtual void dump(const ImpT &op, ostream &out) const { }
	};
	
	using Ops = deque<Op>;
	using PC = Ops::iterator;

	namespace ops {
		struct Call {				
			static const OpType<Call> type;
		};

		struct DDrop {
			static const OpType<DDrop> type;
		};

		struct Drop {
			static const OpType<Drop> type;
		};

		struct Dup {
			static const OpType<Dup> type;
		};

		struct Else {
			static const OpType<Else> type;
			size_t nops;
			Else(size_t nops=0): nops(nops) { }
		};
		
		struct Eqval {
			struct Type: public OpType<Eqval> {
				Type(const string &id): OpType<Eqval>(id) { }
				void dump(const Eqval &op, ostream &out) const override;
			};

			static const Type type;
			const Box lhs;
			Eqval(const Box &lhs): lhs(lhs) { }
		};

		struct Fimp {
			struct Type: public OpType<Fimp> {
				Type(const string &id): OpType<Fimp>(id) { }
				void dump(const Fimp &op, ostream &out) const override;
			};

			static const Type type;
			const FimpPtr ptr;
			
			Fimp(const FimpPtr &ptr): ptr(ptr) { }
		};

		struct FimpRet {
			static const OpType<FimpRet> type;
			const bool end_scope;
			FimpRet(bool end_scope): end_scope(end_scope) { }
		};

		struct Funcall {
			struct Type: public OpType<Funcall> {
				Type(const string &id): OpType<Funcall>(id) { }
				void dump(const Funcall &op, ostream &out) const override;
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

		struct Lambda {
			static const OpType<Lambda> type;
			optional<size_t> start_pc;
			size_t nops;
			bool has_vars;
			
			Lambda(): nops(0), has_vars(false) { }
		};

		struct LambdaRet {
			static const OpType<LambdaRet> type;
		};

		struct Let {
			static const OpType<Let> type;
			const Sym id;
			Let(Sym id): id(id) { }
		};

		struct Push {
			struct Type: public OpType<Push> {
				Type(const string &id): OpType<Push>(id) { }
				void dump(const Push &op, ostream &out) const override;
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
			static const OpType<RSwap> type;
		};

		struct SDrop {
			static const OpType<SDrop> type;
		};

		struct Skip {
			struct Type: public OpType<Skip> {
				Type(const string &id): OpType<Skip>(id) { }

				void dump(const Skip &op, ostream &out) const override {
					out << ' ' << op.nops;
				}
			};

			static const Type type;
			size_t nops;			

			Skip(size_t nops=0): nops(nops) { }
		};

		struct Swap {
			static const OpType<Swap> type;
		};
	}
	
	struct Op {
		const AOpType &type;
		const Pos pos;
		
		template <typename ImpT, typename... ArgsT>
		Op(const OpType<ImpT> &type, Pos pos, ArgsT &&... args);

		virtual ~Op() { }

		template <typename ImpT>
		const ImpT &as() const;

		template <typename ImpT>
		ImpT &as();

		void dump(ostream &out) const {
			out << type.id;
			type.dump(*this, out);
			out << endl;
		}
	private:
		variant<ops::Call, ops::DDrop, ops::Drop, ops::Dup, ops::Else, ops::Eqval,
					  ops::Fimp, ops::FimpRet, ops::Funcall, ops::Get, ops::Lambda,
						ops::LambdaRet, ops::Let, ops::Push, ops::Recall, ops::Rot, ops::RSwap,
						ops::SDrop, ops::Skip, ops::Swap> _imp;
	};
	
	template <typename ImpT, typename... ArgsT>
	Op::Op(const OpType<ImpT> &type, Pos pos, ArgsT &&... args):
		type(type), pos(pos), _imp(ImpT(forward<ArgsT>(args)...)) { }

	template <typename ImpT>
	const ImpT &Op::as() const { return get<ImpT>(_imp); }

	template <typename ImpT>
	ImpT &Op::as() { return get<ImpT>(_imp); }

	template <typename ImpT>
	void OpType<ImpT>::dump(const Op &op, ostream &out) const {
		dump(op.as<ImpT>(), out);
	}
}

#endif
