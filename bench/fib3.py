from bench import bench

def fib(n):
    a, b = 0, 1

    for _ in range(n):
        a, b = b, a
        b += a

    return b

print(int(bench(100000, 'fib', '(20)')))
