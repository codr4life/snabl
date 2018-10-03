#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/run.hpp"
#include "snabl/std.hpp"
#include "snabl/timer.hpp"

namespace snabl {
	namespace libs {
		Home::Home(Env &env): Lib(env, env.sym("home")) {
			env.no_type = add_type<Trait>(env.sym("_"));
			env.maybe_type = add_type<Trait>(env.sym("Maybe"));
			env.nil_type = add_type<NilType>(env.sym("Nil"), {env.maybe_type});
			env.root_type = add_type<Trait>(env.sym("T"), {env.maybe_type});
			env.num_type = add_type<Trait>(env.sym("Num"), {env.root_type});
			env.seq_type = add_type<Trait>(env.sym("Seq"), {env.root_type});
			env.source_type = add_type<Trait>(env.sym("Source"), {env.root_type});
			env.sink_type = add_type<Trait>(env.sym("Sink"), {env.root_type});

			env.meta_type = add_type<MetaType>(env.sym("Type"), {env.root_type});	
			env.bool_type = add_type<BoolType>(env.sym("Bool"), {env.root_type});
			env.char_type = add_type<CharType>(env.sym("Char"), {env.root_type});
			env.error_type = add_type<ErrorType>(env.sym("Error"), {env.root_type});
			env.float_type = add_type<FloatType>(env.sym("Float"), {env.num_type});
			
			env.int_type = add_type<IntType>(env.sym("Int"), {
					env.num_type, env.seq_type});

			env.iter_type = add_type<IterType>(env.sym("Iter"), {
					env.seq_type, env.source_type});
			
			env.stack_type = add_type<StackType>(env.sym("Stack"), {
					env.seq_type, env.sink_type, env.source_type});

			env.str_type = add_type<StrType>(env.sym("Str"), {
					env.seq_type, env.sink_type, env.source_type});
			
			env.sym_type = add_type<SymType>(env.sym("Sym"), {env.root_type});
			env.task_type = add_type<TaskType>(env.sym("Task"), {env.root_type});
			env.time_type = add_type<TimeType>(env.sym("Time"), {env.root_type});
			env.lambda_type = add_type<LambdaType>(env.sym("Lambda"), {env.root_type});
			
			add_macro(env.sym("t"), env.bool_type, true);			
			add_macro(env.sym("f"), env.bool_type, false);			
			add_macro(env.sym("nil"), env.nil_type);			

			add_macro(env.sym("_"), ops::Nop::type);
			add_macro(env.sym("call!"), ops::Call::type);
			add_macro(env.sym("ddrop!"), ops::DDrop::type);
			add_macro(env.sym("drop!"), ops::Drop::type);
			add_macro(env.sym("dup!"), ops::Dup::type);
			add_macro(env.sym("recall!"), ops::Recall::type);
			add_macro(env.sym("return!"), ops::Return::type);
			add_macro(env.sym("rot!"), ops::Rot::type);
			add_macro(env.sym("rswap!"), ops::RSwap::type);
			add_macro(env.sym("sdrop!"), ops::SDrop::type);
			add_macro(env.sym("swap!"), ops::Swap::type);

			add_macro(env.sym("bench:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									const auto form(*in++);									
									auto &op(env.emit(ops::Bench::type, form.pos)
													 .as<ops::Bench>());
									if (in == end) {
										throw SyntaxError(form.pos, "Missing bench form");
									}
									env.compile(*in++);
									env.emit(ops::Stop::type, form.pos);
									op.end_pc = env.ops().size();
								});	

			add_macro(env.sym("if:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									auto &else_skip(env.emit(ops::Else::type, form.pos));
									env.compile(*in++, func, fimp);
									auto &if_skip(env.emit(ops::Jump::type, form.pos));
									else_skip.as<ops::Else>().skip_pc = env.ops().size();
									env.compile(*in++, func, fimp);
									if_skip.as<ops::Jump>().end_pc = env.ops().size();
								});	

			add_macro(env.sym("func:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &lib(env.lib());
									const auto &form(*in++);
									
									if (&in->type != &forms::Fimp::type) {
										throw SyntaxError(form.pos,
																			fmt("Invalid func: %0",
																					{in->type.id}));
									}

									auto &id_form((in++)->as<forms::Fimp>());
									vector<Box> args;
									
									for (const auto id: id_form.type_ids) {
										const auto t(lib.get_type(id));

										if (!t) {
											throw CompileError(form.pos, fmt("Unknown type: %0", {id}));
										}
										
										args.emplace_back(*t);
									}
									
									auto fi = lib.add_fimp(id_form.id, args, *in++);
									Fimp::compile(fi, form.pos);
								});

			add_macro(env.sym("let:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									auto &p(*in++);

									if (&p.type == &forms::Id::type) {
										env.emit(ops::Let::type, form.pos, p.as<forms::Id>().id);
									} else {
										auto &b(p.as<forms::Body>().body);

										for (auto pp = b.rbegin(); pp != b.rend(); pp++) {
											env.emit(ops::Let::type, form.pos, pp->as<forms::Id>().id);
										}
									}
								});
						
			add_macro(env.sym("switch:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);

									vector<ops::Jump *> skips;
									auto &cases((in++)->as<forms::Body>());;

									for (auto f(cases.body.begin()); f != cases.body.end();) {
										if (f+1 != cases.body.end()) {
											env.emit(ops::Dup::type, form.pos);
										}
										
										env.compile(*f++);

										if (f != cases.body.end()) {
											auto &else_op(env.emit(ops::Else::type,
																						 form.pos).as<ops::Else>());
											env.emit(ops::Drop::type, form.pos);
											env.compile(*f++);
											
											if (f != cases.body.end()) {
												skips.push_back(&env.emit(ops::Jump::type,
																									form.pos).as<ops::Jump>());
											}

											else_op.skip_pc = env.ops().size();
										}
									}

									for (auto &s: skips) { s->end_pc = env.ops().size(); }
								});	

			add_macro(env.sym("task:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);

									if (in == end) {
										throw CompileError(form.pos, "Missing task body");
									}
									
									auto &op(env.emit(ops::Task::type, form.pos,
																		&in->type == &forms::Scope::type,
																		env.ops().size())
													 .as<ops::Task>());
									
									env.compile(*in++);
									op.end_pc = env.ops().size();
								});	

			add_macro(env.sym("times:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									const Int i_reg(env.begin_reg());
									env.emit(ops::Times::type, form.pos, i_reg);
									const auto start_pc(env.ops().size());

									auto &jump(env.emit(ops::JumpIf::type, form.pos, [&env, i_reg]() {
												return !env.get_reg<Int>(i_reg)--;
											}).as<ops::JumpIf>());
									
									env.compile(*in++);
									env.end_reg(i_reg);
									env.emit(ops::Jump::type, form.pos, start_pc);
									jump.end_pc = env.ops().size();
								});	

			add_macro(env.sym("try:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									const auto form(*in++);
									auto &op(env.emit(ops::Try::type, form.pos, env.begin_reg())
													 .as<ops::Try>());
									if (in == end) { throw SyntaxError(form.pos, "Missing body"); }
									env.compile(*in++);
									env.emit(ops::TryEnd::type, form.pos, op.state_reg);
									env.end_reg(op.state_reg);
									env.emit(ops::Push::type, form.pos, env.nil_type);
									op.end_pc = env.ops().size();
								});

			add_fimp(env.sym("throw"),
							 {Box(env.root_type)},
							 [&env](Fimp &fimp) {
								 throw UserError(env, env.call().pos, env.pop());
							 });

			add_fimp(env.sym("throw"),
							 {Box(env.error_type)},
							 [&env](Fimp &fimp) {
								 throw *env.pop().as<ErrorPtr>();
							 });

			add_fimp(env.sym("catch"),
							 {Box(env.error_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.pop().as<ErrorPtr>()->val);
							 });

			add_fimp(env.sym("isa"),
							 {Box(env.maybe_type), Box(env.meta_type)},
							 [&env](Fimp &fimp) {
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.isa(y.as<ATypePtr>()));
							 });

