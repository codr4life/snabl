#ifndef SNABL_ITER_HPP
#define SNABL_ITER_HPP

#include "snabl/box.hpp"

namespace snabl {
  class Env;
	
	class Iter {
	public:
		using Imp = function<optional<Box> (Env &env)>;
		
		Iter(Imp imp): _imp(imp), _done(false) { }

		optional<Box> call(Env &env) {
			if (_done) { throw Error("Iter is done"); }
			
			auto v(_imp(env));
			if (!v) { _done = true; }
			return v;
		}

		bool is_done() const { return _done; }
	private:
		Imp _imp;
		bool _done;
	};
}

#endif
