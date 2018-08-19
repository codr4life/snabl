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

	struct OpImp { };
	
	class Op {
	public:
		const AOpType &type;

		template <typename ImpT>
		Op(const OpType<ImpT> &type);

		template <typename ImpT, typename ArgT1, typename... ArgsT>
		Op(const OpType<ImpT> &type, ArgT1 &&arg1, ArgsT &&... args);

		template <typename ImpT>
		ImpT &as() const;
	private:
		std::unique_ptr<OpImp> _imp;
	};

	template <typename ImpT>
	Op::Op(const OpType<ImpT> &type): type(type) { }

	template <typename ImpT, typename ArgT1, typename... ArgsT>
	Op::Op(const OpType<ImpT> &type, ArgT1 &&arg1, ArgsT &&... args):
		type(type), _imp(new ImpT(std::forward<ArgT1, ArgsT...>(arg1, args...))) { }

	template <typename ImpT>
	ImpT &Op::as() const { return *static_cast<ImpT *>(_imp.get()); }
	
	namespace ops {
		struct Begin: public OpImp {
			static const OpType<Begin> type;
			ScopePtr parent;
			Begin(const ScopePtr &parent);
		};

		struct Drop: public OpImp {
			static const OpType<Drop> type;
			Drop();
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

		struct Push: public OpImp {
			static const OpType<Push> type;
			
			Box value;
			Push(const Box &value);
		};
	}
}

#endif
