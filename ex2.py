print("do_the_fucking_lab_@")

def invertbro(n):
    res = 0
    while n > 0:
        res = res*10 + n % 10
        n = n // 10
    return res

a = int(input("type any number (like 619): "))
resultado = invertbro(a)
print(f"the resultaing number isss: {resultado}")
