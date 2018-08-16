#ifndef SNABL_OP_HPP
#define SNABL_OP_HPP

#include "snabl/box.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	class OpType {
	public:
		const std::string id;
		const size_t label_offs;

		OpType(const std::string &id);
	private:
		static size_t next_label_offs;
	};

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

	class Op {
	public:
		const OpType &type;

		union {
			ops::Begin as_begin;
			ops::End as_end;
			ops::Push as_push;
		}; 

		Op(const OpType &type);
		~Op();
	};
}

#endif
