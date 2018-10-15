#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/file.hpp"
#include "snabl/fmt.hpp"

namespace snabl {
  AsyncPtr fopen(Env &env, const string &name, fstream::openmode mode) {
    return async([&env, name, mode]() {
        auto f(make_shared<fstream>(name, mode | ios::binary));

        if (f->fail()) {
          throw RuntimeError(env, fmt("File not found: %0", {name}));
        }
        
        return Box(env.rfile_type, f);
      });
  }
}
