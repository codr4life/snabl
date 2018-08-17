#include "snabl/bin.hpp"
#include "snabl/env.hpp"

#define SNABEL_DISPATCH()																		\
	if (!nops--) { return; }																	\
	goto *op_labels[(_pc++)->type.label_offs];								\

namespace snabl {
	BinFimp::BinFimp(size_t begin, size_t end):
		begin(begin), end(end) { }
	
	Bin::Bin(Env &env): env(env), _pc(_ops.begin()) { }

	std::optional<BinFimp> Bin::get_fimp(const AFimpPtr &ptr) {
		auto found = _fimps.find(ptr);
		return (found == _fimps.end())
			? std::nullopt
			: std::make_optional(found->second);
	}

	Op &Bin::emit_begin(const ScopePtr &parent) {
		return emit_op(ops::Begin::type, parent);
	}

	Op &Bin::emit_end() {
		return emit_op(ops::End::type);
	}

	Op &Bin::emit_push(const Box &value) {
		return emit_op(ops::Push::type, value);
	}
	
	void Bin::run(size_t offs, size_t nops) {
		_pc = _ops.begin() + offs;
		if (!nops) { nops = _ops.size(); }
		
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
