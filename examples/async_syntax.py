def coroutine(fn):
    return fn  # a function containing `await` is already a generator function

async def tick():
    yield None

async def work(n):
    total = 0
    for i in range(n):
        await tick()
        total += i
    return total

async def main():
    a = await work(3)     # after '=' ...
    await tick()          # ... or as a statement
    return a

def run(coro):
    while True:
        try:
            next(coro)
        except StopIteration as e:
            return e.value

assert run(main()) == 3
print('ok')
