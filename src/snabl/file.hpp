#ifndef SNABL_FILE_HPP
#define SNABL_FILE_HPP

#include "snabl/async.hpp"

namespace snabl {
  struct Env;
  
  using FilePtr = shared_ptr<fstream>;
  
  AsyncPtr fopen(Env &env, const string &name, fstream::openmode mode);
}
#endif
