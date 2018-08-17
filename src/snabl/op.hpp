#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/scope.hpp"
#include "snabl/std/any.hpp"

namespace snabl {
	class OpType {
	public:
		const std::string id;
		const size_t label_offs;

		OpType(const std::string &id);
	private:
		static size_t next_label_offs;
	};

	class Op {
	public:
		const OpType &type;

		template <typename ImpT>
		Op(const OpType &type, const ImpT &imp);

		template <typename ValueT>
		ValueT as() const;

		~Op();
	private:
		std::any _imp;
	};

	template <typename ImpT>
	Op::Op(const OpType &type, const ImpT &imp): type(type), _imp(imp) { }

	template <typename ImpT>
	ImpT Op::as() const { return std::any_cast<ImpT>(_imp); }
	
	namespace ops {
		struct Begin {
			static const OpType type;
			ScopePtr parent;
			Begin(const ScopePtr &parent);
		};

		struct End {
			static const OpType type;
			End();
		};

		struct Push {
			static const OpType type;
			
			Box value;
			Push(const Box &value);
		};
	}
}

#endif
