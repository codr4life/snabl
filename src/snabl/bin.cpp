#include "snabl/bin.hpp"
#include "snabl/env.hpp"

#define SNABL_DISPATCH()												\
	if (_pc == _ops.end()) { return; }						\
	goto *op_labels[_pc->type.label_offs];				\

namespace snabl {
	Bin::Bin(Env &env): env(env) { }

	const Ops &Bin::ops() const { return _ops; }

	std::size_t Bin::pc() const { return _pc-_ops.begin(); }

	void Bin::set_pc(std::size_t pc) { _pc = _ops.begin()+pc; }

	void Bin::compile(const Forms::const_iterator &begin,
										const Forms::const_iterator &end) {
		FuncPtr func;
		FimpPtr fimp;

		for (auto i(begin); i != end;) { i->imp->compile(i, end, func, fimp, *this); }
		auto pos(begin->pos);

		if (fimp) {
			fimp->compile(*this, pos);
			emplace_back(ops::Funcall::type, pos, fimp);
		} else if (func) {
			emplace_back(ops::Funcall::type, pos, func);
		}
	}

	void Bin::compile(const Forms &forms) { compile(forms.begin(), forms.end()); }

	void Bin::run(std::size_t offs) {
		_pc = _ops.begin();
		if (offs) { std::advance(_pc, offs); }
		
		static void* op_labels[] = {
			&&op_begin, &&op_drop, &&op_else, &&op_end, &&op_fimp, &&op_funcall,
			&&op_getvar, &&op_push, &&op_putvar, &&op_skip
		};

		SNABL_DISPATCH();
	op_begin:
		env.begin();
		_pc++;
		SNABL_DISPATCH();
	op_drop:
		env.scope()->pop_stack();
		_pc++;
		SNABL_DISPATCH();
	op_else: {
			auto &op(_pc->as<ops::Else>());
			auto v(env.scope()->pop_stack());
			if (!v.as<bool>()) { _pc += op.nops; }
			_pc++;
			SNABL_DISPATCH();
		}
	op_end:
		env.end();
		_pc++;
		SNABL_DISPATCH();
	op_fimp:
		_pc++;
		SNABL_DISPATCH();		
	op_funcall: {
			auto &op(_pc->as<ops::Funcall>());
			FimpPtr fimp(op.fimp);
			
			if (!fimp && op.prev_fimp) { fimp = op.prev_fimp; }
			
			if (fimp) {
				if (!fimp->score(env.stack())) { fimp = nullptr; }
			} else {
				fimp = op.func->get_best_fimp(env.stack());
			}
			
			if (!fimp) { throw Error(fmt("Func not applicable: %0", {fimp->func->id})); }
			if (!op.fimp) { op.prev_fimp = fimp; }

			if (fimp->imp) {
				Fimp::call(fimp);
			} else {
				fimp->compile(*this, _pc->pos);
				env.push_call(fimp, pc());
				set_pc(fimp->pc());
			}
						
			_pc++;
			SNABL_DISPATCH();
		}
	op_getvar: {
			auto &op(_pc->as<ops::GetVar>());		
			auto v(env.get_var(op.id));
			if (!v) { throw Error("Unknown var"); }
			env.push_stack(*v);
			_pc++;
			SNABL_DISPATCH();
		}
	op_push:
		env.push_stack(_pc->as<ops::Push>().value); 
		_pc++;
		SNABL_DISPATCH();
	op_putvar: {
			auto scope(env.scope());
			auto &op(_pc->as<ops::PutVar>());
			auto v(scope->pop_stack());
			scope->put_var(op.id, v);
			_pc++;
			SNABL_DISPATCH();
		}
	op_skip:
		_pc += _pc->as<ops::Skip>().nops+1;
		SNABL_DISPATCH();
	}
}
