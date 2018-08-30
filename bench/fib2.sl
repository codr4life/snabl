func: fib<Int> Int (
  dup if: (< 2) _, (fib, dup - 1) swap + (fib, - 2)
)

say, ms, bench 100 {(fib 20) drop}