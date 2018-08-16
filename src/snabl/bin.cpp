#include "snabl/bin.hpp"
#include "snabl/env.hpp"

namespace snabl {
	Bin::Bin(Env &env): env(env), _pc(_ops.begin()) {
	}

	std::optional<Ops::iterator> Bin::get_fimp_offs(const AFimpPtr &ptr) {
		auto found = _fimp_offs.find(ptr);
		return (found == _fimp_offs.end())
			? std::nullopt
			: std::make_optional(found->second);
	}

#define SNABEL_DISPATCH()														\
	if (_pc == end) { return; }												\
	goto *op_labels[(_pc++)->type.label_offs];				\

	void Bin::run(std::optional<Ops::iterator> begin,
								std::optional<Ops::iterator> end) {
		auto begin_op(_ops.begin());
		_pc = begin.value_or(_ops.begin());
		if (!end) { end = _ops.end(); }
		
		static void* op_labels[] = {
			&&op_push, &&op_begin, &&op_end};

		SNABEL_DISPATCH();
	op_push:
		auto op(_pc->as<ops::Push>());
		env.push_stack(op.value); 
		SNABEL_DISPATCH();
	op_begin:
		env.begin();
		SNABEL_DISPATCH();
	op_end:
		env.end();
		SNABEL_DISPATCH();
	}	
}
