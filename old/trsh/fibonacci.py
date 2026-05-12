def fibonacci(x):
    memo = {0:0, 1:1}

    def pd(x):
        if x in memo:
            return memo[x]
        memo[x] = pd(x-1) + pd(x-2)
        return memo[x]

    return pd(x)