#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/form.hpp"
#include "snabl/pos.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	class AOpType {
	public:
		const string id;
		const size_t label_offs;

		AOpType(const string &id);
	private:
		static size_t next_label_offs;
	};

	template <typename ImpT>
	class OpType: public AOpType {
	public:
		OpType(const string &id);
	};

	template <typename ImpT>
	OpType<ImpT>::OpType(const string &id): AOpType(id) { }

	struct OpImp {
		virtual ~OpImp() { }
		virtual void dump(Env &env, ostream &out) const { };
	};
	
	class Op {
	public:
		const AOpType &type;
		const Pos pos;
		
		template <typename ImpT, typename... ArgsT>
		Op(const OpType<ImpT> &type, Pos pos, ArgsT &&... args);

		virtual ~Op() { }

		template <typename ImpT>
		ImpT &as() const;
		void dump(Env &env, ostream &out) const;
	private:
		unique_ptr<OpImp> _imp;
	};
	
	template <typename ImpT, typename... ArgsT>
	Op::Op(const OpType<ImpT> &type, Pos pos, ArgsT &&... args):
		type(type), pos(pos), _imp(new ImpT(forward<ArgsT>(args)...)) { }

	template <typename ImpT>
	ImpT &Op::as() const { return *static_cast<ImpT *>(_imp.get()); }

	using Ops = deque<Op>;
	using PC = Ops::iterator;

	namespace ops {
		struct Begin: public OpImp {
			static const OpType<Begin> type;
			const ScopePtr parent;
			
			Begin(const ScopePtr &parent=nullptr);
		};

		struct Call: public OpImp {
			static const OpType<Call> type;
			Call();
		};

		struct Drop: public OpImp {
			static const OpType<Drop> type;
			Drop();
		};

		struct Dup: public OpImp {
			static const OpType<Dup> type;
			Dup();
		};

		struct Else: public OpImp {
			static const OpType<Else> type;
			size_t nops;
			Else(size_t nops);
			void dump(Env &env, ostream &out) const override;
		};
		
		struct End: public OpImp {
			static const OpType<End> type;
			End();
		};

		struct FimpRet: public OpImp {
			static const OpType<FimpRet> type;
			const bool end_scope;
			
			FimpRet(bool end_scope);
		};

		struct Funcall: public OpImp {
			static const OpType<Funcall> type;
			const FuncPtr func;
			const FimpPtr fimp;
			FimpPtr prev_fimp;
			Funcall(const FuncPtr &func);
			Funcall(const FimpPtr &fimp);
			void dump(Env &env, ostream &out) const override;
		};
		
		struct GetVar: public OpImp {
			static const OpType<GetVar> type;
			const Sym id;
			GetVar(Sym id);
			void dump(Env &env, ostream &out) const override;
		};

		struct Lambda: public OpImp {
			static const OpType<Lambda> type;
			optional<PC> start_pc;
			size_t nops;
			
			Lambda();
			void dump(Env &env, ostream &out) const override;
		};

		struct LambdaRet: public OpImp {
			static const OpType<LambdaRet> type;
			LambdaRet();
		};

		struct Push: public OpImp {
			static const OpType<Push> type;			
			const Box val;
			Push(const Box &val);
			void dump(Env &env, ostream &out) const override;
		};

		struct PutVar: public OpImp {
			static const OpType<PutVar> type;
			const Sym id;
			PutVar(Sym id);
			void dump(Env &env, ostream &out) const override;
		};
		
		struct Skip: public OpImp {
			static const OpType<Skip> type;
			size_t nops;			
			Skip(size_t nops);
			void dump(Env &env, ostream &out) const override;
		};

		struct Swap: public OpImp {
			static const OpType<Swap> type;
			Swap();
		};
	}
}

#endif
