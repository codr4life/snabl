func: tail-fib<Int Int Int> {
  let: (n a b)

  @n switch: (
	  0? @a
		1? @b,
		-- @b dup! @a + recall!
  )
}

10000 bench: (20 0 1 tail-fib drop!) ms say