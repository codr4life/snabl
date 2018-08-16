#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/std/any.hpp"

namespace snabl {
	class OpType {
	public:
		const std::string id;
		const size_t label_offs;

		OpType(const std::string &id, size_t label_offs);
	};
	
	class Op {
	public:
		const OpType &type;

		Op(const OpType &type, const std::any &data);

		template <typename OpT>
		OpT as();
	private:
		std::any _data;
	};

	template <typename OpT>
	OpT Op::as() { return std::any_cast<OpT>(_data); }

	namespace ops {
		struct Push {
			static const OpType type;
			
			const Box value;
			Push(const Box &value);
		};
	}
}

#endif
