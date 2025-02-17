res = 0
n = int(input("Type the number of elements you want to sum"))
for i in range(n):
    num = float(input(f"Enter number {i+1}: "))
    res += num

print(f"the sum is: {res}")
