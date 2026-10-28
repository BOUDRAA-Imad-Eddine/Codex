max_value = float('-inf')  # Initialise avec une très petite valeur

for i in range(20):
    nombre = float(input("Entrez le nombre N° : "))
    if nombre > max_value:
        max_value = nombre

print("Le plus grand nombre parmi les 20 est:", max_value)