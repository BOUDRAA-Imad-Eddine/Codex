a = float(input("give a :"))
b = int(input("give b :"))

def power(x:float, n:int):
    if n == 0 :
        return 1
    if n == 1 :
        return x
    else :
        return ((power(x,n-1))*(x))
print(power(a,b))
