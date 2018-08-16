#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"

namespace snabl {
	class OpType {
	public:
		const std::string id;
		const size_t label_offs;

		OpType(const std::string &id, size_t label_offs);
	};

	namespace ops {
		struct Push {
			static const OpType type;
			
			Box value;
			Push(const Box &value);
		};
	}

	class Op {
	public:
		const OpType &type;

		union {
			ops::Push as_push;
		}; 

		Op(const OpType &type);
		~Op();
	};
}

#endif
