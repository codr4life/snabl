![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered defining features.

#### Fixation
Snabl allows reordering functions and arguments within expressions. Expressions may be enclosed in ```()```, which evaluates the enclosed expression separately; and divided using ```,```, which evaluates the rest separately.

Example 1
```
func: naive-fib<Int> (
  let: n _			
  if: (@n < 2) @n, (naive-fib, @n --) + (naive-fib, @n - 2)
)
```

#### Concatenation
Much like Forth, Snabl supports directly manipulating the parameter stack. The following example currently runs around 50% faster than Example 1, mostly due to not requiring local variables. ```;``` calls the current function, ```--``` in this case, before evaluating the rest separately.

Example 2
```
func: naive-fib<Int> (
  dup if: (< 2) _, (naive-fib, --; dup) swap + (naive-fib, --)
)
```

#### Typing
Snabl supports strong, first class Types. ```A``` may be used to allow any type except ```Nil```, and ```Maybe``` where missing values are expected.

```
   42 Int?
[t]

   Int Num?
[t]

   42 Maybe?
[t]

   nil Maybe?
[t]

   42 Nil?
[f]

   nil Nil?
[t]
```

#### Function
Snabl embraces but does not mandate functional programming. It supports first class functions, pattern matching, closures and tail call optimization. The following example gets a 25% speed up from using explicit tail recursion.

Example 3
```
func: tail-fib<Int Int Int> (
  let: (n a b) _
  switch: @n, 0? @a 1? @b, --; @b dup @a +; recall
)
```

As before, the same algorithm may be implemented without variables.

Example 4
```
func: tail-fib<Int Int Int> (
  rswap switch: _,
    0? sdrop
    1? drop,
    --; rswap dup rot +; recall
)
```

#### Failure
Snabl offers two flavors of error handling, ```Maybe``` and ```throw```/```catch```. Any value may be passed as ```Maybe```, stray ```nil```'s are usually caught in the next function call/return.

```
   func: foo<A> _
   foo 42
[42]

   foo nil
[nil]
Error in row 1, col 0:
'Func not applicable: foo'

   func: bar<Maybe> _
   bar 42
[42]

   bar nil
[nil]
```

Any value may be thrown. ```try:``` runs its first argument with the error or ```nil``` on the stack depending on what happened while running the second, ```catch``` may be used to retrieve the thrown value.

```
   try: (catch; ++), throw 41
[42]
```

#### Integration
Snabl integrates deeply into C++ and the STL, and is relatively trivial to embed.

Example 5
```
snabl::Env env;

env.home.add_fimp(
  env.sym("iter-fib"),
  {snabl::Box(env.int_type)},
  [](snabl::Call &call) {
    snabl::Env &env(call.scope.env);
    snabl::Box &v(env.peek());
    snabl::Int n(v.as<snabl::Int>()), a(0), b(1);

    while (n-- > 1) {
      std::swap(a, b);
      b += a;
    }

    v.as<Int>() = b;
  });

env.run("say, iter-fib 10");
```

#### Portability
Besides a decent C++17 compiler and CMake, Snabl has no dependencies.

```
$ git clone https://github.com/codr4life/snabl.git
$ cd snabl
$ mkdir build
$ cd build
$ cmake ..
$ sudo make install
$ snabl

Snabl v0.1

Press Return on empty line to eval.

   1 + 2
.. 
[3]
```

### License
MIT