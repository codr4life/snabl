10000 bench: (
  20 0 1 &(
    rswap! switch:,
      0? sdrop!
      1? drop!,
      -- rswap! dup! rot! + recall!
  ) call! drop!
) ms say