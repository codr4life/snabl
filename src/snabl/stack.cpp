#include "snabl/iter.hpp"
#include "snabl/stack.hpp"

namespace snabl {
  ostream &operator <<(ostream &out, const Stack &stack) {
    out << '[';
    char sep(0);
    
    for (auto &v: stack) {
      if (sep) { out << sep; }
      v.dump(out);
      sep = ' ';
    }
    
    out << ']';
    return out;
  }
}
