#include <ctype.h>

#include "snabl/env.hpp"
#include "snabl/parser.hpp"

namespace snabl {
  void Env::compile(string_view in) {
    const string s(in);
    istringstream ss(s);
    compile(ss);
  }

  void Env::compile(istream &in) {
    Forms forms;
    Parser(*this).parse(in, forms);   
    compile(forms);
  }

  void Env::compile(const Forms &forms) { compile(forms.begin(), forms.end()); }
  
  void Env::compile(const Form &form) {
    Forms fs {form};
    auto i(fs.cbegin());
    i->imp->compile(i, i+1, *this);
  }

  void Env::compile(Forms::const_iterator begin, Forms::const_iterator end) {
    for (auto i(begin); i != end;) { i->imp->compile(i, end, *this); }
  }

    void Env::run(string_view in) {
    const string s(in);
    istringstream ss(s);
    run(ss);
  }

  void Env::run(istream &in) {
    Forms fs;
    Parser(*this).parse(in, fs);

    const auto start_pc(ops.size());
    compile(fs.begin(), fs.end());
    
    if (!ops.empty()) {
      jump(start_pc);
      run();
    }
  }

  void Env::run() {
  start:    
    try {
      while (task->pc) { (*task->pc)(); }

      if (task != main_task) {
        task->status = Task::Status::Done;
        auto next(task->next);
        task->prev->next = task->next;
        task->next->prev = task->prev;
        task = next;
        goto start;
      }
    } catch (const UserError &e) {
      if (task->tries.empty()) { throw e; }
      auto &t(*task->tries.back());
      auto &s(get_reg<State>(t.state_reg));     
      s.restore_lib(*this);
      s.restore_scope(*this);
      s.restore_calls(*this);
      s.restore_stack(*this);
      s.restore_splits(*this);
      clear_reg(t.state_reg);
      end_try();
      
      push(error_type, make_shared<UserError>(e));
      jump(t.end_pc);
      goto start;
    }
  }

  static string val_str(const Box &val) {
    stringstream buf;
    buf << val;
    return buf.str();
  }
  
  UserError::UserError(Env &env, Pos pos, const Box &_val):
    RuntimeError(env, pos, val_str(_val)), val(_val) { }
}
