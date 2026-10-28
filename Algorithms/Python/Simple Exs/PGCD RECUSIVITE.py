
def pgcd(A, B):
    if A == B :
        return A
    if A > B :
        return pgcd(A-B, B)
    else :
        return pgcd(A, B - A)

n = int(input("donner la valeur de n : "))
m = int(input("donner la valeur de m : "))

PGCD = pgcd(n, m)
print('pgcd(n,m) = ', PGCD)
