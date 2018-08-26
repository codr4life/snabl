![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered defining features.

#### Mixfix
Snabl allows reordering functions/operators and arguments within expressions. Expressions may be further divided using ```,``` which evaluates the rest of the expression separately, or enclosed in ```()``` which evaluates the enclosed expression and pushes the result.

```
```

#### Multiple Dispatch
Snabl supports dispatching functions on multiple arguments, as opposed to only the type of ```this``` in most languages. Each function name may have multiple implementations with the same number of arguments as long as they have different types.

```
```

#### Gradual Types
Snabl allows but doesn't require specifying types of function arguments and variables.

```
```

#### Reference Counting
Snabl uses reference counting instead of garbage collection, which often means more predictable performance and resource usage.

```
```

#### Integration
Snabl integrates deeply into C++ and the STL and is relatively trivial to embed.

```
```

#### Portability
Besides a decent C++17 compiler and CMake, Snabl has no dependencies.

```
```

### License
MIT