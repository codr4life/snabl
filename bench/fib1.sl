func: tail-fib<I64 I64 I64> {
  let: (n a b)

  @n switch:,
    0? @a
    1? @b,
    -- @b dup! @a + recall!
}

10000 bench: (20 0 1 tail-fib drop!) ms say