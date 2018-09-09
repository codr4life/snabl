![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered defining features.

#### Fixation
Snabl allows reordering functions and arguments within expressions. Expressions may be enclosed in ```()```, which evaluates the enclosed expression separately; and divided using ```,```, which evaluates the rest separately.

Example 1
```
func: fib<Int> Int (
  let: n _			
  if: (@n < 2) @n, (fib, @n --) + (fib, @n - 2)
)
```

#### Concatenation
Much like Forth, Snabl supports directly manipulating the parameter stack. The following example currently runs around 50% faster, mostly due to not requiring local variables. ```;``` calls the current function, ```--``` in this case, before evaluating the next token.

Example 2
```
func: fib<Int> Int (
  dup if: (< 2) _, (fib, --; dup) swap + (fib, --)
)
```

#### Typing
Snabl supports strong, first class Types. ```A``` may be used to allow any type except ```Nil```, and ```Maybe``` where missing values are expected.

```
   42 isa Int
[t]

   Int isa Num
[t]

   42 isa Maybe
[t]

   nil isa Maybe
[t]

   42 isa Nil
[f]

   nil isa Nil
[t]
```

#### Function
Snabl embraces but does not mandate functional programming. It supports first class functions, pattern matching, closures and tail call optimization. The following example gets a 25% speed up from using explicit tail recursion.

Example 3
```
func: fib<Int Int Int> Int (
  let: (n a b) _
  switch: @n 0? @a 1? @b, --; @b dup @a +; recall
)
```

As before, the same algorithm may be implemented without variables.

Example 4
```
func: fib<Int Int Int> Int (
  rswap switch: _
    0? sdrop
    1? drop,
    --; rswap dup rot +; recall
)
```

#### Integration
Snabl integrates deeply into C++ and the STL, and is relatively trivial to embed.

Example 5
```
snabl::Env env;

env.home.add_fimp(
  env.sym("fib"),
  {snabl::Box(env.int_type)}, {env.int_type}, [](snabl::Call &call) {
    snabl::Env &env(call.scope.env);								 
    snabl::Int n(env.pop().as<snabl::Int>()), a(0), b(1);

    while (n-- > 1) {
      std::swap(a, b);
      b += a;
    }

    env.push(env.int_type, b);
  });

env.run("say, fib 10");
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