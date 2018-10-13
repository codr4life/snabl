#include "snabl/env.hpp"
#include "snabl/repl.hpp"
#include "tests.hpp"

using namespace snabl;

enum class Mode { Compile, Default, Repl, Run };

int main(int argc, const char *argv[]) {
  Env env;
  env.use(Pos(-1, -1), env.sym("s.abc"));
  Mode mode(Mode::Default);
  argc--;
  
  for (const char **ap(argv+1); argc; argc--, ap++) {
    const string a(*ap);
    
    if (a[0] == '-') {
      switch (a[1]) {
      case 'c':
        mode = Mode::Compile;
        break;
      default:
        throw Error(fmt("Invalid flag: %0", {a}));
      }
    } else {
      ifstream f(a);
      if (f.fail()) { throw Error(fmt("File not found: %0", {a})); }
      env.compile(f);
      if (mode == Mode::Default) { mode = Mode::Run; }
    }
  }
  
  switch (mode) {
  case Mode::Compile: {
    I64 row(0);
    for (const auto &op: env.ops) {
      cout << row++ << '\t';
      op->dump(cout);
    }
    break;
  }
  case Mode::Default:
  case Mode::Repl:
    all_tests();
    repl(env, cin, cout);
    break;
  case Mode::Run:
    if (!env.ops.empty()) {
      env.jump(I64(0));
      env.run();
    }
    
    break;
  }
  
  return 0;
}
