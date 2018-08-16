#include "snabl/bin.hpp"
#include "snabl/env.hpp"

#define SNABEL_DISPATCH()														\
	if (_pc == end) { return; }												\
	goto *op_labels[(_pc++)->type.label_offs];				\

namespace snabl {
	Bin::Bin(Env &env): env(env), _pc(_ops.begin()) { }

	std::optional<Ops::iterator> Bin::get_fimp_offs(const AFimpPtr &ptr) {
		auto found = _fimp_offs.find(ptr);
		return (found == _fimp_offs.end())
			? std::nullopt
			: std::make_optional(found->second);
	}

	Op &Bin::emit_op(const OpType &type) {
		_ops.emplace_back(ops::Begin::type);
		return _ops.back();
	}

	Op &Bin::emit_begin(const ScopePtr &parent) {
		auto &op(emit_op(ops::Begin::type));
		op.as_begin = ops::Begin(parent);
		return op;
	}

	Op &Bin::emit_end() {
		auto &op(emit_op(ops::End::type));
		op.as_end = ops::End();
		return op;
	}

	Op &Bin::emit_push(const Box &value) {
		auto &op(emit_op(ops::Push::type));
		op.as_push = ops::Push(value);
		return op;
	}
	
	void Bin::run(std::optional<Ops::iterator> begin,
								std::optional<Ops::iterator> end) {
		auto begin_op(_ops.begin());
		_pc = begin.value_or(_ops.begin());
		if (!end) { end = _ops.end(); }
		
		static void* op_labels[] = {
			&&op_begin, &&op_end, &&op_push};

		SNABEL_DISPATCH();
	op_begin:
		env.begin();
		SNABEL_DISPATCH();
	op_end:
		env.end();
		SNABEL_DISPATCH();
	op_push:
		auto &op(_pc->as_push);
		env.push_stack(op.value); 
		SNABEL_DISPATCH();
	}
}
