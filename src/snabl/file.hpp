#ifndef SNABL_FILE_HPP
#define SNABL_FILE_HPP

#include "snabl/box.hpp"

namespace snabl {
  struct Env;
  
  using FilePtr = shared_ptr<fstream>;
  
  Box fopen(Env &env, const string &name, fstream::openmode mode);
}
#endif
