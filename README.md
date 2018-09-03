![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered defining features.

#### Multiple Dispatch
Snabl supports dispatching functions on multiple arguments. Each function name may have multiple implementations, as long as they have the same number of arguments with different types.

#### Gradual Typing
Snabl allows but doesn't require specifying types of values, `A` may be used to allow any type.

#### Reference Counting
Snabl uses reference counting instead of garbage collection, which leads to more predictable performance and resource usage.

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
Much like Forth, Snabl supports directly manipulating the parameter stack. The following example currently runs around 50% faster than Example 1. ```;``` calls the current function, ```--``` in this case, before evaluating the next token.

Example 2
```
func: fib<Int> Int (
  dup if: (< 2) _, (fib, --; dup) swap + (fib, --)
)
```

#### Function
Snabl embraces but does not mandate functional programming. It supports first class functions, pattern matching, closures and tail recursion. The following example currently runs around 500 times faster than Example 2 using a superior, tail recursive algorithm.

Example 3
```
func: fib<Int Int Int> Int (
  let: (n a b) _
  if: (zero? @n) @a (if: (one? @n) @b (@n --; @b dup @a +; recall))
)
```

The same algorithm may be implemented without variables to get another 50% speed up.

Example 4
```
func: fib<Int Int Int> Int (
  rotl swap dup
  if: zero?
    (drop swap drop)
    (dup if: one? (drop drop) (--; rotl swap dup rotl +; recall))
)
```

#### Safety
By default, Snabl trades some speed for safety; ```unsafe:``` may be used to disable most checks and run code around 20% faster.

```
unsafe: (fib 10 0 1)
```

#### Integration
Snabl integrates deeply into C++ and the STL, and is relatively trivial to embed. The following example runs roughly 100 times faster than Example 4 in C++.

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
```

### License
MIT