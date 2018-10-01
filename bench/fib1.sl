func: tail-fib<Int Int Int> {
  let: (n a b)
  @n switch:, 0? @a 1? @b, --; @b dup! @a +; recall!
}

say, (10000 bench:, tail-fib 20 0 1; drop!) ms