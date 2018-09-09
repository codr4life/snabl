from bench import bench

def test():
    try:
        raise 42
    except:
        pass

print(int(bench(100000, 'test', '()')))
