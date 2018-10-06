#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/file.hpp"
#include "snabl/fmt.hpp"

namespace snabl {
  AsyncPtr fopen(Env &env, Pos pos, const string &name, fstream::openmode mode) {
    return async([&env, pos, name, mode]() {
        auto f(make_shared<fstream>(name, mode | ios::binary));

        if (f->fail()) {
          throw RuntimeError(env, pos, fmt("File not found: %0", {name}));
        }
        
        return Box(env.rfile_type, f);
      });
  }
}
