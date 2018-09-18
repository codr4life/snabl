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
		Op *prev_op(ops.empty() ? nullptr : &ops.back());
		compile(fs.begin(), fs.end());
		
		if (!ops.empty()) {
			pc = prev_op ? prev_op->next : &ops.front().imp;
			run();
		}
	}

	void Env::run() {
	enter:		
		try {
			while (pc) { (*pc)(); }
		} catch (const UserError &e) {
			if (_tries.empty()) { throw e; }
			auto &t(*_tries.back());
			_tries.pop_back();

			if (t.state->ncalls < _calls.size()) {
				for (auto c(_calls.begin()+t.state->ncalls);
						 c != _calls.end();
						 c++) {
					auto fi(dynamic_pointer_cast<Fimp>(c->target));
					if (fi) { fi->_is_calling = false; }
				}
			}

			t.state->restore_all(*this);
			t.state.reset();
			push(error_type, make_shared<UserError>(e));
			pc = &t.handler_pc;
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
