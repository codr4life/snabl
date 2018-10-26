#include "snabl/scope.hpp"
#include "snabl/target.hpp"

namespace snabl {
  Target::Target(Scope *parent_scope, PC start_pc, I64 end_pc):
    start_pc(start_pc), end_pc(end_pc) {
    if (parent_scope) { vars.emplace(parent_scope->vars.vals); }
  }
}
