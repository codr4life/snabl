#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/parser.hpp"
#include "snabl/run.hpp"

namespace snabl {
	void Env::run(string_view in) {
		auto offs(ops.size());
		compile(in);
		pc = ops.begin()+offs;		
		run();
	}

	void Env::run(istream &in) {
		Forms fs;
		Parser(*this).parse(in, fs);
		auto offs(ops.size());
		compile(fs.begin(), fs.end());
		pc = ops.begin()+offs;		
		run();
	}

	void Env::run() {
		next = (pc == ops.end()) ? nullptr : &pc->imp;
		
	enter:
		
		try {
			while (next) { (*next)(); }
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
			pc = ops.begin()+*t.handler_pc;
			next = (pc == ops.end()) ? nullptr : &pc->imp;
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
