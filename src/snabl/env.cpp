#include <ctype.h>

#include "snabl/env.hpp"
#include "snabl/parser.hpp"

namespace snabl {
  void Env::use(Sym lib_id, const vector<Sym> def_ids) {
    Lib *lib(get_lib(lib_id));
    if (!lib) { throw CompileError(pos(), fmt("Unknown lib: %0", {lib_id})); }

    if (def_ids.empty()) {
      for (auto &d: lib->lib_defs) { task->lib->use_def(*d.second); }
    } else {
      for (auto did: def_ids) {
        auto d(lib->get_def(did));
        if (!d) { throw CompileError(pos(), fmt("Unknown def: %0", {lib_id})); }
        task->lib->use_def(*d);
      }
    }
  }
  
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
      while (task->pc) { task->pc->run(*this); }

      if (task != &main_task) {
        task->status = Task::Status::Done;
        auto next(task->next);
        task->prev->next = task->next;
        task->next->prev = task->prev;
        task_pool.release(task);
        task = next;
        goto start;
      }
    } catch (const UserError &e) {
      if (!task->tries.size) { throw e; }
      auto &t(task->tries.back());
      auto &s(t.state);
      s.restore_env(*this);
      s.restore_call(*this);
      end_try();
      
      push(error_type, e);
      jump(t.op.end_pc);
    }

    if (task->pc) { goto start; }
  }

  optional<Box> Env::async(const function<optional<Box> ()> &fn) {
    return (is_async())
      ? Box(async_type, AsyncPtr::make(&async_type.pool, 
                                       std::async(launch::async, fn)))
      : fn();
  }

  void Env::push_async(const function<optional<Box> ()> &fn) {
    const auto v(async(fn));
    if (v) { push(*v); }
  }

  void Env::fopen(const string &name, fstream::openmode mode) {
    return push_async([this, name, mode]() {
        auto f(FilePtr::make(&file_type.pool, name, mode | ios::binary));
        
        if (f->fail()) {
          throw RuntimeError(*this, fmt("File not found: %0", {name}));
        }
        
        return Box(rfile_type, f);
      });
  }
}
