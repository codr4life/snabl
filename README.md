![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered defining features.

#### Mixfix
Snabl allows reordering functions/operators and arguments within expressions. Expressions may be further divided using ```,``` which evaluates the rest of the expression separately, or enclosed in ```()``` which evaluates the enclosed expression and pushes the result.

```
func: fib<Int> Int (
  let: n _
  if: (@n < 2) @n, (fib, @n - 1) + (fib, @n - 2)
)
```

#### Stack
Like Forth, Snabl allows directly manipulating the parameter stack from user code.

```
func: fib<Int> Int (
  dup if: (< 2) _, (fib, dup - 1) swap + (fib, - 2)
)
```

#### Multiple Dispatch
Snabl supports dispatching functions on multiple arguments. Each function name may have multiple implementations, as long as they have the same number of arguments with different types.

#### Gradual Types
Snabl allows but doesn't require specifying types of function arguments and variables.

#### Reference Counting
Snabl uses reference counting instead of garbage collection, which leads to more predictable performance and resource usage.

#### Integration
Snabl integrates deeply into C++ and the STL, and is relatively trivial to embed.

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