			add_fimp(env.sym("="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 [&env](Fimp &fimp) {
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.eqval(y));
							 });

			add_fimp(env.sym("=="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 [&env](Fimp &fimp) {
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.equid(y));
							 });

			add_fimp(env.sym("<"),
							 {Box(env.root_type), Box(env.root_type)},
							 [&env](Fimp &fimp) {
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.cmp(y) == Cmp::LT);
							 });
	
			add_fimp(env.sym("int"),
							 {Box(env.float_type)},
							 [&env](Fimp &fimp) {
								 const Float v(env.pop().as<Float>());
								 env.push(env.int_type, Int(v));
							 });

			add_fimp(env.sym("float"),
							 {Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 const Int v(env.pop().as<Int>());
								 env.push(env.float_type, Float(v));
							 });

			add_fimp(env.sym("++"),
							 {Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 env.peek().as<Int>()++;
							 });

			add_fimp(env.sym("--"),
							 {Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 env.peek().as<Int>()--;
							 });
			
			add_fimp(env.sym("+"),
							 {Box(env.int_type), Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 Int y(env.pop().as<Int>());
								 env.peek().as<Int>() += y;
							 });

			add_fimp(env.sym("-"),
							 {Box(env.int_type), Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 Int y(env.pop().as<Int>());
								 env.peek().as<Int>() -= y;
							 });
			
