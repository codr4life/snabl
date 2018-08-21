![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered Snabl's defining features.

#### Mixfix
Snabl allows reordering functions/operators and arguments within expressions. Expressions may be further divided using ```,``` which evaluates the rest of the expression separately, or enclosed in ```()``` which evaluates the enclosed expression separately.

```
```

#### Multiple Dispatch
Snabl supports dispatching functions on multiple arguments, as opposed to only the type of ```this``` in most languages. Each function name may have multiple implementations with the same number of arguments/results but different types.

```
```

#### Gradually Typed
Snabl allows but doesn't require specifying types of function arguments and variables.

```
```

#### Closures
Named and anonymous functions capture their defining environment.

```
```

#### Reference Counted
Snabl uses reference counting instead of garbage collection, which often means more predictable performance and resource usage.

```
```

#### Integrated
Snabl integrates deeply into C++ and the STL.

```
```

#### Independent
Besides a decent C++17 compiler and CMake, Snabl comes without dependencies.

```
```