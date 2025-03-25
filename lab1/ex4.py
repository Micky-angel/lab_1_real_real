x = int(input("Type the value of X"))
n = [0]*1000
conjunto = {}
for i in range(x):
    v = int(input(f"Type the {i+1}st number < 1000 : "))
    n[v] = n[v] + 1
print("The numbers (not repeated) are:")
for i in range(len(n)):
    if n[i] == 1:
        print(i)
