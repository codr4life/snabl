func: fib<Int> Int (
  dup if: (< 2) _, (fib, --; dup) swap + (fib, --)
)

say, bench 100 {fib 20; drop}; ms