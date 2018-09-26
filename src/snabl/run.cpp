#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/parser.hpp"
#include "snabl/run.hpp"

namespace snabl {
	void Env::run(string_view in) {
		const string s(in);
		istringstream ss(s);
		run(ss);
	}

	void Env::run(istream &in) {
		Forms fs;
		Parser(*this).parse(in, fs);

		Ops &ops(_task->_ops);
		Op *prev_op(ops.empty() ? nullptr : &ops.back());
		compile(fs.begin(), fs.end());
		
		if (!ops.empty()) {
			jump(prev_op ? prev_op->next : &ops.front().imp);
			run();
		}
	}

	void Env::run() {
	enter:		
		try {
			while (_task->_pc) { (*_task->_pc)(); }
		} catch (const UserError &e) {
			if (!_try) { throw e; }
			_try->state->restore_lib(*this);
			_try->state->restore_scope(*this);
			_try->state->restore_calls(*this);
			_try->state->restore_stack(*this);
			_try->state->restore_splits(*this);
			_try->state.reset();
			push(error_type, make_shared<UserError>(e));
			jump(_try->handler_pc);
			_try = _try->parent;
			goto enter;
		}
	}
	
	RuntimeError::RuntimeError(Env &env, Pos pos, const string &msg) {
		stringstream buf;
				
		buf << env._stack << endl
				<< "Error in row " << pos.row << ", col " << pos.col << ":\n"
				<< msg;
		
		_what = buf.str();
	}

	const char *RuntimeError::what() const noexcept { return _what.c_str(); }

	static string val_str(const Box &val) {
		stringstream buf;
		buf << val;
		return buf.str();
	}
	
	UserError::UserError(Env &env, Pos pos, const Box &_val):
		RuntimeError(env, pos, val_str(_val)), val(_val) { }
}
