import asyncio
"""
비동기 함수 사용 테스트
"""


async def hello():
    print("Hello, World!!")


def test000():
    loop = asyncio.get_event_loop()
    loop.run_until_complete(hello())
    loop.close()


async def add(a, b):
    print('add: {0} + {1}'.format(a, b))
    await asyncio.sleep(1.0)
    return a + b


async def print_add(a, b):
    result = await add(a, b)
    print('print_add: {0} + {1} = {2}'.format(a, b, result))


def test001():
    loop = asyncio.get_event_loop()
    loop.run_until_complete(print_add(1, 2))
    print("test test")
    loop.close()


if __name__ == "__main__":
    test001()
