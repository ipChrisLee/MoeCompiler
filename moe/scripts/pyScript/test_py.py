if __name__ == '__main__':
    s1: str = '123'
    s2: str = str(123)
    print(id(s1))
    print(id(s2))
    print(hash(s1))
    print(hash(s2))
    se = set()
    se.add(s1)
    se.add(s2)
    print(se)
