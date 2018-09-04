#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/pos.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/scope.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	struct AOpType {
		const string id;
		const size_t label_offs;
		AOpType(const string &id): id(id), label_offs(next_label_offs++) { }
		AOpType(const AOpType &) = delete;
	private:
		static size_t next_label_offs;
	};

	template <typename ImpT>
	struct OpType: public AOpType {
		OpType(const string &id);
	};

	template <typename ImpT>
	OpType<ImpT>::OpType(const string &id): AOpType(id) { }

	struct Op;
	
	using Ops = deque<Op>;
	using PC = Ops::iterator;

	namespace ops {
		struct Begin {
			static const OpType<Begin> type;
			const ScopePtr parent;
			Begin(const ScopePtr &parent=nullptr): parent(parent) { }
		};

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
			Else(): nops(0) { }
		};
		
		struct End {
			static const OpType<End> type;
		};

		struct FimpRet {
			static const OpType<FimpRet> type;
			const bool end_scope;
			FimpRet(bool end_scope): end_scope(end_scope) { }
		};

		struct Funcall {
			static const OpType<Funcall> type;
			const FuncPtr func;
			const FimpPtr fimp;
			
			FimpPtr prev_fimp;
			Funcall(const FuncPtr &func);
			Funcall(const FimpPtr &fimp);
		};
		
		struct GetVar {
			static const OpType<GetVar> type;
			const Sym id;
			GetVar(Sym id): id(id) { }
		};

		struct Lambda {
			static const OpType<Lambda> type;
			optional<size_t> start_pc;
			size_t nops;
			Lambda(): nops(0) { }
		};

		struct LambdaRet {
			static const OpType<LambdaRet> type;
		};

		struct Push {
			static const OpType<Push> type;			
			const Box val;
			Push(const Box &val): val(val) { }
		};

		struct PutVar {
			static const OpType<PutVar> type;
			const Sym id;
			PutVar(Sym id): id(id) { }
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
			static const OpType<Skip> type;
			size_t nops;			
			Skip(): nops(0) { }
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
	private:
		variant<ops::Begin, ops::Call, ops::DDrop, ops::Drop, ops::Dup, ops::Else,
						ops::End, ops::FimpRet, ops::Funcall, ops::GetVar, ops::Lambda,
						ops::LambdaRet, ops::Push, ops::PutVar, ops::Recall, ops::Rot, ops::RSwap,
						ops::SDrop, ops::Skip, ops::Swap> _imp;
	};
	
	template <typename ImpT, typename... ArgsT>
	Op::Op(const OpType<ImpT> &type, Pos pos, ArgsT &&... args):
		type(type), pos(pos), _imp(ImpT(forward<ArgsT>(args)...)) { }

	template <typename ImpT>
	const ImpT &Op::as() const { return get<ImpT>(_imp); }

	template <typename ImpT>
	ImpT &Op::as() { return get<ImpT>(_imp); }
}

#endif
