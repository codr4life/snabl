from bench import bench

def test():
    try:
        raise 42
    except Exception as e:
        pass

print(int(bench(100000, 'test', '()')))
