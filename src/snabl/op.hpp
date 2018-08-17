#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/scope.hpp"
#include "snabl/std/any.hpp"

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

	class Op {
	public:
		const AOpType &type;

		template <typename ImpT>
		Op(const OpType<ImpT> &type, const ImpT &imp);

		template <typename ValueT>
		ValueT as() const;

		~Op();
	private:
		std::any _imp;
	};

	template <typename ImpT>
	Op::Op(const OpType<ImpT> &type, const ImpT &imp): type(type), _imp(imp) { }

	template <typename ImpT>
	ImpT Op::as() const { return std::any_cast<ImpT>(_imp); }
	
	namespace ops {
		struct Begin {
			static const OpType<Begin> type;
			ScopePtr parent;
			Begin(const ScopePtr &parent);
		};

		struct End {
			static const OpType<End> type;
			End();
		};

		struct Push {
			static const OpType<Push> type;
			
			Box value;
			Push(const Box &value);
		};
	}
}

#endif
