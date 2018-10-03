#include "snabl/env.hpp"
#include "snabl/box.hpp"
#include "snabl/task.hpp"
#include "snabl/lib.hpp"
#include "snabl/types/task.hpp"

namespace snabl {
	TaskType::TaskType(Lib &lib, Sym id): Type<TaskPtr>(lib, id) { }

	void TaskType::dump(const Box &val, ostream &out) const {
		auto &t(val.as<TaskPtr>());
		out << "(Task " << t.get() << ')';
	}
}
