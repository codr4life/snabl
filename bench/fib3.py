from bench import bench

def fib(n):
    a, b = 0, 1

    for _ in range(n):
        a, b = b, a
        b += a

    return b

def test():
    fib(20)

print(int(bench(10000)))
