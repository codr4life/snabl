func: fib<Int Int Int> Int (
	rot swap dup
  if: zero?
    (drop swap drop)
    (dup if: one? (drop drop) (--; rot swap dup rot +; recall))
)

say, bench 10000 {fib 20 0 1; drop}; ms