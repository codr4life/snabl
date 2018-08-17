#include "snabl/bin.hpp"
#include "snabl/env.hpp"

#define SNABEL_DISPATCH()														\
	if (_pc == end) { return; }												\
	goto *op_labels[(_pc++)->type.label_offs];				\

namespace snabl {
	BinFimp::BinFimp(const Ops::iterator &begin, const Ops::iterator &end):
		begin(begin), end(end) { }
	
	Bin::Bin(Env &env): env(env), _pc(_ops.begin()) { }

	std::optional<BinFimp> Bin::get_fimp(const AFimpPtr &ptr) {
		auto found = _fimps.find(ptr);
		return (found == _fimps.end())
			? std::nullopt
			: std::make_optional(found->second);
	}

	Op &Bin::emit_begin(const ScopePtr &parent) {
		return emit_op(ops::Begin::type, ops::Begin(parent));
	}

	Op &Bin::emit_end() {
		return emit_op(ops::End::type, ops::End());
	}

	Op &Bin::emit_push(const Box &value) {
		return emit_op(ops::Push::type, ops::Push(value));
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
		env.push_stack(_pc->as<ops::Push>().value); 
		SNABEL_DISPATCH();
	}
}
