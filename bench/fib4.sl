func: fib<Int Int Int> Int (
	rotl swap dup
  if: zero?
    (drop swap drop)
    (dup if: one? (drop drop) (--; rotl swap dup rotl +; recall))
)

say, bench 10000 {fib 20 0 1; drop}; ms