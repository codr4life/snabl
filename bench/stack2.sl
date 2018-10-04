10000 bench: (
  []
  100 times: (dup! [] push)
  100 times: (dup! pop)
  drop!
) ms say