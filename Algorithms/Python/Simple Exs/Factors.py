import math

def factorize(n):
    factors = []
    # Diviser par 2 jusqu'à ce que n soit impair
    while n % 2 == 0:
        factors.append(2)
        n //= 2
    
    # Tester les nombres impairs de 3 jusqu'à sqrt(n)
    for i in range(3, int(math.sqrt(n)) + 1, 2):
        while n % i == 0:
            factors.append(i)
            n //= i
            
    # Si n est un nombre premier et supérieur à 2
    if n > 2:
        factors.append(n)
    
    return factors

# Exemples d'utilisation
n = int(input("Entrez un nombre entier pour obtenir ses facteurs premiers : "))
print(f"Les facteurs premiers de {n} sont : {factorize(n)}")