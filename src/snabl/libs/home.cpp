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
			env.time_type = add_type<TimeType>(env.sym("Time"), {env.root_type});
			env.lambda_type = add_type<LambdaType>(env.sym("Lambda"), {env.root_type});
			
			add_macro(env.sym("t"), env.bool_type, true);			
			add_macro(env.sym("f"), env.bool_type, false);			
			add_macro(env.sym("nil"), env.nil_type);			

			add_macro(env.sym("_"), ops::Nop::type);
			add_macro(env.sym("call"), ops::Call::type);
			add_macro(env.sym("ddrop"), ops::DDrop::type);
			add_macro(env.sym("drop"), ops::Drop::type);

			add_macro(env.sym("dup"), ops::Dup::type);
			add_macro(env.sym("recall"), ops::Recall::type);
			add_macro(env.sym("rot"), ops::Rot::type);
			add_macro(env.sym("rswap"), ops::RSwap::type);
			add_macro(env.sym("sdrop"), ops::SDrop::type);
			add_macro(env.sym("swap"), ops::Swap::type);

			add_macro(env.sym("try:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									const auto &form(*in++);
									auto &op(env.emit(ops::Try::type, form.pos).as<ops::Try>());
									if (in == end) { throw SyntaxError(form.pos, "Missing handler"); }
									const auto &handler(*in++);
									if (in == end) { throw SyntaxError(form.pos, "Missing body"); }
									env.compile(*in++);
									env.emit(ops::TryEnd::type, form.pos);
									auto &prev_op(env.emit(ops::Push::type, form.pos, env.nil_type));
									env.compile(handler);
									
									op.handler_pc = [&env, &prev_op, &op]() {
										env.pc = prev_op.next;
										if (env.pc) { op.handler_pc = *env.pc; }
									};
								});
			
			add_macro(env.sym("let:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									auto &p(*in++);

									if (&p.type != &forms::Id::type && &p.type != &forms::Sexpr::type) {
										throw SyntaxError(p.pos, "Invalid place");
									}

									if (in == end) { throw Error("Missing value"); }
									env.compile(*in++);
									
									if (&p.type == &forms::Id::type) {
										env.emit(ops::Let::type, form.pos, p.as<forms::Id>().id);
									} else {
										auto &body(p.as<forms::Sexpr>().body);
										
										for (auto pp = body.rbegin(); pp != body.rend(); pp++) {
											env.emit(ops::Let::type, form.pos, pp->as<forms::Id>().id);
										}
									}
								});

			add_macro(env.sym("if:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									env.compile(*in++, func, fimp);
									auto &else_skip(env.emit(ops::Else::type, form.pos));
									env.compile(*in++, func, fimp);
									auto &if_skip(env.emit(ops::Skip::type, form.pos));
									env.compile(*in++, func, fimp);
									else_skip.as<ops::Else>().skip_pc = *if_skip.next;
									auto &end_op(env.ops.back());
									auto &ifs(if_skip.as<ops::Skip>());
									ifs.end_pc = [&env, &end_op, &ifs]() {
										env.pc = end_op.next;
										if (env.pc) { ifs.end_pc = *env.pc; }
									};
								});	

			add_macro(env.sym("switch:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									if (in == end) { throw Error("Missing value"); }
									env.compile(*in++);
									vector<ops::Skip *> skips;
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
												skips.push_back(&env.emit(ops::Skip::type,
																									form.pos).as<ops::Skip>());
											}

											auto &end_op(env.ops.back());											
											else_op.skip_pc = [&env, &end_op, &else_op]() {
												env.pc = end_op.next;
												if (env.pc) { else_op.skip_pc = *env.pc; }
											};
										}
									}

									auto &end_op(env.ops.back());
									for (auto &s: skips) {
										s->end_pc = [&env, &end_op, s]() {
											env.pc = end_op.next;
											if (env.pc) { s->end_pc = *env.pc; }
										};
									}
								});	
			
			add_macro(env.sym("func:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &lib(env.lib());
									const auto &form(*in++);
									const auto &id_form((in++)->as<forms::Id>());

									auto &args_form(*in++);
									vector<Box> args;
									
									if (&args_form.type == &forms::TypeList::type) {
										auto &ids(args_form.as<forms::TypeList>().ids);

										for (const auto id: ids) {
											const auto t(lib.get_type(id));
											if (!t) { throw Error(fmt("Unknown type: %0", {id})); }
											args.emplace_back(*t);
										}
									} else {
										throw SyntaxError(args_form.pos,
																			fmt("Invalid func args: %0",
																					{args_form.type.id}));
									}

									auto fi = lib.add_fimp(id_form.id, args, *in++);
									Fimp::compile(fi, form.pos);
								});

			add_fimp(env.sym("throw"),
							 {Box(env.root_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 throw UserError(env, env.target()->call()->pos, env.pop());
							 });

			add_fimp(env.sym("throw"),
							 {Box(env.error_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 throw *env.pop().as<ErrorPtr>();
							 });

			add_fimp(env.sym("catch"),
							 {Box(env.error_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.pop().as<ErrorPtr>()->val);
							 });

			add_fimp(env.sym("isa"),
							 {Box(env.maybe_type), Box(env.meta_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.isa(y.as<ATypePtr>()));
							 });

			add_fimp(env.sym("="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.eqval(y));
							 });

			add_fimp(env.sym("=="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.equid(y));
							 });

			add_fimp(env.sym("<"),
							 {Box(env.root_type), Box(env.root_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.cmp(y) == Cmp::LT);
							 });
	
			add_fimp(env.sym("int"),
							 {Box(env.float_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 const Float v(env.pop().as<Float>());
								 env.push(env.int_type, Int(v));
							 });

			add_fimp(env.sym("float"),
							 {Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 const Int v(env.pop().as<Int>());
								 env.push(env.float_type, Float(v));
							 });

			add_fimp(env.sym("++"),
							 {Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.peek().as<Int>()++;
							 });

			add_fimp(env.sym("--"),
							 {Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.peek().as<Int>()--;
							 });
			
			add_fimp(env.sym("+"),
							 {Box(env.int_type), Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Int y(env.pop().as<Int>());
								 env.peek().as<Int>() += y;
							 });

			add_fimp(env.sym("-"),
							 {Box(env.int_type), Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Int y(env.pop().as<Int>());
								 env.peek().as<Int>() -= y;
							 });
			
			add_fimp(env.sym("*"),
							 {Box(env.int_type), Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Int y(env.pop().as<Int>());
								 env.peek().as<Int>() *= y;
							 });

			add_fimp(env.sym("bool"),
							 {Box(env.maybe_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.bool_type, env.pop().as_bool());
							 });

			add_fimp(env.sym("iter"),
							 {Box(env.seq_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.iter_type, env.pop().iter());
							 });

			add_fimp(env.sym(".."),
							 {Box(env.seq_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 auto i(env.pop().iter());

								 while (!i->is_done()) {
									 auto v(i->call(env));
									 if (v) { env.push(*v); }
								 }
							 });
			
			add_fimp(env.sym("dump"),
							 {Box(env.maybe_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.pop().dump(cerr);
								 cerr << endl;
							 });

			add_fimp(env.sym("say"),
							 {Box(env.maybe_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.pop().print(cout);
								 cout << endl;
							 });
			
			add_fimp(env.sym("len"),
							 {Box(env.str_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.int_type, env.pop().as<StrPtr>()->size());
							 });

			add_fimp(env.sym("len"),
							 {Box(env.stack_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.int_type, env.pop().as<StackPtr>()->size());
							 });

			add_fimp(env.sym("ns"),
							 {Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.time_type, env.pop().as<Int>());
							 });			

			add_fimp(env.sym("ms"),
							 {Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.time_type, Time::ms(env.pop().as<Int>()));
							 });			

			add_fimp(env.sym("ms"),
							 {Box(env.time_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 env.push(env.int_type, env.pop().as<Time>().as_ms());
							 });
			
			add_fimp(env.sym("sleep"),
							 {Box(env.time_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 const Time time(env.pop().as<Time>());
								 this_thread::sleep_for(nanoseconds(time.ns));
							 });

			add_fimp(env.sym("test="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 Box y(env.pop()), x(env.pop());
								 
								 if (!x.eqval(y)) {
									 throw Error(fmt("Expected (%0), was (%1)", {y, x}));
								 }
							 });

			add_fimp(env.sym("bench"),
							 {Box(env.int_type), Box(env.root_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
								 const Box target(env.pop());
								 const Int reps(env.pop().as<Int>());
								 Pos pos(env.target()->call()->pos);								 
								 for (int i(0); i < reps/2; i++) { target.call(pos, true); }
								 
								 Timer t;
								 for (Int i(0); i < reps; i++) { target.call(pos, true); }
								 env.push(env.time_type, t.ns());
							 });

			add_fimp(env.sym("fib"),
							 {Box(env.int_type)},
							 [](const FimpPtr &fimp) {
								 Env &env(fimp->func->lib.env);
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
