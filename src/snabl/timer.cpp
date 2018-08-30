#include "snabl/timer.hpp"

namespace snabl {
	steady_clock::time_point Timer::now() { return steady_clock::now(); }

	Timer::Timer(): _time(now()) { }

	void Timer::reset() { _time = now(); }
	
	Int Timer::ns() const {
		return duration_cast<nanoseconds>(now() - _time).count();
	}
}
