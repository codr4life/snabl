func: naive-fib<Int> Int (
  dup if: (< 2) _, (naive-fib, --; dup) swap + (naive-fib, --)
)

say, bench 100 {naive-fib 20; drop}; ms