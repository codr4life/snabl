&(
  20 0 1 &(
    rswap!
    switch:,
      0? sdrop!
      1? drop!,
      -- rswap! dup! rot! + recall!
  ) call!
)

10000 bench: (dup! call! drop!) ms say