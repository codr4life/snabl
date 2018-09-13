#ifndef SNABL_ITER_HPP
#define SNABL_ITER_HPP

#include "snabl/box.hpp"

namespace snabl {
  class Env;
	
	class Iter {
	public:
		using Imp = function<optional<Box> (Env &env)>;
		
		Iter(Imp imp): _imp(imp), _is_done(false) { }

		optional<Box> call(Env &env) {
			if (_is_done) { throw Error("Iter is done"); }
			
			auto v(_imp(env));
			if (!v) { _is_done = true; }
			return v;
		}

		bool is_done() const { return _is_done; }
	private:
		Imp _imp;
		bool _is_done;
	};
}

#endif
