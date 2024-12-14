try:
    import lz4
except ImportError:
    print('lz4 is not enabled, skipping test...')
    exit()

def test(data: bytes):
    compressed = lz4.compress(data)
    decompressed = lz4.decompress(compressed)
    assert data == decompressed
    return len(compressed) / len(data)

test(b'')
test(b'hello world')

import random

def gen_data():
    values = []
    for i in range(random.randint(0, 10000)):
        values.append(i % 256)
    return bytes(values)


for i in range(100):
    ratio = test(gen_data())
    # print(f'compression ratio: {ratio:.2f}')

# test 1GB random data
rnd = [random.randint(0, 255) for _ in range(1024*1024*1024//16)]
test(bytes(rnd))
