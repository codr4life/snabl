#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/fimp.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	class AOpType {
	public:
		const std::string id;
		const size_t label_offs;

		AOpType(const std::string &id);
	private:
		static size_t next_label_offs;
	};

	template <typename ImpT>
	class OpType: public AOpType {
	public:
		OpType(const std::string &id);
	};

	template <typename ImpT>
	OpType<ImpT>::OpType(const std::string &id): AOpType(id) { }

	struct OpImp {
		virtual ~OpImp() { }
	};
	
	class Op {
	public:
		const AOpType &type;
		const Pos pos;
		
		template <typename ImpT>
		Op(const OpType<ImpT> &type, const Pos &pos);

		template <typename ImpT, typename ArgT1, typename... ArgsT>
		Op(const OpType<ImpT> &type, const Pos &pos, ArgT1 &&arg1, ArgsT &&... args);

		virtual ~Op() { }

		template <typename ImpT>
		ImpT &as() const;
	private:
		std::unique_ptr<OpImp> _imp;
	};

	template <typename ImpT>
	Op::Op(const OpType<ImpT> &type, const Pos &pos): type(type), pos(pos) { }

	template <typename ImpT, typename ArgT1, typename... ArgsT>
	Op::Op(const OpType<ImpT> &type,
				 const Pos &pos,
				 ArgT1 &&arg1, ArgsT &&... args):
		type(type),
		pos(pos),
		_imp(new ImpT(std::forward<ArgT1, ArgsT...>(arg1, args...))) { }

	template <typename ImpT>
	ImpT &Op::as() const { return *static_cast<ImpT *>(_imp.get()); }
	
	namespace ops {
		struct Begin: public OpImp {
			static const OpType<Begin> type;
			Begin();
		};

		struct Drop: public OpImp {
			static const OpType<Drop> type;
			Drop();
		};

		struct Else: public OpImp {
			static const OpType<Else> type;
			size_t nops;
			
			Else(size_t nops);
		};
		
		struct End: public OpImp {
			static const OpType<End> type;
			End();
		};

		struct Funcall: public OpImp {
			static const OpType<Funcall> type;
			
			AFuncPtr func;
			AFimpPtr fimp, prev_fimp;
			
			Funcall(const AFuncPtr &func);
			Funcall(const AFimpPtr &fimp);
		};

		struct GetVar: public OpImp {
			static const OpType<GetVar> type;
			const Sym id;
			GetVar(const Sym &id);
		};

		struct Push: public OpImp {
			static const OpType<Push> type;
			
			Box value;
			Push(const Box &value);
		};

		struct PutVar: public OpImp {
			static const OpType<PutVar> type;
			const Sym id;
			PutVar(const Sym &id);
		};

		struct Skip: public OpImp {
			static const OpType<Skip> type;
			size_t nops;
			
			Skip(size_t nops);
		};
	}
}

#endif
