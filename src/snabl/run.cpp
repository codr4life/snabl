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
		const auto start_pc(ops.size());
		compile(fs.begin(), fs.end());
		
		if (!ops.empty()) {
			jump(start_pc);
			run();
		}
	}

	void Env::run() {
	enter:		
		try {
			while (_task->_pc) { (*_task->_pc)(); }
		} catch (const UserError &e) {
			if (!_try) { throw e; }
			auto &s(get_reg<State>(_try->state_reg));			
			s.restore_lib(*this);
			s.restore_scope(*this);
			s.restore_calls(*this);
			s.restore_stack(*this);
			s.restore_splits(*this);
			clear_reg(_try->state_reg);
			
			push(error_type, make_shared<UserError>(e));
			jump(_try->handler_pc);
			
			const auto pr(_try->parent_reg);
			_try = get_reg<ops::Try *>(pr);
			clear_reg(pr);
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
