#ifndef SNABL_TIMER_HPP
#define SNABL_TIMER_HPP

#include "snabl/types/int.hpp"

namespace snabl {
	class Timer {
	public:
		static steady_clock::time_point now();

		Timer();
		void reset();
		Int ns() const;
	private:
		steady_clock::time_point _time;
	};
}

#endif
