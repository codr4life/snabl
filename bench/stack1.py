from bench import bench

def test():
    lst = []
    for _ in range(100): lst.append([])
    for _ in range(100): lst.pop()

print(int(bench(10000, 'test', '()')))
