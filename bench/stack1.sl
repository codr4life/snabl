10000 bench: {
  [] let: lst
  100 times: (@lst [] push)
  100 times: (@lst pop)
} ms say