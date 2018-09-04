func: fib<Int Int Int> Int (
	rswap dup
  if: zero?
    (drop sdrop),
    dup if: one? ddrop, --; rswap dup rot +; recall
)

say, bench 10000 {fib 20 0 1; drop}; ms