			add_fimp(env.sym("*"),
							 {Box(env.int_type), Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 Int y(env.pop().as<Int>());
								 env.peek().as<Int>() *= y;
							 });

			add_fimp(env.sym("bool"),
							 {Box(env.maybe_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.bool_type, env.pop().as_bool());
							 });

			add_fimp(env.sym("push"),
							 {Box(env.sink_type), Box(env.root_type)},
							 [&env](Fimp &fimp) {
								 auto v(env.pop());
								 env.pop().push(v);
							 });
			
			add_fimp(env.sym("peek"),
							 {Box(env.source_type)},
							 [&env](Fimp &fimp) {
								 auto v(env.pop().peek());
								 env.push(v ? *v : Box(env.nil_type));
							 });

			add_fimp(env.sym("pop"),
							 {Box(env.source_type)},
							 [&env](Fimp &fimp) {
								 env.pop().pop();
							 });

			add_fimp(env.sym("iter"),
							 {Box(env.seq_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.iter_type, env.pop().iter());
							 });

			add_fimp(env.sym(".."),
							 {Box(env.seq_type)},
							 [&env](Fimp &fimp) {
								 auto i(env.pop().iter());

								 while (!i->is_done()) {
									 auto v(i->call(env));
									 if (v) { env.push(*v); }
								 }
							 });
			
			add_fimp(env.sym("dump"),
							 {Box(env.maybe_type)},
							 [&env](Fimp &fimp) {
								 env.pop().dump(cerr);
								 cerr << endl;
							 });

			add_fimp(env.sym("say"),
							 {Box(env.maybe_type)},
							 [&env](Fimp &fimp) {
								 env.pop().print(cout);
								 cout << endl;
							 });
			
			add_fimp(env.sym("len"),
							 {Box(env.str_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.int_type, env.pop().as<StrPtr>()->size());
							 });

			add_fimp(env.sym("len"),
							 {Box(env.stack_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.int_type, env.pop().as<StackPtr>()->size());
							 });

			add_fimp(env.sym("ns"),
							 {Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.time_type, env.pop().as<Int>());
							 });			

			add_fimp(env.sym("ms"),
							 {Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.time_type, Time::ms(env.pop().as<Int>()));
							 });			

			add_fimp(env.sym("ms"),
							 {Box(env.time_type)},
							 [&env](Fimp &fimp) {
								 env.push(env.int_type, env.pop().as<Time>().as_ms());
							 });
			
			add_fimp(env.sym("sleep"),
							 {Box(env.time_type)},
							 [&env](Fimp &fimp) {
								 const Time time(env.pop().as<Time>());
								 this_thread::sleep_for(nanoseconds(time.ns));
							 });

			add_fimp(env.sym("test="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 [&env](Fimp &fimp) {
								 Box y(env.pop()), x(env.pop());
								 
								 if (!x.eqval(y)) {
									 throw RuntimeError(env,
																			env.call().pos,
																			fmt("Expected (%0), was (%1)", {y, x}));
								 }
							 });

			add_fimp(env.sym("fib"),
							 {Box(env.int_type)},
							 [&env](Fimp &fimp) {
								 Box &v(env.peek());
								 Int n(v.as<Int>()), a(0), b(1);
								 
								 while (n-- > 1) {
									 swap(a, b);
									 b += a;
								 }

								 v.as<Int>() = b;
							 });
		}
	}
}
